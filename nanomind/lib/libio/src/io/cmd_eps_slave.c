#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <io/nanopower2.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <dev/i2c.h>
#include <util/error.h>
#include <util/console.h>
#include <util/log.h>

/* EPS I2C slave mode commands */
static int slave_node = NODE_EPS;
// Defined in cmd_eps
extern eps_config_t nanopower_config;
extern int cmd_eps_config_edit(struct command_context *ctx);



int eps_slave_node(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int node;
	if (sscanf(args, "%u", &node) != 1)
		return CMD_ERROR_SYNTAX;
	printf("Setting EPS slave node address to %u\r\n", node);
	slave_node = node;

	return CMD_ERROR_NONE;

}

int eps_slave_ping(struct command_context *ctx) {

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = CSP_PING; // Ping port
	frame->data[1] = 0x55;
	frame->len = 2;
	frame->len_rx = 3;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	if (i2c_receive(0, &frame, 20) == E_NO_ERR) {
		printf("Received a reply from EPS!\r\n");
	} else {
		printf("No reply from EPS\r\n");
	}

	csp_buffer_free(frame);
	return CMD_ERROR_NONE;

}

int eps_slave_reboot(struct command_context *ctx) {

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = CSP_REBOOT; // Ping port
	frame->data[1] = 0x80;
	frame->data[2] = 0x07;
	frame->data[3] = 0x80;
	frame->data[4] = 0x07;
	frame->len = 5;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	printf("Sent reboot command to EPS\r\n");
	return CMD_ERROR_NONE;

}

int eps_slave_hk(struct command_context *ctx) {
	int bintmp;
	printf("Requesting EPS HK data\r\n");
	eps_hk_1_t * chkparam;

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_HK; // Ping port
	frame->len = 1;
	frame->len_rx = 2 + (uint8_t) sizeof(eps_hk_1_t);
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	if (i2c_receive(0, &frame, 20) != E_NO_ERR)
		return CMD_ERROR_FAIL;

	chkparam = (eps_hk_1_t *)&frame->data[2];

	chkparam->pv[0] = csp_ntoh16(chkparam->pv[0]);
	chkparam->pv[1] = csp_ntoh16(chkparam->pv[1]);
	chkparam->pv[2] = csp_ntoh16(chkparam->pv[2]);
	chkparam->pc = csp_ntoh16(chkparam->pc);
	chkparam->bv = csp_ntoh16(chkparam->bv);
	chkparam->sc = csp_ntoh16(chkparam->sc);
	chkparam->temp[0] = csp_ntoh16(chkparam->temp[0]);
	chkparam->temp[1] = csp_ntoh16(chkparam->temp[1]);
	chkparam->temp[2] = csp_ntoh16(chkparam->temp[2]);
	chkparam->temp[3] = csp_ntoh16(chkparam->temp[3]);
	chkparam->temp[4] = csp_ntoh16(chkparam->temp[4]);
	chkparam->temp[5] = csp_ntoh16(chkparam->temp[5]);
	chkparam->latchup[0] = csp_ntoh16(chkparam->latchup[0]);
	chkparam->latchup[1] = csp_ntoh16(chkparam->latchup[1]);
	chkparam->latchup[2] = csp_ntoh16(chkparam->latchup[2]);
	chkparam->latchup[3] = csp_ntoh16(chkparam->latchup[3]);
	chkparam->latchup[4] = csp_ntoh16(chkparam->latchup[4]);
	chkparam->latchup[5] = csp_ntoh16(chkparam->latchup[5]);
	chkparam->sw_errors = csp_ntoh16(chkparam->sw_errors);
	chkparam->bootcount = csp_ntoh16(chkparam->bootcount);

	printf("ADC sample:\r\n");
	printf("Temp 1 %"PRId16"\r\n", chkparam->temp[0]);
	printf("Temp 2 %"PRId16"\r\n", chkparam->temp[1]);
	printf("Temp 3 %"PRId16"\r\n", chkparam->temp[2]);
	printf("Temp PCB %"PRId16"\r\n", chkparam->temp[3]);
	printf("BV %u\r\n", chkparam->bv);
	printf("PC %u\r\n", chkparam->pc);
	printf("SC %u\r\n", chkparam->sc);
	printf("PV1 %u\r\n", chkparam->pv[0]);
	printf("PV2 %u\r\n", chkparam->pv[1]);
	printf("PV3 %u\r\n", chkparam->pv[2]);
	printf("Latch up 1 %u\r\n", chkparam->latchup[0]);
	printf("Latch up 2 %u\r\n", chkparam->latchup[1]);
	printf("Latch up 3 %u\r\n", chkparam->latchup[2]);
	printf("Latch up 4 %u\r\n", chkparam->latchup[3]);
	printf("Latch up 5 %u\r\n", chkparam->latchup[4]);
	printf("Latch up 6 %u\r\n", chkparam->latchup[5]);
	printf("User Channel Status %02X ; \r\n", chkparam->channel_status);
	bintmp = chkparam->channel_status;
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
	printf("Battery temperature 1 %"PRId16"\r\n", chkparam->temp[4]);
	printf("Battery temperature 2 %"PRId16"\r\n", chkparam->temp[5]);
	printf("Reset %d   BootCount %d   SW Err %d   PPT mode %d\r\n",chkparam->reset,chkparam->bootcount,chkparam->sw_errors,chkparam->ppt_mode);

	csp_buffer_free(frame);
	return CMD_ERROR_NONE;

}


