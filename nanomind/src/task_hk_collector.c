/**
 * Housekeeping Collector
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#if ENABLE_TASK_HK

#include <string.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <dev/adc.h>
#include <dev/spi.h>
#include <dev/lm70.h>
#include <util/console.h>
#include <util/timestamp.h>

#include <rrstore/rrstore.h>
#include <log/log.h>
#include <log/log_level.h>

#include <io/nanomind.h>

#include "clock_store.h"
#include <util/clock.h>

rrstore_handle hk_store;
static obc_beacon_t hks = {};
static xSemaphoreHandle hk_lock = NULL;

#define LM70_MAP_SIZE	6
static spi_chip_t lm70_chip[LM70_MAP_SIZE];
static int lm70_map[LM70_MAP_SIZE] = {2, 4, 6, 9, 11, 13};
extern spi_dev_t spi_dev;

void task_hk_collector(void * param) {

	int i;
	spi_chip_t * chip;
	portTickType xLastWakeTime = xTaskGetTickCount();
	uint16_t * adc_buffer;
	hk_lock = xSemaphoreCreateMutex();

	if (rrstore_open(&hk_store, "/boot/hk.rrs") != 0) {
		printf("Trying to create new RRstore\r\n");
		if (rrstore_new(&hk_store, "/boot/hk.rrs", sizeof(obc_beacon_t), 100) != 0) {
			printf("Failed to open RRstore for HK\r\nQuitting\r\n");
		}
	}

	for (i = 0; i < LM70_MAP_SIZE; i++) {
		lm70_spi_setup_cs(&spi_dev, &lm70_chip[i], lm70_map[i]);
	}

	while(1) {

		/* Get EPS Housekeeping */
		chkparam_t eps_hk;
		int eps_status = eps_get_hk(&eps_hk);

		/* Get COM Housekeeping */
		nanocom_data_t com_hk;
		int com_status = com_get_status(&com_hk, 5);

		xSemaphoreTake(hk_lock, 100);

		memset(&hks, 0x00, sizeof(obc_beacon_t));

		/* Get OBC Housekeeping */
		adc_buffer = adc_start_blocking(1);

		timestamp_t time;
		clock_get_time(&time);
		hks.time_sec = time.tv_sec;

		hks.obc.temp1 = adc_buffer[0];
		hks.obc.temp2 = adc_buffer[1];
		for (i = 0; i < LM70_MAP_SIZE; i++) {
			chip = &lm70_chip[i];
			hks.obc.panel_temp[i] = lm70_read_raw(chip);
		}

		/* COM */
		if (com_status) {
			memcpy(&hks.com, &com_hk, sizeof(nanocom_data_t));
		} else {
			memset(&hks.com, 0, sizeof(nanocom_data_t));
		}

		/* LOG Levels */
		log_level_reply_t * reply = log_level_count(NODE_OBC, OBC_PORT_LOGD, 1000);
		if (reply->levels >= 4) {
			hks.log.levels[0] = (uint16_t) reply->level[0];
			hks.log.levels[1] = (uint16_t) reply->level[1];
			hks.log.levels[2] = (uint16_t) reply->level[2];
			hks.log.levels[3] = (uint16_t) reply->level[3];
		}

		/* EPS */
		if (eps_status) {
			hks.eps.bootcount = eps_hk.bootcount;
			hks.eps.bv = eps_hk.bv;
			hks.eps.channel_status = eps_hk.channel_status;
			hks.eps.pc= eps_hk.pc;
			hks.eps.pv[0] = eps_hk.pv[0];
			hks.eps.pv[1] = eps_hk.pv[1];
			hks.eps.pv[2] = eps_hk.pv[2];
			hks.eps.sc = eps_hk.sc;
			hks.eps.temp[0] = eps_hk.temp[0];
			hks.eps.temp[1] = eps_hk.temp[1];
			hks.eps.temp[2] = eps_hk.temp[2];
			hks.eps.temp[3] = eps_hk.temp[3];
		} else {
			hks.eps.bootcount = 0;
			hks.eps.bv = 0;
			hks.eps.channel_status = 0;
			hks.eps.pc = 0;
			hks.eps.pv[0] = 0;
			hks.eps.pv[1] = 0;
			hks.eps.pv[2] = 0;
			hks.eps.sc = 0;
			hks.eps.temp[0] = 0;
			hks.eps.temp[1] = 0;
			hks.eps.temp[2] = 0;
			hks.eps.temp[3] = 0;
		}

		xSemaphoreGive(hk_lock);

		rrstore_add(&hk_store, &hks);

		vTaskDelayUntil(&xLastWakeTime, 10000);

	}

}

void hk_collector_get_copy(obc_beacon_t * dest) {
	xSemaphoreTake(hk_lock, 100);
	memcpy(dest, &hks, sizeof(obc_beacon_t));
	xSemaphoreGive(hk_lock);
}

#endif // ENABLE_CDH_HK
