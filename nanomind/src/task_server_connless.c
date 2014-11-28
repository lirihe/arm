/**
 * Connection Less CSP Server
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <csp/csp.h>

#include <io/nanomind.h>

#include <util/console.h>

void task_server_connless(void * parameters) {

	csp_packet_t * packet;
	csp_socket_t * socket = csp_socket(CSP_SO_CONN_LESS);
	if (socket == NULL)
		vTaskDelete(NULL);
	csp_bind(socket, OBC_PORT_CONN_LESS_TEST);

	while(1) {

		packet = csp_recvfrom(socket, CSP_MAX_DELAY);
		if (packet == NULL)
			continue;

		printf("Packet received on connless port. Length: %u\r\n", packet->length);
		packet->data[20] = '\0';
		printf("%s", packet->data);

		if (packet->id.sport == 0) {
			csp_buffer_free(packet);
			continue;
		}

		/* Send a reply */
		if (csp_sendto(packet->id.pri, packet->id.src, packet->id.sport, packet->id.dport, 0, packet, 0) < 0) {
			printf("Failed to send reply\r\n");
			csp_buffer_free(packet);
			continue;
		}

	}

}

static int cmd_clp(struct command_context *ctx) {

	csp_packet_t * packet = csp_buffer_get(100);

	char * str = "Hello World\r\n";
	sprintf((char *) packet->data, str);
	packet->length = strlen(str) + 1;

	if (csp_sendto(CSP_PRIO_NORM, NODE_OBC, OBC_PORT_CONN_LESS_TEST, 0, CSP_O_CRC32 | CSP_O_HMAC | CSP_O_XTEA, packet, 0) < 0) {
		printf("Error sending packet\r\n");
		csp_buffer_free(packet);
	}

	return CMD_ERROR_NONE;

}

static int cmd_clp_reply(struct command_context *ctx) {

	csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, NODE_OBC, OBC_PORT_CONN_LESS_TEST, 5000, 0);

	csp_packet_t * packet = csp_buffer_get(100);

	char * str = "Hello World\r\n";
	sprintf((char *) packet->data, str);
	packet->length = strlen(str) + 1;

	if (!csp_send(conn, packet, 0)) {
		printf("Error sending packet\r\n");
		csp_buffer_free(packet);
		csp_close(conn);
		return CMD_ERROR_FAIL;
	}

	printf("Waiting for reply\r\n");

	packet = csp_read(conn, 5000);
	if (packet == NULL) {
		printf("No reply!\r\n");
		csp_close(conn);
		return CMD_ERROR_FAIL;
	}

	printf("Reply received. Length %u\r\n", packet->length);
	packet->data[20] = '\0';
	printf("%s", packet->data);

	csp_buffer_free(packet);
	csp_close(conn);

	return CMD_ERROR_NONE;

}

struct command connless_subcommands[] = {
	{
		.name = "packet",
		.help = "Send packet",
		.handler = cmd_clp,
	},{
		.name = "reply",
		.help = "Send packet with reply",
		.handler = cmd_clp_reply,
	}
};

struct command __root_command connless_commands[] = {
	{
		.name = "connless",
		.help = "Connection less CSP tests",
		.chain = INIT_CHAIN(connless_subcommands),
	}
};

void cmd_task_server_connless_setup(void) {
	command_register(connless_commands);
}

