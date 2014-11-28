/*
 * cmd_com.c
 *
 *  Created on: 27/01/2010
 *      Author: johan
 */

#include <stdlib.h>
#include <string.h>
#include <io/nanocom.h>
#include <command/command.h>
#include <inttypes.h>
#include <util/log.h>

static uint8_t com_node = NODE_COM;
static uint32_t com_timeout = 2000;

int cmd_com_get_status(struct command_context *ctx) {
	nanocom_data_t data;
	if (!com_get_status(&data, com_node, com_timeout)) {
		printf("NETWORK ERROR\r\n");
		return CMD_ERROR_FAIL;
	}

	printf("Bits corrected total\t\t\t\t%u\r\n", (unsigned int) data.bit_corr_tot);
	printf("Bytes corrected total\t\t\t\t%u\r\n", (unsigned int) data.byte_corr_tot);
	printf("RX packets\t\t\t\t\t%u\r\n", (unsigned int) data.rx);
	printf("RX checksum errors\t\t\t\t%u\r\n", (unsigned int) data.rx_err);
	printf("TX packets\t\t\t\t\t%u\r\n", (unsigned int) data.tx);
	printf("Freq. Error\t\t\t\t\t%d\r\n", data.last_rferr);
	printf("Last RSSI\t\t\t\t\t%d\r\n", data.last_rssi);
	printf("Last A temp\t\t\t\t\t%d\r\n", data.last_temp_a);
	printf("Last B temp\t\t\t\t\t%d\r\n", data.last_temp_b);
	printf("Last TX current\t\t\t\t\t%u\r\n", data.last_txcurrent);
	printf("Last Battery Voltage\t\t\t\t%u\r\n", data.last_batt_volt);
	printf("Bootcount\t\t\t\t\t%u\r\n", (unsigned int) data.bootcount);

	return CMD_ERROR_NONE;
}

int cmd_com_get_log_rssi(struct command_context *ctx) {

	nanocom_rssi_t logd[10];
	uint8_t length = 10;
	if (!com_get_log_rssi(logd, &length, com_node, com_timeout)) {
		printf("NETWORK ERROR\r\n");
		return CMD_ERROR_FAIL;
	}
	int i;
	for (i = 0; i < length; i++) {
		printf("Time %"PRIu32",\tRSSI %"PRId16",\tRFERR %"PRId16"\r\n", logd[i].time, logd[i].rssi, logd[i].rferr);
	}

	return CMD_ERROR_NONE;

}

int cmd_com_get_log_hk(struct command_context *ctx) {

	nanocom_hk_t hkd[10];
	uint8_t length = 10;
	if (!com_get_hk(hkd, &length, com_node, com_timeout)) {
		printf("NETWORK ERROR\r\n");
		return CMD_ERROR_FAIL;
	}
	int i;
	for (i = 0; i < length; i++) {
		printf("Time %"PRIu32",\tTemp A %d,\tTemp B %d,\tBattV %u\r\n",hkd[i].time, hkd[i].temp_a, hkd[i].temp_b, hkd[i].batt_volt);
	}

	return CMD_ERROR_NONE;

}

int cmd_com_get_calibration(struct command_context *ctx) {

	nanocom_calibrate_t cal;
	if (!com_get_calibration(com_node, &cal, com_timeout)) {
		printf("NETWORK ERROR\r\n");
		return CMD_ERROR_FAIL;
	}

	printf("RAW rssi %d, rferr %d\t\tCON rssi %d dBm, rferr %d Hz\r\n", cal.raw_rssi, cal.raw_rferr, cal.con_rssi, cal.con_rferr);

	return CMD_ERROR_NONE;

}

nanocom_conf_t com_conf;

int cmd_com_get_conf(struct command_context *ctx) {
	com_get_conf(&com_conf, com_node, com_timeout);
	com_print_conf(&com_conf);
	return CMD_ERROR_NONE;
}

int cmd_com_set_conf(struct command_context *ctx) {
	com_set_conf(&com_conf, com_node, com_timeout);
	return CMD_ERROR_NONE;
}

int cmd_com_restore_conf(struct command_context *ctx) {
	com_restore_conf(com_node);
	return CMD_ERROR_NONE;
}

