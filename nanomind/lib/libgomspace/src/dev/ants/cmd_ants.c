/**
 * ISIS AntS Driver
 *
 * @brief Command line functions for controlling the antenna subsystem.
 * Warning: Do not use these functions while the flightplanner is also
 * using the antenna subsystem.
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>
#include <inttypes.h>

#include <util/console.h>
#include <util/error.h>
#include <util/hexdump.h>
#include <util/log.h>

#include <dev/isis_ants.h>

static uint8_t cmd_antx_current_addr = ISIS_ANTS_DFL_ADDR_A;

int cmd_ants_status(struct command_context *ctx) {

	/* Deployment status */
	isis_ants_status_t status;
	if (isis_ants_status(cmd_antx_current_addr, &status) == 0) {

		printf("Antenna Deployer Status:\r\n");

		if (status.armed)
			printf("\tARMED\r\n");
		else
			printf("\tNot armed\r\n");

		if (status.switch_ignore)
			printf("\tIGNORING SWITCHES\r\n");
		else
			printf("\tNot ingnoring switches\r\n");

		printf("\tAntenna 1: Switch ");
		if (status.ant[0].not_deployed)
			printf("not deployed");
		else
			printf("DEPLOYED");

		printf(", Timer ");
		if (status.ant[0].time_limit_reached)
			printf("REACHED");
		else
			printf("not reached");

		printf(", Currently ");
		if (status.ant[0].deployment_active)
			printf("active");
		else
			printf("not active");

		printf(", Count %"PRIu8, status.ant[0].activation_count);
		printf(", Time %"PRIu16, status.ant[0].activation_time);

		printf("\r\n");

		printf("\tAntenna 2: Switch ");
		if (status.ant[1].not_deployed)
			printf("not deployed");
		else
			printf("DEPLOYED");

		printf(", Timer ");
		if (status.ant[1].time_limit_reached)
			printf("REACHED");
		else
			printf("not reached");

		printf(", Currently ");
		if (status.ant[1].deployment_active)
			printf("active");
		else
			printf("not active");

		printf(", Count %"PRIu8, status.ant[1].activation_count);
		printf(", Time %"PRIu16, status.ant[1].activation_time);

		printf("\r\n");

		printf("\tAntenna 3: Switch ");
		if (status.ant[2].not_deployed)
			printf("not deployed");
		else
			printf("DEPLOYED");

		printf(", Timer ");
		if (status.ant[2].time_limit_reached)
			printf("REACHED");
		else
			printf("not reached");

		printf(", Currently ");
		if (status.ant[2].deployment_active)
			printf("active");
		else
			printf("not active");

		printf(", Count %"PRIu8, status.ant[2].activation_count);
		printf(", Time %"PRIu16, status.ant[2].activation_time);

		printf("\r\n");

		printf("\tAntenna 4: Switch ");
		if (status.ant[3].not_deployed)
			printf("not deployed");
		else
			printf("DEPLOYED");

		printf(", Timer ");
		if (status.ant[3].time_limit_reached)
			printf("REACHED");
		else
			printf("not reached");

		printf(", Currently ");
		if (status.ant[3].deployment_active)
			printf("active");
		else
			printf("not active");

		printf(", Count %"PRIu8, status.ant[3].activation_count);
		printf(", Time %"PRIu16, status.ant[3].activation_time);

		printf("\r\n");
	}

	/* Temperature */
	uint16_t temp_data;
	if (isis_ants_temp(cmd_antx_current_addr, &temp_data) == 0) {
		printf("\tTemperature sensor reading: %f mV\r\n", (3.3/1024) * temp_data);
	}

	return CMD_ERROR_NONE;
}

int cmd_ants_deploy_single(struct command_context *ctx) {

	char * args = command_args(ctx);
	int isis_ant;
	unsigned int override, time_sec;
	if (args == NULL || sscanf(args, "%d %u %u", &isis_ant, &time_sec, &override) != 3)
		return CMD_ERROR_SYNTAX;

	printf("Deploying antenna %d with %"PRIu8" seconds timeout, override %u\r\n", isis_ant, time_sec, override);

	int status = isis_ants_deploy_single(cmd_antx_current_addr, isis_ant, time_sec, override);
	printf("Deploy status %d\r\n", status);

	return CMD_ERROR_NONE;

}

int cmd_ants_deploy_auto(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int time_sec;
	if (args == NULL || sscanf(args, "%u", &time_sec) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Deploying antennas with %"PRIu8" seconds timeout\r\n", time_sec);

	int status = isis_ants_deploy_auto(cmd_antx_current_addr, time_sec);
	printf("Deploy status %d\r\n", status);

	return CMD_ERROR_NONE;

}

int cmd_ants_deploy_cancel(struct command_context *ctx) {

	int status = isis_ants_deploy_cancel(cmd_antx_current_addr);
	printf("Cancel status %d\r\n", status);
	return CMD_ERROR_NONE;

}

int cmd_ants_disarm(struct command_context *ctx) {

	int status = isis_ants_disarm(cmd_antx_current_addr);
	printf("Disarm status %d\r\n", status);
	return CMD_ERROR_NONE;

}

int cmd_ants_arm(struct command_context *ctx) {

	int status = isis_ants_arm(cmd_antx_current_addr);
	printf("Arm status %d\r\n", status);
	return CMD_ERROR_NONE;

}

int cmd_ants_reset(struct command_context *ctx) {

	int status = isis_ants_reset(cmd_antx_current_addr);
	printf("Reset status %d\r\n", status);
	return CMD_ERROR_NONE;

}

int cmd_ants_addr(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int addr;
	if (args == NULL || sscanf(args, "%x", &addr) != 1)
		return CMD_ERROR_SYNTAX;

	cmd_antx_current_addr = addr;
	printf("Set cmd_antx_curerent_addr = %hhx", cmd_antx_current_addr);

	return CMD_ERROR_NONE;

}

command_t __sub_command ant_subcommands[] = {
	{
		.name = "status",
		.help = "Antenna status",
		.handler = cmd_ants_status,
	},{
		.name = "deploy",
		.help = "Antenna deploy single [1-4] [time sec]",
		.usage = "<antenna number> <timeout> <override>",
		.handler = cmd_ants_deploy_single,
	},{
		.name = "autodeploy",
		.help = "Antenna deploy auto [time sec]",
		.usage = "<timeout>",
		.handler = cmd_ants_deploy_auto,
	},{
		.name = "cancel",
		.help = "Antenna deploy cancel",
		.handler = cmd_ants_deploy_cancel,
	},{
		.name = "disarm",
		.help = "Antenna disarm",
		.handler = cmd_ants_disarm,
	},{
		.name = "arm",
		.help = "Antenna arm",
		.handler = cmd_ants_arm,
	},{
		.name = "reset",
		.help = "Antenna reset",
		.handler = cmd_ants_reset,
	},{
		.name = "addr",
		.help = "Antenna set A/B. Address 0 = A, otherwise B",
		.usage = "<address>",
		.handler = cmd_ants_addr,
	},
};

command_t __root_command ant_commands[] = {
	{
		.name = "ant",
		.help = "Antenna subsystem",
		.chain = INIT_CHAIN(ant_subcommands),
	},
};

void cmd_ants_setup(void) {
	command_register(ant_commands);
}

