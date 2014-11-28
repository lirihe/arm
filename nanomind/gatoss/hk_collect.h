/**
 * Header for Housekeeping Collector
 *
 * @author Morten Jensen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef TASK_HK_COLL
#define TASK_HK_COLL

#include <conf_nanomind.h>
#include <io/nanocom.h>
#include <io/nanopower2.h>
#include <io/gatoss.h>
#include <io/nanohub.h>
#include <io/cdh.h>
#ifdef WITH_ADCS
#include <adcs/adcs_types.h>
#endif

#include "obc_hk.h"

typedef struct {
	uint32_t 				time_sec;
	uint8_t					ping_res;					//! Ping result from pinging the different systems
	nanocom_data_t 			com;
	eps_hk_t				eps;
	gatoss_hk_t				gatoss;
	obc_hk_t				obc;
#ifdef WITH_ADCS
	adcs_bdot_data_t		adcs_bdot;
	GS_ADCS_Mode_t 			adcs_state;
#endif
	nanohub_hk_t			nanohub;
	nanohub_knivesdata_t 	nanohub_knives;
#ifdef WITH_ADCS
	/* beacon b */
	adcs_ephem_scpos_t adcs_ephem_scpos;
	adcs_ephem_sunpos_t adcs_ephem_sunpos;
	adcs_ephem_mag_t adcs_ephem_mag;
	GS_ADCS_LQR_t adcs_ctrl_data;
	adcs_ukf_get_filtmeasurements_t adcs_kf_filtmeas;
	adcs_ukf_get_measurements_t adcs_kf_meas;
	adcs_ukf_get_data_t adcs_kf_data;
	adcs_ukf_get_state_t adcs_kf_state;
	adcs_sensor_get_sunsensor_t adcs_sun;
#endif
} hk_cache_t;

/**
 * Collect housekeeping
 * @return a pointer to the updated housekeeping cache
 */
hk_cache_t * hk_collect(void);

#endif /* IO_H_ */
