/**
 * Housekeeping Collector
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <conf_nanomind.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <util/clock.h>
#include <util/timestamp.h>
#include <util/log.h>
#include <csp/csp.h>

#include <io/nanocom.h>
#include <io/nanopower2.h>
#include <io/gatoss.h>
#include <io/nanohub.h>
#include <io/cdh.h>
#include <sns/sns.h>
#ifdef WITH_ADCS
#include <adcs/adcs_types.h>
#include <adcs/adcs_client.h>
#include <GS_ADCS.h>
#include <GS_ADCS_UKF.h>
#include <GS_ADCS_BDOT.h>
#endif
#include "obc_hk.h"

#include "hk_collect.h"
#include "hk_pack.h"
#include "config.h"

static hk_cache_t hk_cache;

hk_cache_t * hk_collect(void) {

	timestamp_t time;
	clock_get_time(&time);
	hk_cache.time_sec = time.tv_sec;
	hk_cache.ping_res = 0;

	/* Get OBC Housekeeping */
	obc_hk_get(&hk_cache.obc);

	/* Get EPS Housekeeping */
	eps_set_node(SNS("eps"));
	if (eps_hk_get(&hk_cache.eps) == sizeof(eps_hk_t)) {
		hk_cache.ping_res |= (1 << 0);
	} else {
		log_count("HK_NO_EPS");
	}

	/* Get COM Housekeeping */
	if (hk_cache.eps.output[3] == 1) {
		if (com_get_status(&hk_cache.com, SNS("com"), 250) == sizeof(nanocom_data_t)) {
			hk_cache.ping_res |= (1 << 1);
		} else {
			log_count("HK_NO_COM");
		}
	}

	/* Get NANOHUB Housekeeping */
	if (hk_cache.eps.output[4] == 1) {
		if (hub_get_hk(&hk_cache.nanohub) == sizeof(nanohub_hk_t)) {
			hub_knife(99, 0, 0, 0, &hk_cache.nanohub_knives);
			hk_cache.ping_res |= (1 << 2);
		} else {
			log_count("HK_NO_HUB");
		}
	}

	/* Get GATOSS Housekeeping */
	if (hk_cache.eps.output[0] == 1) {
		if (gatoss_hk(&hk_cache.gatoss, 0) == sizeof(struct gatoss_hk)) {
			hk_cache.ping_res |= (1 << 3);
		} else {
			log_count("HK_NO_GATOSS");
		}
	}

	/* Get ADCS Housekeeping */
