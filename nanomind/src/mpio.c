/**
 * NanoMind MPIO driver
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <dev/arm/at91sam7.h>
#include <command/command.h>
#include "mpio.h"

static inline AT91PS_GPT io_base(int pin) {
	switch(pin) {
	default:
	case 0:
	case 1:
	case 2:
		return AT91C_BASE_GPT0;
	case 3:
	case 4:
	case 5:
		return AT91C_BASE_GPT1;
	case 6:
		return AT91C_BASE_GPT2;
	}
}

static inline unsigned long io_mask(int pin) {
	switch(pin) {
	default:
	case 0:	return 1 << (1 + 16);
	case 1: return 1 << (0 + 16);
	case 2: return 1 << (2 + 16);
	case 3:	return 1 << (1 + 16);
	case 4: return 1 << (0 + 16);
	case 5: return 1 << (2 + 16);
	case 6: return 1 << (1 + 16);
	}
}

void io_init(int pin, int output) {

	/* Enable */
	io_base(pin)->PER = io_mask(pin);
	io_base(pin)->ECR = 1;

	/* Set input/output */
	if (output) {
		io_base(pin)->OER = io_mask(pin);
		io_base(pin)->MDDR = io_mask(pin);
	} else {
		io_base(pin)->ODR = io_mask(pin);
	}

}

int io_get(int pin) {
	return (io_base(pin)->PDSR & io_mask(pin)) > 0 ? 1 : 0;
}

void io_set(int pin) {
	io_base(pin)->SODR = io_mask(pin);
}

void io_clear(int pin) {
	io_base(pin)->CODR = io_mask(pin);
}

int mpio_read_all(struct command_context *ctx) {
	int i;
	for (i = 0; i < 7; i++)
		printf("Pin %u is %u\r\n", i, io_get(i));
	return CMD_ERROR_NONE;
}

int mpio_init_out_all(struct command_context *ctx) {
	int i;
	for (i = 0; i < 7; i++)
		io_init(i, 1);
	return CMD_ERROR_NONE;
}

int mpio_init_in_all(struct command_context *ctx) {
	int i;
	for (i = 0; i < 7; i++)
		io_init(i, 0);
	return CMD_ERROR_NONE;
}

int mpio_toggle_all(struct command_context *ctx) {
	static int val = 0;
	int i;
	for (i = 0; i < 7; i++) {
		if (val % 2) {
			printf("Set %u\r\n", i);
			io_set(i);
		} else {
			printf("Clear %u\r\n", i);
			io_clear(i);
		}
	}
	val++;
	return CMD_ERROR_NONE;
}

struct command mpio_sub_commands[] = {
	{
		.name = "read",
		.help = "Get value of MPIO pins",
		.handler = mpio_read_all,
	},{
		.name = "init-out",
		.help = "Initialise MPIO as outputs",
		.handler = mpio_init_out_all,
	},{
		.name = "init-in",
		.help = "Initialise MPIO as inputs",
		.handler = mpio_init_in_all,
	},{
		.name = "toggle",
		.help = "Toggle all MPIO pins",
		.handler = mpio_toggle_all,
	},
};

struct command __root_command mpio_commands[] = {
	{
		.name = "mpio",
		.help = "MPIO commands",
		.chain = INIT_CHAIN(mpio_sub_commands),
	},
};

void cmd_mpio_setup(void) {
	command_register(mpio_commands);
}

