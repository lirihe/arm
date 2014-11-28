/**
 * Gatoss specifc nanomind interface
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <inttypes.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <util/log.h>
#include <util/color_printf.h>
#include <util/byteorder.h>
#include <io/cdh.h>
#include <util/driver_debug.h>

static cdh_beacon_listener_param_t * param = NULL;

static uint8_t cdh_node = CDH_NODE;

void cdh_beacon_ntoh(cdh_beacon * beacon) {
	beacon->beacon_time = csp_ntoh32(beacon->beacon_time);

	/* Beacon A */
	if (beacon->beacon_flags & (1 << 0)) {

		beacon->a.obc.bootcount = csp_ntoh16(beacon->a.obc.bootcount);
		beacon->a.obc.temp1 = csp_ntoh16(beacon->a.obc.temp1);
		beacon->a.obc.temp2 = csp_ntoh16(beacon->a.obc.temp2);
		beacon->a.obc.panel_temp[0] = csp_ntoh16(beacon->a.obc.panel_temp[0]);
		beacon->a.obc.panel_temp[1] = csp_ntoh16(beacon->a.obc.panel_temp[1]);
		beacon->a.obc.panel_temp[2] = csp_ntoh16(beacon->a.obc.panel_temp[2]);
		beacon->a.obc.panel_temp[3] = csp_ntoh16(beacon->a.obc.panel_temp[3]);
		beacon->a.obc.panel_temp[4] = csp_ntoh16(beacon->a.obc.panel_temp[4]);
		beacon->a.obc.panel_temp[5] = csp_ntoh16(beacon->a.obc.panel_temp[5]);

		beacon->a.com.byte_corr_tot = csp_ntoh16(beacon->a.com.byte_corr_tot);
		beacon->a.com.rx = csp_ntoh16(beacon->a.com.rx);
		beacon->a.com.rx_err = csp_ntoh16(beacon->a.com.rx_err);
		beacon->a.com.tx = csp_ntoh16(beacon->a.com.tx);
		beacon->a.com.last_temp_a = csp_ntoh16(beacon->a.com.last_temp_a);
		beacon->a.com.last_temp_b = csp_ntoh16(beacon->a.com.last_temp_b);
		beacon->a.com.last_rssi = csp_ntoh16(beacon->a.com.last_rssi);
		beacon->a.com.last_rferr = csp_ntoh16(beacon->a.com.last_rferr);
		beacon->a.com.last_batt_volt = csp_ntoh16(beacon->a.com.last_batt_volt);
		beacon->a.com.last_txcurrent = csp_ntoh16(beacon->a.com.last_txcurrent);
		beacon->a.com.bootcount = csp_ntoh16(beacon->a.com.bootcount);

		for (int i = 0; i < 3; i++)
			beacon->a.eps.vboost[i] = csp_ntoh16(beacon->a.eps.vboost[i]);
		beacon->a.eps.vbatt = csp_ntoh16(beacon->a.eps.vbatt);
		for (int i = 0; i < 6; i++)
			beacon->a.eps.curout[i] = csp_ntoh16(beacon->a.eps.curout[i]);
		for (int i = 0; i < 3; i++)
			beacon->a.eps.curin[i] = csp_ntoh16(beacon->a.eps.curin[i]);
		beacon->a.eps.cursun = csp_ntoh16(beacon->a.eps.cursun);
		beacon->a.eps.cursys = csp_ntoh16(beacon->a.eps.cursys);
		for (int i = 0; i < 6; i++)
			beacon->a.eps.temp[i] = csp_ntoh16(beacon->a.eps.temp[i]);
		beacon->a.eps.counter_boot = csp_ntoh16(beacon->a.eps.counter_boot);
		beacon->a.eps.counter_wdt_i2c = csp_ntoh16(beacon->a.eps.counter_wdt_i2c);
		beacon->a.eps.counter_wdt_gnd = csp_ntoh16(beacon->a.eps.counter_wdt_gnd);
		beacon->a.eps.bootcause = beacon->a.eps.bootcause;
		for (int i = 0; i < 6; i++)
			beacon->a.eps.latchup[i] = csp_ntoh32(beacon->a.eps.latchup[i]);
		beacon->a.eps.battmode = beacon->a.eps.battmode;

		beacon->a.gatoss.average_fps_5min = csp_ntoh16(beacon->a.gatoss.average_fps_5min);
		beacon->a.gatoss.average_fps_1min = csp_ntoh16(beacon->a.gatoss.average_fps_1min);
		beacon->a.gatoss.average_fps_10sec = csp_ntoh16(beacon->a.gatoss.average_fps_10sec);
		beacon->a.gatoss.plane_count = csp_ntoh16(beacon->a.gatoss.plane_count);
		beacon->a.gatoss.frame_count = csp_ntoh32(beacon->a.gatoss.frame_count);
		beacon->a.gatoss.last_icao = csp_ntoh32(beacon->a.gatoss.last_icao);
		beacon->a.gatoss.last_altitude = csp_ntoh32(beacon->a.gatoss.last_altitude);
		beacon->a.gatoss.last_timestamp = csp_ntoh32(beacon->a.gatoss.last_timestamp);
		beacon->a.gatoss.last_lat = util_ntohflt(beacon->a.gatoss.last_lat);
		beacon->a.gatoss.last_lon = util_ntohflt(beacon->a.gatoss.last_lon);
		beacon->a.gatoss.crc_corrected = csp_ntoh32(beacon->a.gatoss.crc_corrected);
		beacon->a.gatoss.bootcount = csp_ntoh16(beacon->a.gatoss.bootcount);
		beacon->a.gatoss.bootcause = csp_ntoh16(beacon->a.gatoss.bootcause);

		beacon->a.hub.bootcount = csp_ntoh16(beacon->a.hub.bootcount);
		beacon->a.hub.burns[0] = csp_ntoh16(beacon->a.hub.burns[0]);
		beacon->a.hub.burns[1] = csp_ntoh16(beacon->a.hub.burns[1]);

		beacon->a.adcs.mag[0] = util_htonflt(beacon->a.adcs.mag[0]);
		beacon->a.adcs.mag[1] = util_htonflt(beacon->a.adcs.mag[1]);
		beacon->a.adcs.mag[2] = util_htonflt(beacon->a.adcs.mag[2]);
		beacon->a.adcs.tumblenorm[0] = util_htonflt(beacon->a.adcs.tumblenorm[0]);
		beacon->a.adcs.tumblenorm[1] = util_htonflt(beacon->a.adcs.tumblenorm[1]);
		beacon->a.adcs.tumblerate[0] = util_htonflt(beacon->a.adcs.tumblerate[0]);
		beacon->a.adcs.tumblerate[1] = util_htonflt(beacon->a.adcs.tumblerate[1]);
		beacon->a.adcs.tumblerate[2] = util_htonflt(beacon->a.adcs.tumblerate[2]);
		beacon->a.adcs.torquerduty[0] = util_htonflt(beacon->a.adcs.torquerduty[0]);
		beacon->a.adcs.torquerduty[1] = util_htonflt(beacon->a.adcs.torquerduty[1]);
		beacon->a.adcs.torquerduty[2] = util_htonflt(beacon->a.adcs.torquerduty[2]);

	/* Beacon B */
	} else if (beacon->beacon_flags & (1 << 1)) {
		int i;
	    beacon->b.sun[0] = csp_hton16(beacon->b.sun[0]);
	    beacon->b.sun[1] = csp_hton16(beacon->b.sun[1]);
	    beacon->b.sun[2] = csp_hton16(beacon->b.sun[2]);
	    beacon->b.sun[3] = csp_hton16(beacon->b.sun[3]);
	    beacon->b.sun[4] = csp_hton16(beacon->b.sun[4]);
	    beacon->b.ineclipse = beacon->b.ineclipse;
	    for (i=0; i<16; i++)
	    	beacon->b.xest[i] = util_htonflt(beacon->b.xest[i]);
	    for (i=0; i<9; i++)
	    	beacon->b.zfilt[i] = util_htonflt(beacon->b.zfilt[i]);
	    beacon->b.enable = csp_hton16(beacon->b.enable);
	    beacon->b.ref_q[0] = util_htonflt(beacon->b.ref_q[0]);
	    beacon->b.ref_q[1] = util_htonflt(beacon->b.ref_q[1]);
	    beacon->b.ref_q[2] = util_htonflt(beacon->b.ref_q[2]);
	    beacon->b.ref_q[3] = util_htonflt(beacon->b.ref_q[3]);
	    beacon->b.err_q[0] = util_htonflt(beacon->b.err_q[0]);
	    beacon->b.err_q[1] = util_htonflt(beacon->b.err_q[1]);
	    beacon->b.err_q[2] = util_htonflt(beacon->b.err_q[2]);
	    beacon->b.err_q[3] = util_htonflt(beacon->b.err_q[3]);
	    beacon->b.ierr_q[0] = util_htonflt(beacon->b.ierr_q[0]);
	    beacon->b.ierr_q[1] = util_htonflt(beacon->b.ierr_q[1]);
	    beacon->b.ierr_q[2] = util_htonflt(beacon->b.ierr_q[2]);
	    beacon->b.err_rate[0] = util_htonflt(beacon->b.err_rate[0]);
	    beacon->b.err_rate[1] = util_htonflt(beacon->b.err_rate[1]);
	    beacon->b.err_rate[2] = util_htonflt(beacon->b.err_rate[2]);
	    beacon->b.sc_reci[0] = util_htonflt(beacon->b.sc_reci[0]);
	    beacon->b.sc_reci[1] = util_htonflt(beacon->b.sc_reci[1]);
	    beacon->b.sc_reci[2] = util_htonflt(beacon->b.sc_reci[2]);
	    beacon->b.sun_eci[0] = util_htonflt(beacon->b.sun_eci[0]);
	    beacon->b.sun_eci[1] = util_htonflt(beacon->b.sun_eci[1]);
	    beacon->b.sun_eci[2] = util_htonflt(beacon->b.sun_eci[2]);
	    beacon->b.mag_eci[0] = util_htonflt(beacon->b.mag_eci[0]);
	    beacon->b.mag_eci[1] = util_htonflt(beacon->b.mag_eci[1]);
	    beacon->b.mag_eci[2] = util_htonflt(beacon->b.mag_eci[2]);
	}

}

