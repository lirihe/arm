/**
 * @file cmd_ds1302.c
 * Debug command interface for the Maxim DS1302 RTC
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include <util/console.h>
#include <util/hexdump.h>
#include <dev/arm/ds1302.h>

int cmd_rtc_set(struct command_context *ctx) {

	unsigned long time;
	char *remain;
	struct ds1302_clock clock;

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	time = strtoul(ctx->argv[1], &remain, 10);
	if (time == ULONG_MAX || remain == ctx->argv[1])
		return CMD_ERROR_SYNTAX;

	if (ds1302_time_to_clock((time_t *) &time, &clock) < 0)
		return CMD_ERROR_FAIL;

	if (ds1302_clock_write_burst(&clock) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int cmd_rtc_get(struct command_context *ctx) {
	
	unsigned long time;
	struct ds1302_clock clock;
	
	if (ds1302_clock_read_burst(&clock) < 0)
		return CMD_ERROR_FAIL;

	if (ds1302_clock_to_time((time_t *) &time, &clock) < 0)
		return CMD_ERROR_FAIL;

	printf("Time is: %s", ctime((time_t *) &time));

	return CMD_ERROR_NONE;
}

int cmd_rtc_write(struct command_context *ctx) {

	uint8_t data[DS1302_RAM_BYTES];
	
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	strncpy((char *) data, ctx->argv[1], DS1302_RAM_BYTES);

	if (ds1302_write_ram_burst(data) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int cmd_rtc_read(struct command_context *ctx) {
	
	uint8_t data[DS1302_RAM_BYTES];

	if (ds1302_read_ram_burst(data) < 0)
		return CMD_ERROR_FAIL;

	hex_dump(data, DS1302_RAM_BYTES);

	return CMD_ERROR_NONE;
}

command_t __sub_command rtc_clock_subcommands[] = {
	{
		.name = "set",
		.help = "Set RTC",
		.handler = cmd_rtc_set,
		.usage = "<time>",
	},{
		.name = "get",
		.help = "Get current RTC value",
		.handler = cmd_rtc_get,
	}
};

command_t __sub_command rtc_ram_subcommands[] = {
	{
		.name = "read",
		.help = "Read RTC RAM",
		.handler = cmd_rtc_read,
	},{
		.name = "write",
		.help = "Write RTC RAM",
		.usage = "<data>",
		.handler = cmd_rtc_write,
	}
};

command_t __sub_command rtc_subcommands[] = {
	{
		.name = "clock",
		.help = "Clock subcommands",
		.chain = INIT_CHAIN(rtc_clock_subcommands),
	},{
		.name = "ram",
		.help = "RAM subcommands",
		.chain = INIT_CHAIN(rtc_ram_subcommands),
	}
};

command_t __sub_command __root_command rtc_commands[] = {
	{
		.name = "rtc",
		.help = "DS1302 RTC commands",
		.chain = INIT_CHAIN(rtc_subcommands),
	},
};

void cmd_rtc_setup(void) {
	command_register(rtc_commands);
}
