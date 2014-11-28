/**
 * Housekeeping Task
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <csp/csp.h>
#include <util/log.h>

#include "config.h"
#include "hk_collect.h"
#include "hk_pack.h"
#include "hk_store.h"

#define HK_STORE_PURGE_INTERVAL	600

void task_hk(void * param __attribute__((unused))) {

	portTickType time_now;
	portTickType last_collect = 0;
	portTickType last_beacon[CDH_HK_BEACON_TYPES] = {};
	portTickType last_store[CDH_HK_BEACON_TYPES] = {};
	portTickType last_fs_write = 0;
	hk_cache_t * hk = NULL;

	/* Wait for FS to settle */
	vTaskDelay(5 * configTICK_RATE_HZ);
	time_now = xTaskGetTickCount();

	/* Init store */
	hk_store_init();

	/* Pre-purge once per boot in order to prevent infinite files if
	 * reboot loop < HK_STORE_PURGE_INTERVAL */
	hk_store_purge(200, 0);

	while(1) {

		/* Wake every sec. to check timeouts */
		vTaskDelayUntil(&time_now, (1 * configTICK_RATE_HZ));

		/* Collect Housekeeping */
		if (time_now >= last_collect + (config.collect_interval * configTICK_RATE_HZ)) {

			/* Collect housekeeping data from all subsystems */
			last_collect = time_now;
			hk = hk_collect();
			hk_pack(hk);

		}

		/* For each beacon type */
		for (int t = 0; t < CDH_HK_BEACON_TYPES; t++) {

			/* Check for beacon B which should only be run when kalman filter is running */
			if ((t == 1) && (hk->adcs_state.determinationMode != ADS_KALMANFILTER) && (config.force_beacon_b == 0))
				continue;

			cdh_beacon * beacon = hk_pack_get(t);

			/* Store */
			if (time_now >= last_store[t] + (config.interval[t].store * configTICK_RATE_HZ)) {
				last_store[t] = time_now;
				hk_store_add(beacon);
			}

			/* Transmit beacon */
			if (time_now >= last_beacon[t] + (config.interval[t].transmit * configTICK_RATE_HZ)) {
				last_beacon[t] = time_now;

				/* Get packet ready */
				csp_packet_t * packet = csp_buffer_get(sizeof(cdh_beacon));
				if (packet == NULL)
					continue;

				/* Copy data and set length */
				memcpy(packet->data, beacon, sizeof(cdh_beacon));
				packet->length = sizeof(cdh_beacon);

				/* Set the live flag in packet */
				cdh_beacon * tx_beacon = (cdh_beacon *) packet->data;
				tx_beacon->beacon_flags |= CDH_HK_BEACON_LIVE;

				/* Send out packet */
				if (csp_sendto(CSP_PRIO_NORM, CDH_NODE_BEACON_SERVER, CDH_PORT_BEACON_SERVER, 0, CSP_SO_NONE, packet, 0) != CSP_ERR_NONE)
					csp_buffer_free(packet);

			}

		}

		/* Filesystem writeback */
		if (time_now >= last_fs_write + (HK_STORE_PURGE_INTERVAL * configTICK_RATE_HZ)) {
			last_fs_write = time_now;
			hk_store_purge(200, 0);
		}

	}

}
