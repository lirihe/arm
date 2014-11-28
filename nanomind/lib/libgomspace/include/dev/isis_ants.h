/**
 * ISIS AntS Driver
 *
 * @brief This driver uses the Gomspace I2C driver. Ensure that the controller
 * has been initialized before trying to communicate with the antenna subsystem.
 * Also ensure that power is provided to the antenna subsystem.
 *
 * It is not needed to initialise this driver. The only setting is the active
 * antenna system node address which can be changed using the functions
 * select_addr_a|b.
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef ISIS_ANTS_H_
#define ISIS_ANTS_H_

#include <stdint.h>

#define ISIS_ANTS_DFL_ADDR_A			0x31
#define ISIS_ANTS_DFL_ADDR_B			0x32

/**
 * Status return for each antenna
 */
typedef struct isis_ant_status_s {
	uint8_t not_deployed;			//! Value of deployment switch
	uint8_t time_limit_reached;		//! Set to 1 if deployment was stopped due to a time limit
	uint8_t deployment_active;		//! Set to 1 while burning
	uint8_t activation_count;		//! Counts the number of activations since ANTS reset
	uint16_t activation_time;		//! Counts the number of 1/20 seconds since ANTS reset
} isis_ant_status_t;

/**
 * Status return for entire ants system
 */
typedef struct isis_ants_status_s {
	uint8_t armed;					//! Global system ARM status
	uint8_t switch_ignore;			//! This flag should change when using the override deploy, but it does not make sense to have globally
	isis_ant_status_t ant[4];		//! This is the detailed data structure for each antenna
} isis_ants_status_t;

/**
 * Get all status parameters
 * @param status structure containing parameters
 * @return 0 if ok, -1 if first packet failed, -2 if succeeding packets fails
 */
int isis_ants_status(uint8_t i2c_addr, isis_ants_status_t * status);

/**
 * Get temperature
 * @param temp raw adc value, needs to be converted by (3.3/1024) * adc_value
 * @return 0 if ok, -1 otherwise
 */
int isis_ants_temp(uint8_t i2c_addr, uint16_t * temp);

/**
 * Automatically deploy all antennas
 * @param time_sec maximum allowed burn time (beware too high a value may damage burn resistor)
 * @return 0 if ok, -1 otherwise
 */
int isis_ants_deploy_auto(uint8_t i2c_addr, uint8_t time_sec);

/**
 * Try to deploy single antenna
 * @param isis_ant_nr antenna number [1-4]
 * @param time_sec maximum allowed burn time (beware too high a value may damage burn resistor)
 * @param override 0 = do not use override, use override otherwise.
 * @return 0 if ok, -1 otherwise
 */
int isis_ants_deploy_single(uint8_t i2c_addr, int isis_ant_nr, uint8_t time_sec, unsigned int override);

/**
 * Cancel deployment
 * @return 0 if I2C send ok, -1 otherwise
 */
int isis_ants_deploy_cancel(uint8_t i2c_addr);

/**
 * Disarm
 * @return 0 if I2C send ok, -1 otherwise
 */
int isis_ants_disarm(uint8_t i2c_addr);

/**
 * Arm
 * @return 0 if I2C send ok, -1 otherwise
 */
int isis_ants_arm(uint8_t i2c_addr);

/**
 * Reset
 * @return 0 if I2C send ok, -1 otherwise
 */
int isis_ants_reset(uint8_t i2c_addr);

#endif /* ISIS_ANTS_H_ */
