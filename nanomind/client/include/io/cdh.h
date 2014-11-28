/**
 * IO header for NanoMind A712-D revision and onwards
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef CDH_H_
#define CDH_H_

#include <stdint.h>
#include <csp/csp.h>

/**
 * Port numbers and node is extended from:
 *  <io/nanomind.h>
 */
#define CDH_NODE							1
#define CDH_PORT_CONF						23
#define CDH_PORT_BEACON_LOG					24
#define CDH_PORT_BEACON_STORE				25

/**
 * Ground station address and port
 */

#define CDH_NODE_BEACON_SERVER				10
#define CDH_PORT_BEACON_SERVER				30

#define CDH_HK_BEACON_A						(1 << 0)
#define CDH_HK_BEACON_B						(1 << 1)
#define CDH_HK_BEACON_LIVE					(1 << 3)
#define CDH_HK_BEACON_TYPES					2

/* CDH_PORT_CONF: Commands in data[0] */
enum cdh_conf_cmd {
	CDH_CONF_CMD_GET,    //!< CDH_CONF_CMD_GET
	CDH_CONF_CMD_SET,    //!< CDH_CONF_CMD_SET
	CDH_CONF_CMD_LOAD,   //!< CDH_CONF_CMD_LOAD
	CDH_CONF_CMD_SAVE,   //!< CDH_CONF_CMD_SAVE
	CDH_CONF_CMD_RESTORE,//!< CDH_CONF_CMD_RESTORE
};

/* CDH_PORT_CONF: Network data type */
typedef struct __attribute__((packed)) {
	uint32_t 	collect_interval;
	struct {
		uint32_t store;
		uint32_t transmit;
	} interval[CDH_HK_BEACON_TYPES];
	uint8_t force_beacon_b;
} cdh_conf_t;

/* CDH_PORT_BEACON_LOG: Request */
struct __attribute__((__packed__)) cdh_beacon_req {
	uint32_t	from;
	uint32_t	to;
	uint16_t	min_interval;
	uint16_t	max_count;
	uint8_t		mask;
};

/* CDH_PORT_BEACON_STORE: Request */
struct __attribute__((__packed__)) cdh_beacon_store {
	uint32_t	from;
	uint32_t	to;
	uint32_t	interval;
	char		path[50];
};

/* BEACON A */
struct __attribute__((__packed__)) cdh_beacon_a {
	struct __attribute__((packed)) {
		uint16_t bootcount;				// Total boot count
		int16_t temp1;					// Board temp1 * 4 in [C]
		int16_t temp2;					// Board temp2 * 4 in [C]
		int16_t  panel_temp[6];			// Panel temperatures * 4 in [C]
	} obc;
	struct __attribute__((packed)) {
		uint16_t byte_corr_tot;			// Total bytes corrected by reed-solomon
		uint16_t rx;					// Total packets detected
		uint16_t rx_err;				// Total packets with error
		uint16_t tx;					// Total packets transmitted
		int16_t  last_temp_a;			// Last temperature A in [C]
		int16_t  last_temp_b;			// Last temperature B in [C]
		int16_t  last_rssi;				// Last detected RSSI [dBm]
		int16_t  last_rferr;			// Last detected RF-error [Hz]
		uint16_t last_batt_volt;		// Last sampled battery voltage [mV/10]
		uint16_t last_txcurrent;		// Last TX current [mA]
		uint16_t bootcount;				// Total bootcount
	} com;
	struct __attribute__ ((__packed__)) {
		uint16_t vboost[3];								//! Voltage of boost converters [mV] [PV1, PV2, PV3]
		uint16_t vbatt;									//! Voltage of battery [mV]
		uint16_t curout[6];								//! Current out [mA]
		uint16_t curin[3];								//! Current in [mA]
		uint16_t cursun;								//! Current from boost converters
		uint16_t cursys;								//! Current out of battery
		int16_t temp[6];								//! Temperature sensors [0 = TEMP1, TEMP2, TEMP3, TEMP4, BATT0, BATT1]
		uint8_t output;									//! Status of outputs
		uint16_t counter_boot; 							//! Number of EPS reboots
		uint16_t counter_wdt_i2c;						//! Number of WDT I2C reboots
		uint16_t counter_wdt_gnd;						//! Number of WDT GND reboots
		uint8_t	bootcause;								//! Cause of last EPS reset
		uint16_t latchup[6];							//! Number of latch-ups
		uint8_t battmode;								//! Mode for battery [0 = normal, 1 = undervoltage, 2 = overvoltage]
	} eps;
	struct __attribute__((__packed__)) {
		uint16_t average_fps_5min;
		uint16_t average_fps_1min;
		uint16_t average_fps_10sec;
		uint16_t plane_count;
		uint32_t frame_count;
		uint32_t last_icao;
		uint32_t last_timestamp;
		float last_lat;
		float last_lon;
		uint32_t last_altitude;
		uint32_t crc_corrected;
		uint16_t bootcount;
		uint16_t bootcause;
	} gatoss;
	struct __attribute__((packed)) {
		int8_t  temp;					// Temperature of nanohub in [C]
		uint16_t bootcount;				// Total bootcount
		uint8_t  reset;					// Reset cause:
		uint8_t  sense_status;			// Status on feedback switches and arm switch [ARM1 ARM0 K1S3 K1S2 K1S1 K1S0 K0S1 K0S0]
		uint16_t burns[2];				// Number of burn tries [K1B1 K1B0]
	} hub;
	struct __attribute__((packed)) {
		float tumblerate[3];
		float tumblenorm[2];
		float mag[3];
		uint8_t status;					// [xxxxxxab] a = magvalid, b = detumbled
		float torquerduty[3];
		uint8_t ads;					// State [xxxxyyyy] x = state, y = dstate
		uint8_t acs;					// State [xxxxyyyy] x = state, y = dstate
	    uint8_t sunsensor_packed[8];
	} adcs;
};

