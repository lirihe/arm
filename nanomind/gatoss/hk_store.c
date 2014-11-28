/**
 * Housekeeping Store
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <inttypes.h>

#include <io/cdh.h>
#include <fat_sd/ff.h>
#include <util/log.h>
#include <util/clock.h>
#include <util/timestamp.h>
#include <csp/csp_endian.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <uthash/utarray.h>
#include <util/driver_debug.h>

#include "hk_store.h"
#include "config.h"

#define HK_RAM_SIZE				250000
#define HK_COUNT				(HK_RAM_SIZE / sizeof(cdh_beacon))
#define HK_TIMESTAMP_DIVIDER	60
#define HK_FILE_MAX_COUNT		720

/* Main memory */
static cdh_beacon beacons[HK_COUNT];
static int beacon_in = 0;
static int beacon_out = 0;

/* Current file */
static FIL fd = {};
static unsigned int fd_timestamp = 0;
static int fs_ok = 0;
static int fd_count = 0;

typedef struct {
    unsigned int time;
    unsigned int size;
} hk_item_t;
UT_icd intpair_icd = {sizeof(hk_item_t), NULL, NULL, NULL};

/* Helper function to get all file timestamps in hk folder */
int hk_store_list_folder(UT_array ** list) {

	FATDIR dir;
	FILINFO info;
	hk_item_t item;

	/* Try to open HK dir */
	int result;
	if ((result = f_opendir(&dir, "hk")) != FR_OK) {
		log_error("HK_LIST_FAIL", "Result: %u", result);
		return -1;
	}

	/* Init array */
	utarray_new(*list, &intpair_icd);

	while (f_readdir(&dir, &info) == FR_OK) {

		/* Look for end condition */
		if (info.fname[0] == 0) break;
		if (info.fname[0] == '.') continue;

		/* Convert filename to timestamp */
		unsigned int timestamp = 0;
		sscanf(info.fname, "%u.db", &timestamp);

		/* Do not list current active file */
		if (timestamp == fd_timestamp)
			continue;

		item.time = timestamp;
		item.size = info.fsize;

		/* Append to list */
		utarray_push_back(*list, &item);

	}

	int itemsort(const void *a,const void*b) {
	    int _a = ((hk_item_t *) a)->time;
	    int _b = ((hk_item_t *) b)->time;
	    return _b - _a;
	}
	utarray_sort(*list, itemsort);

	return 0;

}

int hk_store_add(cdh_beacon * beacon) {

	/* Update input/output pointers */
	beacon_in = (beacon_in + 1) % HK_COUNT;
	if (beacon_in == beacon_out)
		beacon_out = (beacon_out + 1) % HK_COUNT;

	/* Copy to RAM */
	memcpy(&beacons[beacon_in], beacon, sizeof(cdh_beacon));

	/* Copy to FS */
	if (fs_ok) {
		unsigned int written;
		int result = f_write(&fd, beacon, sizeof(cdh_beacon), &written);
		if (result != FR_OK || written == 0) {
			log_error("HK_WRITE_ERROR", "Failed to write HK to SD-Card\r\n");
			f_close(&fd);
			fs_ok = 0;
			return -1;
		}
		f_sync(&fd);
	}

	/* Rotate files */
	if (++fd_count > HK_FILE_MAX_COUNT) {
		f_close(&fd);
		fd_count = 0;
		hk_store_init();
	}

	return 0;

}

