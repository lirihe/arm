/*
 * eps.h
 *
 *  Created on: Sep 6, 2009
 *      Author: johan
 */

#ifndef EPS2_H_
#define EPS2_H_

#include <stdint.h>

/* CSP Address */
#define NODE_EPS	2

/* CSP Port numbers */
#define EPS_PORT_RESET  7 //For slave interface
#define EPS_PORT_HK	8
#define EPS_PORT_SET_OUTPUT 9
#define EPS_PORT_SET_SINGLE_OUTPUT 10
#define EPS_PORT_SET_VBOOST 11
#define EPS_PORT_SET_PPTMODE 12
#define EPS_PORT_HEATER 13 // For slave interface
#define EPS_PORT_RESET_COUNTERS 15
#define EPS_PORT_RESET_WDT_GND 16
#define EPS_PORT_CONFIG_CMD 17
#define EPS_PORT_CONFIG_GET 18
#define EPS_PORT_CONFIG_SET 19
#define EPS_PORT_HARDRESET 20
#define EPS_PORT_CONFIG2_CMD 21
#define EPS_PORT_CONFIG2_GET 22
#define EPS_PORT_CONFIG2_SET 23

typedef struct __attribute__((packed)) {
	uint16_t vboost[3];								//! Voltage of boost converters [mV] [PV1, PV2, PV3]
	uint16_t vbatt;									//! Voltage of battery [mV]
	uint16_t curin[3];								//! Current in [mA]
	uint16_t cursun;								//! Current from boost converters
	uint16_t cursys;								//! Current out of battery
	uint16_t reserved;
} eps_hk_vi_t;

typedef struct __attribute__((packed)) {
	uint16_t curout[6];								//! Current out [mA]
	uint8_t output[8];								//! Status of outputs
	uint16_t output_on_delta[8];					//! Time till power on
	uint16_t output_off_delta[8];					//! Time till power off
	uint16_t latchup[6];							//! Number of latch-ups
} eps_hk_out_t;



typedef struct __attribute__((packed)) {
	uint32_t wdt_i2c_time_left;						//! Time left on I2C wdt
	uint32_t wdt_gnd_time_left;						//! Time left on I2C wdt
	uint8_t  wdt_csp_pings_left[2];					//! Pings left on CSP wdt
	uint32_t counter_wdt_i2c;						//! Number of WDT I2C reboots
	uint32_t counter_wdt_gnd;						//! Number of WDT GND reboots
	uint32_t counter_wdt_csp[2];					//! Number of WDT CSP reboots
} eps_hk_wdt_t;


typedef struct __attribute__((packed)) {
	uint32_t counter_boot; 							//! Number of EPS reboots
	int16_t temp[6];								//! Temperature sensors [0 = TEMP1, TEMP2, TEMP3, TEMP4, BATT0, BATT1]
	uint8_t	bootcause;								//! Cause of last EPS reset
	uint8_t battmode;								//! Mode for battery [0 = normal, 1 = undervoltage, 2 = overvoltage]
	uint8_t pptmode;								//! Mode of PPT tracker
	uint16_t reserved2;
} eps_hk_basic_t;