void cdh_beacon_set_node(uint8_t node) {
	cdh_node = node;
}

void cdh_beacon_print(cdh_beacon * beacon) {
	color_printf(COLOR_BLUE, "\r\n");
	color_printf(COLOR_BLUE, "***********************\r\n");
	color_printf(COLOR_BLUE, "*       BEACON        *\r\n");
	color_printf(COLOR_BLUE, "***********************\r\n\r\n");

	color_printf(COLOR_GREEN, "Time: %"PRIu32", mask: %"PRIx8"\r\n", beacon->beacon_time, beacon->beacon_flags);

	/* Beacon A */
	if (beacon->beacon_flags & (1 << 0)) {

		color_printf(COLOR_YELLOW, "\r\nOBC:\r\n");
		color_printf(COLOR_GREEN, "Temp board: [%.1f, %.1f], Panels: [%.1f %.1f %.1f %.1f %.1f %.1f], Boot count: [%"PRIu16"]\r\n",
				beacon->a.obc.temp1 / 4.0,
				beacon->a.obc.temp2 / 4.0,
				beacon->a.obc.panel_temp[0] / 4.0,
				beacon->a.obc.panel_temp[1] / 4.0,
				beacon->a.obc.panel_temp[2] / 4.0,
				beacon->a.obc.panel_temp[3] / 4.0,
				beacon->a.obc.panel_temp[4] / 4.0,
				beacon->a.obc.panel_temp[5] / 4.0,
				beacon->a.obc.bootcount);

		color_printf(COLOR_YELLOW, "\r\nCOM:\r\n");
		color_printf(COLOR_GREEN, "RX: %"PRIu32" ok, %"PRIu32" err with [%"PRIu32" byte] corrected\r\n",
				beacon->a.com.rx,
				beacon->a.com.rx_err,
				beacon->a.com.byte_corr_tot);
		color_printf(COLOR_GREEN, "TX: %"PRIu32"\r\n",
				beacon->a.com.tx);
		color_printf(COLOR_GREEN, "RSSI: %"PRId16", RFerr: %"PRId16", Temp A: %"PRId16", B: %"PRId16", Vbat: %"PRIu16", TXcur: %"PRIu16", Boots: %"PRIu16"\r\n",
				beacon->a.com.last_rssi,
				beacon->a.com.last_rferr,
				beacon->a.com.last_temp_a,
				beacon->a.com.last_temp_b,
				beacon->a.com.last_batt_volt,
				beacon->a.com.last_txcurrent,
				beacon->a.com.bootcount);

		color_printf(COLOR_YELLOW, "\r\nEPS:\r\n");
		color_printf(COLOR_GREEN, "Vbat: %"PRIu16"mV, In: %"PRIu16"mA, Out %"PRIu16"mA\r\n",
				beacon->a.eps.vbatt,
				beacon->a.eps.cursun,
				beacon->a.eps.cursys);
		color_printf(COLOR_GREEN, "Panel voltage: [%"PRIu16"mV, %"PRIu16"mV, %"PRIu16"mV]\r\n",
				beacon->a.eps.vboost[0],
				beacon->a.eps.vboost[1],
				beacon->a.eps.vboost[2]);
		color_printf(COLOR_GREEN, "Temp: [C1: %"PRId16", C2: %"PRId16", C3: %"PRId16", Board: %"PRId16", Batt1: %"PRId16", Batt2: %"PRId16"]\r\n",
				beacon->a.eps.temp[0],
				beacon->a.eps.temp[1],
				beacon->a.eps.temp[2],
				beacon->a.eps.temp[3],
				beacon->a.eps.temp[4],
				beacon->a.eps.temp[5]);
		color_printf(COLOR_GREEN, "Latchup: [%"PRId16", %"PRId16", %"PRId16", %"PRId16", %"PRId16", %"PRId16"]\r\n",
				beacon->a.eps.latchup[0],
				beacon->a.eps.latchup[1],
				beacon->a.eps.latchup[2],
				beacon->a.eps.latchup[3],
				beacon->a.eps.latchup[4],
				beacon->a.eps.latchup[5]);
		color_printf(COLOR_GREEN, "boot count: %"PRIu16", cause: %"PRIu8"\r\n",
				beacon->a.eps.counter_boot,
				beacon->a.eps.bootcause);

		if ((beacon->beacon_flags & 0x80) > 0) {
			color_printf(COLOR_YELLOW, "\r\nGATOSS:\r\n");
			color_printf(COLOR_GREEN, "Last position: ICAO %"PRIx32", lat: %f, lon: %f, alt: %"PRIu32", time: %"PRIu32"\r\n",
					beacon->a.gatoss.last_icao,
					beacon->a.gatoss.last_lat,
					beacon->a.gatoss.last_lon,
					beacon->a.gatoss.last_altitude,
					beacon->a.gatoss.last_timestamp);
			color_printf(COLOR_GREEN, "FPS: %"PRIu16", %"PRIu16", %"PRIu16" , planes: %"PRIu16", frames: %"PRIu32", crc-corrected: %"PRIu32"\r\n",
					beacon->a.gatoss.average_fps_5min,
					beacon->a.gatoss.average_fps_1min,
					beacon->a.gatoss.average_fps_10sec,
					beacon->a.gatoss.plane_count,
					beacon->a.gatoss.frame_count,
					beacon->a.gatoss.crc_corrected);
			color_printf(COLOR_GREEN, "boot count: %"PRIu16", cause: %"PRIu16"\r\n",
					beacon->a.gatoss.bootcount,
					beacon->a.gatoss.bootcause);
		}

		color_printf(COLOR_YELLOW, "\r\nHUB:\r\n");
		color_printf(COLOR_GREEN, "temp: %"PRIi8", bootcount: %"PRIu16", bootcause: %"PRIu8"\r\n",
				beacon->a.hub.temp,
				beacon->a.hub.bootcount,
				beacon->a.hub.reset);
		color_printf(COLOR_GREEN, "Status Sense 0x%"PRIx8"\r\n",
				beacon->a.hub.sense_status);
		color_printf(COLOR_GREEN, "Burns [K1B1: %"PRIu16", K1B0: %"PRIu16"]\r\n",
				beacon->a.hub.burns[0],
				beacon->a.hub.burns[1]);

		color_printf(COLOR_YELLOW, "\r\nADCS:\r\n");
		color_printf(COLOR_GREEN, "Detumbled: %"PRIu8"\r\n", beacon->a.adcs.status & 1);
		color_printf(COLOR_GREEN, "Mag [0]: %f, [1]: %f, [2]: %f\r\n", beacon->a.adcs.mag[0], beacon->a.adcs.mag[1], beacon->a.adcs.mag[2]);
		color_printf(COLOR_GREEN, "Magvalid: %"PRIu8"\r\n", (beacon->a.adcs.status >> 1) & 1);
		color_printf(COLOR_GREEN, "Torquerduty [0]: %f, [1]: %f, [2]: %f\r\n", beacon->a.adcs.torquerduty[0], beacon->a.adcs.torquerduty[1], beacon->a.adcs.torquerduty[2]);
		color_printf(COLOR_GREEN, "Tumblenorm [0]: %f, [1]: %f\r\n", beacon->a.adcs.tumblenorm[0], beacon->a.adcs.tumblenorm[1]);
		color_printf(COLOR_GREEN, "Tumblerate [0]: %f, [1]: %f, [2]: %f\r\n", beacon->a.adcs.tumblerate[0], beacon->a.adcs.tumblerate[1], beacon->a.adcs.tumblerate[2]);
		color_printf(COLOR_GREEN, "ADS state %u desired %u\r\n", beacon->a.adcs.ads >> 4, beacon->a.adcs.ads & 0xf);
		color_printf(COLOR_GREEN, "ACS state %u desired %u\r\n", beacon->a.adcs.acs >> 4, beacon->a.adcs.acs & 0xf);

		/* Unpack sunsensors */
		uint16_t sunsensor[5];
		sunsensor[0] = beacon->a.adcs.sunsensor_packed[5] & 0xf;
		sunsensor[0] = (sunsensor[0] << 8) | beacon->a.adcs.sunsensor_packed[0];
		sunsensor[1] = (beacon->a.adcs.sunsensor_packed[5] >> 4) & 0xf;
		sunsensor[1] = (sunsensor[1] << 8) | beacon->a.adcs.sunsensor_packed[1];
		sunsensor[2] = beacon->a.adcs.sunsensor_packed[6] & 0xf;
		sunsensor[2] = (sunsensor[2] << 8) | beacon->a.adcs.sunsensor_packed[2];
		sunsensor[3] = (beacon->a.adcs.sunsensor_packed[6] >> 4) & 0xf;
		sunsensor[3] = (sunsensor[3] << 8) | beacon->a.adcs.sunsensor_packed[3];
		sunsensor[4] = beacon->a.adcs.sunsensor_packed[7] & 0xf;
		sunsensor[4] = (sunsensor[4] << 8) | beacon->a.adcs.sunsensor_packed[4];

		color_printf(COLOR_GREEN, "Sun sensor [+x +y -x -y -z]: %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16"\r\n",
				sunsensor[0], sunsensor[1], sunsensor[2], sunsensor[3],	sunsensor[4]);

	/* Beacon B */
	} else if (beacon->beacon_flags & (1 << 1)) {
		int i;
		printf("Beacon B\r\n");
	    printf("KF X: ");
	    		for (i=0;i<16;i++) printf("%f ",beacon->b.xest[i]);
	    printf("\r\n");
	    printf("KF Z: ");
	    		for (i=0;i<9;i++) printf("%f ",beacon->b.zfilt[i]);
	    printf("\r\n");
	    printf("KF E (%X): ",beacon->b.enable);
	    		for (i=0;i<12;i++) printf("%u ",(beacon->b.enable >> i) & 0x01 );
	    printf("\r\n");
	    printf("Sun sensor [+x +y -x -y -z]: %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16"\r\n",beacon->b.sun[0],beacon->b.sun[1],beacon->b.sun[2],beacon->b.sun[3],beacon->b.sun[4]);
	    printf("Eclipse: %"PRIu8"\r\n",beacon->b.ineclipse);
	    printf("ref q: ");
	    		for (i=0;i<4;i++) printf("%f ",beacon->b.ref_q[i]);
	    printf("\r\n");
	    printf("err q: ");
	    		for (i=0;i<4;i++) printf("%f ",beacon->b.err_q[i]);
	    printf("\r\n");
	    printf("ierr q: ");
	    		for (i=0;i<3;i++) printf("%f ",beacon->b.ierr_q[i]);
	    printf("\r\n");
	    printf("err_rate: ");
	    		for (i=0;i<3;i++) printf("%f ",beacon->b.err_rate[i]);
	    printf("\r\n");
	    printf("sc_reci q: ");
	    		for (i=0;i<3;i++) printf("%f ",beacon->b.sc_reci[i]);
	    printf("\r\n");
	    printf("sun_eci q: ");
	    		for (i=0;i<3;i++) printf("%f ",beacon->b.sun_eci[i]);
	    printf("\r\n");
	    printf("mag_eci: ");
	    		for (i=0;i<3;i++) printf("%f ",beacon->b.mag_eci[i]);
	    printf("\r\n");
	}

}

int cdh_beacon_get(uint32_t from, uint32_t to, uint16_t min_interval, uint16_t max_count, uint8_t mask, uint32_t timeout) {

	struct cdh_beacon_req req;
	req.from = csp_hton32(from);
	req.to = csp_hton32(to);
	req.min_interval = csp_hton16(min_interval);
	req.max_count = csp_hton16(max_count);
	req.mask = mask;

	uint32_t opt = CSP_O_NONE;
	if (timeout == 999999) {
		opt = CSP_O_RDP;
		timeout = 10000;
	}

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, cdh_node, CDH_PORT_BEACON_LOG, timeout, opt);
	if (conn == NULL)
		return -1;

	/* Prepare request */
	csp_packet_t * packet = csp_buffer_get(sizeof(req));
	if (packet == NULL)
		return -1;

	/* Copy request to packet */
	packet->length = sizeof(req);
	memcpy(packet->data, &req, sizeof(req));

	/* Send request */
	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -1;
	}

	/* Wait for replies */
	while ((packet = csp_read(conn, timeout)) != NULL) {

		cdh_beacon * beacon = (void *) packet->data;

		cdh_beacon_ntoh(beacon);

		if (driver_debug_enabled(DEBUG_OBC_HK_CLIENT_PRINT))
			cdh_beacon_print(beacon);
		else
			printf("Beacon %"PRIu32"\r\n", beacon->beacon_time);

		if (param != NULL && param->callback != NULL)
			(*param->callback)(packet);

		csp_buffer_free(packet);

	}

	csp_close(conn);
	return 0;
}


