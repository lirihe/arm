/**
 * @file gatoss_cmd.c
 * GATOSS Microcontroller
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <command/command.h>
#include <io/gatoss.h>
#include <util/color_printf.h>
#include <util/timestamp.h>
#include <util/clock.h>

gatoss_conf_t gatoss_conf;
int conf_initialized = 0;

int gatoss_cmd_set_node(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	gatoss_set_node(atoi(ctx->argv[1]));
	return CMD_ERROR_NONE;
}

int gatoss_cmd_power(unsigned int state, struct command_context * ctx) {

	uint16_t channels = 0;

	int i;
	for (i = 1; i < ctx->argc; i++) {
		int channel = atoi(ctx->argv[i]);
		if (channel >= 1 && channel <= 9) {
			channels |= 1 << (channel - 1);
		} else {
			if (strcmp("all", ctx->argv[i]) != 0) continue;
			channels = GATOSS_POWER_CHANNEL_ALL;
		}
	}
	printf("Setting channels 0x%03"PRIx16" to %u\r\n", channels, state);

	uint16_t channel_status;
	if (gatoss_power(channels, state, &channel_status) > 0)
		printf("Remote power state: 0x%03"PRIx16"\r\n", channel_status);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_power_on(struct command_context * ctx) {
	return gatoss_cmd_power(1, ctx);
}

int gatoss_cmd_power_off(struct command_context * ctx) {
	return gatoss_cmd_power(0, ctx);
}

int gatoss_cmd_power_status(struct command_context * ctx) {
	uint16_t channel_status;
	if (gatoss_power(0, 0, &channel_status) > 0)
		printf("Remote power state: 0x%03"PRIx16"\r\n", channel_status);
	return CMD_ERROR_NONE;
}

int gatoss_cmd_hk(struct command_context * ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint8_t save = atoi(ctx->argv[1]);

	struct gatoss_hk hk;
	int ret = gatoss_hk(&hk, save);
	if (ret < 0) {
		printf("Error getting hk %u\r\n", ret);
		return CMD_ERROR_FAIL;
	}

	printf("Current usages:\r\n");
	printf("\t3.3V (BOARD): %"PRIu16" mA\r\n", hk.current_3v3_board);
	printf("\t5.0V (BOARD): %"PRIu16" mA\r\n", hk.current_5v0_board);
	printf("\t1.2V: %"PRIu16" mA\r\n", hk.current_1v2);
	printf("\t2.5V: %"PRIu16" mA\r\n", hk.current_2v5);
	printf("\t3.3V (FPGA): %"PRIu16" mA\r\n", hk.current_3v3_fpga);
	printf("\t3.3V (ADC): %"PRIu16" mA\r\n", hk.current_3v3_adc);
	printf("\r\n");
	printf("Bootcause: %"PRIu16"\r\n", hk.bootcause);
	printf("Bootcount: %"PRIu32"\r\n", hk.bootcount);
	printf("Average fps 10 sec: %u\r\n", hk.average_fps_10sec);
	printf("Average fps 1 min: %u\r\n", hk.average_fps_1min);
	printf("Average fps 5 min: %u\r\n", hk.average_fps_5min);
	printf("Frames: %"PRIu32"\r\n", hk.frame_count);
	printf("Planes: %"PRIu16"\r\n", hk.plane_count);
	printf("CRC corrected: %"PRIu32"\r\n", hk.crc_corrected);
	printf("Time %"PRIu32"\r\n", hk.current_time);
	printf("----- LAST POSITION -----\r\n");
	printf("ICAO: %"PRIx32", Lat: %f, Lon: %f, Alt: %"PRIu32", Time: %"PRIu32"\r\n", hk.last_icao24, hk.last_pos.lat, hk.last_pos.lon, hk.last_pos.altitude, hk.last_pos.timestamp);
	printf("----- PERSISTENT VARS -----\r\n");
	printf("Total frames: %"PRIu32"\r\n", hk.tot_frames);
	printf("Total planes: %"PRIu32"\r\n", hk.tot_planes);
	printf("Total CRC corrected: %"PRIu32"\r\n", hk.tot_crc_corrected);
	printf("FPGA crc count: %"PRIu32"\r\n", hk.fpga_crc_cnt);

	return CMD_ERROR_NONE;

}

int gatoss_cmd_load_fpga(struct command_context * ctx) {

	if (ctx->argc != 4)
		return CMD_ERROR_SYNTAX;

	uint8_t load_now = atoi(ctx->argv[2]);
	uint8_t store_as_dfl = atoi(ctx->argv[3]);

	int result = gatoss_load_fpga(ctx->argv[1], load_now, store_as_dfl);
	printf("Load result %d\r\n", result);

	if (result < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}

int gatoss_cmd_load_uc(struct command_context * ctx) {

	if (ctx->argc != 4)
		return CMD_ERROR_SYNTAX;

	uint8_t load_now = atoi(ctx->argv[2]);
	uint8_t store_as_dfl = atoi(ctx->argv[3]);

	int result = gatoss_load_uc(ctx->argv[1], load_now, store_as_dfl);
	printf("Load result %d\r\n", result);

	if (result < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}

int gatoss_cmd_list_short(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint32_t filtering = atoi(ctx->argv[1]);

	if (gatoss_list_short(filtering) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_list_full(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint32_t filtering = atoi(ctx->argv[1]);

	if (gatoss_list_full(filtering) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_plane_full(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint32_t icao24;
	if (sscanf(ctx->argv[1], "%x", (unsigned int *) &icao24) != 1)
		return CMD_ERROR_SYNTAX;

	struct gatoss_plane_full plane;

	int count = gatoss_plane_full(&plane, icao24);
	if (count < 0)
		return CMD_ERROR_FAIL;

	gatoss_pos_t pos;
	gatoss_positions_decompress(&plane.compos, &pos);

	if (plane.icao24 == 0) {
		color_printf(COLOR_RED, "No such plane %x\r\n", icao24);
		return CMD_ERROR_FAIL;
	}

	color_printf(COLOR_GREEN, "[%06"PRIX32"] ", plane.icao24);
	color_printf(COLOR_GREEN, "<%8s>", plane.id);
	color_printf(COLOR_GREEN, " C: %4"PRIu16"/%4"PRIu16" [%4"PRIu32"-%4"PRIu32"]", plane.position_counter, plane.frame_counter, plane.first_contact, plane.last_contact);
	color_printf(COLOR_GREEN, " La: %7.03f", pos.lat);
	color_printf(COLOR_GREEN, " Lo: %7.03f", pos.lon);
	color_printf(COLOR_GREEN, " A: %5"PRIu32, pos.altitude);
	color_printf(COLOR_GREEN, " H: %3.00f", plane.heading / 150.0);
	color_printf(COLOR_GREEN, " S: %03"PRIu16, plane.speed);
	color_printf(COLOR_GREEN, "\r\n");

	int i;
	printf("Positions received: %u\r\n", count);
	for (i = 0; i < count; i++) {
		gatoss_positions_decompress(&plane.positions[i], &pos);
		color_printf(COLOR_BLUE, "\t[%5"PRIu32"], La: %7.03f, Lo: %7.03f, A: %5"PRIu32"\r\n", pos.timestamp, pos.lat, pos.lon, pos.altitude);
	}

	return CMD_ERROR_NONE;
}

int gatoss_cmd_plane_positions(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	uint32_t icao24;
	if (sscanf(ctx->argv[1], "%x", (unsigned int *) &icao24) != 1)
		return CMD_ERROR_SYNTAX;

	uint32_t decimation = atoi(ctx->argv[2]);

	if (gatoss_plane_positions(icao24, decimation) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_plane_contacts(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint32_t icao24;
	if (sscanf(ctx->argv[1], "%x", (unsigned int *) &icao24) != 1)
		return CMD_ERROR_SYNTAX;

	if (gatoss_plane_contacts(icao24) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_file_all_frames(struct command_context *ctx) {

	if (ctx->argc != 5)
		return CMD_ERROR_SYNTAX;

	uint32_t start_time = atoi(ctx->argv[1]);
	uint32_t run_duration = atoi(ctx->argv[2]);
	uint32_t max_size = atoi(ctx->argv[3]);
	char * path = ctx->argv[4];

	if (gatoss_file_all_frames(start_time, run_duration, max_size, path) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_file_all_positions(struct command_context *ctx) {

	if (ctx->argc != 5)
		return CMD_ERROR_SYNTAX;

	uint32_t start_time = atoi(ctx->argv[1]);
	uint32_t run_duration = atoi(ctx->argv[2]);
	uint32_t max_size = atoi(ctx->argv[3]);
	char * path = ctx->argv[4];

	if (gatoss_file_all_positions(start_time, run_duration, max_size, path) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_file_plane_positions(struct command_context *ctx) {

	if (ctx->argc != 6)
		return CMD_ERROR_SYNTAX;

	uint32_t icao24;
	if (sscanf(ctx->argv[1], "%x", (unsigned int *) &icao24) != 1)
		return CMD_ERROR_SYNTAX;

	uint32_t begin_time = atoi(ctx->argv[2]);
	uint32_t end_time = atoi(ctx->argv[3]);
	uint32_t max_size = atoi(ctx->argv[4]);
	char * path = ctx->argv[5];

	struct gatoss_file_plane_reply reply;

	if (gatoss_file_plane(icao24, 0, begin_time, end_time, max_size, path, &reply) < 0)
		return CMD_ERROR_FAIL;

	printf("Blob: 0x%08"PRIX32" %"PRIu32"\r\n", reply.mem_addr, reply.mem_size);

	return CMD_ERROR_NONE;

}

int gatoss_cmd_file_plane_frames(struct command_context *ctx) {

	if (ctx->argc != 6)
		return CMD_ERROR_SYNTAX;

	uint32_t icao24;
	if (sscanf(ctx->argv[1], "%x", (unsigned int *) &icao24) != 1)
		return CMD_ERROR_SYNTAX;

	uint32_t begin_time = atoi(ctx->argv[2]);
	uint32_t end_time = atoi(ctx->argv[3]);
	uint32_t max_size = atoi(ctx->argv[4]);
	char * path = ctx->argv[5];

	struct gatoss_file_plane_reply reply;

	if (gatoss_file_plane(icao24, 1, begin_time, end_time, max_size, path, &reply) < 0)
		return CMD_ERROR_FAIL;

	printf("Blob: 0x%08"PRIX32" %"PRIu32"\r\n", reply.mem_addr, reply.mem_size);

	return CMD_ERROR_NONE;

}

int gatoss_cmd_file_raw_samples(struct command_context *ctx) {

	if (ctx->argc != 6)
		return CMD_ERROR_SYNTAX;

	uint8_t noise_sample = atoi(ctx->argv[1]);
	uint32_t timeout = atoi(ctx->argv[2]);
	uint32_t max_count = atoi(ctx->argv[3]);
	int32_t threshold = atoi(ctx->argv[4]);
	char * path = ctx->argv[5];

	if (gatoss_file_raw_samples(noise_sample, timeout, max_count, threshold, path) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int gatoss_cmd_threshold(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	int32_t amount = atoi(ctx->argv[1]);

	if (gatoss_threshold(amount) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}

int gatoss_cmd_sel_adc(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	int8_t adc = atoi(ctx->argv[1]);

	if (gatoss_sel_adc(adc) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}

int gatoss_cmd_conf_get(struct command_context *ctx) {
	gatoss_conf_get(&gatoss_conf);
	conf_initialized = 1;
	gatoss_conf_print(&gatoss_conf);
	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_set(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	gatoss_conf_print(&gatoss_conf);
	gatoss_conf_set(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_restore(struct command_context *ctx) {
	gatoss_conf_restore();
	conf_initialized = 0;
	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_fpga(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	if (ctx->argc != 8)
		return CMD_ERROR_SYNTAX;

	uint8_t image = atoi(ctx->argv[1]);
	int8_t wdt = atoi(ctx->argv[2]);
	int32_t threshold_a = atoi(ctx->argv[3]);
	int32_t threshold_b = atoi(ctx->argv[4]);
	int32_t threshold_count = atoi(ctx->argv[5]);
	int8_t adc = atoi(ctx->argv[6]);
	int16_t adc_ival = atoi(ctx->argv[7]);

	gatoss_conf.fpga_image = image;
	gatoss_conf.fpga_wdt_enable = wdt;
	gatoss_conf.fpga_threshold_a = threshold_a;
	gatoss_conf.fpga_threshold_b = threshold_b;
	gatoss_conf.fpga_threshold_count = threshold_count;
	gatoss_conf.fpga_adc = adc;
	gatoss_conf.fpga_adc_switch_interval = adc_ival;

	gatoss_conf_print(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_passive_interval(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	if (ctx->argc != 7)
		return CMD_ERROR_SYNTAX;

	gatoss_conf.passive_inhibit = atoi(ctx->argv[1]);
	gatoss_conf.passive_interval_list_short = atoi(ctx->argv[2]);
	gatoss_conf.passive_interval_list_full = atoi(ctx->argv[3]);
	gatoss_conf.passive_interval_plane = atoi(ctx->argv[4]);
	gatoss_conf.passive_live_min_tx_interval = atoi(ctx->argv[5]);
	gatoss_conf.passive_live_interval = atoi(ctx->argv[6]);

	gatoss_conf_print(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_passive_filter(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	int8_t filt_short = atoi(ctx->argv[1]);
	int32_t filt_full = atoi(ctx->argv[2]);

	gatoss_conf.passive_filter_list_short = filt_short;
	gatoss_conf.passive_filter_list_full = filt_full;

	gatoss_conf_print(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_other(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	gatoss_conf.sd_enable = atoi(ctx->argv[1]);
	gatoss_conf_print(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_db(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	gatoss_conf.plane_enable_frame_log = atoi(ctx->argv[1]);
	gatoss_conf.plane_position_log_interval = atoi(ctx->argv[2]);
	gatoss_conf_print(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_garbage_collector(struct command_context *ctx) {

	if (conf_initialized == 0)
		printf("WARNING setting uninitialized conf!!!\r\n");

	if (ctx->argc != 8)
		return CMD_ERROR_SYNTAX;

	gatoss_conf.garbage_collector_interval = atoi(ctx->argv[1]);
	gatoss_conf.garbage_plane_inactive_interval = atoi(ctx->argv[2]);
	gatoss_conf.garbage_plane_no_lock_inactive_interval = atoi(ctx->argv[3]);
	gatoss_conf.garbage_plane_frame_count = atoi(ctx->argv[4]);
	gatoss_conf.garbage_plane_frame_interval = atoi(ctx->argv[5]);
	gatoss_conf.garbage_pos_lifetime = atoi(ctx->argv[6]);
	gatoss_conf.garbage_frame_lifetime = atoi(ctx->argv[7]);

	gatoss_conf_print(&gatoss_conf);

	return CMD_ERROR_NONE;
}

int gatoss_cmd_conf_print(struct command_context *ctx) {
	gatoss_conf_print(&gatoss_conf);
	return CMD_ERROR_NONE;
}

int gatoss_cmd_recover_fs(struct command_context *ctx) {
	int result = gatoss_format_fs();
	printf("Format result is %d\r\n", result);
	return CMD_ERROR_NONE;
}

int gatoss_cmd_blob_ls(struct command_context *ctx) {
	if (gatoss_blob_ls() < 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int gatoss_cmd_db_store(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	int result = gatoss_db_store(ctx->argv[1]);
	printf("Store to %s, result %d\r\n", ctx->argv[1], result);
	return CMD_ERROR_NONE;
}

int gatoss_cmd_db_load(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	int result = gatoss_db_load(ctx->argv[1]);
	printf("Load from %s, result %d\r\n", ctx->argv[1], result);
	return CMD_ERROR_NONE;
}

struct command power_subcommands[] = {
	{
		.name = "on",
		.help = "Power on",
		.usage = "<channels> (1,2,3,4,all)",
		.handler = gatoss_cmd_power_on,
	}, {
		.name = "off",
		.help = "Power off",
		.usage = "<channels> (1,2,3,4,all)",
		.handler = gatoss_cmd_power_off,
	}, {
		.name = "state",
		.help = "Power state",
		.handler = gatoss_cmd_power_status,
	}
};

struct command gatoss_subcommands[] = {
	{
		.name = "power",
		.help = "Power systems",
		.chain = INIT_CHAIN(power_subcommands),
	}, {
		.name = "hk",
		.help = "Request HK",
		.usage = "<save>",
		.handler = gatoss_cmd_hk,
	}, {
		.name = "threshold",
		.help = "Change the synchronisation threshold on the FPGA for the ADC in use",
		.usage = "<amount>",
		.handler = gatoss_cmd_threshold,
	},{
		.name = "select_adc",
		.help = "Select adc (0 for ADC A and 1 for ADC B)",
		.usage = "<adc>",
		.handler = gatoss_cmd_sel_adc,
	},{
		.name = "load_fpga",
		.help = "Load new FPGA image",
		.usage = "<path> <load_now> <store_as_dfl>",
		.handler = gatoss_cmd_load_fpga,
	}, {
		.name = "load_uc",
		.help = "Load new uC image",
		.usage = "<path> <load_now> <store_as_dfl>",
		.handler = gatoss_cmd_load_uc,
	}, {
		.name = "recoverfs",
		.help = "Format FS",
		.handler = gatoss_cmd_recover_fs,
	}, {
		.name = "list_short",
		.help = "LIST_SHORT",
		.usage = "<filtering>",
		.handler = gatoss_cmd_list_short,
	}, {
		.name = "list_full",
		.help = "LIST_FULL",
		.usage ="<filtering>",
		.handler = gatoss_cmd_list_full,
	}, {
		.name = "plane_full",
		.help = "PLANE_FULL",
		.usage = "<icao24/bufferid>",
		.handler = gatoss_cmd_plane_full,
	}, {
		.name = "plane_positions",
		.help = "PLANE_POSITIONS",
		.usage ="<icao24/bufferid> <Decimation step [km]>",
		.handler = gatoss_cmd_plane_positions,
	}, {
		.name = "plane_contacts",
		.help = "PLANE_CONTACTS",
		.usage ="<icao24/bufferid>",
		.handler = gatoss_cmd_plane_contacts,
	}, {
		.name = "file_plane_positions",
		.help = "FILE_PLANE_POSITIONS",
		.usage = "<icao24/bufferid> <begin time> <end time> <max_size> <path>",
		.handler = gatoss_cmd_file_plane_positions,
	}, {
		.name = "file_plane_frames",
		.help = "FILE_PLANE_FRAMES",
		.usage = "<icao24/bufferid> <begin time> <end time> <max_size> <path>",
		.handler = gatoss_cmd_file_plane_frames,
	}, {
		.name = "file_all_frames",
		.help = "FILE_ALL_FRAMES: Start frame logging task",
		.usage = "<start_time> <run_duration> <max_size> <path>",
		.handler = gatoss_cmd_file_all_frames,
	}, {
		.name = "file_all_positions",
		.help = "FILE_ALL_POSITIONS: Start position logging task",
		.usage = "<start_time> <run_duration> <max_size> <path>",
		.handler = gatoss_cmd_file_all_positions,
	}, {
		.name = "file_raw_samples",
		.help = "FILE_RAW_SAMPLES: Start raw sample logging task",
		.usage = "<noise_sample> <timeout> <max_count> <trigger threshold> <path>",
		.handler = gatoss_cmd_file_raw_samples,
	}, {
		.name = "conf_get",
		.help = "Fetch configuration",
		.handler = gatoss_cmd_conf_get,
	}, {
		.name = "conf_set",
		.help = "Send configuration",
		.handler = gatoss_cmd_conf_set,
	}, {
		.name = "conf_restore",
		.help = "Send conf restore command",
		.handler = gatoss_cmd_conf_restore,
	}, {
		.name = "conf_fpga",
		.help = "Set FPGA configuration",
		.usage = "<image> <wdt enable> <threshold A> <threshold B> <threshold count> <adc> <adc switching interval>",
		.handler = gatoss_cmd_conf_fpga,
	}, {
		.name = "conf_pi",
		.help = "Set passive intervals",
		.usage = "<inhibit> <short> <full> <plane> <live min tx> <live interval>",
		.handler = gatoss_cmd_conf_passive_interval,
	}, {
		.name = "conf_pf",
		.help = "Set passive filter",
		.usage = "<short> <full>",
		.handler = gatoss_cmd_conf_passive_filter,
	}, {
		.name = "conf_db",
		.help = "Set database setting",
		.usage = "<store_frame> <pos log interval>",
		.handler = gatoss_cmd_conf_db,
	}, {
		.name = "conf_gc",
		.help = "Set garbage collector configuration ",
		.usage = "<collector ival> <plane inactive ival> <plane no lock ival> <frame cnt> <frame ival> <pos lifetime> <frame lifetime>",
		.handler = gatoss_cmd_conf_garbage_collector,
	}, {
		.name = "conf_other",
		.help = "Set other options",
		.usage = "<sd_enable>",
		.handler = gatoss_cmd_conf_other,
	}, {
		.name = "conf_print",
		.help = "Show the current local conf",
		.handler = gatoss_cmd_conf_print,
	}, {
		.name = "blobs",
		.help = "blob listing",
		.handler = gatoss_cmd_blob_ls,
	}, {
		.name = "db_store",
		.help = "save entire db to file",
		.usage = "<path>",
		.handler = gatoss_cmd_db_store,
	}, {
		.name = "db_load",
		.help = "load entire db from file",
		.usage = "<path>",
		.handler = gatoss_cmd_db_load,
	}, {
		.name = "node",
		.help = "set node",
		.usage = "<node>",
		.handler = gatoss_cmd_set_node,
	}
};

struct command __root_command gatoss_commands[] = {
	{
		.name = "gatoss",
		.help = "GATOSS subsystem",
		.chain = INIT_CHAIN(gatoss_subcommands),
	}
};

void gatoss_cmd_setup(void) {
	command_register(gatoss_commands);
}