int cmd_com_conf_baud_rx(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int baud;
	if (sscanf(args, "%u", &baud) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting RX baud to %u\r\n", baud);
	com_conf.rx_baud = baud;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_baud_tx(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int baud;
	if (sscanf(args, "%u", &baud) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting TX baud to %u\r\n", baud);
	com_conf.tx_baud = baud;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_preamble(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int preamble;
	if (sscanf(args, "%u", &preamble) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting Preamble to %u\r\n", preamble);
	com_conf.preamble_length = preamble;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_max_temp(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	com_conf.tx_max_temp = atoi(ctx->argv[1]);
	printf("Setting max_temp to %"PRId16"\r\n", com_conf.tx_max_temp);
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_enable(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int enable;
	if (sscanf(args, "%u", &enable) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse enable to %u\r\n", enable);
	com_conf.morse_enable = enable;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_mode(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int value;
	if (sscanf(args, "%u", &value) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse mode to %u\r\n", value);
	com_conf.morse_mode = value;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_cycle(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int cycle;
	if (sscanf(args, "%u", &cycle) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse cycle to %u\r\n", cycle);
	com_conf.morse_cycle = cycle;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_interval(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int interval;
	if (sscanf(args, "%u", &interval) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse interval to %u\r\n", interval);
	com_conf.morse_inter_delay = interval;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_wpm(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int wpm;
	if (sscanf(args, "%u", &wpm) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse wpm to %u\r\n", wpm);
	com_conf.morse_wpm = wpm;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_batt_level(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int batt_level;
	if (sscanf(args, "%u", &batt_level) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse battery level to %u\r\n", batt_level);
	com_conf.morse_bat_level = batt_level;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_pospone(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int morse_pospone;
	if (sscanf(args, "%u", &morse_pospone) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse wpm to %u\r\n", morse_pospone);
	com_conf.morse_pospone = morse_pospone;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_toggle(struct command_context *ctx) {

	if (ctx->argc != 8)
		return CMD_ERROR_SYNTAX;

	/* <volt> <rxc> <txc> <tempa> <tempb> <rssi> <rferr> */
	com_conf.morse_en_voltage 	= (atoi(ctx->argv[1])) ? 1 : 0;
	com_conf.morse_en_rx_count 	= (atoi(ctx->argv[2])) ? 1 : 0;
	com_conf.morse_en_tx_count 	= (atoi(ctx->argv[3])) ? 1 : 0;
	com_conf.morse_en_temp_a 	= (atoi(ctx->argv[4])) ? 1 : 0;
	com_conf.morse_en_temp_b	= (atoi(ctx->argv[5])) ? 1 : 0;
	com_conf.morse_en_rssi 		= (atoi(ctx->argv[6])) ? 1 : 0;
	com_conf.morse_en_rf_err 	= (atoi(ctx->argv[7])) ? 1 : 0;

	printf("Setting morse toggles\r\n");
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_morse_string(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	memcpy(com_conf.morse_text, command_args(ctx), 20);
	printf("Setting morse text\r\n");
	com_print_conf(&com_conf);
	return CMD_ERROR_NONE;
}

int cmd_com_conf_do_viterbi(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int do_viterbi;
	if (sscanf(args, "%u", &do_viterbi) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse wpm to %u\r\n", do_viterbi);
	com_conf.do_viterbi = do_viterbi;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_do_rs(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int do_rs;
	sscanf(args, "%u", &do_rs);
	printf("Setting morse wpm to %u\r\n", do_rs);
	com_conf.do_rs = do_rs;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_do_random(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int do_random;
	if (sscanf(args, "%u", &do_random) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting morse wpm to %u\r\n", do_random);
	com_conf.do_random = do_random;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_conf_set_hkinterval(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int hkinterval;
	if (sscanf(args, "%u", &hkinterval) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting HK interval to %u\r\n", hkinterval);
	com_conf.hk_interval = hkinterval;
	com_print_conf(&com_conf);

	return CMD_ERROR_NONE;
}

int cmd_com_set_node(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int node_parse;
	if (sscanf(args, "%u", &node_parse) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting node to %u\r\n", node_parse);
	com_node = node_parse;

	return CMD_ERROR_NONE;
}

int cmd_com_set_tx_inhibit(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	uint8_t value = atoi(ctx->argv[1]);
	printf("Setting TX inhibit to %u\r\n", value);
	int result = com_set_tx_inhibit(com_node, value, 1000);
	printf("Result: %d\r\n", result);
	return CMD_ERROR_NONE;
}

command_t __sub_command com_subcommands[] = {
	{
		.name = "node",
		.help = "Set node",
		.usage = "<node>",
		.handler = cmd_com_set_node,
	},{
		.name = "gs",
		.help = "Get status",
		.handler = cmd_com_get_status,
	},{
		.name = "rssi",
		.help = "Get last RSSI readings",
		.handler = cmd_com_get_log_rssi,
	},{
		.name = "hk",
		.help = "Get last HK readings",
		.handler = cmd_com_get_log_hk,
	},{
		.name = "cal",
		.help = "Get calibration data",
		.handler = cmd_com_get_calibration,
	},{
		.name = "cg",
		.help = "Get configuration",
		.handler = cmd_com_get_conf,
	},{
		.name = "cs",
		.help = "Set configuration",
		.handler = cmd_com_set_conf,
	},{
		.name = "cr",
		.help = "Restore configuration",
		.handler = cmd_com_restore_conf,
	},{
		.name = "preamble",
		.help = "Configure Preamble in [ms]",
		.usage = "<ms>",
		.handler = cmd_com_conf_preamble,
	},{
		.name = "baudrx",
		.help = "Configure baud RX",
		.usage = "<speed>",
		.handler = cmd_com_conf_baud_rx,
	},{
		.name = "baudtx",
		.help = "Configure baud TX",
		.usage = "<speed>",
		.handler = cmd_com_conf_baud_tx,
	},{
		.name = "maxtemp",
		.help = "Automatic switch off tx at max temperature",
		.usage = "<degC>",
		.handler = cmd_com_conf_max_temp,
	},{
		.name = "menable",
		.help = "Configure Morse Enable",
		.usage = "<1=on|0=off>",
		.handler = cmd_com_conf_morse_enable,
	},{
		.name = "mmode",
		.help = "Configure Morse Mode",
		.usage = "<0=CW, 1=FM+CW, 2=FM>",
		.handler = cmd_com_conf_morse_mode,
	},{
		.name = "mcycle",
		.help = "Enable cycling of morse parameters",
		.usage = "<1=on|0=off>",
		.handler = cmd_com_conf_morse_cycle,
	},{
		.name = "minterval",
		.help = "Configure Morse interval",
		.usage = "<seconds>",
		.handler = cmd_com_conf_morse_interval,
	},{
		.name = "mwpm",
		.help = "Configure Morse wpm",
		.usage = "<wpm>",
		.handler = cmd_com_conf_morse_wpm,
	},{
		.name = "mbatt",
		.help = "Configure Morse battery level",
		.usage = "<level>",
		.handler = cmd_com_conf_morse_batt_level,
	},{
		.name = "mpone",
		.help = "Configure Morse postpone",
		.usage = "<seconds>",
		.handler = cmd_com_conf_morse_pospone,
	},{
		.name = "mtoggle",
		.help = "Toggle morse settings",
		.usage = "<volt> <rxc> <txc> <tempa> <tempb> <rssi> <rferr>",
		.handler = cmd_com_conf_morse_toggle,
	},{
		.name = "mtext",
		.help = "Set morse text",
		.usage = "<string>",
		.handler = cmd_com_conf_morse_string,
	},{
		.name = "vit",
		.help = "Configure do Viterbi",
		.usage = "<bool>",
		.handler = cmd_com_conf_do_viterbi,
	},{
		.name = "rs",
		.help = "Configure do Reed-Solomon",
		.usage = "<bool>",
		.handler = cmd_com_conf_do_rs,
	},{
		.name = "random",
		.help = "Configure do randomization",
		.usage = "<bool>",
		.handler = cmd_com_conf_do_random,
	},{
		.name = "hkinterval",
		.help = "Configure hk interval",
		.usage = "<seconds>",
		.handler = cmd_com_conf_set_hkinterval,
	},{
		.name = "inhibit",
		.help = "Set the TX inhibit flag",
		.usage = "<value>",
		.handler = cmd_com_set_tx_inhibit,
	}
};

command_t __root_command com_commands[] = {
	{
		.name = "com",
		.help = "COM subsystem",
		.chain = INIT_CHAIN(com_subcommands),
	}
};

void cmd_com_setup(int node_arg) {
	com_node = node_arg;
	command_register(com_commands);
}
