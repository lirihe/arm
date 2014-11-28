#ifndef BPX_H_
#define BPX_H_

#include <stdint.h>

#define NODE_BPX			    6

#define BPX_PORT_ADC			8
#define BPX_PORT_HK				9
#define BPX_PORT_CAL			10
#define BPX_PORT_RESET_COUNTERS	15

#define BPX_CAL_CHARGE		0
#define BPX_CAL_DISCHARGE 	1
#define BPX_CAL_VBATT		2
#define BPX_CAL_PT100		3
#define BPX_COUNT_CAL		4

typedef struct __attribute__((packed)) {
	union {
		struct {
			uint16_t cur_charge;
			uint16_t cur_discharge;
			uint16_t vbatt;
			uint16_t pt100;
			uint16_t lm60;
		};
		uint16_t adc[5];
	};
} bpx_adc_t;

typedef struct __attribute__((packed)) {
	uint16_t cur_charge;		//! Charge current in mA
	uint16_t cur_discharge;		//! Discharge current in mA
	uint16_t vbatt;				//! Battery voltage in mV
	int16_t pt100;				//! Battery temperature in degC
	int16_t lm60;				//! BPX board temperature in degC
	uint32_t counter_boot; 		//! Number of reboots
	uint8_t	bootcause;			//! Cause of last reset
} bpx_hk_t;

typedef struct __attribute__((packed)) {
	uint8_t id;					//! ID
	float a;					//! Gain
	float b;					//! Offset
} bpx_calibration_t;

/**
 * Return all ADC values in mV
 * @param adc pointer to struct
 * @return -1 if err 0 if ok
 */
int bpx_adc_get(bpx_adc_t * adc);

/**
 * Return houskeeping values
 * @param hk pointer to struct
 * @return -1 if err 0 if ok
 */
int bpx_hk_get(bpx_hk_t * hk);

/**
 * Set calibration functions
 * use BPX_CAL_* for cal_id
 */
int bpx_cal_set(bpx_calibration_t * cal);

/**
 * Reset EEPROM based counters
 * @return csp status
 */
int bpx_counters_reset(void);



void bpx_node_set(uint8_t node);


#endif /* BPX_H_ */
