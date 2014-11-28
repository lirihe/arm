/**
 * @file gatoss_client.c
 * GATOSS Microcontroller
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <io/gatoss.h>
#include <util/byteorder.h>
#include <util/color_printf.h>
#include <util/clock.h>
#include <util/timestamp.h>

static uint8_t node_gatoss = GATOSS_NODE;

void gatoss_set_node(uint8_t node) {
	node_gatoss = node;
}

int gatoss_power_on(uint16_t channels) {
	return gatoss_power(channels, 1, NULL);
}

int gatoss_power_off(uint16_t channels) {
	return gatoss_power(channels, 0, NULL);
}

int gatoss_power(uint16_t channels, int state, uint16_t * channels_state) {

	/* Fill in request */
	struct gatoss_power_req req = { channels, state };
	struct gatoss_power_state reply;

	/* Send request */
	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_POWER, (channels_state == NULL) ? 0 : 5000, &req, sizeof(req), &reply, sizeof(reply));
	if (ret <= 0)
		return ret;

	/* Push information back to user */
	if (channels_state != NULL)
		*channels_state = csp_ntoh16(reply.channels_state);
	return ret;

}

int gatoss_hk(struct gatoss_hk * hk, uint8_t save) {

	/* Fill in request */
	struct gatoss_hk_req req = {save};

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_HK, 5000, &req, sizeof(req), hk, sizeof(struct gatoss_hk));
	if (ret <= 0)
		return ret;

	/* Byte order conversion */
	hk->current_1v2 		= util_betoh16(hk->current_1v2);
	hk->current_2v5 		= util_betoh16(hk->current_2v5);
	hk->current_3v3_adc 	= util_betoh16(hk->current_3v3_adc);
	hk->current_3v3_board 	= util_betoh16(hk->current_3v3_board);
	hk->current_3v3_fpga 	= util_betoh16(hk->current_3v3_fpga);
	hk->current_5v0_board 	= util_betoh16(hk->current_5v0_board);
	hk->bootcause			= util_betoh16(hk->bootcause);
	hk->bootcount			= util_betoh32(hk->bootcount);
	hk->average_fps_10sec	= util_betoh16(hk->average_fps_10sec);
	hk->average_fps_1min	= util_betoh16(hk->average_fps_1min);
	hk->average_fps_5min	= util_betoh16(hk->average_fps_5min);
	hk->frame_count			= util_betoh32(hk->frame_count);
	hk->plane_count			= util_betoh16(hk->plane_count);
	hk->last_icao24			= util_betoh32(hk->last_icao24);
	hk->last_pos.altitude	= util_betoh32(hk->last_pos.altitude);
	hk->last_pos.timestamp	= util_betoh32(hk->last_pos.timestamp);
	hk->last_pos.lat		= util_ntohflt(hk->last_pos.lat);
	hk->last_pos.lon		= util_ntohflt(hk->last_pos.lon);
	hk->crc_corrected		= util_betoh32(hk->crc_corrected);
	hk->current_time		= util_betoh32(hk->current_time);
	hk->tot_frames			= util_betoh32(hk->tot_frames);
	hk->tot_planes			= util_betoh32(hk->tot_planes);
	hk->tot_crc_corrected	= util_betoh32(hk->tot_crc_corrected);
	hk->fpga_crc_cnt		= util_betoh32(hk->fpga_crc_cnt);

	return ret;

}

int gatoss_load_fpga(char * path, uint8_t load_now, uint8_t store_as_dfl) {

	struct gatoss_load_req req;

	strncpy((char *) req.path, (char *) path, GATOSS_LOAD_FILE_LEN_MAX);
	req.load_now = load_now;
	req.store_as_dfl = store_as_dfl;

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_LOAD_FPGA, 5000, &req, sizeof(req), &reply, 1);
	if (ret <= 0)
		return -1;

	return reply;

}

