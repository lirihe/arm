#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <malloc.h>

#include <freertos/FreeRTOS.h>

#include <util/crc32.h>
#include <util/log.h>

#include <fat_sd/ffconf.h>
#include <fat_sd/ff.h>

#include <ftp/ftp_server.h>

/* Chunk status markers */
static const char const * packet_missing = "-";
static const char const * packet_ok = "+";

/* Current bitmap and file */
static FIL fs_file, fs_map;
static FATDIR dirp;
static FIL * fd_file = NULL;
static FIL * fd_map = NULL;
static char * mem_map = NULL;

/* Current file info */
char file_name[FTP_PATH_LENGTH];
uint32_t file_size;
uint32_t file_chunk_size;
uint32_t file_chunks;

/* Directory pointer used in listing */
char dirpath[FTP_PATH_LENGTH];

ftp_return_t ftp_fat_abort(void * state);
ftp_return_t ftp_fat_timeout(void * state);

/* Calculate CRC of current file */
static int file_crc(uint32_t * crc_arg) {

	/* Calculate CRC32 */
	uint8_t byte, bytes[256];
	uint32_t crc = 0xFFFFFFFF;
	unsigned int i, j;

	if (fd_file == NULL)
		return -1;

	/* Flush file before calculating CRC */
	f_sync(fd_file);
	f_lseek(fd_file, 0);

	int result;
	for (i = 0; i < file_size / 256; i++) {
		if ((result = f_read(fd_file, bytes, 256, NULL)) != FR_OK)
			return -1;
		for (j = 0; j < 256; j++)
			crc = chksum_crc32_step(crc, bytes[j]);
	}
	for (i = 0; i < file_size % 256; i++) {
		if ((result = f_read(fd_file, &byte, 1, NULL)) != FR_OK)
			return -1;
		crc = chksum_crc32_step(crc, byte);
	}
	crc = (crc ^ 0xFFFFFFFF);
	*crc_arg = crc;

	return 0;

}

ftp_return_t ftp_fat_upload(void * state, const char const * path, uint32_t memaddr, uint32_t size, uint32_t chunk_size) {

	/* Map file name */
	char map[100];

	file_size = size;
	file_chunk_size = chunk_size;
	file_chunks = (size + chunk_size - 1) / chunk_size;
	strncpy(file_name, path, FTP_PATH_LENGTH);
	file_name[FTP_PATH_LENGTH - 1] = '\0';

	/* Abort if a previous transfer was incomplete */
	if (fd_file || fd_map || mem_map) {
		printf("Aborting previous transfer %p %p %p\r\n", fd_file, fd_map, mem_map);
		ftp_fat_timeout(state);
	}

	/* Assign pointers */
	fd_file = &fs_file;
	fd_map = &fs_map;

	/* Try to open file */
	if (f_open(fd_file, (const char *) path, FA_OPEN_ALWAYS | FA_WRITE | FA_READ) != FR_OK) {
		printf("Server: Failed to create data file\r\n");
		return FTP_RET_IO;
	}

	/* Allocate memory map */
	if (mem_map == NULL) {
		mem_map = malloc(file_chunks);
		if (mem_map == NULL) {
			f_close(fd_file);
			fd_file = NULL;
			return FTP_RET_NOSPC;
		}
	}

	/* Try to create a new map file */
	strncpy(map, path, FTP_PATH_LENGTH);
	strcpy((char *) map + strlen(path) - 4, ".MAP");

	/* Check if file already exists */
	if (f_open(fd_map, map, FA_OPEN_EXISTING | FA_WRITE | FA_READ) == FR_OK) {

		unsigned int read;
		if (f_read(fd_map, mem_map, file_chunks, &read) != FR_OK) {
			f_close(fd_file);
			fd_file = NULL;
			f_close(fd_map);
			fd_map = NULL;
			return FTP_RET_IO;
		}

	} else {

		unsigned int i;

		/* Create new file */
		if (f_open(fd_map, map, FA_OPEN_ALWAYS | FA_WRITE | FA_READ) != FR_OK) {
			printf("Failed to create bitmap %s\r\n", map);
			f_close(fd_file);
			fd_file = NULL;
			free(mem_map);
			mem_map = NULL;
			return FTP_RET_IO;
		}

		/* Clear contents */
		for (i = 0; i < file_chunks; i++) {
			int result = f_write(fd_map, packet_missing, 1, NULL);
			if (result != FR_OK) {
				printf("Failed to clear bitmap %u\r\n", result);
				f_close(fd_map);
				fd_map = NULL;
				f_close(fd_file);
				fd_file = NULL;
				free(mem_map);
				mem_map = NULL;
				return FTP_RET_IO;
			}
			mem_map[i] = *packet_missing;
		}

		f_sync(fd_map);

	}

	return FTP_RET_OK;

}