int eps_slave_hk2(struct command_context *ctx) {

	printf("Requesting EPS HK2 data\r\n");
	eps_hk_t * chkparam;

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_HK;
	frame->data[1] = 0;
	frame->len = 2;
	frame->len_rx = 2 + (uint8_t) sizeof(eps_hk_t);
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	if (i2c_receive(0, &frame, 20) != E_NO_ERR)
		return CMD_ERROR_FAIL;

	chkparam = (eps_hk_t *)&frame->data[2];
	eps_hk_unpack(chkparam);
	eps_hk_print(chkparam);

	csp_buffer_free(frame);
	return CMD_ERROR_NONE;

}


int eps_slave_output(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int outputs;
	printf("console args: %s\r\n", args);
	if (sscanf(args, "%X", &outputs) != 1)
		return CMD_ERROR_SYNTAX;
	printf("Outputs 0x%02X\r\n", outputs);

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_SET_OUTPUT; // Ping port
	frame->data[1] = outputs;
	frame->len = 2;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	return CMD_ERROR_NONE;

}

int eps_slave_single_output(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int channel;
	unsigned int mode;
	int delay;
	printf("Input channel, mode (0=off, 1=on), and delay\r\n");
	if (sscanf(args, "%u %u %d", &channel, &mode, &delay) != 3)
		return CMD_ERROR_SYNTAX;
	printf("Channel %d is set to %d with delay %d\r\n", channel, mode, delay);

	eps_output_set_single_req eps_switch;
	eps_switch.channel = (uint8_t)channel;
	eps_switch.mode = (uint8_t)mode;
	eps_switch.delay = csp_hton16((int16_t)delay);

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_SET_SINGLE_OUTPUT; // Ping port
	memcpy(&frame->data[1], &eps_switch, sizeof(eps_switch));
	frame->len = 1 + sizeof(eps_switch);
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	return CMD_ERROR_NONE;

}

int eps_slave_volt(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int pv1, pv2, pv3;
	if (sscanf(args, "%u %u %u", &pv1, &pv2, &pv3) != 3)
		return CMD_ERROR_SYNTAX;
	printf("PV1: %04d PV2: %04d PV3: %04d\r\n", pv1, pv2, pv3);

	uint16_t pvolt[3];
	pvolt[0] = csp_hton16(pv1);
	pvolt[1] = csp_hton16(pv2);
	pvolt[2] = csp_hton16(pv3);

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_SET_VBOOST; // Ping port
	memcpy(&frame->data[1], &pvolt, 3 * sizeof(uint16_t));
	frame->len = 1 + 3 * sizeof(uint16_t);
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	return CMD_ERROR_NONE;

}

int eps_slave_ppt_mode(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int mode;
	printf("EPS_PPT_MODE_OFF = 0, EPS_PPT_MODE_AUTO = 1, EPS_PPT_MODE_FIXED= 2\r\n");
	if (sscanf(args, "%d", &mode) != 1)
		return CMD_ERROR_SYNTAX;
	printf("Mode %d\r\n", mode);

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_SET_PPTMODE; // Ping port
	frame->data[1] = (uint8_t)mode;
	frame->len = 2;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	return CMD_ERROR_NONE;

}


int eps_slave_hardreset(struct command_context *ctx) {


	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_HARDRESET;
	frame->len = 1;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}
	return CMD_ERROR_NONE;
}

int eps_slave_wdt_gnd_reset(struct command_context *ctx) {


	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_RESET_WDT_GND;
	frame->data[1] = 0x78;
	frame->len = 2;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}
	return CMD_ERROR_NONE;
}

