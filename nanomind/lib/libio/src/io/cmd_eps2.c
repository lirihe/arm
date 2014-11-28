/**
 * NanoPower firmware
 *
 * @author Morten Bisgaard
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <inttypes.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <io/nanopower2.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <util/console.h>
#include <util/log.h>

eps_config_t nanopower_config;
eps_config2_t nanopower_config2;

int cmd_eps_node(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	eps_set_node(atoi(ctx->argv[1]));
	return CMD_ERROR_NONE;
}

int cmd_eps_hk_get(struct command_context *ctx) {
	eps_hk_t hk = {};
	if (eps_hk_get(&hk) >= 0)
		eps_hk_print(&hk);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_eps_hk_vi_get(struct command_context *ctx) {
	eps_hk_vi_t hk = {};
	if (eps_hk_vi_get(&hk) >= 0)
		eps_hk_vi_print_text(&hk);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_eps_hk_out_get(struct command_context *ctx) {
	eps_hk_out_t hk = {};
	if (eps_hk_out_get(&hk) >= 0)
		eps_hk_out_print_text(&hk);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_eps_hk_wdt_get(struct command_context *ctx) {
	eps_hk_wdt_t hk = {};
	if (eps_hk_wdt_get(&hk) >= 0)
		eps_hk_wdt_print_text(&hk);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_eps_hk_basic_get(struct command_context *ctx) {
	eps_hk_basic_t hk = {};
	if (eps_hk_basic_get(&hk) >= 0)
		eps_hk_basic_print_text(&hk);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_eps_config_get(struct command_context *ctx) {
	eps_config_get(&nanopower_config);
	eps_config_print(&nanopower_config);
	return CMD_ERROR_NONE;
}
int cmd_eps_config_print(struct command_context *ctx) {
	eps_config_print(&nanopower_config);
	return CMD_ERROR_NONE;
}
int cmd_eps_config_set(struct command_context *ctx) {
	eps_config_set(&nanopower_config);
	return CMD_ERROR_NONE;
}
int cmd_eps_config_restore(struct command_context *ctx) {
	eps_config_cmd(1);
	return CMD_ERROR_NONE;
}
int cmd_eps_config_edit(struct command_context *ctx) {

	int i;
	int scan;
	int ret __attribute__((unused));

	printf("Edit EPS configuration\r\n");
	printf("Press <.> to skip\r\n");

	printf("PPT mode: %u > ", nanopower_config.ppt_mode);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config.ppt_mode = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config.ppt_mode);
	ret = scanf("%*c");

	printf("Battheater mode: %u > ", nanopower_config.battheater_mode);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config.battheater_mode = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config.battheater_mode);
	ret = scanf("%*c");

	printf("Battheater LOW: %u > ", nanopower_config.battheater_low);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config.battheater_low = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config.battheater_low);
	ret = scanf("%*c");

	/* PPT mode */
	printf("Battheater HIGH: %u > ", nanopower_config.battheater_high);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config.battheater_high = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config.battheater_high);
	ret = scanf("%*c");

	/* VBOOST [3] */
	printf("VBOOST voltage[3]\r\n");
	for (i = 0; i < 3; i++) {
		printf("vboost[i] %u > ", nanopower_config.vboost[i]);
		fflush(stdout);
		if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
			nanopower_config.vboost[i] = scan;
		printf(" [%u] > %u\r\n", ret, nanopower_config.vboost[i]);
		ret = scanf("%*c");
	}

	/* OUTPUT[8] */
	for (i = 0; i < 8; i++) {

		printf("OUTPUT[%u]\r\n", i);

		/* Normal Mode */
		printf("Startup value[%u] %u > ", i, nanopower_config.output_normal_value[i]);
		fflush(stdout);
		if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
			nanopower_config.output_normal_value[i] = scan;
		printf(" [%u] > %u\r\n", ret, nanopower_config.output_normal_value[i]);
		ret = scanf("%*c");

		/* Safe Mode */
		printf("Safe value[%u] %u > ", i, nanopower_config.output_safe_value[i]);
		fflush(stdout);
		if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
			nanopower_config.output_safe_value[i] = scan;
		printf(" [%u] > %u\r\n", ret, nanopower_config.output_safe_value[i]);
		ret = scanf("%*c");

		/* ON delay */
		printf("ON-delay[%u] %u > ", i, nanopower_config.output_initial_on_delay[i]);
		fflush(stdout);
		if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
			nanopower_config.output_initial_on_delay[i] = scan;
		printf(" [%u] > %u\r\n", ret, nanopower_config.output_initial_on_delay[i]);
		ret = scanf("%*c");

		/* OFF delay */
		printf("OFF-delay[%u] %u > ", i, nanopower_config.output_initial_off_delay[i]);
		fflush(stdout);
		if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
			nanopower_config.output_initial_off_delay[i] = scan;
		printf(" [%u] > %u\r\n", ret, nanopower_config.output_initial_off_delay[i]);
		ret = scanf("%*c");

	}

	return CMD_ERROR_NONE;

}