ftp_return_t ftp_fat_download(void * state, const char const * path, uint32_t memaddr, uint32_t memsize, uint32_t chunk_size, uint32_t * size, uint32_t * crc) {

	FILINFO statbuf = {0};
#if _USE_LFN
	char lfn[(_MAX_LFN + 1) * 2];
	statbuf.lfname = lfn;
	statbuf.lfsize = sizeof(lfn);
#endif

	/* Assign pointers */
	fd_file = &fs_file;

	/* Try to open file */
	if (f_open(fd_file, path, FA_OPEN_EXISTING | FA_READ) != FR_OK)
		return FTP_RET_NOENT;

	/* Read size */
	if (f_stat(path, &statbuf) != FR_OK)
		return FTP_RET_NOENT;

	*size = statbuf.fsize;
	file_size = *size;
	file_chunk_size = chunk_size;
	file_chunks = (file_size + chunk_size - 1) / chunk_size;
	strncpy(file_name, path, FTP_PATH_LENGTH);
	file_name[FTP_PATH_LENGTH - 1] = '\0';

	/* Calculate CRC */
	if (file_crc(crc) != 0)
		return FTP_RET_IO;

	return FTP_RET_OK;

}

ftp_return_t ftp_fat_write_chunk(void * state, uint32_t chunk, uint8_t * data, uint32_t size) {

	/* Write to file */
	if (f_lseek(fd_file, chunk * file_chunk_size) != FR_OK)
		goto chunk_write_error;
	if (f_write(fd_file, data, size, NULL) != FR_OK)
		goto chunk_write_error;
	if (f_sync(fd_file) != FR_OK)
		goto chunk_write_error;

	return FTP_RET_OK;

chunk_write_error:
	printf("Filesystem write error\r\n");
	f_close(fd_file);
	fd_file = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_fat_read_chunk(void * state, uint32_t chunk, uint8_t * data, uint32_t size) {

	/* Read from file */
	if (f_lseek(fd_file, chunk * file_chunk_size) != FR_OK)
		goto chunk_read_error;
	if (f_read(fd_file, data, size, NULL) != FR_OK)
		goto chunk_read_error;

	return FTP_RET_OK;

chunk_read_error:
	printf("Filesystem read error (chunk %"PRIu32", size %"PRIu32")\r\n", chunk, size);
	f_close(fd_file);
	fd_file = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_fat_get_status(void * state, uint32_t chunk, int * status) {

	if (status) {
		*status = (mem_map[chunk] == *packet_ok);
		return FTP_RET_OK;
	} else {
		return FTP_RET_INVAL;
	}

}

ftp_return_t ftp_fat_set_status(void * state, uint32_t chunk) {

	/* Write to map file */
	if (f_lseek(fd_map, chunk) != FR_OK)
		goto status_write_error;
	if (f_write(fd_map, packet_ok, 1, NULL) != FR_OK)
		goto status_write_error;
	if (f_sync(fd_map) != FR_OK)
		goto status_write_error;

	/* Mark in RAM map */
	mem_map[chunk] = *packet_ok;

	return FTP_RET_OK;

status_write_error:
	printf("Failed to write status for chunk\r\n");
	f_close(fd_map);
	return FTP_RET_IO;

}

ftp_return_t ftp_fat_get_crc(void * state, uint32_t * crc) {

	return (crc != NULL && file_crc(crc) == 0) ? FTP_RET_OK : FTP_RET_INVAL;

}

ftp_return_t ftp_fat_timeout(void * state) {

	/* Close files */
	if (fd_file) {
		f_close(fd_file);
		fd_file = NULL;
	}
	if (fd_map) {
		f_close(fd_map);
		fd_map = NULL;
	}
	if (mem_map) {
		free(mem_map);
		mem_map = NULL;
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_fat_done(void * state) {

	/* Close handles */
	ftp_fat_timeout(state);

	/* Generate map path */
	char map[100];
	file_name[FTP_PATH_LENGTH-1] = '\0';
	strncpy(map, file_name, FTP_PATH_LENGTH);
	strcpy((char *) map + strlen(file_name) - 4, ".MAP");

	/* Remove map */
	f_unlink(map);

	return FTP_RET_OK;

}

ftp_return_t ftp_fat_abort(void * state) {

	/* Remove map */
	if (ftp_fat_done(state) != FTP_RET_OK) {
		return FTP_RET_IO;
	}

	/* Remove file */
	if (remove(file_name) != 0) {
		printf("Failed to remove %s\r\n", file_name);
		return FTP_RET_IO;
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_fat_list(void * state, char * path, uint16_t * entries) {

	printf("Fat list\r\n");

	int res;
	FILINFO ent = {0};
	FATDIR tdirp = {0};

	if (!path || !entries)
		return FTP_RET_INVAL;

	printf("Fat list 1\r\n");

	uint16_t entries_count = 0;

	res = f_opendir(&tdirp, path);
	if (res != FR_OK)
		return FTP_RET_NOENT;

	printf("Fat list 2\r\n");

	strncpy(dirpath, path, FTP_PATH_LENGTH);

	/* Count entries */
	while ((res = f_readdir(&tdirp, &ent)) == FR_OK && ent.fname[0] != 0)
		entries_count++;

	/* Copy entries */
	*entries = 2;

	printf("Fat list 3\r\n");

	/* Open for entry function */
	res = f_opendir(&dirp, path);
	if (res != FR_OK)
		return FTP_RET_NOENT;

	printf("Fat list 4\r\n");

	return FTP_RET_OK;

}

ftp_return_t ftp_fat_entry(void * state, ftp_list_entry_t * ent) {

	FILINFO dent = {0};
	char * fn;

#if _USE_LFN
	char lfn[(_MAX_LFN + 1) * 2];
	dent.lfname = lfn;
	dent.lfsize = sizeof(lfn);
#endif

	/* Loop through directories */
	if (f_readdir(&dirp, &dent) == FR_OK && dent.fname[0] != 0) {
		if (ent == NULL)
			return FTP_RET_INVAL;

		/* Name */
#if _USE_LFN
		fn = *dent.lfname ? dent.lfname : dent.fname;
#else
		fn = dent.fname;
#endif
		strncpy(ent->path, fn, FTP_PATH_LENGTH);
		ent->path[FTP_PATH_LENGTH-1] = '\0';

		/* Type */
		ent->type = (dent.fattrib & AM_DIR ? FTP_LIST_DIR : FTP_LIST_FILE);

		/* Size */
		ent->size = dent.fsize;
	} else {
		return FTP_RET_NOENT;
	}

	return FTP_RET_OK;

}

ftp_backend_t backend_fat = {
	.upload 		= ftp_fat_upload,
	.download 		= ftp_fat_download,
	.chunk_write	= ftp_fat_write_chunk,
	.chunk_read 	= ftp_fat_read_chunk,
	.status_get 	= ftp_fat_get_status,
	.status_set 	= ftp_fat_set_status,
	.list 			= ftp_fat_list,
	.entry 			= ftp_fat_entry,
	//.remove		= ftp_fat_remove,
	//.move			= ftp_fat_move,
	.crc 			= ftp_fat_get_crc,
	.abort 			= ftp_fat_abort,
	.done 			= ftp_fat_done,
	.timeout		= ftp_fat_timeout,
};
