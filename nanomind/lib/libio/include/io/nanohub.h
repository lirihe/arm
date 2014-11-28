/**
 * NanoHub firmware
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef NANOHUB_H_
#define NANOHUB_H_

#include <stdint.h>

#define NANOHUB_CSP_ADDR					3

/* CSP Port numbers */
#define NANOHUB_PORT_ADC					7
#define NANOHUB_PORT_GYRO					8
#define NANOHUB_PORT_DIO					9
#define NANOHUB_PORT_RESET_WDT				10
#define NANOHUB_PORT_SET_SINGLE_SWITCH		11
#define NANOHUB_PORT_CONF_CMD				12
#define NANOHUB_PORT_CONF					13
#define NANOHUB_PORT_DEPLOY					14
#define NANOHUB_PORT_HK						15
#define NANOHUB_PORT_SPI					16

typedef struct __attribute__((packed)) {
  uint16_t time[2];        // Time in seconds/10
  uint8_t retries[2];      // Number of retries on no-release (using sense inputs)
  uint32_t bootdelay[2];   // Delay on doing burn after boot [seconds/10] (0=never)
  uint8_t enable[2];      // Enable or disable knife
  uint16_t delay[2];       // Delay between burns [seconds/10]
  uint8_t sensemask[2];    // Which sense channels belongs to which output
  uint8_t sense_no;        // 1=no (deployable stowed when sense is low), 0=nc (deployable stowed when sense is high)
} nanohub_conf_knife_t;

typedef struct __attribute__((packed)) {
  uint16_t bootdelay;    // Delay on doing burn after boot [seconds/10] (0=never)
} nanohub_conf_switch_t;

typedef struct __attribute__((packed)) {
  uint8_t enable;        // Enable mask for digital io port (1=dio, 2=adc)
  uint8_t dirmask;       // Direction mask for digital io port
  uint8_t outputvalue;   // Sets the output value (0 or 1) for the io port
} nanohub_conf_dio_t;

typedef struct __attribute__((packed)) {
  uint32_t wdt_timeout;      				// External wdt timeout [s]
  uint8_t wdt_enable;              		 	// External wdt enable
  uint32_t com_tx_inhibit_timeout;        	// Com tx inhibit message period
  uint8_t com_tx_inhibit_enable;          	// Com tx inhibit message enable
} nanohub_conf_timing_t;

typedef struct  __attribute__((packed)) {
  nanohub_conf_knife_t knife0;
  nanohub_conf_knife_t knife1;
  nanohub_conf_switch_t switch0;
  nanohub_conf_switch_t switch1;
  nanohub_conf_timing_t timing;
  nanohub_conf_dio_t dio;
  uint32_t checksum;
} nanohub_conf_t;

typedef struct __attribute__((packed)) {
  uint16_t burns[2];						// Number of burn tries
  uint8_t mode[2];							// At timeout of timer switch to this mode
  uint32_t timer[2];						// Current value of timer
} nanohub_knifedata_t;

typedef struct __attribute__((packed)) {
	nanohub_knifedata_t knife[2];
	uint8_t sense_status;                   // Status on feedback switches and arm switch [ARM1 ARM0 K1S3 K1S2 K1S1 K1S0 K0S1 K0S0]
	uint8_t burn_status;					// 1 = burning right now, 0 = not burning [x x x x K1B1 K1B0 K0B1 K0B0]
} nanohub_knivesdata_t;

typedef struct __attribute__((packed)) {
  uint16_t on_timer[2];
  uint16_t off_timer[2];
  uint16_t faults[2];
  uint8_t switchstatus;
} nanohub_switchdata_t;

typedef struct __attribute__((packed)) {
	uint8_t channel;
	uint8_t mode;
	uint16_t delay;
} nanohub_switch_t;

typedef struct __attribute__((packed)) {
  uint8_t knife;
  uint8_t channel;
  uint16_t delay;
  uint16_t duration;
} nanohub_knife_t;

typedef struct __attribute__((packed)) {
	int16_t temp;					// Temperature of nanohub in [C]
	uint32_t bootcount;		        // Total bootcount
	uint8_t reset;	        	    // Reset cause:
	uint8_t gyrostatus;	            // Gyro-status byte [7...0] [x x x x  x x x ON]
} nanohub_hk_t;

int hub_set_node(uint8_t node);
int hub_get_hk(nanohub_hk_t *nanohub_hk);
int hub_set_single_output(uint8_t channel, uint8_t mode, uint16_t delay, nanohub_switchdata_t *switchdata);
int hub_reset_wdt(uint8_t magic, uint32_t *timer);
int hub_get_adc_single(uint8_t channel, uint16_t *data);
int hub_get_adc_all(uint16_t *data);
int hub_gyro(uint8_t cmd, uint8_t dest, uint8_t port, int32_t *gyrodata);
int hub_conf_cmd(uint8_t cmd, uint8_t *configmode);
int hub_get_conf(nanohub_conf_t *conf);
int hub_set_conf(nanohub_conf_t *conf);
void hub_print_conf(nanohub_conf_t *configuration);
void hub_edit_conf(nanohub_conf_t *configuration);
int hub_knife(uint8_t knife, uint8_t channel, uint16_t delay, uint16_t duration,  nanohub_knivesdata_t *knivesdata);

#endif /* NANOHUB_H_ */