void hk_store_iterate(hk_store_iterate_callback callback, unsigned int from, unsigned int to) {

	unsigned int last_timestamp_ram = 0;

	/*  First go through RAM */
	int beacon_ptr = beacon_in;
	while (beacon_ptr != beacon_out) {
		if ((*callback)(&beacons[beacon_ptr]) < 0)
			return;
		last_timestamp_ram = csp_ntoh32(beacons[beacon_ptr].beacon_time);
		if (beacon_ptr == 0)
			beacon_ptr = HK_COUNT;
		beacon_ptr--;
	}

	/* List files */
	UT_array * list = NULL;
	if (hk_store_list_folder(&list) < 0)
		return;
	if (list == NULL)
		return;

	hk_item_t * item = NULL;
	int break_next = 0;
	while ((item = (void *) utarray_next(list, item))) {
		printf("hk/%u.db\t\t%u\r\n", item->time, item->size);

		/* Files which are greater than timestamp contains only newer elements */
		if ((to) && (item->time > to / HK_TIMESTAMP_DIVIDER))
			continue;

		/* Files which are greater than last timestamp from RAM contains only newer elements */
		if ((last_timestamp_ram) && (item->time > last_timestamp_ram / HK_TIMESTAMP_DIVIDER))
			continue;

		/* If the file start time is older than current time,
		 * do open it, but open this as the last one */
		if ((from) && (item->time < from / HK_TIMESTAMP_DIVIDER))
			break_next = 1;

		/* Process file */
		char hkpath[20];
		sprintf(hkpath, "hk/%u.db", item->time);
		printf("Open %s\r\n", hkpath);

		/* Try to open */
		FIL fd;
		if ((f_open(&fd, hkpath, FA_READ | FA_OPEN_EXISTING)) != FR_OK) {
			log_error("HK_OPEN_FAIL", "%s", hkpath);
			break;
		}

		/* Read one element at a time */
		cdh_beacon beacon;
		unsigned int bytesread;
		unsigned int bytestotal = 0;

		/* Read the file from behind */
		while(1) {

			/* Check if we read the entire file */
			if (bytestotal >= fd.fsize)
				break;

			/* Seek */
			if (f_lseek(&fd, fd.fsize - sizeof(cdh_beacon) - bytestotal) != FR_OK) {
				driver_debug(DEBUG_OBC_GC, "Seek fail\r\n");
				break;
			}

			/* Read */
			if (f_read(&fd, &beacon, sizeof(cdh_beacon), &bytesread) != FR_OK) {
				driver_debug(DEBUG_OBC_GC, "Read fail\r\n");
				break;
			}

			/* Check for end */
			if (bytesread != sizeof(cdh_beacon))
				break;

			if ((*callback)(&beacon) < 0) {
				break_next = 1;
				break;
			}

			/* Increment and check for end */
			bytestotal += bytesread;

		}

		/* Close file */
		f_close(&fd);

		if (break_next)
			break;

	}

	utarray_free(list);


}

int hk_store_init(void) {

	/* Create folder */
	f_mkdir("hk");

	/* Open a new file for housekeeping per boot
	 * With the filename = timestamp */
	timestamp_t time_now;
	clock_get_time(&time_now);
	fd_timestamp = time_now.tv_sec / HK_TIMESTAMP_DIVIDER;
	char path[50];
	sprintf(path, "hk/%u.db", fd_timestamp);
	driver_debug(DEBUG_OBC_GC, "Opening file %s", path);

	/* Try to open */
	if ((f_open(&fd, path, FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK) {
		log_error("HK_OPEN_FAIL", "%s", path);
		fs_ok = 0;
		return -1;
	}

	fs_ok = 1;
	return 0;

}

void hk_store_purge(unsigned int max_file_count, unsigned int max_age_sec) {

	/* List files */
	UT_array * list = NULL;
	if (hk_store_list_folder(&list) < 0)
		return;
	if (list == NULL)
		return;

	/* List timestamps */
	driver_debug(DEBUG_OBC_GC, "TIMESTAMPS:\r\n");
	unsigned int count = 0;
	hk_item_t * item = NULL;
	while ((item = (void *) utarray_next(list, item))) {

		driver_debug(DEBUG_OBC_GC, "hk/%u.db\t\t%u\r\n", item->time, item->size);

		/* Delete empty file */
		if (item->size == 0) {
			char hkpath[20];
			sprintf(hkpath, "hk/%u.db", item->time);
			f_unlink(hkpath);
			driver_debug(DEBUG_OBC_GC, " ^^ DELETE EMPTY ^^\r\n");
			continue;
		}

		/* Count and purge files */
		++count;
		if (count > max_file_count) {
			char hkpath[20];
			sprintf(hkpath, "hk/%u.db", item->time);
			f_unlink(hkpath);
			driver_debug(DEBUG_OBC_GC, " ^^ DELETE MAX COUNT ^^\r\n");
			continue;
		}

		/* Delete old files */
		if ((max_age_sec > 0) && (fd_timestamp > 0)) {
			if (fd_timestamp > item->time + max_age_sec) {
				char hkpath[20];
				sprintf(hkpath, "hk/%u.db", item->time);
				f_unlink(hkpath);
				driver_debug(DEBUG_OBC_GC, " ^^ DELETE TOO OLD ^^\r\n");
				continue;
			}
		}

	}

	utarray_free(list);

}
