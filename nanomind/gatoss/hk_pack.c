/**
 * Housekeeping pack
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include <csp/csp_endian.h>
#include <util/byteorder.h>

#include <io/cdh.h>

#include "hk_collect.h"
#include "hk_store.h"

static cdh_beacon beacon[CDH_HK_BEACON_TYPES];

static void hk_pack_basic(hk_cache_t * hk, cdh_beacon * beacon, uint8_t flags) {
	beacon->beacon_time = csp_hton32(hk->time_sec);
	beacon->beacon_flags = flags;
}

static void hk_pack_beacon_a(hk_cache_t * hk, cdh_beacon * beacon, uint8_t flags) {

	/* Basic */
	hk_pack_basic(hk, beacon, flags);

	/* OBC */
	beacon->a.obc.bootcount = csp_hton16((uint16_t) hk->obc.bootcount);
	beacon->a.obc.temp1 = csp_hton16(hk->obc.temp1);
	beacon->a.obc.temp2 = csp_hton16(hk->obc.temp2);
	beacon->a.obc.panel_temp[0] = csp_hton16(hk->obc.panel_temp[0]);
	beacon->a.obc.panel_temp[1] = csp_hton16(hk->obc.panel_temp[1]);
	beacon->a.obc.panel_temp[2] = csp_hton16(hk->obc.panel_temp[2]);
	beacon->a.obc.panel_temp[3] = csp_hton16(hk->obc.panel_temp[3]);
	beacon->a.obc.panel_temp[4] = csp_hton16(hk->obc.panel_temp[4]);
	beacon->a.obc.panel_temp[5] = csp_hton16(hk->obc.panel_temp[5]);

	/* COM */
	beacon->a.com.byte_corr_tot = csp_hton16(hk->com.byte_corr_tot);
	beacon->a.com.rx = csp_hton16(hk->com.rx);
	beacon->a.com.rx_err = csp_hton16(hk->com.rx_err);
	beacon->a.com.tx = csp_hton16(hk->com.tx);
	beacon->a.com.last_temp_a = csp_hton16(hk->com.last_temp_a);
	beacon->a.com.last_temp_b = csp_hton16(hk->com.last_temp_b);
	beacon->a.com.last_rssi = csp_hton16(hk->com.last_rssi);
	beacon->a.com.last_rferr = csp_hton16(hk->com.last_rferr);
	beacon->a.com.last_batt_volt = csp_hton16(hk->com.last_batt_volt);
	beacon->a.com.last_txcurrent = csp_hton16(hk->com.last_txcurrent);
	beacon->a.com.bootcount = csp_hton16(hk->com.bootcount);

	/* EPS */
	for (int i = 0; i < 3; i++)
		beacon->a.eps.vboost[i] = csp_hton16(hk->eps.vboost[i]);
	beacon->a.eps.vbatt = csp_hton16(hk->eps.vbatt);
	for (int i = 0; i < 6; i++)
		beacon->a.eps.curout[i] = csp_hton16(hk->eps.curout[i]);
	for (int i = 0; i < 3; i++)
		beacon->a.eps.curin[i] = csp_hton16(hk->eps.curin[i]);
	beacon->a.eps.cursun = csp_hton16(hk->eps.cursun);
	beacon->a.eps.cursys = csp_hton16(hk->eps.cursys);
	for (int i = 0; i < 6; i++)
		beacon->a.eps.temp[i] = csp_hton16(hk->eps.temp[i]);
	beacon->a.eps.output = 0;
	for (int i = 0; i < 8; i++)
		beacon->a.eps.output |= hk->eps.output[i] << i;
	beacon->a.eps.counter_boot = csp_hton16((uint16_t) hk->eps.counter_boot);
	beacon->a.eps.counter_wdt_i2c = csp_hton16((uint16_t) hk->eps.counter_wdt_i2c);
	beacon->a.eps.counter_wdt_gnd = csp_hton16((uint16_t) hk->eps.counter_wdt_gnd);
	beacon->a.eps.bootcause = hk->eps.bootcause;
	for (int i = 0; i < 6; i++)
		beacon->a.eps.latchup[i] = csp_hton32(hk->eps.latchup[i]);
	beacon->a.eps.battmode = hk->eps.battmode;

	/* GATOSS */
	beacon->a.gatoss.average_fps_5min = csp_hton16(hk->gatoss.average_fps_5min);
	beacon->a.gatoss.average_fps_1min = csp_hton16(hk->gatoss.average_fps_1min);
	beacon->a.gatoss.average_fps_10sec = csp_hton16(hk->gatoss.average_fps_10sec);
	beacon->a.gatoss.plane_count = csp_hton16(hk->gatoss.plane_count);
	beacon->a.gatoss.frame_count = csp_hton32(hk->gatoss.frame_count);
	beacon->a.gatoss.crc_corrected = csp_hton32(hk->gatoss.crc_corrected);
	beacon->a.gatoss.last_icao = csp_hton32(hk->gatoss.last_icao24);
	beacon->a.gatoss.last_timestamp = csp_hton32(hk->gatoss.last_pos.timestamp);
	beacon->a.gatoss.last_altitude = csp_hton32(hk->gatoss.last_pos.altitude);
	beacon->a.gatoss.last_lat = util_htonflt(hk->gatoss.last_pos.lat);
	beacon->a.gatoss.last_lon = util_htonflt(hk->gatoss.last_pos.lon);
	beacon->a.gatoss.bootcount = csp_hton16((uint16_t) hk->gatoss.bootcount);
	beacon->a.gatoss.bootcause = csp_hton16(hk->gatoss.bootcause);

	/* HUB */
	beacon->a.hub.temp = (int8_t) hk->nanohub.temp;
	beacon->a.hub.bootcount = csp_hton16((uint16_t) hk->nanohub.bootcount);
	beacon->a.hub.reset = hk->nanohub.reset;
	beacon->a.hub.sense_status = hk->nanohub_knives.sense_status;
	beacon->a.hub.burns[0] = csp_hton16(hk->nanohub_knives.knife[1].burns[1]);
	beacon->a.hub.burns[1] = csp_hton16(hk->nanohub_knives.knife[1].burns[0]);

	/* ADCS */