int cmd_eps_slave_config_get(struct command_context *ctx) {
	printf("Requesting EPS Config\r\n");

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_CONFIG_GET;
	frame->len = 1;
	frame->len_rx = 2 + (uint8_t) sizeof(eps_config_t);
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	if (i2c_receive(0, &frame, 20) != E_NO_ERR)
		return CMD_ERROR_FAIL;

	memcpy(&nanopower_config, &frame->data[2], sizeof(eps_config_t));
	eps_config_unpack(&nanopower_config);
	eps_config_print(&nanopower_config);
	return CMD_ERROR_NONE;
}


int cmd_eps_slave_config_print(struct command_context *ctx) {
	eps_config_print(&nanopower_config);
	return CMD_ERROR_NONE;
}

int cmd_eps_slave_config_set(struct command_context *ctx) {
	printf("Setting EPS Config\r\n");

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_CONFIG_SET;
	frame->len = 1 + (uint8_t) sizeof(eps_config_t);
	frame->len_rx = 2;
	frame->retries = 0;

	memcpy(&frame->data[1], &nanopower_config, sizeof(eps_config_t));
	eps_config_pack((eps_config_t *) &frame->data[1]);
	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	if (i2c_receive(0, &frame, 200) != E_NO_ERR)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}

int cmd_eps_slave_config_restore(struct command_context *ctx) {


	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_CONFIG_CMD;
	frame->data[1] = 1;
	frame->len = 2;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	return CMD_ERROR_NONE;
}

int cmd_eps_slave_config_edit(struct command_context *ctx) {

	cmd_eps_config_edit(NULL);
	return CMD_ERROR_NONE;

}





int eps_slave_reset_persistent(struct command_context *ctx) {

	uint8_t magic = 0x42;

	i2c_frame_t * frame;
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = slave_node;
	frame->data[0] = EPS_PORT_RESET_COUNTERS;
	frame->data[1] = (uint8_t) magic;
	frame->len = 2;
	frame->len_rx = 0;
	frame->retries = 0;

	if (i2c_send(0, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		return CMD_ERROR_NOMEM;
	}

	return CMD_ERROR_NONE;

}

command_t __sub_command eps_slave_config_subcommands[] = {
	{
		.name = "get",
		.help = "Conf get",
		.handler = cmd_eps_slave_config_get,
	},{
		.name = "set",
		.help = "Conf set",
		.handler = cmd_eps_slave_config_set,
	},{
		.name = "edit",
		.help = "Edit local config",
		.handler = cmd_eps_slave_config_edit,
	},{
		.name = "print",
		.help = "Print local config",
		.handler = cmd_eps_slave_config_print,
	},{
		.name = "restore",
		.help = "Restore config from default",
		.handler = cmd_eps_slave_config_restore,
	}
};


command_t __sub_command eps_slavecommands[] = {
	{
		.name = "node",
		.help = "Set slave EPS address in OBC host table",
		.handler = eps_slave_node,
	},{
		.name = "hk",
		.help = "Get slave HK",
		.handler = eps_slave_hk,
	},{
		.name = "hk2",
		.help = "Get slave new HK",
		.handler = eps_slave_hk2,
	},{
		.name = "outputmask",
		.help = "Set slave on/off",
		.usage = "<output port hex>",
		.handler = eps_slave_output,
	},{
		.name = "output",
		.help = "Set slave channel on/off",
		.usage = "<channel> <1/0> <delay>",
		.handler = eps_slave_single_output,
	},{
		.name = "vboost",
		.help = "Set slave pvolt, arguments  pv1, pv2, pv3 (mV)",
		.handler = eps_slave_volt,
	},{
		.name = "ppt",
		.help = "Set slave PPT mode",
		.handler = eps_slave_ppt_mode,
	},{
		.name = "ping",
		.help = "Slave ping",
		.handler = eps_slave_ping,
	},{
		.name = "reboot",
		.help = "Slave reboot",
		.handler = eps_slave_reboot,
	},{
		.name = "persistentreset",
		.help = "Set persistent variables = 0",
		.handler = eps_slave_reset_persistent,
	},{
		.name = "hardreset",
		.help = "Completely powercycle EPS",
		.handler = eps_slave_hardreset,
	},{
		.name = "resetwdt",
		.help = "Resets the WDT GND",
		.handler = eps_slave_wdt_gnd_reset,
	},{
		.name = "conf",
		.help = "Configuration",
		.chain = INIT_CHAIN(eps_slave_config_subcommands),
	}
};

command_t __root_command epsslave_commands[] = {
	{
		.name = "epsslave",
		.help = "EPS slave commands",
		.chain = INIT_CHAIN(eps_slavecommands),
	}
};

void cmd_epsslave_setup(void) {
	command_register(epsslave_commands);
}
