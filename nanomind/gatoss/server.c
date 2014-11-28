/**
 * Nanomind CDH server for GATOSS mission
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <io/cdh.h>

void task_cdh_server(void * param __attribute__((unused))) {

	/* Prepare socket */
	csp_conn_t * conn;
	csp_socket_t * socket = csp_socket(CSP_SO_NONE);
	csp_bind(socket, CDH_PORT_CONF);
	csp_bind(socket, CDH_PORT_BEACON_LOG);
	csp_bind(socket, CDH_PORT_BEACON_STORE);
	csp_listen(socket, 10);

	while (1) {

		/* Wait for incoming connection, or timeout */
		conn = csp_accept(socket, CSP_MAX_DELAY);

		/* Only continue if a valid connection is present */
		if (conn == NULL)
			continue;

		/* Dispatch connection to services */
		switch (csp_conn_dport(conn)) {

			case CDH_PORT_CONF: {
				extern void service_conf(csp_conn_t * conn);
				service_conf(conn);
				break;
			}

			case CDH_PORT_BEACON_LOG: {
				extern void service_hk_beacons(csp_conn_t * conn);
				service_hk_beacons(conn);
				break;
			}

			case CDH_PORT_BEACON_STORE: {
				extern void service_hk_beacon_store(csp_conn_t * conn);
				//service_hk_beacon_store(conn);
				break;
			}

		}

		/* Ensure connection is closed */
		csp_close(conn);
	}

}