int cmd_eps_config2_get(struct command_context *ctx) {
	eps_config2_get(&nanopower_config2);
	eps_config2_print(&nanopower_config2);
	return CMD_ERROR_NONE;
}
int cmd_eps_config2_print(struct command_context *ctx) {
	eps_config2_print(&nanopower_config2);
	return CMD_ERROR_NONE;
}
int cmd_eps_config2_set(struct command_context *ctx) {
	eps_config2_set(&nanopower_config2);
	return CMD_ERROR_NONE;
}
int cmd_eps_config2_restore(struct command_context *ctx) {
	eps_config2_cmd(1);
	return CMD_ERROR_NONE;
}
int cmd_eps_config2_confirm(struct command_context *ctx) {
	eps_config2_cmd(2);
	return CMD_ERROR_NONE;
}
int cmd_eps_config2_edit(struct command_context *ctx) {

	int scan;
	int ret __attribute__((unused));

	printf("Edit EPS configuration 2\r\n");
	printf("Press <.> to skip\r\n");
	printf("You are about to change battery level voltages.\r\n");
	printf("Make sure that: maxvolt > normalvolt > safevolt > criticalvolt \r\n\r\n");

	printf("Batt max volt: %u > ", nanopower_config2.batt_maxvoltage);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config2.batt_maxvoltage = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config2.batt_maxvoltage);
	printf("You are trying to set battery protection maximum voltage to %u mV\r\n",nanopower_config2.batt_maxvoltage);
	printf("It is VERY important that you NEVER set the max voltage to more than what the installed battery can handle!!!!\r\n\r\n");

	ret = scanf("%*c");
	fflush(stdin);
	printf("Batt normal volt: %u > ", nanopower_config2.batt_normalvoltage);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config2.batt_normalvoltage = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config2.batt_normalvoltage);
	ret = scanf("%*c");
	fflush(stdin);

	printf("Batt safe volt: %u > ", nanopower_config2.batt_safevoltage);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config2.batt_safevoltage = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config2.batt_safevoltage);
	ret = scanf("%*c");
	fflush(stdin);

	printf("Batt critical volt: %u > ", nanopower_config2.batt_criticalvoltage);
	fflush(stdout);
	if ((ret = scanf("%u%*[^\r]", &scan)) == 1)
		nanopower_config2.batt_criticalvoltage = scan;
	printf(" [%u] > %u\r\n", ret, nanopower_config2.batt_criticalvoltage);
	ret = scanf("%*c");
	fflush(stdin);


	if (nanopower_config2.batt_maxvoltage <= nanopower_config2.batt_normalvoltage)
		printf("\r\nWoops, you  have set max volt to less that normal volt, please change this!\r\n");
	if (nanopower_config2.batt_normalvoltage <= nanopower_config2.batt_safevoltage)
		printf("\r\nWoops, you  have set normal volt to less that safe volt, please change this!\r\n");
	if (nanopower_config2.batt_safevoltage <= nanopower_config2.batt_criticalvoltage)
		printf("\r\nWoops, you  have set safe volt to less that critical volt, please change this!\r\n");


	return CMD_ERROR_NONE;

}



