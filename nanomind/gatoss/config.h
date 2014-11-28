/**
 * Nanomind config for GATOSS mission
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef CONF_H_
#define CONF_H_

#include <stdint.h>

typedef struct {
	uint32_t collect_interval;							//! Housekeeping collector run interval [sec]
	struct {
		uint32_t store;
		uint32_t transmit;
	} interval[2];
	uint8_t force_beacon_b;
	uint32_t checksum;									//! CRC-32
} conf_t;

extern conf_t config;

int conf_load(char * path);
int conf_save(char * path);
void conf_save_as_default();
void conf_restore();

#endif /* CONF_H_ */