typedef struct __attribute__((packed)) {
	uint16_t vboost[3];								//! Voltage of boost converters [mV] [PV1, PV2, PV3]
	uint16_t vbatt;									//! Voltage of battery [mV]
	uint16_t curin[3];								//! Current in [mA]
	uint16_t cursun;								//! Current from boost converters
	uint16_t cursys;								//! Current out of battery
	uint16_t reserved;
	uint16_t curout[6];								//! Current out [mA]
	uint8_t output[8];								//! Status of outputs
	uint16_t output_on_delta[8];					//! Time till power on
	uint16_t output_off_delta[8];					//! Time till power off
	uint16_t latchup[6];							//! Number of latch-ups
	uint32_t wdt_i2c_time_left;						//! Time left on I2C wdt
	uint32_t wdt_gnd_time_left;						//! Time left on I2C wdt
	uint8_t  wdt_csp_pings_left[2];					//! Pings left on CSP wdt
	uint32_t counter_wdt_i2c;						//! Number of WDT I2C reboots
	uint32_t counter_wdt_gnd;						//! Number of WDT GND reboots
	uint32_t counter_wdt_csp[2];					//! Number of WDT CSP reboots
	uint32_t counter_boot; 							//! Number of EPS reboots
	int16_t temp[6];								//! Temperature sensors [0 = TEMP1, TEMP2, TEMP3, TEMP4, BATT0, BATT1]
	uint8_t	bootcause;								//! Cause of last EPS reset
	uint8_t battmode;								//! Mode for battery [0 = normal, 1 = undervoltage, 2 = overvoltage]
	uint8_t pptmode;								//! Mode of PPT tracker
	uint16_t reserved2;
} eps_hk_t;

typedef struct __attribute__ ((__packed__)) {
	uint16_t pv[3];									//! Photo-voltaic input voltage [mV]
	uint16_t pc;									//! Total photo current [mA]
	uint16_t bv;									//! Battery voltage [mV]
	uint16_t sc;									//! Total system current [mA]
	int16_t  temp[6];	   							//! Temp. of boost converters and on-board battery [degC] (Conv 1, Conv 2, Conv 3, Board)
	uint16_t latchup[6];							//! Number of latch-ups on each output 5V and +3V3 channel [5V1 5V2 5V3 3.3V1 3.3V2 3.3V3]
	uint8_t	 reset;		    						//! Cause of last EPS reset
	uint16_t bootcount;     						//! Number of EPS reboots
	uint16_t sw_errors;								//! Number of errors in the eps software
	uint8_t  ppt_mode;								//! 0 = Hardware, 1 = MPPT, 2 = Fixed SW PPT.
	uint8_t  channel_status;						//! Mask of output channel status, 1=on, 0=off [MSB NC NC 5V1 5V2 5V3 3.3V1 3.3V2 3.3V3 LSB]
} eps_hk_1_t;

/**
 * Pack and unpack hk
 * @param hk pointer to hk
 */
void eps_hk_pack(eps_hk_t * hk);
void eps_hk_unpack(eps_hk_t * hk);
void eps_hk_vi_pack(eps_hk_vi_t * hk);
void eps_hk_vi_unpack(eps_hk_vi_t * hk);
void eps_hk_out_pack(eps_hk_out_t * hk);
void eps_hk_out_unpack(eps_hk_out_t * hk);
void eps_hk_wdt_pack(eps_hk_wdt_t * hk);
void eps_hk_wdt_unpack(eps_hk_wdt_t * hk);
void eps_hk_basic_pack(eps_hk_basic_t * hk);
void eps_hk_basic_unpack(eps_hk_basic_t * hk);
void eps_hk_1_pack(eps_hk_1_t * hk);

/**
 * Get EPS housekeeping
 * @param hk pointer to empty hk structure
 * @return csp status
 */
int eps_hk_get(eps_hk_t * hk);
int eps_hk_vi_get(eps_hk_vi_t * hk);
int eps_hk_out_get(eps_hk_out_t * hk);
int eps_hk_wdt_get(eps_hk_wdt_t * hk);
int eps_hk_basic_get(eps_hk_basic_t * hk);

/**
 * Print out hk structure to console
 * @param hk pointer to hk data
 */
void eps_hk_print(eps_hk_t * hk);
void eps_hk_vi_print_text(eps_hk_vi_t * hk);
void eps_hk_out_print_text(eps_hk_out_t * hk);
void eps_hk_wdt_print_text(eps_hk_wdt_t * hk);
void eps_hk_basic_print_text(eps_hk_basic_t * hk);