int gatoss_load_uc(char * path, uint8_t load_now, uint8_t store_as_dfl) {

	struct gatoss_load_req req;

	strncpy((char *) req.path, (char *) path, GATOSS_LOAD_FILE_LEN_MAX);
	req.load_now = load_now;
	req.store_as_dfl = store_as_dfl;

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_LOAD_UC, 5000, &req, sizeof(req), &reply, 1);
	if (ret <= 0)
		return -1;

	return reply;

}


int gatoss_threshold(int32_t amount) {

	int32_t req = csp_hton32(amount);
	int32_t reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_THRESHOLD, 5000, &req, sizeof(req), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -2;
	else
		return reply;

}

int gatoss_sel_adc(uint8_t adc) {
	uint8_t req = adc;
	int8_t reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_SEL_ADC, 5000, &req, sizeof(req), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -2;
	else
		return reply;

}

int gatoss_format_fs(void) {

	unsigned char result;
	int status = csp_transaction(CSP_PRIO_HIGH, node_gatoss, GATOSS_PORT_RECOVER_FS, 25000, NULL, 0, &result, 1);
	if (status != 1)
		return -2;

	return result;

}

int gatoss_list_short(uint32_t filtering) {

	struct gatoss_list_req req;
	req.filtering = csp_hton32(filtering);

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_LIST_SHORT, 5000, CSP_O_NONE);
	if (conn == NULL)
		return -1;

	/* Send request */
	csp_packet_t * packet = csp_buffer_get(sizeof(req));
	packet->length = sizeof(req);
	memcpy(packet->data, &req, sizeof(req));

	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -1;
	}

	while ((packet = csp_read(conn, 5000)) != NULL) {

		struct gatoss_list_short * list = (void *) packet->data;
		printf("Received %"PRIu8" planes\r\n", list->count);

		int i = 0;
		for (i = 0; i < list->count; i++) {

			gatoss_pos_t pos;
			gatoss_positions_decompress(&list->planes[i].compos, &pos);

			uint32_t icao24 = (list->planes[i].icao24[0]) | (list->planes[i].icao24[1]<<8) | (list->planes[i].icao24[2]<<16);

			unsigned int color = COLOR_GREEN;

			color_printf(color, "[%06"PRIX32"]", icao24);
			color_printf(color, " [%4"PRIu32"]", pos.timestamp);
			color_printf(color, " La: %7.03f", pos.lat);
			color_printf(color, " Lo: %7.03f", pos.lon);
			color_printf(color, " A: %5"PRIu32, pos.altitude);
			color_printf(color, "\r\n");

		}

		csp_buffer_free(packet);
		if (list->end_flag)
			break;

	}

	csp_close(conn);
	return 0;

}

int gatoss_list_full(uint32_t filtering) {

	struct gatoss_list_req req;
	req.filtering = csp_hton32(filtering);

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_LIST_FULL, 5000, CSP_O_NONE);
	if (conn == NULL)
		return -1;

	/* Send request */
	csp_packet_t * packet = csp_buffer_get(sizeof(req));
	packet->length = sizeof(req);
	memcpy(packet->data, &req, sizeof(req));

	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -1;
	}

	while ((packet = csp_read(conn, 5000)) != NULL) {

		struct gatoss_list_full * list = (void *) packet->data;
		printf("Received %"PRIu8" planes\r\n", list->count);

		int i = 0;
		for (i = 0; i < list->count; i++) {
			list->planes[i].icao24 = csp_ntoh32(list->planes[i].icao24);
			list->planes[i].last_contact = csp_ntoh32(list->planes[i].last_contact);
			list->planes[i].first_contact = csp_ntoh32(list->planes[i].first_contact);
			list->planes[i].frame_counter = csp_hton16(list->planes[i].frame_counter);
			list->planes[i].position_counter = csp_hton16(list->planes[i].position_counter);
			list->planes[i].heading = csp_ntoh16(list->planes[i].heading);
			list->planes[i].speed = csp_ntoh16(list->planes[i].speed);
			gatoss_pos_t pos;
			gatoss_positions_decompress(&list->planes[i].compos, &pos);

			int color = COLOR_GREEN;

			color_printf(color, "[%06"PRIX32"] ", list->planes[i].icao24);
			color_printf(color, "<%8s>", list->planes[i].id);
			color_printf(color, " C: %4"PRIu16"/%4"PRIu16" [%4"PRIu32"-%4"PRIu32"]", list->planes[i].position_counter, list->planes[i].frame_counter, list->planes[i].first_contact, list->planes[i].last_contact);
			color_printf(color, " La: %7.03f", pos.lat);
			color_printf(color, " Lo: %7.03f", pos.lon);
			color_printf(color, " A: %5"PRIu32, pos.altitude);
			color_printf(color, " H: %3.00f", list->planes[i].heading / 150.0);
			color_printf(color, " S: %03"PRIu16, list->planes[i].speed);
			color_printf(color, "\r\n");

		}

		csp_buffer_free(packet);
		if (list->end_flag)
			break;

	}

	csp_close(conn);
	return 0;

}