int cdh_beacon_store(char * path, uint32_t from, uint32_t to, uint32_t interval) {

	/* Prepare request */
	struct cdh_beacon_store req;
	req.from = csp_hton32(from);
	req.to = csp_hton32(to);
	req.interval = csp_hton32(interval);
	strncpy(req.path, path, 50);

	printf("Requesting store from %"PRIu32" to %"PRIu32" path %s\r\n", req.from, req.to, req.path);

	int8_t reply;
	if (csp_transaction(CSP_PRIO_HIGH, cdh_node, CDH_PORT_BEACON_STORE, 10000, (void *) &req, sizeof(req), &reply, 1) != sizeof(reply))
		reply = -2;

	return reply;
}


void cdh_beacon_task(void * param_arg) {

	param = param_arg;
	csp_packet_t * packet = NULL;
	csp_socket_t * socket = csp_socket(CSP_SO_CONN_LESS);
	if (socket == NULL)
		return;
	if (csp_bind(socket, param->port) != CSP_ERR_NONE) {
		csp_close(socket);
		return;
	}

	while(1) {

		packet = csp_recvfrom(socket, CSP_MAX_DELAY);
		if (packet == NULL)
			continue;

		cdh_beacon * beacon = (cdh_beacon *) packet->data;
		cdh_beacon_ntoh(beacon);

		if (param->callback != NULL)
			(*param->callback)(packet);

		if (driver_debug_enabled(DEBUG_OBC_HK_CLIENT_PRINT))
			cdh_beacon_print(beacon);

		csp_buffer_free(packet);

	}


}