#ifdef WITH_ADCS
	beacon->a.adcs.status = hk->adcs_bdot.magvalid << 1 | hk->adcs_bdot.detumbled;
	beacon->a.adcs.torquerduty[0] = util_htonflt(hk->adcs_bdot.torquerduty[0]);
	beacon->a.adcs.torquerduty[1] = util_htonflt(hk->adcs_bdot.torquerduty[1]);
	beacon->a.adcs.torquerduty[2] = util_htonflt(hk->adcs_bdot.torquerduty[2]);
	beacon->a.adcs.mag[0] = util_htonflt(hk->adcs_bdot.mag[0]);
	beacon->a.adcs.mag[1] = util_htonflt(hk->adcs_bdot.mag[1]);
	beacon->a.adcs.mag[2] = util_htonflt(hk->adcs_bdot.mag[2]);
	beacon->a.adcs.tumblenorm[0] = util_htonflt(hk->adcs_bdot.tumblenorm[0]);
	beacon->a.adcs.tumblenorm[1] = util_htonflt(hk->adcs_bdot.tumblenorm[1]);
	beacon->a.adcs.tumblerate[0] = util_htonflt(hk->adcs_bdot.tumblerate[0]);
	beacon->a.adcs.tumblerate[1] = util_htonflt(hk->adcs_bdot.tumblerate[1]);
	beacon->a.adcs.tumblerate[2] = util_htonflt(hk->adcs_bdot.tumblerate[2]);

	/* Pack states */
	beacon->a.adcs.ads = hk->adcs_state.determinationMode << 4 | hk->adcs_state.desiredDeterminationMode;
	beacon->a.adcs.acs = hk->adcs_state.controlMode << 4 | hk->adcs_state.desiredControlMode;

	/* Pack sunsensor data */
	beacon->a.adcs.sunsensor_packed[0] = hk->adcs_sun.sunsensor[0] & 0xff;
	beacon->a.adcs.sunsensor_packed[1] = hk->adcs_sun.sunsensor[1] & 0xff;
	beacon->a.adcs.sunsensor_packed[2] = hk->adcs_sun.sunsensor[3] & 0xff;
	beacon->a.adcs.sunsensor_packed[3] = hk->adcs_sun.sunsensor[4] & 0xff;
	beacon->a.adcs.sunsensor_packed[4] = hk->adcs_sun.sunsensor[5] & 0xff;
	beacon->a.adcs.sunsensor_packed[5] = ((hk->adcs_sun.sunsensor[0] >> 8) & 0xf) | ((hk->adcs_sun.sunsensor[1] >> 4) & 0xf0);
	beacon->a.adcs.sunsensor_packed[6] = ((hk->adcs_sun.sunsensor[3] >> 8) & 0xf) | ((hk->adcs_sun.sunsensor[4] >> 4) & 0xf0);
	beacon->a.adcs.sunsensor_packed[7] = ((hk->adcs_sun.sunsensor[5] >> 8) & 0xf);

