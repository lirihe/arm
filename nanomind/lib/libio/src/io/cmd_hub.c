/**
 * cmd_eps.c
 *
 *  Created on: 16/12/2009
 */

#include <inttypes.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <io/nanohub.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#ifndef __linux__
#include <dev/i2c.h>
#include <util/error.h>
#endif

#include <util/log.h>
#include <util/console.h>
#include <util/binaryprint.h>
#include <io/cmd.h>

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#define printf(s, ...) printf_P(PSTR(s), ## __VA_ARGS__)
#endif

static nanohub_conf_t nanohub_config;

int cmd_hub_node(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int node;
	if (sscanf(args, "%u", &node) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting HUB node address to %u\r\n", node);
	hub_set_node(node);

	return CMD_ERROR_NONE;

}

int cmd_hub_hk(struct command_context *ctx) {

	printf("Requesting HUB HK data\r\n");
	nanohub_hk_t nanohub_hk;
	if (!hub_get_hk(&nanohub_hk)) {
		printf("Error requesting HK\r\n");
		return CMD_ERROR_FAIL;
	}

	printf("Temperature %"PRId16"\r\n", nanohub_hk.temp);
	printf("Bootcount %"PRIu32"\r\n", nanohub_hk.bootcount);
	printf("Reset  %d\r\n", nanohub_hk.reset);
	printf("Gyro ON %u\r\n", (nanohub_hk.gyrostatus >> 0) & 1);
	printf("\r\n");
	return CMD_ERROR_NONE;
}


int cmd_hub_adc(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int channel;
	uint16_t adcdata[8];

	if (sscanf(args, "%u", &channel) == 1)
	{
		hub_get_adc_single((uint8_t) channel, &adcdata[channel]);
		printf("ADCdata: single channel %u = %u mV\r\n", channel,adcdata[channel]);
	}
	else
	{
		hub_get_adc_all(adcdata);
		printf("ADCdata: [0...7]  %u %u %u %u %u %u %u %u mV\r\n", adcdata[0], adcdata[1], adcdata[2], adcdata[3], adcdata[4], adcdata[5], adcdata[6], adcdata[7]);
	}
	return CMD_ERROR_NONE;
}

int cmd_hub_single_output(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int channel;
	unsigned int mode;
	unsigned int delay;
	nanohub_switchdata_t hubswitch;

	printf("Input channel, mode (0=off, 1=on), and delay\r\n");
	if (sscanf(args, "%u %u %u", &channel,&mode,&delay) == 3)
	{
		hub_set_single_output((uint8_t) channel, (uint8_t) mode, (uint16_t) delay, &hubswitch);
		printf("Switchdata:\r\n channel %d is set to %d with delay %d\r\n", channel,mode,delay);
	}
	else
	{
		hub_set_single_output(99, 0, 0, &hubswitch);
		printf("Switchdata:\r\n");
	}
	printf(" ontimer  ch0:%u ch1:%u \r\n",hubswitch.on_timer[0],hubswitch.on_timer[1]);
    printf(" offtimer ch0:%u ch1:%u \r\n",hubswitch.off_timer[0],hubswitch.off_timer[1]);
	printf(" faults   ch0:%u ch1:%u \r\n",hubswitch.faults[0],hubswitch.faults[1]);
	printf(" status   ch0:%u ch1:%u \r\n",(hubswitch.switchstatus)&0x01, (hubswitch.switchstatus>>1)&0x01);
	
	
	return CMD_ERROR_NONE;
}

int cmd_hub_knife(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int knife;
	unsigned int channel;
	unsigned int delay;
	unsigned int duration;
	nanohub_knivesdata_t knivesdata;

	printf("Input knife, channel, delay and duration\r\n");
	if (sscanf(args, "%u %u %u %u", &knife,&channel,&delay,&duration) == 4)
	{
		if (!hub_knife((uint8_t) knife,(uint8_t) channel, (uint16_t) delay, (uint16_t) duration, &knivesdata)) {
			printf("Error do deploy\r\n");
			return CMD_ERROR_FAIL;
		};
		printf("Knivesdata:\r\n knife %d channel %d is set to deploy in %d with duration %d\r\n",knife, channel,delay,duration);
	}
	else
	{
		if (!hub_knife(99, 0, 0, 0, &knivesdata)) {
			printf("Error get deploy\r\n");
			return CMD_ERROR_FAIL;
		}
		printf("Knivesdata:\r\n");
	}

	  printf("Knife0 timer:  %"PRIu32" %"PRIu32"\r\n",knivesdata.knife[0].timer[0],knivesdata.knife[0].timer[1]);
	  printf("Knife0 mode:   %"PRIu8" %"PRIu8"\r\n",knivesdata.knife[0].mode[0],knivesdata.knife[0].mode[1]);
	  printf("Knife0 burns:  %"PRIu8" %"PRIu8"\r\n",knivesdata.knife[0].burns[0],knivesdata.knife[0].burns[1]);
	  printf("Knife1 timer:  %"PRIu32" %"PRIu32"\r\n",knivesdata.knife[1].timer[0],knivesdata.knife[1].timer[1]);
	  printf("Knife1 mode:   %"PRIu8" %"PRIu8"\r\n",knivesdata.knife[1].mode[0],knivesdata.knife[1].mode[1]);
	  printf("Knife1 burns:  %"PRIu8" %"PRIu8"\r\n",knivesdata.knife[1].burns[0],knivesdata.knife[1].burns[1]);
	  printf("Burn status(%X): "BYTETOBINARYPATTERN"\r\n",knivesdata.burn_status, BYTETOBINARY(knivesdata.burn_status));
	  printf("Sense status: "BYTETOBINARYPATTERN"\r\n", BYTETOBINARY(knivesdata.sense_status));


	return CMD_ERROR_NONE;
}


int cmd_hub_reset_wdt(struct command_context *ctx) {

	//char * args = command_args(ctx);
	uint8_t magic=0xA5;
	uint32_t timer;

	hub_reset_wdt(magic,&timer);
	printf("Reset done. Hub WDT will time out in %"PRIu32" s\r\n", timer);

	return CMD_ERROR_NONE;
}
int cmd_hub_get_wdt(struct command_context *ctx) {

	//char * args = command_args(ctx);
	uint8_t magic=0x00;
	uint32_t timer;

	hub_reset_wdt(magic,&timer);
	printf("Hub WDT will time out in %"PRIu32" s\r\n", timer);

	return CMD_ERROR_NONE;
}

int cmd_hub_gyro_start(struct command_context *ctx) {

	uint8_t cmd=1;

	int32_t gyrodata[3];
	hub_gyro(cmd,0,0,gyrodata);

	printf("Gyros started \r\n");

	return CMD_ERROR_NONE;
}

int cmd_hub_gyro_stop(struct command_context *ctx) {

	uint8_t cmd=0;
	int32_t gyrodata[3];

	hub_gyro(cmd,0,0,gyrodata);
	printf("Gyros stopped \r\n");

	return CMD_ERROR_NONE;
}

int cmd_hub_gyro_get(struct command_context *ctx) {

	uint8_t cmd=99;
	int32_t gyrodata[3];

	hub_gyro(cmd,0,0,gyrodata);
	printf("Gyro data: x=%f, y=%f, z=%f deg/s\r\n",((float)gyrodata[0])*1e-6,((float)gyrodata[1])*1e-6,((float)gyrodata[2])*1e-6);

	return CMD_ERROR_NONE;
}


int cmd_hub_get_conf(struct command_context *ctx) {
	if (!hub_get_conf(&nanohub_config)) {
		printf("Error requesting conf\r\n");
		return CMD_ERROR_FAIL;
	}
	else
		hub_print_conf(&nanohub_config);

	return CMD_ERROR_NONE;
}


int cmd_hub_print_conf(struct command_context *ctx) {

	hub_print_conf(&nanohub_config);

	return CMD_ERROR_NONE;
}

int cmd_hub_set_conf(struct command_context *ctx) {
	if (!hub_set_conf(&nanohub_config)) {
		printf("Error sending conf\r\n");
		return CMD_ERROR_FAIL;
	}
	return CMD_ERROR_NONE;
}

int cmd_hub_edit_conf(struct command_context *ctx) {
	hub_edit_conf(&nanohub_config);
	return CMD_ERROR_NONE;
}


int cmd_hub_load_conf(struct command_context *ctx) {
  uint8_t configmode;
  if (!hub_conf_cmd(0,&configmode)) {
    printf("Error sending to hub\r\n");
    return CMD_ERROR_FAIL;
  }
  printf("Hub is running in configmode %d\n\r",configmode);
  return CMD_ERROR_NONE;
}

int cmd_hub_save_conf(struct command_context *ctx) {
  uint8_t configmode;
  if (!hub_conf_cmd(1,&configmode)) {
    printf("Error sending to hub\r\n");
    return CMD_ERROR_FAIL;
  }
  printf("Hub is running in configmode %d\n\r",configmode);
  return CMD_ERROR_NONE;
}

int cmd_hub_loaddefault_conf(struct command_context *ctx) {
  uint8_t configmode;
  if (!hub_conf_cmd(2,&configmode)) {
    printf("Error sending to hub\r\n");
    return CMD_ERROR_FAIL;
  }
  printf("Hub is running in configmode %d\n\r",configmode);
  return CMD_ERROR_NONE;
}

int cmd_hub_savedefault_conf(struct command_context *ctx) {
  uint8_t configmode;
  if (!hub_conf_cmd(3,&configmode)) {
    printf("Error sending to hub\r\n");
    return CMD_ERROR_FAIL;
  }
  printf("Hub is running in configmode %d\n\r",configmode);
  return CMD_ERROR_NONE;
}

command_t __sub_command hub_gyro_subcommands[] = {
	{
		.name = "start",
		.help = "Start gyros",
		.handler = cmd_hub_gyro_start,
	},{
		.name = "stop",
		.help = "Stop gyros",
		.handler = cmd_hub_gyro_stop,
	},{
		.name = "get",
		.help = "get gyro data",
		.handler = cmd_hub_gyro_get,
	}
};

command_t __sub_command hub_conf_subcommands[] = {
	{
		.name = "get",
		.help = "Get Config",
		.handler = cmd_hub_get_conf,
	},{
		.name = "set",
		.help = "Set Config",
		.handler = cmd_hub_set_conf,
	},{
		.name = "print",
		.help = "Print Config",
		.handler = cmd_hub_print_conf,
	},{
		.name = "edit",
		.help = "Edit Config",
		.handler = cmd_hub_edit_conf,
	},{
		.name = "save",
		.help = "Save config on Hub",
		.handler = cmd_hub_save_conf,
	},{
		.name = "load",
		.help = "Load config on Hub",
		.handler = cmd_hub_load_conf,
	},{
		.name = "savedefault",
		.help = "Save default config on Hub",
		.handler = cmd_hub_savedefault_conf,
	},{
		.name = "loaddefault",
		.help = "Load default config on Hub",
		.handler = cmd_hub_loaddefault_conf,
	}
};


command_t __sub_command hub_subcommands[] = {
	{
		.name = "node",
		.help = "Set HUB address in OBC host table",
		.usage = "<node>",
		.handler = cmd_hub_node,
	},{
		.name = "hk",
		.help = "Get HK",
		.usage = "",
 		.handler = cmd_hub_hk,

	},{
		.name = "rwdt",
		.help = "ResetWDT",
		.usage = "",
 		.handler = cmd_hub_reset_wdt,
	},{
		.name = "gwdt",
		.help = "GetWDT",
		.usage = "",
	 	.handler = cmd_hub_get_wdt,
	},{
		.name = "so",
		.help = "Set channel on/off",
		.usage = "<channel> <mode> <delay> (empty to read)",
 		.handler = cmd_hub_single_output,
	},{
		.name = "conf",
		.help = "Config",
		.chain = INIT_CHAIN(hub_conf_subcommands),
 	},{
		.name = "adc",
		.help = "Get ADC",
		.usage = "",
 		.handler = cmd_hub_adc,
	},{
		.name = "gyro",
		.help = "Gyro",
		.chain = INIT_CHAIN(hub_gyro_subcommands),
	},{
		.name = "knife",
		.help = "Do knife deploy",
		.usage = "<knife> <channel> <delay> <duration>",
		.handler = cmd_hub_knife,
	},
};

command_t __root_command hub_commands[]= {
	{
		.name = "hub",
		.help = "HUB subsystem",
		.chain = INIT_CHAIN(hub_subcommands),
	},
};

void cmd_hub_setup(void) {
	command_register(hub_commands);
}