#ifdef WITH_ADCS
	/**
	 * Yes, what you are about to see is global variables being used
	 * to get data from the innermost workings of ADCS.
	 * We bypass the CSP layer here only because:
	 *  1) We are pressed on CPU cycles, and any bit can help
	 *  2) We wish to reduce the collection time
	 *  3) We know what we are doing
	 * And Yes, there is a race condition on the floating point variables
	 * being accessed both from the ADCS task and the HK task, but
	 * since it's a read only, and we can filter out bad values on
	 * the GS, we don't care. We certainly would not like
	 * to block the ADCS task.
	 */
	extern GS_ADCS_Ephem_t ephemeris;
	extern GS_ADCS_LQR_t lqrdata;
	extern GS_ADCS_UKF_Data_t ukf_data;
	extern GS_ADCS_UKF_Measurement_t ukf_measurements;
	extern GS_ADCS_MeasurementsData_t rawDataPool;
	extern GS_ADCS_OutputData_t adcs_outputdata;
	extern GS_ADCS_BDOT_Data_t bdot_data;
	extern GS_ADCS_Mode_t adcs_mode;

	unsigned int i;

	/* ADCS_CMD_BDOT_GET_TUMB_RATE */
	for (i = 0; i < 3; i++)
		hk_cache.adcs_bdot.tumblerate[i] = adcs_outputdata.bdotRateVectorEstimate[i];
	for (i = 0; i < 2; i++)
		hk_cache.adcs_bdot.tumblenorm[i] = adcs_outputdata.bdotRateEstimate[i];
	for (i = 0; i < 3; i++)
		hk_cache.adcs_bdot.torquerduty[i] = adcs_outputdata.torquerDutyCycle[i];
	for (i = 0; i < 3; i++)
		hk_cache.adcs_bdot.mag[i] = rawDataPool.magnetometer[i];
	hk_cache.adcs_bdot.magvalid = bdot_data.magvalid;
	hk_cache.adcs_bdot.detumbled = bdot_data.Detumbled;

	/* ADCS_CMD_GET_ADCS_STATE */
	hk_cache.adcs_state.determinationMode = adcs_mode.determinationMode;
	hk_cache.adcs_state.desiredDeterminationMode = adcs_mode.desiredDeterminationMode;
	hk_cache.adcs_state.controlMode = adcs_mode.controlMode;
	hk_cache.adcs_state.desiredControlMode = adcs_mode.desiredControlMode;

	/* ADCS_CMD_SENSOR_SUN_GET */
	for (i = 0; i < 6; i++)
		hk_cache.adcs_sun.sunsensor[i] = (uint16_t)rawDataPool.sunsensor[i];

	/* Beacon B */
	if (hk_cache.adcs_state.determinationMode >= ADS_KALMANFILTER || config.force_beacon_b > 0) {

		/* ADCS_CMD_EPHEM_GET_SCVEL */
		for (i = 0; i < 3; i++)
			hk_cache.adcs_ephem_scpos.scpos[i] = ephemeris.scRECI[i];

		/* ADCS_CMD_EPHEM_GET_SUNPOS */
		for (i = 0; i < 3; i++)
			hk_cache.adcs_ephem_sunpos.sunpos[i] = ephemeris.sunECI[i];

		/* ADCS_CMD_EPHEM_GET_MAG */
		for (i = 0; i < 3; i++)
			hk_cache.adcs_ephem_mag.mag[i] = ephemeris.magECI[i];

		/* ADCS_CMD_CTRL_GET_DATA */
		memcpy(&hk_cache.adcs_ctrl_data, &lqrdata, sizeof(GS_ADCS_LQR_t));

		/* ADCS_CMD_UKF_GET_DATA */
		hk_cache.adcs_kf_data.ineclipse = ukf_data.InEclipse;
		hk_cache.adcs_kf_data.iterations = ukf_data.Iterations;
		hk_cache.adcs_kf_data.kf_converged = ukf_data.KFconverged;
		for (i = 0; i < 6; i++)
			hk_cache.adcs_kf_data.sunsensor_max[i] = ukf_data.sunsensorMax[i];
		hk_cache.adcs_kf_data.ts = ukf_data.Ts;

		/* ADCS_CMD_UKF_GET_FILTMEASUREMENTS */
		hk_cache.adcs_kf_filtmeas.dimensions = GS_ADCS_UKF_Get_NUM_Z();
		for (i = 0; i < GS_ADCS_UKF_Get_NUM_Z(); i++)
			hk_cache.adcs_kf_filtmeas.filtmeasurements[i] = ukf_measurements.MeasurementFilt[i];

		/* ADCS_CMD_UKF_GET_MEASUREMETS */
		hk_cache.adcs_kf_meas.dimensions = GS_ADCS_UKF_Get_NUM_SENS();
		for (i = 0; i < GS_ADCS_UKF_Get_NUM_SENS(); i++) {
			hk_cache.adcs_kf_meas.measurements[i] = ukf_measurements.Measurement[i];
			hk_cache.adcs_kf_meas.enabled[i] = ukf_measurements.Enable[i];
		}

		/* ADCS_CMD_UKF_GET_STATE */
		hk_cache.adcs_kf_state.dimensions = GS_ADCS_UKF_Get_NUM_X();
		for (i = 0; i < GS_ADCS_UKF_Get_NUM_X(); i++)
			hk_cache.adcs_kf_state.state[i] = ukf_data.EstimatedState[i];

	}

#endif

	return &hk_cache;

}