#endif

}

static void hk_pack_beacon_b(hk_cache_t * hk, cdh_beacon * beacon, uint8_t flags) {
	int i;
	/* Basic */
	hk_pack_basic(hk, beacon, flags);


    beacon->b.sun[0] = csp_hton16(hk->adcs_sun.sunsensor[0]);
    beacon->b.sun[1] = csp_hton16(hk->adcs_sun.sunsensor[1]);
    beacon->b.sun[2] = csp_hton16(hk->adcs_sun.sunsensor[3]);
    beacon->b.sun[3] = csp_hton16(hk->adcs_sun.sunsensor[4]);
    beacon->b.sun[4] = csp_hton16(hk->adcs_sun.sunsensor[5]);
    beacon->b.ineclipse = (uint8_t) hk->adcs_kf_data.ineclipse;
    for (i=0; i<16; i++)
    	beacon->b.xest[i] = util_htonflt(hk->adcs_kf_state.state[i]);
    for (i=0; i<9; i++)
    	beacon->b.zfilt[i] = util_htonflt(hk->adcs_kf_filtmeas.filtmeasurements[i]);
	beacon->b.enable = 0;
    for (i=0;i<12;i++)
    	beacon->b.enable |=  (hk->adcs_kf_meas.enabled[i] << i);
    beacon->b.enable = csp_hton16(beacon->b.enable);
    beacon->b.ref_q[0] = util_htonflt(hk->adcs_ctrl_data.ref_q.q1);
    beacon->b.ref_q[1] = util_htonflt(hk->adcs_ctrl_data.ref_q.q2);
    beacon->b.ref_q[2] = util_htonflt(hk->adcs_ctrl_data.ref_q.q3);
    beacon->b.ref_q[3] = util_htonflt(hk->adcs_ctrl_data.ref_q.q4);
    beacon->b.err_q[0] = util_htonflt(hk->adcs_ctrl_data.err_q.q1);
    beacon->b.err_q[1] = util_htonflt(hk->adcs_ctrl_data.err_q.q2);
    beacon->b.err_q[2] = util_htonflt(hk->adcs_ctrl_data.err_q.q3);
    beacon->b.err_q[3] = util_htonflt(hk->adcs_ctrl_data.err_q.q4);
    beacon->b.ierr_q[0] = util_htonflt(hk->adcs_ctrl_data.ierr_q.q1);
    beacon->b.ierr_q[1] = util_htonflt(hk->adcs_ctrl_data.ierr_q.q2);
    beacon->b.ierr_q[2] = util_htonflt(hk->adcs_ctrl_data.ierr_q.q3);
    beacon->b.err_rate[0] = util_htonflt(hk->adcs_ctrl_data.err_rate[0]);
    beacon->b.err_rate[1] = util_htonflt(hk->adcs_ctrl_data.err_rate[1]);
    beacon->b.err_rate[2] = util_htonflt(hk->adcs_ctrl_data.err_rate[2]);
    beacon->b.sc_reci[0] = util_htonflt(hk->adcs_ephem_scpos.scpos[0]);
    beacon->b.sc_reci[1] = util_htonflt(hk->adcs_ephem_scpos.scpos[1]);
    beacon->b.sc_reci[2] = util_htonflt(hk->adcs_ephem_scpos.scpos[2]);
    beacon->b.sun_eci[0] = util_htonflt(hk->adcs_ephem_sunpos.sunpos[0]);
    beacon->b.sun_eci[1] = util_htonflt(hk->adcs_ephem_sunpos.sunpos[1]);
    beacon->b.sun_eci[2] = util_htonflt(hk->adcs_ephem_sunpos.sunpos[2]);
    beacon->b.mag_eci[0] = util_htonflt(hk->adcs_ephem_mag.mag[0]);
    beacon->b.mag_eci[1] = util_htonflt(hk->adcs_ephem_mag.mag[1]);
    beacon->b.mag_eci[2] = util_htonflt(hk->adcs_ephem_mag.mag[2]);
}

void hk_pack(hk_cache_t *data) {

	/* Update beacon_a data */
	hk_pack_beacon_a(data, &beacon[0], CDH_HK_BEACON_A | data->ping_res << 4);
	hk_pack_beacon_b(data, &beacon[1], CDH_HK_BEACON_B | data->ping_res << 4);

}

cdh_beacon * hk_pack_get(uint8_t beacon_type) {
	if (beacon_type > CDH_HK_BEACON_TYPES - 1)
		return NULL;
	return &beacon[beacon_type];
}