int gatoss_plane_full(struct gatoss_plane_full * plane, uint32_t icao24) {

	struct gatoss_plane_full_req req;
	req.icao24 = csp_hton32(icao24);

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_PLANE_FULL, 5000, &req, sizeof(req), plane, -1);
	if (ret <= 0)
		return -1;

	plane->icao24 = csp_ntoh32(plane->icao24);
	plane->first_contact = csp_ntoh32(plane->first_contact);
	plane->last_contact = csp_ntoh32(plane->last_contact);
	plane->frame_counter = csp_ntoh16(plane->frame_counter);
	plane->position_counter = csp_ntoh16(plane->position_counter);
	plane->speed = csp_ntoh16(plane->speed);
	plane->heading = csp_ntoh16(plane->heading);

	return (ret - offsetof(struct gatoss_plane_full, positions)) / sizeof(plane->positions[0]);

}

int gatoss_plane_positions(uint32_t icao24, uint32_t min_distance) {

	struct gatoss_plane_positions_req req;
	req.icao24 = csp_hton32(icao24);
	req.min_distance = csp_hton32(min_distance);

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_PLANE_POSITIONS, 5000, CSP_O_NONE);
	if (conn == NULL)
		return -1;

	/* Send request */
	csp_packet_t * packet = csp_buffer_get(sizeof(req));
	packet->length = sizeof(req);
	memcpy(packet->data, &req, sizeof(req));

	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -1;
	}

	while ((packet = csp_read(conn, 5000)) != NULL) {

		struct gatoss_plane_positions * list = (void *) packet->data;
		printf("Received %"PRIu8" positions from [%06"PRIx32"] end flag [%"PRIu8"]\r\n", list->count, csp_hton32(list->icao24), list->end_flag);

		int i = 0;
		for (i = 0; i < list->count; i++) {
			gatoss_pos_t pos;
			gatoss_positions_decompress(&list->positions[i], &pos);

			int color = COLOR_GREEN;
			color_printf(color, " La: %7.03f", pos.lat);
			color_printf(color, " Lo: %7.03f", pos.lon);
			color_printf(color, " A: %5"PRIu32, pos.altitude);
			color_printf(color, " [%4"PRIu32"]", pos.timestamp);
			color_printf(color, "\r\n");

		}

		csp_buffer_free(packet);
		if (list->end_flag)
			break;

	}

	csp_close(conn);
	return 0;

}

int gatoss_plane_contacts(uint32_t icao24) {

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_PLANE_CONTACTS, 5000, CSP_O_NONE);
	if (conn == NULL)
		return -1;

	/* Prepare request */
	csp_packet_t * packet = csp_buffer_get(sizeof(struct gatoss_plane_contacts_req));
	packet->length = sizeof(struct gatoss_plane_contacts);
	struct gatoss_plane_contacts_req * req = (void *) packet->data;
	req->icao24 = csp_hton32(icao24);

	/* Send request */
	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -1;
	}

	/* Read replies */
	while ((packet = csp_read(conn, 5000)) != NULL) {

		struct gatoss_plane_contacts * reply = (void *) packet->data;
		printf("Received %"PRIu8" contact timestamps\r\n", reply->count);

		int i = 0;
		for (i = 0; i < reply->count; i++) {
			int color = COLOR_GREEN;
			color_printf(color, " Time: %u", csp_ntoh32(reply->abs_time) + csp_ntoh16(reply->delta_time[i]));
			color_printf(color, "\r\n");
		}

		csp_buffer_free(packet);
		if (reply->end_flag)
			break;

	}

	csp_close(conn);
	return 0;

}

