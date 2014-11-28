/*
 * magnetometer.h
 *
 *  Created on: Aug 31, 2009
 *      Author: karl
 */

#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

#include <command/command.h>

/**
 * @file
 * @brief Driver interface for magnetometer
 * \addtogroup Magnetometer
 *
 * @{
 */

/**
 * Rates
 */
typedef enum mag_rate_e {
    MAG_RATE_0_5 = 0,//!< MAG_RATE_0_5
    MAG_RATE_1   = 1,//!< MAG_RATE_1
    MAG_RATE_2   = 2,//!< MAG_RATE_2
    MAG_RATE_5   = 3,//!< MAG_RATE_5
    MAG_RATE_10  = 4,//!< MAG_RATE_10
    MAG_RATE_20  = 5,//!< MAG_RATE_20
    MAG_RATE_50  = 6,//!< MAG_RATE_50
} mag_rate_t;

/**
 * Measurement modes
 */
typedef enum mag_meas_e {
    MAG_MEAS_NORM = 0,//!< MAG_MEAS_NORM
    MAG_MEAS_POS  = 1,//!< MAG_MEAS_POS
    MAG_MEAS_NEG  = 2 //!< MAG_MEAS_NEG
} mag_meas_t;


/**
 * Gain settings
 */
typedef enum mag_gain_e {
	MAG_GAIN_0_7  = 0,//!< MAG_GAIN_0_7 (max value is 0.7 G)
	MAG_GAIN_1_0  = 1,//!< MAG_GAIN_1_0 (max value is 1.0 G)
	MAG_GAIN_1_5  = 2,//!< MAG_GAIN_1_5 (max value is 1.5 G)
	MAG_GAIN_2_0  = 3,//!< MAG_GAIN_2_0 (max value is 2.0 G)
	MAG_GAIN_3_2  = 4,//!< MAG_GAIN_3_2 (max value is 3.2 G)
	MAG_GAIN_3_8  = 5,//!< MAG_GAIN_3_8 (max value is 3.8 G)
	MAG_GAIN_4_5  = 6,//!< MAG_GAIN_4_5 (max value is 4.5 G)
	MAG_GAIN_6_5  = 7 //!< MAG_GAIN_6_5 (max value is 6.5 G)
} mag_gain_t;

/**
 * Mode Settings
 */
typedef enum mag_mode_e {
	MAG_MODE_CONTINUOUS = 0,
	MAG_MODE_SINGLE     = 1,
	MAG_MODE_IDLE       = 2,
	MAG_MODE_SLEEP      = 3
} mag_mode_t;

/**
 * Set rate of magnetometer, use the mag_rate_t enum type
 * @param rate
 * @param meas
 * @param gain
 * @return Error code E_NO_ERR if set is successful.
 */
int mag_set_conf(mag_rate_t new_rate, mag_meas_t new_meas, mag_gain_t new_gain);
mag_gain_t mag_get_gain();
mag_rate_t mag_get_rate();
mag_meas_t mag_get_meas();

/**
 * Set mode of magnetometer
 * @param new_mode
 */
int mag_set_mode(mag_mode_t new_mode);

/**
 * Struct for returning magnetometer readings.
 * Values are in milli Gauss
 */
typedef struct mag_data_s {
	float x;
	float y;
	float z;
} mag_data_t;

/**
 * Initialise device
 * @return Error code E_NO_ERR if init is successful.
 */
int mag_init(void);
int mag_init_force(void);


/**
 * Request a reading.
 *
 * @param data Pointer to a mag_data_t structure where reading will be returned (unit: mG)
 * @return Error code E_NO_ERR if reading is successful.
 */
int mag_read(mag_data_t * data);

/**
 * Request a reading.
 *
 * @param data Pointer to a mag_data_t structure where reading will be returned (unit: raw)
 * @return Error code E_NO_ERR if reading is successful.
 */
int mag_read_raw(mag_data_t * data);



/**
 * Request a single reading.
 *
 * @param data Pointer to a mag_data_t structure where reading will be returned
 * @return Error code E_NO_ERR if reading is successful.
 */
int mag_read_single(mag_data_t * data);
int mag_read_single_alt(mag_data_t * data);


/**
 * Test program that continuously reads out the magnetic field at 10Hz
 */
int mag_loop(struct command_context *ctx);
int mag_loop_fast(struct command_context *ctx);
int mag_loop_raw(struct command_context *ctx);
int mag_loop_noformat(struct command_context *ctx);
int mag_test_single(struct command_context *ctx);
int mag_test_single_alt(struct command_context *ctx);
int mag_test_bias(struct command_context *ctx);
int mag_get_info(struct command_context *ctx);


/**
 * }@
 */



#endif /* MAGNETOMETER_H_ */
