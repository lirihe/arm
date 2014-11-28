/**
 * cmd_eps.c
 *
 *  Created on: 16/12/2009
 */

#include <inttypes.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <io/nanopower.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#ifndef __linux__
#include <dev/i2c.h>
#include <util/error.h>
#endif

#include <util/console.h>
#include <util/log.h>

int eps_node(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int node;
	if (sscanf(args, "%u", &node) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting EPS node address to %u\r\n", node);
	eps_set_node(node);

	return CMD_ERROR_NONE;

}

int eps_hk(struct command_context *ctx) {
	int bintmp;
	printf("Requesting EPS HK data\r\n");
	chkparam_t chkparam;
	if (!eps_get_hk(&chkparam)) {
		printf("Error requesting HK\r\n");
		return CMD_ERROR_FAIL;
	}

	printf("ADC sample:\r\n");
	printf("Temp 1 %"PRId16"\r\n", chkparam.temp[0]);
	printf("Temp 2 %"PRId16"\r\n", chkparam.temp[1]);
	printf("Temp 3 %"PRId16"\r\n", chkparam.temp[2]);
	printf("Temp PCB %"PRId16"\r\n", chkparam.temp[3]);
	printf("BV %u\r\n", chkparam.bv);
	printf("PC %u\r\n", chkparam.pc);
	printf("SC %u\r\n", chkparam.sc);
	printf("PV1 %u\r\n", chkparam.pv[0]);
	printf("PV2 %u\r\n", chkparam.pv[1]);
	printf("PV3 %u\r\n", chkparam.pv[2]);
	printf("Latch up 1 %u\r\n", chkparam.latchup[0]);
	printf("Latch up 2 %u\r\n", chkparam.latchup[1]);
	printf("Latch up 3 %u\r\n", chkparam.latchup[2]);
	printf("Latch up 4 %u\r\n", chkparam.latchup[3]);
	printf("Latch up 5 %u\r\n", chkparam.latchup[4]);
	printf("Latch up 6 %u\r\n", chkparam.latchup[5]);
	printf("User Channel Status %02X\r\n", chkparam.channel_status);
	bintmp = chkparam.channel_status;
	int n;
	for(n=0; n<8; n++)
	   {
	      if((bintmp & 0x80) !=0)
	      {
	         printf("1");
	      }
	      else
	      {
	         printf("0");
	      }
	      if (n==3)
	      {
	         printf(" "); /* insert a space between nybbles */
	      }
	      bintmp = bintmp<<1;
	   }
	printf("\r\n");
	printf("Battery temperature 1 %"PRId16"\r\n", chkparam.batt_temp[0]);
	printf("Battery temperature 2 %"PRId16"\r\n", chkparam.batt_temp[1]);
	printf("Reset %d   BootCount %d   SW Err %d   PPT mode %d\r\n",chkparam.reset,chkparam.bootcount,chkparam.sw_errors,chkparam.ppt_mode);

	return CMD_ERROR_NONE;
}


int eps_hk2(struct command_context *ctx) {

	printf("Requesting EPS HK2 data\r\n");
	chkparam2_t chkparam2;
	if (!eps_get_hk2(&chkparam2)) {
		printf("Error requesting HK2\r\n");
		return CMD_ERROR_FAIL;
	}

	printf("SOC %"PRIu8" %%\r\n", chkparam2.SOC);
	printf("Icount %"PRId32" As\r\n", chkparam2.Icount);
	printf("Uptime %"PRIu32" s\r\n", chkparam2.UpTime);
	printf("Mode %"PRIu8" \r\n", chkparam2.Mode);
	printf("TimeoutGndWdt %"PRIu32" s\r\n", chkparam2.TimeoutGndWDT);
	int i;
	printf("SwitchOnTimer  ");
	for (i=0;i<6;i++)
	{
		printf(" %"PRId16", ", chkparam2.SwitchOnTimer[i]);
	}
	printf(" s \r\n ");
	printf("SwitchOffTimer  ");
	for (i=0;i<6;i++)
	{
		printf(" %"PRId16", ", chkparam2.SwitchOffTimer[i]);
	}
	printf(" s \r\n ");
	printf("PPT fixed ref %"PRIu16" %"PRIu16" %"PRIu16" mV\r\n", chkparam2.PPTFixedRef[0], chkparam2.PPTFixedRef[1], chkparam2.PPTFixedRef[2]);
	printf("I2C wdt timeouts %"PRIu16", GND wdt timeouts %"PRIu16"\r\n", chkparam2.i2c_wdt_timeouts, chkparam2.gnd_wdt_timeouts);
	printf("I2C obc ping timeouts %"PRIu16", I2C com i3c timeouts %"PRIu16"\r\n", chkparam2.i2c_ping_timeouts[0], chkparam2.i2c_ping_timeouts[0]);




	return CMD_ERROR_NONE;
}

int eps_version(struct command_context *ctx) {

	printf("Requesting EPS version\r\n");
	eps_version_t version;
	if (!eps_get_version(&version)) {
		printf("Error requesting version\r\n");
		return CMD_ERROR_FAIL;
	}
	printf("Version  %s %s %s %s\r\n", version.type, version.ver, version.date, version.time);

	return CMD_ERROR_NONE;
}

int eps_output(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int outputs;
	printf("console args: %s\r\n", args);
	if (sscanf(args, "%X", &outputs) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Outputs 0x%02X\r\n", outputs);

	eps_set_output((uint8_t) outputs);

	return CMD_ERROR_NONE;

}
int eps_single_output(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int channel;
	unsigned int mode;
	int delay;
	printf("Input channel, mode (0=off, 1=on), and delay\r\n");
	if (sscanf(args, "%u %u %d", &channel,&mode,&delay) != 3)
		return CMD_ERROR_SYNTAX;
	printf("Channel %d is set to %d with delay %d\r\n", channel,mode,delay);

	eps_set_single_output((uint8_t) channel, (uint8_t) mode, (int16_t) delay);

	return CMD_ERROR_NONE;

}

int eps_volt(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int pv1, pv2, pv3;
	if (sscanf(args, "%u %u %u", &pv1, &pv2, &pv3) != 3)
		return CMD_ERROR_SYNTAX;
	printf("PV1: %04d PV2: %04d PV3: %04d\r\n", pv1, pv2, pv3);
	eps_set_pv_volt((uint16_t) pv1, (uint16_t) pv2, (uint16_t) pv3);

	return CMD_ERROR_NONE;
}

int eps_ppt_mode(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int mode;
	printf("EPS_PPT_MODE_OFF = 0, EPS_PPT_MODE_AUTO = 1, EPS_PPT_MODE_FIXED= 2\r\n");
	if (sscanf(args, "%d", &mode) != 1)
		return CMD_ERROR_SYNTAX;
	printf("Mode %d\r\n", mode);

	eps_set_ppt_mode((uint8_t) mode);

	return CMD_ERROR_NONE;

}

int eps_heater(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint8_t heater_mode = atoi(ctx->argv[1]);
	if (heater_mode != 0 && heater_mode != 1)
		return CMD_ERROR_SYNTAX;

	uint8_t heater_mode_reply;
	eps_auto_heater(heater_mode, &heater_mode_reply);

	printf("Heater mode %u\r\n", heater_mode_reply);

	return CMD_ERROR_NONE;

}

int cmd_eps_reset_persistent(struct command_context *ctx) {
	eps_reset_persistent();
	return CMD_ERROR_NONE;
}

int cmd_eps_reset_gndwdt(struct command_context *ctx) {
	eps_reset_gndwdt();
	return CMD_ERROR_NONE;
}



int cmd_eps_bootdelay(struct command_context *ctx) {

	printf("Input channel delays [seconds] (-99 disables): x x x x x x \r\n If no input, only GET is performed.\r\n");
	char * args = command_args(ctx);
	eps_bootdelay_t bootdelay;
	int delay[6];

	if (sscanf(args, "%d %d %d %d %d %d", &delay[0], &delay[1], &delay[2], &delay[3], &delay[4], &delay[5]) == 6)
	{
		bootdelay.delay[0] = csp_hton16(delay[0]);
		bootdelay.delay[1] = csp_hton16(delay[1]);
		bootdelay.delay[2] = csp_hton16(delay[2]);
		bootdelay.delay[3] = csp_hton16(delay[3]);
		bootdelay.delay[4] = csp_hton16(delay[4]);
		bootdelay.delay[5] = csp_hton16(delay[5]);

		if (!eps_set_bootdelay(&bootdelay))
		{
			printf("Error set bootdelay\r\n");
			return CMD_ERROR_FAIL;
		}
	}

	if (!eps_get_bootdelay(&bootdelay)) {
		printf("Error get bootdelay\r\n");
		return CMD_ERROR_FAIL;
	}
	printf("Bootdelay is:\r\n");

	bootdelay.delay[0] = csp_ntoh16(bootdelay.delay[0]);
	bootdelay.delay[1] = csp_ntoh16(bootdelay.delay[1]);
	bootdelay.delay[2] = csp_ntoh16(bootdelay.delay[2]);
	bootdelay.delay[3] = csp_ntoh16(bootdelay.delay[3]);
	bootdelay.delay[4] = csp_ntoh16(bootdelay.delay[4]);
	bootdelay.delay[5] = csp_ntoh16(bootdelay.delay[5]);
	printf(" 0:%d  1:%d  2:%d  3:%d  4:%d  5:%d [s]\r\n",bootdelay.delay[0],bootdelay.delay[1],bootdelay.delay[2],bootdelay.delay[3],bootdelay.delay[4],bootdelay.delay[5]);

return CMD_ERROR_NONE;
}

command_t __sub_command eps_subcommands[] = {
	{
		.name = "node",
		.help = "Set EPS address in OBC host table",
		.usage = "<node>",
		.handler = eps_node,
	},{
		.name = "hk",
		.help = "Get HK",
		.handler = eps_hk,
	},{
		.name = "hk2",
		.help = "Get HK2",
		.handler = eps_hk2,
	},{
		.name = "ver",
		.help = "Get version",
		.handler = eps_version,
	},{
		.name = "o",
		.help = "Set on/off, argument hex value of output char",
		.usage = "<outputs>",
		.handler = eps_output,
	},{
		.name = "so",
		.help = "Set channel on/off, argument (channel) to (1 or 0)",
		.usage = "<channel> <mode> <delay>",
		.handler = eps_single_output,
	},{
		.name = "v",
		.help = "Set pvolt, arguments hex pv1, pv2, pv3 (mV)",
		.usage = "<pv1> <pv2> <pv3>",
		.handler = eps_volt,
	},{
		.name = "ppt",
		.help = "Set PPT mode. off = 0, auto = 1, fixed = 2",
		.usage = "<mode>",
		.handler = eps_ppt_mode,
	},{
		.name = "heater",
		.help = "Set heater auto mode. off = 0, auto = 1",
		.usage = "<mode>",
		.handler = eps_heater,
	},{
		.name = "persistentreset",
		.help = "Set persistent variables = 0",
		.handler = cmd_eps_reset_persistent,
	},{
		.name = "gndwdtreset",
		.help = "Resets the ground com WDT",
		.handler = cmd_eps_reset_gndwdt,
	},{
		.name = "bootdelay",
		.help = "Set/Get boot delay for channels: x x x x x x [s]",
		.handler = cmd_eps_bootdelay,
	},
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