int gatoss_file_plane(uint32_t icao24, uint8_t format, uint32_t begin_time, uint32_t end_time, uint32_t max_size, char * path, struct gatoss_file_plane_reply * reply) {

	struct gatoss_file_plane_req req;
	req.format = format;
	req.icao24 = csp_hton32(icao24);
	req.begin_time = csp_hton32(begin_time);
	req.end_time = csp_hton32(end_time);
	req.max_size = csp_hton32(max_size);
	strncpy(req.path, path, 50);

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_FILE_PLANE, 5000, &req, sizeof(req), reply, sizeof(*reply));
	if (ret != sizeof(struct gatoss_file_plane_reply))
		return -2;

	reply->mem_addr = csp_ntoh32(reply->mem_addr);
	reply->mem_size = csp_ntoh32(reply->mem_size);
	return 0;

}

int gatoss_file_all_frames(uint32_t start_time, uint32_t run_duration, uint32_t max_size, char * path) {

	struct gatoss_file_all_frames_req req;
	req.start_time = csp_hton32(start_time);
	req.run_duration = csp_hton32(run_duration);
	req.max_size = csp_hton32(max_size);
	strncpy(req.path, path, 50);

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_FILE_ALL_FRAMES, 5000, &req, sizeof(req), &reply, sizeof(char));
	if (ret != sizeof(char))
		return -2;
	else
		return reply;

}

int gatoss_file_all_positions(uint32_t start_time, uint32_t run_duration, uint32_t max_size, char * path) {

	struct gatoss_file_all_frames_req req;
	req.start_time = csp_hton32(start_time);
	req.run_duration = csp_hton32(run_duration);
	req.max_size = csp_hton32(max_size);
	strncpy(req.path, path, 50);

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_FILE_ALL_POSITIONS, 5000, &req, sizeof(req), &reply, sizeof(char));
	if (ret != sizeof(char))
		return -2;
	else
		return reply;

}

int gatoss_file_raw_samples(uint8_t noise_sample, uint32_t timeout, uint32_t max_count, int32_t threshold, char * path) {

	struct gatoss_file_raw_samples_req req;
	req.noise_sample = noise_sample;
	req.timeout = csp_hton32(timeout);
	req.max_count = csp_hton32(max_count);
	req.threshold = csp_hton32(threshold);
	strncpy(req.path, path, 50);

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_FILE_RAW_SAMPLES, 5000, &req, sizeof(req), &reply, sizeof(char));
	if (ret != sizeof(char))
		return -2;
	else
		return reply;

}

int gatoss_conf_set(gatoss_conf_t * config) {

	config->fpga_threshold_a = csp_hton32(config->fpga_threshold_a);
	config->fpga_threshold_b = csp_hton32(config->fpga_threshold_b);
	config->fpga_adc_switch_interval = csp_hton16(config->fpga_adc_switch_interval);
	config->passive_inhibit = csp_hton16(config->passive_inhibit);
	config->passive_interval_list_short = csp_hton16(config->passive_interval_list_short);
	config->passive_interval_list_full = csp_hton16(config->passive_interval_list_full);
	config->passive_interval_plane = csp_hton16(config->passive_interval_plane);
	config->passive_live_min_tx_interval = csp_hton16(config->passive_live_min_tx_interval);
	config->passive_live_interval = csp_hton16(config->passive_live_interval);
	config->passive_filter_list_short = csp_hton16(config->passive_filter_list_short);
	config->passive_filter_list_full = csp_hton16(config->passive_filter_list_full);
	config->garbage_collector_interval = csp_hton16(config->garbage_collector_interval);
	config->garbage_plane_inactive_interval = csp_hton16(config->garbage_plane_inactive_interval);
	config->garbage_plane_no_lock_inactive_interval = csp_hton16(config->garbage_plane_no_lock_inactive_interval);
	config->garbage_plane_frame_count = csp_hton16(config->garbage_plane_frame_count);
	config->garbage_plane_frame_interval = csp_hton16(config->garbage_plane_frame_interval);
	config->garbage_pos_lifetime = csp_hton16(config->garbage_pos_lifetime);
	config->garbage_frame_lifetime = csp_hton16(config->garbage_frame_lifetime);

	return csp_transaction(CSP_PRIO_HIGH, node_gatoss, GATOSS_PORT_CONF, 5000, (void *) config, sizeof(gatoss_conf_t), NULL, 0);
}

