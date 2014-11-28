/**
 * Nanomind config for GATOSS mission
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <io/cdh.h>

static uint8_t cdh_node = CDH_NODE;

int cdh_conf_get(cdh_conf_t * conf) {

	uint8_t cmd = CDH_CONF_CMD_GET;
	int status = csp_transaction(CSP_PRIO_HIGH, cdh_node, CDH_PORT_CONF, 10000, &cmd, 1, (void *) conf, sizeof(cdh_conf_t));
	if (status == 0)
		return 0;

	/* Convert from network to host byte order */
	conf->collect_interval = csp_ntoh32(conf->collect_interval);
	for (unsigned int i = 0; i < CDH_HK_BEACON_TYPES; i++) {
		conf->interval[i].store = csp_ntoh32(conf->interval[i].store);
		conf->interval[i].transmit = csp_ntoh32(conf->interval[i].transmit);
	}

	return status;

}

int cdh_conf_set(cdh_conf_t * conf) {

	/* Prepare temp buffer and set command */
	uint8_t data[1 + sizeof(cdh_conf_t)];
	data[0] = CDH_CONF_CMD_SET;

	/* Make a copy of config, to not alter user config */
	cdh_conf_t conf_copy;
	memcpy(&conf_copy, conf, sizeof(cdh_conf_t));

	/* Modify config from host byte-order to network */
	conf_copy.collect_interval = csp_hton32(conf_copy.collect_interval);
	for (unsigned int i = 0; i < CDH_HK_BEACON_TYPES; i++) {
		conf_copy.interval[i].store = csp_hton32(conf_copy.interval[i].store);
		conf_copy.interval[i].transmit = csp_hton32(conf_copy.interval[i].transmit);
	}

	/* Copy modified copy of config to output buffer */
	memcpy(&data[1], &conf_copy, sizeof(cdh_conf_t));

	uint8_t reply;
	return csp_transaction(CSP_PRIO_HIGH, cdh_node, CDH_PORT_CONF, 10000, (void *) data, sizeof(data), &reply, 1);

}

int cdh_conf_load(char * path) {

	uint8_t data[1 + 50];
	data[0] = CDH_CONF_CMD_LOAD;
	strncpy((char *) &data[1], (const char *) path, 50);

	uint8_t reply;
	return csp_transaction(CSP_PRIO_HIGH, cdh_node, CDH_PORT_CONF, 10000, (void *) data, sizeof(data), &reply, 1);

}

int cdh_conf_save(char * path) {

	uint8_t data[1 + 50];
	data[0] = CDH_CONF_CMD_SAVE;
	strncpy((char *) &data[1], (const char *) path, 50);

	uint8_t reply;
	return csp_transaction(CSP_PRIO_HIGH, cdh_node, CDH_PORT_CONF, 10000, (void *) data, sizeof(data), &reply, 1);

}

int cdh_conf_restore(void) {
	uint8_t data[1];
	data[0] = CDH_CONF_CMD_RESTORE;
	uint8_t reply;
	return csp_transaction(CSP_PRIO_HIGH, cdh_node, CDH_PORT_CONF, 10000, (void *) data, sizeof(data), &reply, 1);
}

void cdh_conf_set_node(uint8_t node) {
	cdh_node = node;
}

