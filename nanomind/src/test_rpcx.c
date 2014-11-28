/**
 * @file test_rpcx.c
 * RPC without XML
 *
 * @author Jeppe Ledet-Pedersen
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include <command/command.h>
#include <command/rpcx.h>
#include <util/byteorder.h>

#include <conf_gomspace.h>

#ifdef WITH_CDH

struct field summarize_input[] = {
	{.name = "num1", .type = ARG_INT8(0), .help = "First input value"},
	{.name = "num2", .type = ARG_INT16(0), .help = "Second input value"},
	{.name = "fill", .type = ARG_STRING(10, "hep"), .help = "Fill string"},
};

struct field summarize_output[] = {
	{.name = "result1", .type = ARG_UINT16(0), .help = "Summarize result"},
};

int summarize_handler(struct command_context *ctx) {
	int8_t num1 = 0;
	int16_t num2 = 0, result;
	char fill[11];

	if (rpcx_unpack(ctx->rpc_buf, &ctx->command->input, &num1, &num2, &fill) < 0) {
		printf("Could not unpack arguments\r\n");
		return CMD_ERROR_FAIL;
	}

	/* Calculate result */
	result = num1 + num2;
	printf("num1 %d, num2 %d, fill %s\r\n", num1, num2, fill);

	if (rpcx_pack(ctx->rpc_buf, &ctx->command->output, &result) < 0) {
		printf("Could not pack arguments\r\n");
		return CMD_ERROR_FAIL;
	}

	return CMD_ERROR_NONE;
}

command_t __root_command test_rpcx_commands[] = {
	{
		.name = "summarize",
		.help = "Test RPC by summarizing two numbers",
		.handler = summarize_handler,
		.input = INIT_FIELDS(summarize_input),
		.output = INIT_FIELDS(summarize_output),
	},
};

void cmd_test_rpcx_setup(void) {
	command_register(test_rpcx_commands);
}

#endif