int gatoss_conf_get(gatoss_conf_t * config) {
	int status = csp_transaction(CSP_PRIO_HIGH, node_gatoss, GATOSS_PORT_CONF, 5000, NULL, 0, (void *) config, sizeof(gatoss_conf_t));
	if (status == 0)
		return 0;

	config->fpga_threshold_a = csp_ntoh32(config->fpga_threshold_a);
	config->fpga_threshold_b = csp_ntoh32(config->fpga_threshold_b);
	config->fpga_adc_switch_interval = csp_ntoh16(config->fpga_adc_switch_interval);
	config->passive_inhibit = csp_ntoh16(config->passive_inhibit);
	config->passive_interval_list_short = csp_ntoh16(config->passive_interval_list_short);
	config->passive_interval_list_full = csp_ntoh16(config->passive_interval_list_full);
	config->passive_interval_plane = csp_ntoh16(config->passive_interval_plane);
	config->passive_live_min_tx_interval = csp_ntoh16(config->passive_live_min_tx_interval);
	config->passive_live_interval = csp_ntoh16(config->passive_live_interval);
	config->passive_filter_list_short = csp_ntoh16(config->passive_filter_list_short);
	config->passive_filter_list_full = csp_ntoh16(config->passive_filter_list_full);
	config->garbage_collector_interval = csp_ntoh16(config->garbage_collector_interval);
	config->garbage_plane_inactive_interval = csp_ntoh16(config->garbage_plane_inactive_interval);
	config->garbage_plane_no_lock_inactive_interval = csp_ntoh16(config->garbage_plane_no_lock_inactive_interval);
	config->garbage_plane_frame_count = csp_ntoh16(config->garbage_plane_frame_count);
	config->garbage_plane_frame_interval = csp_ntoh16(config->garbage_plane_frame_interval);
	config->garbage_pos_lifetime = csp_ntoh16(config->garbage_pos_lifetime);
	config->garbage_frame_lifetime = csp_ntoh16(config->garbage_frame_lifetime);

	return status;
}

int gatoss_conf_restore(void) {
	return csp_transaction(CSP_PRIO_HIGH, node_gatoss, GATOSS_PORT_RESTORE, 0, NULL, 0, NULL, 0);
}

