/**
 * Command-line interface to logging functions
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <supervisor/supervisor.h>

#include <util/log.h>
#include <util/console.h>
#include <util/timestamp.h>

command_t __root_command sv_commands[] = {
	{
		.name = "sv",
		.help = "Print supervisor table",
		.handler = sv_print,
	},
};

/** cmd_sv_setup
 * Setup supervisor command
 */
void cmd_sv_setup(void) {
	command_register(sv_commands);
}
