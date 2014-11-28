/*
 * cmd_cam.c
 *
 *  Created on: Jun 1, 2010
 *      Author: karl
 */

#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <io/nanocam.h>
#include <ftp/ftp_client.h>
#include <dev/usart.h>
#include <util/console.h>
#include <util/hexdump.h>
#include <util/log.h>

static uint8_t cam_input = CAM_INPUT_PREVIEW;
static uint8_t cam_format = CAM_STORE_JPG;
static uint8_t cam_sensor = 0;
static uint8_t cam_scale_down = 0;
static uint32_t cam_timeout = 20000;

int cmd_cam_node(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	nanocam_set_node(atoi(ctx->argv[1]));
	return CMD_ERROR_NONE;
}

int cmd_cam_set_timeout(struct command_context *ctx) {
	if (sscanf(command_args(ctx), "%"SCNu32, &cam_timeout) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Timeout set to %"PRIu32"\r\n", cam_timeout);

	return CMD_ERROR_NONE;
}

int cmd_cam_set_input(struct command_context *ctx) {
	uint32_t parse;
	if (sscanf(command_args(ctx), "%"SCNu32, &parse) != 1)
		return CMD_ERROR_SYNTAX;

	cam_input = (uint8_t) parse;
	printf("Input set to %"PRIu8"\r\n", cam_input);

	return CMD_ERROR_NONE;
}

int cmd_cam_set_format(struct command_context *ctx) {
	uint32_t parse;
	if (sscanf(command_args(ctx), "%"SCNu32, &parse) != 1)
		return CMD_ERROR_SYNTAX;

	cam_format = (uint8_t) parse;
	printf("Format set to %"PRIu8"\r\n", cam_format);

	return CMD_ERROR_NONE;
}

int cmd_cam_set_scale(struct command_context *ctx) {
	uint32_t parse;
	if (sscanf(command_args(ctx), "%"SCNu32, &parse) != 1)
		return CMD_ERROR_SYNTAX;

	cam_scale_down = (uint8_t) parse;
	printf("Scale set to %"PRIu8"\r\n", cam_scale_down);

	return CMD_ERROR_NONE;
}

int cmd_cam_set_sensor(struct command_context *ctx) {
	uint32_t parse;
	if (sscanf(command_args(ctx), "%"SCNu32, &parse) != 1)
		return CMD_ERROR_SYNTAX;

	cam_sensor = (uint8_t) parse;
	printf("Sensor set to %"PRIu8"\r\n", cam_sensor);

	return CMD_ERROR_NONE;
}

int cmd_cam_snap(struct command_context *ctx) {

	printf("Snapping from input %"PRIu8", timeout %"PRIu32"\r\n", cam_input, cam_timeout);

	uint8_t raw_capture[64];
	uint32_t hist[16];
	int result = nanocam_snap(cam_input, raw_capture, cam_timeout);
	if (result < 0) {
		printf("Snap error: %d\r\n", result);
		return CMD_ERROR_FAIL;
	}
	printf("\r\n");
	printf("Snap result: %d\r\n", result);
	memcpy(hist, raw_capture, 64);
	//hex_dump(raw_capture, 64);
	printf("­\r\n");
	printf("BLACK-------------------------\r\n");
	for(int i=0; i<16; i++) {
		for (unsigned int c = 0; c < (hist[i] * 150 / (2048 * 1536)); c++) {
			printf("x");
		}
		printf("\r\n");
	}
	printf("WHITE-------------------------\r\n");
	printf("­\r\n");

	printf("\r\n");

	return CMD_ERROR_NONE;

}

int cmd_cam_autofocus(struct command_context *ctx) {

	printf("Auto focus routine running....");
	printf("\r\n");

	while(1) {
		if (usart_messages_waiting(USART_CONSOLE))
			break;

		uint32_t result = nanocam_autofocus(cam_input, cam_timeout);

		printf("Focus number: %"PRIu32"\r\n", result);
	}
	printf("\r\n");
	return CMD_ERROR_NONE;
}

int cmd_cam_imgstat(struct command_context *ctx) {
	char cl[80];
	memset(cl, '\n', 80);
	cl[0] = '\r';
	cl[79] = '\0';
	cam_stat_t stat;

	printf("Image status routine running....");
	printf("\r\n");

	while(1) {
		if (usart_messages_waiting(USART_CONSOLE))
			break;

		nanocam_imgstat(&stat, cam_timeout);

		printf("­%s", cl);
		printf("Lightness overall: %"PRIu8"%%\r\n", stat.light[0]);
		printf("Lightness green 1: %"PRIu8"%%\r\n", stat.light[1]);
		printf("Lightness green 2: %"PRIu8"%%\r\n", stat.light[4]);
		printf("Lightness red    : %"PRIu8"%%\r\n", stat.light[2]);
		printf("Lightness blue   : %"PRIu8"%%\r\n", stat.light[3]);
		printf("Histogram:\r\n");
		for(int i=0; i<16; i++) {
			int c = 0;
			printf("|");
			for(c=0; c<stat.hist[i]; c++) {
				printf("x");
				if(c >= 69) break;
			}
			printf("\r\n");
		}
		printf("-- END --\r\n");
		printf("­\r\n");

	}
	printf("\r\n");
	return CMD_ERROR_NONE;
}


int cmd_cam_store(struct command_context *ctx) {

	char filename[40];

	if (ctx->argc == 2) {
		char * args = command_args(ctx);
		if (sscanf(args, "%s", filename) != 1)
			return CMD_ERROR_SYNTAX;
	} else if (ctx->argc == 1) {
		memset(filename, 0, 40);
	} else {
		return CMD_ERROR_SYNTAX;
	}

	cam_store_reply_t reply;
	int result = nanocam_store(cam_input, cam_format, cam_scale_down, filename, cam_timeout, &reply);
	if (result < 0) {
		printf("Nanocam store error: %"PRId8"\r\n", result);
	}

	printf("Result %"PRIu8"\r\n", reply.result);
	printf("Image 0x%08"PRIx32" %"PRIu32"\r\n", reply.image_ptr, reply.image_size);

	return CMD_ERROR_NONE;

}

int cmd_cam_temp(struct command_context *ctx) {
	float temp;
	if (nanocam_temp(&temp, cam_timeout) == 0) {
		printf("Temp: %02f\r\n", temp);
	} else {
		printf("Failed to contact camera\r\n");
	}

	return CMD_ERROR_NONE;
}

int cmd_cam_twi_read(struct command_context *ctx) {

	char * args = command_args(ctx);
	int reg;
	if (sscanf(args, "%x", &reg) != 1)
		return CMD_ERROR_SYNTAX;

	uint16_t return_val;
	if (nanocam_twi_read(reg, &return_val, cam_timeout) == 0) {
		printf("Register 0x%02X value 0x%04X\r\n", reg, return_val);
	} else {
		printf("Failed to contact camera\r\n");
	}

	return CMD_ERROR_NONE;

}

int cmd_cam_twi_write(struct command_context *ctx) {

	char * args = command_args(ctx);
	int reg, value;
	if (sscanf(args, "%x %x", &reg, &value) != 2)
		return CMD_ERROR_SYNTAX;

	int result = nanocam_twi_write(reg, value, cam_timeout);
	if (result >= 0) {
		printf("Register 0x%02X value 0x%04X result %d\r\n", reg, value, result);
	} else {
		printf("Failed to contact camera\r\n");
	}

	return CMD_ERROR_NONE;

}

int cmd_cam_img_list(struct command_context *ctx) {

	/* Create storage for variable length cam list */
	unsigned char buf[256];
	cam_list_t * list = (void *) &buf[0];

	if (nanocam_img_list(list, 10000) < 0) {
		printf("No reply from nanocam\r\n");
		return CMD_ERROR_FAIL;
	}

	printf("Images in RAM %"PRIu8"\r\n", list->count);

	/* Iterate */
	unsigned int i;
	for (i = 0; i < list->count; i++) {
		printf("[%02u] F:%"PRIu8", T:%"PRIu32" 0x%08"PRIx32" %"PRIu32"\r\n", i, list->images[i].format, list->images[i].time, list->images[i].ptr, list->images[i].size);
	}

	return CMD_ERROR_NONE;

}

int cmd_cam_recover_fs(struct command_context *ctx) {
	int result = nanocam_format_fs();
	printf("Format result is %d\r\n", result);
	return CMD_ERROR_NONE;
}

int cmd_cam_conf_restore(struct command_context *ctx) {
	int result = nanocam_conf_restore();
	printf("Restore result is %d\r\n", result);
	return CMD_ERROR_NONE;
}

int cmd_cam_conf_save(struct command_context *ctx) {
	int result = nanocam_conf_save();
	printf("Save result is %d\r\n", result);
	return CMD_ERROR_NONE;
}


#ifdef _X86_
int cmd_cam_focus(struct command_context *ctx) {

	int result;

	uint8_t raw_capture[1+64];
	result = nanocam_snap(cam_input, raw_capture, cam_timeout);
	if (result < 0) {
		printf("Snap error: %d\r\n", result);
		return CMD_ERROR_FAIL;
	}
	printf("Snap result: %d\r\n", result);
	hex_dump(raw_capture, 64);
	printf("\r\n");

	cam_store_reply_t reply;
	result = nanocam_store(cam_input, cam_format, cam_scale_down, "", cam_timeout, &reply);
	if (result < 0) {
		printf("Store error: %"PRId8"\r\n", result);
		return CMD_ERROR_FAIL;
	}

	printf("Store result %"PRIu8"\r\n", reply.result);
	printf("Image 0x%08"PRIx32" %"PRIu32"\r\n", reply.image_ptr, reply.image_size);

	unsigned int ftp_size;
	result = ftp_download(6, 15, "imgout.jpg", 0, 200, reply.image_ptr, reply.image_size, NULL, &ftp_size);

	if (result != 0)
		return CMD_ERROR_FAIL;

	if (ftp_status_reply() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_crc() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_done() != 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;
}
#endif

command_t __sub_command cam_confcommands[] = {
		{
				.name = "restore",
				.help = "Restore configuration",
				.handler = cmd_cam_conf_restore,
		},{
				.name = "save",
				.help = "Save configuration",
				.handler = cmd_cam_conf_save,
		},
};

command_t __sub_command cam_subcommands[] = {
		{
				.name = "node",
				.help = "Set CAM address in host table",
				.usage = "<node>",
				.handler = cmd_cam_node,
		},{
				.name = "timeout",
				.help = "Set timeout",
				.usage = "<time>",
				.handler = cmd_cam_set_timeout,
		},{
				.name = "input",
				.help = "Set input (1=codec, 0=preview)",
				.usage = "<input>",
				.handler = cmd_cam_set_input,
		},{
				.name = "format",
				.help = "Set format (0=raw, 1=bmp, 2=jpg)",
				.usage = "<format>",
				.handler = cmd_cam_set_format,
		},{
				.name = "scale",
				.help = "Set scale",
				.usage = "<scale>",
				.handler = cmd_cam_set_scale,
		},{
				.name = "sensor",
				.help = "Set sensor",
				.usage = "<id>",
				.handler = cmd_cam_set_sensor,
		},{
				.name = "snap",
				.help = "Snap picture",
				.handler = cmd_cam_snap,
		},{
				.name = "store",
				.help = "Store picture",
				.usage = "<path>",
				.handler = cmd_cam_store,
		},{
				.name = "temp",
				.help = "Get temperature",
				.handler = cmd_cam_temp,
		},{
				.name = "read",
				.help = "I2C Reg reg",
				.usage = "<reg-hex>",
				.handler = cmd_cam_twi_read,
		},{
				.name = "write",
				.help = "I2C Write reg",
				.usage = "<reg-hex> <value-hex>",
				.handler = cmd_cam_twi_write,
		},{
				.name = "list",
				.help = "List RAM images",
				.handler = cmd_cam_img_list,
		},{
				.name = "recoverfs",
				.help = "Format FS",
				.handler = cmd_cam_recover_fs,
		},{
				.name = "af",
				.help = "Auto focus routine",
				.handler = cmd_cam_autofocus,
		},{
				.name = "stat",
				.help = "Image statistics routine",
				.handler = cmd_cam_imgstat,
		},{
				.name = "conf",
				.help = "configuration system",
				.chain = INIT_CHAIN(cam_confcommands),
		},
#ifdef _X86_
		{
				.name = "focus",
				.help = "Snap, store and download loop",
				.handler = cmd_cam_focus,
		},
#endif
};

command_t __root_command cam_commands[] = {
		{
				.name = "cam",
				.help = "CAM subsystem",
				.chain = INIT_CHAIN(cam_subcommands),
		}
};

void cmd_cam_setup(void) {
	command_register(cam_commands);
}
