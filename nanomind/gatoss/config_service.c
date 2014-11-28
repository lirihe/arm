/**
 * Nanomind config for GATOSS mission
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <io/cdh.h>
#include "config.h"

void service_conf(csp_conn_t * conn) {

	/* Receive packet from connection */
	csp_packet_t * packet = csp_read(conn, 1);
	if (packet == NULL)
		return;

	/* First byte contains command */
	uint8_t conf_cmd = packet->data[0];

	switch(conf_cmd) {

	case CDH_CONF_CMD_GET: {
		cdh_conf_t * conf = (void *) packet->data;

		/* Copy config from host to network struct */
		conf->collect_interval = csp_hton32(config.collect_interval);
		for (unsigned int i = 0; i < CDH_HK_BEACON_TYPES; i++) {
			conf->interval[i].store = csp_hton32(config.interval[i].store);
			conf->interval[i].transmit = csp_hton32(config.interval[i].transmit);
		}
		conf->force_beacon_b = config.force_beacon_b;

		packet->length = sizeof(cdh_conf_t);
		if (csp_send(conn, packet, 0) == 1)
			packet = NULL;
		goto out;
	}

	case CDH_CONF_CMD_SET: {
		cdh_conf_t * conf = (void *) &packet->data[1];

		/* Copy config from network to host struct */
		config.collect_interval = csp_hton32(conf->collect_interval);
		for (unsigned int i = 0; i < CDH_HK_BEACON_TYPES; i++) {
			config.interval[i].store = csp_hton32(conf->interval[i].store);
			config.interval[i].transmit = csp_hton32(conf->interval[i].transmit);
		}
		config.force_beacon_b = conf->force_beacon_b;

		goto out_ack;
	}

	case CDH_CONF_CMD_LOAD: {

		char * path = (void *) &packet->data[1];
		if (strlen(path) == 0 || strlen(path) > 49)
			goto out;

		conf_load(path);
		goto out_ack;

	}

	case CDH_CONF_CMD_SAVE: {

		char * path = (void *) &packet->data[1];
		if (strlen(path) == 0 || strlen(path) > 49)
			goto out;

		conf_save(path);
		goto out_ack;

	}

	case CDH_CONF_CMD_RESTORE: {
		conf_restore();
		goto out_ack;
	}

	}

out_ack:

	packet->data[0] = 1;
	packet->length = 1;
	if (csp_send(conn, packet, 0) == 1)
		packet = NULL;
out:

	/* Ensure packet is free */
	if (packet != NULL)
		csp_buffer_free(packet);

}
