/*
 * client_adcs.h
 *
 *  Created on: Mar 24, 2011
 *      Author: jal
 */

#ifndef CLIENT_ADCS_H_
#include <adcs/adcs_types.h>


void adcs_node_set(uint8_t node);

int adcs_cmd(adcs_server_cmd_t cmd);


/* General cmd's */
int adcs_start(void);
int adcs_stop(void);
int adcs_restart(void);
int adcs_pause(void);
int adcs_get_adcs_state(GS_ADCS_Mode_t *mode);
int adcs_set_ads_state(DeterminationMode_t state);
int adcs_set_acs_state(ControlMode_t mode);
int adcs_get_adcs_status(GS_ADCS_Status_t *status);
int adcs_get_time(double *jDate, bool_t *valid, unsigned int *looptime, unsigned int *maxlooptime);
int adcs_log_req(adcs_log_req_t logreq);
int adcs_log_abort(void);
int adcs_log_status(adcs_log_status_t *status);

/* Parameter commands */
int adcs_param_get_all(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_sc(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_sv(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_bdot(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_kf(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_ctrl1(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_ctrl2(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_sensor(GS_ADCS_Config_t *adcs_config);
int adcs_param_get_act(GS_ADCS_Config_t *adcs_config);

int adcs_param_getwork(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_all(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_sc(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_sv(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_bdot(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_kf(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_ctrl1(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_ctrl2(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_sensor(GS_ADCS_Config_t *adcs_config);
int adcs_param_set_act(GS_ADCS_Config_t *adcs_config);

/* BDOT commands */
int adcs_bdot_get_data(adcs_bdot_data_t *value);
int adcs_bdot_force_detumbled(void);

/* Actuator commands */
int adcs_act_get_torquers(adcs_act_get_torquers_t *value);

/* Sensor commands */
int adcs_sensor_init(void);
int adcs_sensor_mag_get(adcs_sensor_get_magnetometer_t *value);
int adcs_sensor_sun_get(adcs_sensor_get_sunsensor_t *value);
int adcs_sensor_gyro_get(adcs_sensor_get_gyro_t *value);
int adcs_sensor_mag_get_raw(adcs_sensor_get_magnetometer_t *value);
int adcs_sensor_sun_get_raw(adcs_sensor_get_sunsensor_t *value);
int adcs_sensor_gyro_get_raw(adcs_sensor_get_gyro_t *value);
int adcs_sensor_temp_get(adcs_sensor_get_temperature_t *value);

/* Matrix commands */
int adcs_get_matrix( adcs_matrix_type_t type, float *matrix, int *rows, int *cols);

/* UKF commands */
int adcs_ukf_get_data(adcs_ukf_get_data_t *data);
int adcs_ukf_get_measurements(adcs_ukf_get_measurements_t *measurements);
int adcs_ukf_get_state(adcs_ukf_get_state_t *state);
int adcs_ukf_get_filtmeasurements(adcs_ukf_get_filtmeasurements_t *measurements);

/* Ephemeris commands */
int adcs_ephem_get_tle(GS_ADCS_TLE_t *value);
int adcs_ephem_set_tle(GS_ADCS_TLE_t value);
int adcs_ephem_get_scpos(adcs_ephem_scpos_t *value);
int adcs_ephem_get_scvel(adcs_ephem_scvel_t *value);
int adcs_ephem_get_sunpos(adcs_ephem_sunpos_t *value);
int adcs_ephem_get_mag(adcs_ephem_mag_t *value);
int adcs_ephem_get_qio(adcs_ephem_q_i_o_t *value);
int adcs_ephem_get_qie(adcs_ephem_q_i_e_t *value);
int adcs_ephem_get_eclipsetime(adcs_ephem_eclipsetime_t *value);

/* Ctrl commands */
int adcs_ctrl_reset_int(void);
int adcs_ctrl_get_data(GS_ADCS_LQR_t *data);
int adcs_ctrl_get_flipdata(GS_ADCS_flip_t *data);


#define CLIENT_ADCS_H_

#endif /* CLIENT_ADCS_H_ */
