/**
 * Nanomind config for GATOSS mission
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

static cdh_conf_t cmd_conf;

static void conf_cmd_print(void) {
	printf("Collect Interval: %"PRIu32"\r\n", cmd_conf.collect_interval);
	for (unsigned int i = 0; i < CDH_HK_BEACON_TYPES; i++) {
		printf("[%u] store: %"PRIu32", transmit: %"PRIu32"\r\n", i, cmd_conf.interval[i].store, cmd_conf.interval[i].transmit);
	}
	printf("Force beacon b: %"PRIu8"\r\n", cmd_conf.force_beacon_b);
}

int conf_cmd_get(struct command_context *ctx) {
	cdh_conf_get(&cmd_conf);
	conf_cmd_print();
	return CMD_ERROR_NONE;
}

int conf_cmd_set(struct command_context *ctx) {
	if (ctx->argc != 7)
		return CMD_ERROR_SYNTAX;

	cmd_conf.collect_interval = atoi(ctx->argv[1]);
	cmd_conf.interval[0].store = atoi(ctx->argv[2]);
	cmd_conf.interval[0].transmit = atoi(ctx->argv[3]);
	cmd_conf.interval[1].store = atoi(ctx->argv[4]);
	cmd_conf.interval[1].transmit = atoi(ctx->argv[5]);
	cmd_conf.force_beacon_b = atoi(ctx->argv[6]);

	cdh_conf_set(&cmd_conf);
	conf_cmd_print();
	return CMD_ERROR_NONE;
}

int conf_cmd_load(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	cdh_conf_load(ctx->argv[1]);
	return CMD_ERROR_NONE;
}

int conf_cmd_save(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	cdh_conf_save(ctx->argv[1]);
	return CMD_ERROR_NONE;
}

int conf_cmd_restore(struct command_context *ctx) {
	cdh_conf_restore();
	return CMD_ERROR_NONE;
}

struct command config_cmd_subcommands[] = {
	{
		.name = "get",
		.help = "Get",
		.handler = conf_cmd_get,
	}, {
		.name = "set",
		.help = "Set",
		.usage = "<collect interval> <0:store> <0:tx> <1:store> <1:tx> <1:force>",
		.handler = conf_cmd_set,
	},{
		.name = "save",
		.help = "Save",
		.usage = "<path>",
		.handler = conf_cmd_save,
	}, {
		.name = "load",
		.help = "load",
		.usage = "<path>",
		.handler = conf_cmd_load,
	}, {
		.name = "restore",
		.help = "restore default",
		.handler = conf_cmd_restore,
	}
};