typedef struct __attribute__((packed)) {
	uint8_t ppt_mode;								//! Mode for MPPT [1 = AUTO, 2 = FIXED]
	uint8_t battheater_mode;						//! Mode for battheater [0 = Manual, 1 = Auto]
	int8_t battheater_low;							//! Turn heater on at [degC]
	int8_t battheater_high;							//! Trun heater off at [degC]
	uint8_t output_normal_value[8];					//! Nominal mode output value
	uint8_t output_safe_value[8];					//! Safe mode output value
	uint16_t output_initial_on_delay[8];			//! Output driver is initialised with these on delays [s]
	uint16_t output_initial_off_delay[8];			//! Output driver is initialised with these off delays [s]
	uint16_t vboost[3];								//! Fixed PPT point for boost converters [mV]
} eps_config_t;


typedef struct __attribute__((packed)) {
	uint16_t batt_maxvoltage;
	uint16_t batt_safevoltage;
	uint16_t batt_criticalvoltage;
	uint16_t batt_normalvoltage;
	uint32_t wdt_i2c;
	uint32_t wdt_gnd;
	uint8_t  cspwdt_channel[2];
	uint8_t  cspwdt_add[2];
} eps_config2_t;


/**
 * Set FIXED ppt voltage
 * @param vboost1 in mV
 * @param vboost2 in mV
 * @param vboost3 in mV
 * @return csp status
 */
int eps_vboost_set(unsigned int vboost1, unsigned int vboost2, unsigned int vboost3);

/**
 * Set PPT mode
 * @param mode 0 = HW, 1 = MPPT, 2 = FIXED
 * @return csp status
 */
int eps_pptmode_set(unsigned char mode);

/**
 * Pack, unpack and print config
 * @param conf pointer to config
 */
void eps_config_pack(eps_config_t * conf);
void eps_config_unpack(eps_config_t * conf);
void eps_config_print(eps_config_t * conf);
void eps_config2_pack(eps_config2_t * conf2);
void eps_config2_unpack(eps_config2_t * conf2);
void eps_config2_print(eps_config2_t * conf2);

/**
 * Do config command
 * @param cmd
 * @return csp status
 */
int eps_config_cmd(uint8_t cmd);
int eps_config2_cmd(uint8_t cmd);

/**
 * Get running EPS config
 * @param config poitner to config
 * @return csp status
 */
int eps_config_get(eps_config_t * config);
int eps_config2_get(eps_config2_t * config2);

/**
 * Set running config on EPS
 * @param config pointer to config
 * @return csp status
 */
int eps_config_set(eps_config_t * config);
int eps_config2_set(eps_config2_t * config2);

/**
 * Set all outputs with mask
 * @param output output mask
 * @return csp status
 */
int eps_output_set(uint8_t output);

typedef struct __attribute__((packed)) {
	uint8_t channel;
	uint8_t mode;
	int16_t delay;
} eps_output_set_single_req;

typedef struct __attribute__((packed)) {
  uint16_t on_timer[2];
  uint16_t off_timer[2];
  uint16_t faults[2];
  uint8_t switchstatus;
} eps_output_get_single_req;

/**
 * Set single output
 * @param channel output channel number
 * @param mode 1 = ON, 0 = OFF
 * @param delay time to wait in ticks (0 = now)
 * @return csp status
 */
int eps_output_set_single(uint8_t channel, uint8_t mode, int16_t delay);

/**
 * Reset EEPROM based counters
 * @return csp status
 */
int eps_counters_reset(void);

/**
 * Perform HARD reset of EPS
 * @return csp status
 */
int eps_hardreset(void);

/**
 * Kick WDT GND
 * @return csp status
 */
int eps_wdt_gnd_reset(void);

/**
 * Configure CSP timeout for outgoing EPS requests
 * @param timeout
 */
void eps_set_timeout(uint32_t timeout);

/**
 * Configure CSP address for outgoing EPS requests
 * @param node
 */
void eps_set_node(uint8_t node);

#endif /* EPS2_H_ */