/* BEACON AB */
struct __attribute__((__packed__)) cdh_beacon_b {
	    uint16_t sun[5]; 		// sun sensor +x +y -x -y -z
	    uint8_t ineclipse;		// 0=in sun, 1=ineclipse
	    float xest[16]; 		// State vector [_i^cq ^c\omega b_mag b_gyro \tau] in total 16 states
	    float zfilt[9]; 		// Filt meas vector [v_sun v_mag omega_gyro]
	    uint16_t enable; 		// Enable vector (bit mask) for meas vector [xsunsensor,3xmag,3xgyro]
	    float ref_q[4];			// Control ref
		float err_q[4];			// control err
	    float ierr_q[3];		// Control ierr
	    float err_rate[3];		// Control err rate
	    float  sc_reci[3];		// Ephem satellite pos
	    float  sun_eci[3];		// Ephem sun pos
	    float  mag_eci[3];		// Ephem mag
};

/* BEACON */
typedef struct __attribute__((packed)) {
	uint32_t 	beacon_time;							//! Time the beacon was formed
	uint8_t 	beacon_flags;							//! Which type of beacon is this
	union {
		struct cdh_beacon_a a;							//! A is common housekeeping
		struct cdh_beacon_b b;							//! B is extended ADCS
	};
} cdh_beacon;

int cdh_conf_get(cdh_conf_t * conf);
int cdh_conf_set(cdh_conf_t * conf);
int cdh_conf_load(char * path);
int cdh_conf_save(char * path);
int cdh_conf_restore(void);
void cdh_conf_set_node(uint8_t node);

void cdh_beacon_print(cdh_beacon * beacon);
int cdh_beacon_get(uint32_t from, uint32_t to, uint16_t min_interval, uint16_t max_count, uint8_t mask, uint32_t timeout);
int cdh_beacon_store(char * path, uint32_t from, uint32_t to, uint32_t interval);
void cdh_beacon_set_node(uint8_t node);

/**
 * Beacon listener task
 *
 * The beacon listener task can be started on any node, it will listen for beacons
 * on the specified port number, convert to host order and send to a callback function
 *
 * The default callback function is cdh_beacon_print if NULL is set in parameters
 *
 * @param param pointer to paramaters containing port and callback
 */
typedef void (*cdh_beacon_listener_callback_t)(csp_packet_t * packet);
typedef struct {
	uint8_t port;
	cdh_beacon_listener_callback_t callback;
} cdh_beacon_listener_param_t;
void cdh_beacon_task(void * param);

void cdh_cmd_setup(void);
void cdh_beacon_ntoh(cdh_beacon * beacon);

#endif /* CDH_H_ */
