/**
 * @file cmd_ftp.c
 * FTP Client Command interface
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <inttypes.h>
#include <libgen.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <ftp/ftp_client.h>
#include <ftp/ftp_types.h>

#include <util/console.h>
#include <util/timestamp.h>
#include <util/log.h>

#if 0
int ftp_upload(uint8_t host, uint8_t port, const char * path, uint8_t type, int chunk_size, uint32_t addr, const char * remote_path, uint32_t * size, uint32_t * checksum);
int ftp_status_request(void);
int ftp_data(int count);
#endif

/* Settings */
static unsigned int ftp_host = 6;
static unsigned int ftp_port = 15;
static unsigned int ftp_chunk_size = 200;

/* State variables */
static unsigned int ftp_size;

int cmd_ftp_set_host_port(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	ftp_host = atoi(ctx->argv[1]);
	ftp_port = atoi(ctx->argv[2]);

	return CMD_ERROR_NONE;

}

int cmd_ftp_download_file(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH || strlen(remote_path) < 3)
		return CMD_ERROR_SYNTAX;

	char * local_path = basename(remote_path);

	int status = ftp_download(ftp_host, ftp_port, local_path, 2, ftp_chunk_size, 0, 0, remote_path, &ftp_size);

	if (status != 0)
		return CMD_ERROR_FAIL;

	if (ftp_status_reply() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_crc() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_done() != 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}


int cmd_ftp_upload_file(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[2];
	char * local_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH || strlen(remote_path) < 3)
		return CMD_ERROR_SYNTAX;
	if (strlen(local_path) < 3)
		return CMD_ERROR_SYNTAX;


	int status = ftp_upload(ftp_host, ftp_port, local_path, 2, ftp_chunk_size,  0, remote_path, &ftp_size, NULL);
	if (status != 0)
		return CMD_ERROR_FAIL;
	if (ftp_status_request() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_data(0) != 0)
	  return CMD_ERROR_FAIL;
	if (ftp_crc() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_done() != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}


int cmd_ftp_download_mem(struct command_context *ctx) {

	if (ctx->argc != 4)
		return CMD_ERROR_SYNTAX;

	char * local_path = ctx->argv[1];
	int memaddr;
	int memsize;

	if (strlen(local_path) > FTP_PATH_LENGTH || strlen(local_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (sscanf(ctx->argv[2], "%x", &memaddr) != 1)
		return CMD_ERROR_SYNTAX;

	if (sscanf(ctx->argv[3], "%u", &memsize) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Downloading from addr 0x%X size 0x%X to file %s\r\n", memaddr, memsize, local_path);

	int status = ftp_download(ftp_host, ftp_port, local_path, 0, ftp_chunk_size, memaddr, memsize, NULL, &ftp_size);

	if (status != 0)
		return CMD_ERROR_FAIL;

	if (ftp_status_reply() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_crc() != 0)
		return CMD_ERROR_FAIL;
	if (ftp_done() != 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}

int cmd_ftp_download_run(struct command_context *ctx) {
	if (ftp_status_reply() != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_crc(struct command_context *ctx) {
	if (ftp_crc() != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_done(struct command_context *ctx) {
	if (ftp_done() != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_ls(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH || strlen(remote_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (ftp_list(ftp_host, ftp_port, 2, remote_path) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}

int cmd_ftp_remove(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH || strlen(remote_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (ftp_remove(ftp_host, ftp_port, 2, remote_path) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}

int cmd_ftp_move(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	char * from_path = ctx->argv[1];
	if (strlen(from_path) > FTP_PATH_LENGTH || strlen(from_path) < 3)
		return CMD_ERROR_SYNTAX;

	char * to_path = ctx->argv[2];
	if (strlen(to_path) > FTP_PATH_LENGTH || strlen(to_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (ftp_move(ftp_host, ftp_port, 2, from_path, to_path) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}

command_t __sub_command ftp_subcommands[] = {
	{
		.name = "done",
		.help = "done transfer",
		.handler = cmd_ftp_done,
	},{
		.name = "ls",
		.help = "list files",
		.handler = cmd_ftp_ls,
	},{
		.name = "rm",
		.help = "rm files",
		.handler = cmd_ftp_remove,
	},{
		.name = "mv",
		.help = "move files",
		.handler = cmd_ftp_move,
	},{
		.name = "crc",
		.help = "crc transfer",
		.handler = cmd_ftp_crc,
	},{
		.name = "download_run",
		.help = "Continue download (send status reply)",
		.handler = cmd_ftp_download_run,
	},{
		.name = "server",
		.help = "set host and port",
		.handler = cmd_ftp_set_host_port,
	},{
		.name = "upload_file",
		.help = "Upload file",
		.usage = "<local filename> <remote filename> ",
		.handler = cmd_ftp_upload_file,
	},{
		.name = "download_file",
		.help = "Download file",
		.usage = "<filename>",
		.handler = cmd_ftp_download_file,
	},{
		.name = "download_mem",
		.help = "Download memory",
		.handler = cmd_ftp_download_mem,
	},
};

command_t __root_command ftp_commands[] = {
	{
		.name = "ftp",
		.help = "FTP commands",
		.chain = INIT_CHAIN(ftp_subcommands),
	}
};

void cmd_ftp_setup(void) {
	command_register(ftp_commands);
}