void gatoss_conf_print(gatoss_conf_t * gatoss_conf) {
	color_printf(COLOR_BLUE, "*** FPGA ***\r\n");
	printf("FPGA image: %u\r\n", gatoss_conf->fpga_image);
	printf("Health check enabled: %u\r\n", gatoss_conf->fpga_wdt_enable);
	printf("Threshold A: %i\r\n", (int)gatoss_conf->fpga_threshold_a);
	printf("Threshold B: %i\r\n", (int)gatoss_conf->fpga_threshold_b);
	printf("Threshold count: %i\r\n", (int)gatoss_conf->fpga_threshold_count);
	printf("ADC: %u\r\n", gatoss_conf->fpga_adc);
	printf("ADC switching interval: %u\r\n", gatoss_conf->fpga_adc_switch_interval);

	color_printf(COLOR_BLUE, "\r\n*** PASSIVE ***\r\n");
	printf("INHIBIT: %u sec\r\nSHORT: %u sec [%u]\r\nFULL: %u sec [%u]\r\nPLANE: %u sec\r\n",
			gatoss_conf->passive_inhibit,
			gatoss_conf->passive_interval_list_short, gatoss_conf->passive_filter_list_short,
			gatoss_conf->passive_interval_list_full, gatoss_conf->passive_filter_list_full,
			gatoss_conf->passive_interval_plane);
	printf("LIVE: interval %u sec, decimation step %u sec\r\n", gatoss_conf->passive_live_interval, gatoss_conf->passive_live_min_tx_interval);

	color_printf(COLOR_BLUE, "\r\n*** GARBAGE COLLECTOR ***\r\n");
	printf("GC interval: %u sec\r\nLocked plane inactive after: %u sec\r\nNot locked plane inactive after: %u sec\r\n",
			gatoss_conf->garbage_collector_interval,
			gatoss_conf->garbage_plane_inactive_interval,
			gatoss_conf->garbage_plane_no_lock_inactive_interval);
	printf("Del if frame count less than: %u after %u sec\r\n",
			gatoss_conf->garbage_plane_frame_count,
			gatoss_conf->garbage_plane_frame_interval);
	printf("Position lifetime: %u min, frame lifetime %u min\r\n",
			gatoss_conf->garbage_pos_lifetime,
			gatoss_conf->garbage_frame_lifetime);

	color_printf(COLOR_BLUE, "\r\n*** Database ***\r\n");
	printf("Save frames per plane: %u\r\n", gatoss_conf->plane_enable_frame_log);
	printf("Plane position log min interval: %u s\r\n", gatoss_conf->plane_position_log_interval);

	color_printf(COLOR_BLUE, "\r\n*** Other ***\r\n");
	printf("Enable SD-card: %u\r\n", gatoss_conf->sd_enable);

}

int gatoss_blob_ls(void) {

	unsigned int i;

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_BLOB_LS, 5000, CSP_O_NONE);
	if (conn == NULL)
		return -1;

	/* Send request */
	csp_packet_t * packet = csp_buffer_get(sizeof(uint32_t));
	packet->length = 0;

	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -1;
	}

	color_printf(COLOR_BLUE, "\t     Time\t      Addr\t     Used\t     Size\t Flags\tName\r\n");

	/* Wait for replies */
	while ((packet = csp_read(conn, 5000)) != NULL) {

		struct gatoss_blob_list * list = (void *) packet->data;

		for (i = 0; i < list->count; i++) {
			color_printf(COLOR_GREEN,
					"%"PRIu16"\t%9"PRIu32"\t0x%08"PRIX32"\t%9"PRIu32"\t%9"PRIu32"\t0x%04"PRIX16"\t%s\r\n",
					csp_ntoh16(list->blobs[i].id),
					csp_ntoh32(list->blobs[i].creation_time),
					csp_ntoh32(list->blobs[i].mem_addr),
					csp_ntoh32(list->blobs[i].mem_used),
					csp_ntoh32(list->blobs[i].mem_size),
					csp_ntoh16(list->blobs[i].user_flags),
					list->blobs[i].name);
		}

		csp_buffer_free(packet);
		if (list->end_flag)
			break;

	}

	csp_close(conn);
	return 0;

}

int gatoss_db_store(char * path) {

	struct gatoss_db_store_req req;
	req.type = 1;
	strncpy(req.path, path, 50);

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_DB_STORE, 5000, &req, sizeof(req), &reply, 1);
	if (ret != 1)
		return -2;

	return reply;

}

int gatoss_db_load(char * path) {

	struct gatoss_db_store_req req;
	req.type = 0;
	strncpy(req.path, path, 50);

	char reply;

	int ret = csp_transaction(CSP_PRIO_NORM, node_gatoss, GATOSS_PORT_DB_STORE, 5000, &req, sizeof(req), &reply, 1);
	if (ret != 1)
		return -2;

	return reply;

}
