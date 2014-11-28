/**
 * Housekeeping service
 *
 * @author Morten Jensen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdlib.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <util/clock.h>
#include <util/log.h>

#include "hk_collect.h"
#include "hk_store.h"

void service_hk_beacons(csp_conn_t * conn) {

	csp_packet_t * packet;
	uint32_t beacon_time;
	uint32_t last_found_time = 0;
	uint16_t count = 0;

	if ((packet = csp_read(conn, 0)) == NULL)
		return;

	/* Parse incoming request */
	struct cdh_beacon_req * req = (void *) packet->data;
	uint32_t from = csp_ntoh32(req->from);
	uint32_t to = csp_ntoh32(req->to);
	uint16_t min_interval = csp_ntoh16(req->min_interval);
	uint16_t max_count = csp_ntoh16(req->max_count);
	uint8_t mask = req->mask;
	csp_buffer_free(packet);

	/* Get time */
	timestamp_t time;
	clock_get_time(&time);

	/* Loop through all beacons using a iterator pattern */
	int my_local_iterator(cdh_beacon * beacon) {

		/* Fetch beacon time */
		beacon_time = csp_ntoh32(beacon->beacon_time);

		/* Data is normally sorted, but hk store may give a few duplicates when moving from
		 * RAM to the file based storage, so ensure that the order is correct and skip duplicates
		 */
		if ((last_found_time > 0) && (beacon_time > last_found_time))
			return 0;

		/* From */
		if (from > 0 && from > beacon_time)
			return 0;

		/* To */
		if (to > 0 && to < beacon_time)
			return 0;

		/* Min interval */
		if (min_interval > 0 && abs(last_found_time - beacon_time) < min_interval)
			return 0;

		/* Mask */
		if ((mask > 0) && ((beacon->beacon_flags & mask) == 0))
			return 0;

		/* This frame is accepted */
		last_found_time = beacon_time;

		/* Max count */
		if (max_count > 0 && count++ >= max_count)
			return -1;

		/* Get packet ready */
		csp_packet_t * packet = csp_buffer_get(sizeof(cdh_beacon));
		if (packet == NULL)
			return -1;

		/* Copy data and set length */
		memcpy(packet->data, beacon, sizeof(cdh_beacon));
		packet->length = sizeof(cdh_beacon);

		/* Send out packet */
		if (!csp_send(conn, packet, 1 * configTICK_RATE_HZ)) {
			csp_buffer_free(packet);
			return -1;
		}

		return 0;

	}

	hk_store_iterate(&my_local_iterator, from, to);

}

#if 0
void service_hk_beacon_store(csp_conn_t * conn) {

	/* Read packet */
	csp_packet_t * packet;
	if ((packet = csp_read(conn, 0)) == NULL)
		return;

	/* Parse incoming request */
	struct cdh_beacon_store * req = (void *) packet->data;
	req->from = csp_ntoh32(req->from);
	req->to = csp_ntoh32(req->to);
	req->interval = csp_ntoh32(req->interval);

	/* Call store backend */
	int8_t result = (int8_t) hk_store_file(req->path, req->from, req->to, req->interval);

	/* Generate reply */
	packet->data[0] = result;
	packet->length = sizeof(result);

	/* Send out packet */
	if (!csp_send(conn, packet, 0))
		csp_buffer_free(packet);

}
#endif
