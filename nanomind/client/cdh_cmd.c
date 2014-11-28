/**
 * Gatoss specifc nanomind interface
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <command/command.h>

#include <io/cdh.h>

extern struct command config_cmd_subcommands[];

int hk_cmd_get_beacon(struct command_context *ctx) {

	if (ctx->argc != 7)
		return CMD_ERROR_SYNTAX;

	uint32_t from = atoi(ctx->argv[1]);
	uint32_t to = atoi(ctx->argv[2]);
	uint16_t min_interval = atoi(ctx->argv[3]);
	uint16_t max_count = atoi(ctx->argv[4]);
	uint8_t mask = atoi(ctx->argv[5]);
	uint32_t timeout = atoi(ctx->argv[6]);

	cdh_beacon_get(from, to, min_interval, max_count, mask, timeout);

	return CMD_ERROR_NONE;
}

int hk_cmd_beacon_store(struct command_context *ctx) {

	if (ctx->argc != 5)
		return CMD_ERROR_SYNTAX;

	uint32_t from = atoi(ctx->argv[1]);
	uint32_t to = atoi(ctx->argv[2]);
	uint32_t interval = atoi(ctx->argv[3]);

	int result = cdh_beacon_store(ctx->argv[4], from, to, interval);

	printf("Result: %d\r\n", result);

	return CMD_ERROR_NONE;
}

int hk_cmd_set_node(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	cdh_beacon_set_node(atoi(ctx->argv[1]));
	cdh_conf_set_node(atoi(ctx->argv[1]));
	return CMD_ERROR_NONE;
}

struct command cdh_cmd_subcommands[] = {
	{
		.name = "beacon",
		.help = "Request from beacon log",
		.usage = "<from> <to> <min_interval> <max_count> <mask> <timeout>",
		.handler = hk_cmd_get_beacon,
	}, {
		.name = "store",
		.help = "Save beacons to file",
		.usage = "<from> <to> <interval> <path>",
		.handler = hk_cmd_beacon_store,
	}, {
		.name = "conf",
		.help = "Configuration",
		.chain = {.list = config_cmd_subcommands, .count = 5}
	}, {
		.name = "node",
		.help = "Set Node",
		.handler = hk_cmd_set_node,
	}
};

struct command __root_command cdh_cmd[] = {
	{
		.name = "cdh",
		.help = "CDH/OBC subsystem",
		.chain = INIT_CHAIN(cdh_cmd_subcommands),
	}
};

void cdh_cmd_setup(void) {
	command_register(cdh_cmd);
}