int cmd_eps_output(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int outputs;
	printf("console args: %s\r\n", args);
	if (sscanf(args, "%X", &outputs) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Outputs 0x%02X\r\n", outputs);

	eps_output_set((uint8_t) outputs);

	return CMD_ERROR_NONE;

}

int cmd_eps_output_set_single(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int channel;
	unsigned int mode;
	int delay;
	printf("Input channel, mode (0=off, 1=on), and delay\r\n");
	if (sscanf(args, "%u %u %d", &channel,&mode,&delay) != 3)
		return CMD_ERROR_SYNTAX;
	printf("Channel %d is set to %d with delay %d\r\n", channel,mode,delay);

	eps_output_set_single((uint8_t) channel, (uint8_t) mode, (int16_t) delay);

	return CMD_ERROR_NONE;

}

int cmd_eps_counters_reset(struct command_context *ctx) {
	eps_counters_reset();
	return CMD_ERROR_NONE;
}

int cmd_eps_hardreset(struct command_context *ctx) {
	eps_hardreset();
	return CMD_ERROR_NONE;
}

int cmd_eps_gnd_reset(struct command_context *ctx) {
	eps_wdt_gnd_reset();
	return CMD_ERROR_NONE;
}

int cmd_eps_ppt_set(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	unsigned char pptmode = atoi(ctx->argv[1]);
	eps_pptmode_set(pptmode);
	return CMD_ERROR_NONE;
}

int cmd_eps_vboost_set(struct command_context *ctx) {
	if (ctx->argc != 4)
		return CMD_ERROR_SYNTAX;
	unsigned int vboost1 = atoi(ctx->argv[1]);
	unsigned int vboost2 = atoi(ctx->argv[2]);
	unsigned int vboost3 = atoi(ctx->argv[3]);
	eps_vboost_set(vboost1, vboost2, vboost3);
	return CMD_ERROR_NONE;
}

command_t __sub_command eps_config_subcommands[] = {
	{
		.name = "get",
		.help = "Conf get",
		.handler = cmd_eps_config_get,
	},{
		.name = "set",
		.help = "Conf set",
		.handler = cmd_eps_config_set,
	},{
		.name = "edit",
		.help = "Edit local config",
		.handler = cmd_eps_config_edit,
	},{
		.name = "print",
		.help = "Print local config",
		.handler = cmd_eps_config_print,
	},{
		.name = "restore",
		.help = "Restore config from default",
		.handler = cmd_eps_config_restore,
	}
};

command_t __sub_command eps_config2_subcommands[] = {
	{
		.name = "get",
		.help = "Conf 2 get",
		.handler = cmd_eps_config2_get,
	},{
		.name = "set",
		.help = "Conf 2 set",
		.handler = cmd_eps_config2_set,
	},{
		.name = "edit",
		.help = "Edit local config 2",
		.handler = cmd_eps_config2_edit,
	},{
		.name = "print",
		.help = "Print local config 2",
		.handler = cmd_eps_config2_print,
	},{
		.name = "restore",
		.help = "Restore config 2 from default",
		.handler = cmd_eps_config2_restore,
	},{
		.name = "confirm",
		.help = "Confirm config 2",
		.handler = cmd_eps_config2_confirm,
	}
};


command_t __sub_command eps_hk_subcommands[] = {
	{
		.name = "vi",
		.help = "Get Voltages and Currents",
		.handler = cmd_eps_hk_vi_get,
	},{
		.name = "out",
		.help = "Get output switch data",
		.handler = cmd_eps_hk_out_get,
	},{
		.name = "wdt",
		.help = "Get wdt data",
		.handler = cmd_eps_hk_wdt_get,
	},{
		.name = "basic",
		.help = "Get basic housekeeping data",
		.handler = cmd_eps_hk_basic_get,
	}
};



command_t __sub_command eps_subcommands[] = {
	{
		.name = "node",
		.help = "Set EPS address in OBC host table",
		.usage = "<node>",
		.handler = cmd_eps_node,
	},{
		.name = "hk",
		.help = "Get HK",
		.handler = cmd_eps_hk_get,
	},{
		.name = "hksub",
		.help = "Get HK sub structs",
		.chain = INIT_CHAIN(eps_hk_subcommands),
	},{
		.name = "conf",
		.help = "Configuration",
		.chain = INIT_CHAIN(eps_config_subcommands),
	},{
		.name = "conf2",
		.help = "Configuration 2",
		.chain = INIT_CHAIN(eps_config2_subcommands),
	}, {
		.name = "outputmask",
		.help = "Set on/off, argument hex value of output char",
		.usage = "<outputs>",
		.handler = cmd_eps_output,
	},{
		.name = "output",
		.help = "Set channel on/off, argument (channel) to (1 or 0)",
		.usage = "<channel> <mode> <delay>",
		.handler = cmd_eps_output_set_single,
	},{
		.name = "resetcounters",
		.help = "Reset all counters to zero",
		.handler = cmd_eps_counters_reset,
	},{
		.name = "resetwdt",
		.help = "Resets the WDT GND",
		.handler = cmd_eps_gnd_reset,
	},{
		.name = "hardreset",
		.help = "Completely powercycle EPS",
		.handler = cmd_eps_hardreset,
	},{
		.name = "ppt",
		.help = "change current PPT mode",
		.usage = "<mode>",
		.handler = cmd_eps_ppt_set,
	},{
		.name = "vboost",
		.help = "change current VBOOST voltage",
		.usage = "<vboost1> <vboost2> <vboost3>",
		.handler = cmd_eps_vboost_set,
	}
};

command_t __root_command eps_commands[] = {
	{
		.name = "eps",
		.help = "EPS subsystem",
		.chain = INIT_CHAIN(eps_subcommands),
	}
};

void cmd_eps_setup(void) {
	command_register(eps_commands);
}
