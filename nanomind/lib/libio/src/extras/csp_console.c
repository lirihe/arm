#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <csp/csp.h>
#include <csp/csp_cmp.h>
#include <csp/csp_endian.h>
#include <util/console.h>
#include <util/hexdump.h>
#include <util/base16.h>
#include <util/log.h>

#include <conf_io.h>
#if ENABLE_SNS
#include <sns/sns.h>
#endif

int cmd_ping(struct command_context *ctx) {

	char * args = command_args(ctx), features[10] = "";
	unsigned int timeout = 5000, size = 1, node = 0;
	uint32_t options = CSP_O_NONE;
	char name[11] = "";

	int scanned = sscanf(args, "%10s %u %u %9s", name, &timeout, &size, features);

	if (scanned < 1 || scanned > 4)
		return CMD_ERROR_SYNTAX;
	
	if (scanned == 4) {
		if (strchr(features, 'r'))
			options |= CSP_O_RDP;
		if (strchr(features, 'x'))
			options |= CSP_O_XTEA;
		if (strchr(features, 'h'))
			options |= CSP_O_HMAC;
		if (strchr(features, 'c'))
			options |= CSP_O_CRC32;
	}

	printf("Ping name %s, timeout %u, size %u: ", name, timeout, size);

	char * endptr;
	long int name_int = strtol(name, &endptr, 10);
	if (name_int != LONG_MIN && name_int != LONG_MAX && endptr != name) {
		node = name_int;
	}
#if ENABLE_SNS
	else {
		node = SNS(name);
	}
#endif

	int time = csp_ping(node, timeout, size, options);

	if (time < 0) {
		printf("Timeout\r\n");
	} else if (time <= 1) {
		printf("Reply in <1 tick\r\n");
	} else {
		printf("Reply in %u ms\r\n", (unsigned int) time);
	}

	return CMD_ERROR_NONE;

}

int cmd_ps(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int node, timeout = 1;

	if (sscanf(args, "%u %u", &node, &timeout) != 2)
		return CMD_ERROR_SYNTAX;

	csp_ps(node, timeout);

	return CMD_ERROR_NONE;

}

int cmd_memfree(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int node, timeout = 1;

	if (sscanf(args, "%u %u", &node, &timeout) != 2)
		return CMD_ERROR_SYNTAX;

	csp_memfree(node, timeout);

	return CMD_ERROR_NONE;

}

int cmd_reboot(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int node;

	if (sscanf(args, "%u", &node) != 1)
		return CMD_ERROR_SYNTAX;

	csp_reboot(node);

	return CMD_ERROR_NONE;

}

int cmd_buf_free(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int node, timeout = 1;

	if (sscanf(args, "%u %u", &node, &timeout) != 2)
		return CMD_ERROR_SYNTAX;

	csp_buf_free(node, timeout);

	return CMD_ERROR_NONE;
}

int cmd_uptime(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int node, timeout = 1;

	if (sscanf(args, "%u %u", &node, &timeout) != 2)
		return CMD_ERROR_SYNTAX;

	csp_uptime(node, timeout);

	return CMD_ERROR_NONE;
}

#ifdef CSP_DEBUG

int cmd_csp_route_print_table(struct command_context *ctx) {
	csp_route_print_table();
	return CMD_ERROR_NONE;
}

int cmd_csp_route_print_interfaces(struct command_context *ctx) {
	csp_route_print_interfaces();
	return CMD_ERROR_NONE;
}
int cmd_csp_conn_print_table(struct command_context *ctx) {
	csp_conn_print_table();
	return CMD_ERROR_NONE;
}

int cmd_csp_debug(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	int level = atoi(ctx->argv[1]);
	int value = atoi(ctx->argv[2]);
	csp_debug_set_level(level, value);

	return CMD_ERROR_NONE;

}
#endif

#if CSP_USE_RDP
int cmd_csp_rdp_set_opt(struct command_context *ctx) {
	char * args = command_args(ctx);
	unsigned int window_size;
	unsigned int conn_timeout;
	unsigned int packet_timeout;
	unsigned int delayed_acks;
	unsigned int ack_timeout;
	unsigned int ack_delay_count;
	if (sscanf(args, "%u %u %u %u %u %u", &window_size, &conn_timeout,
			&packet_timeout, &delayed_acks, &ack_timeout, &ack_delay_count) != 6)
		return CMD_ERROR_SYNTAX;

	printf("Setting arguments to: window size %u, conn timeout %u, packet timeout %u, delayed acks %u, ack timeout %u, ack delay count %u\r\n", window_size, conn_timeout, packet_timeout, delayed_acks, ack_timeout, ack_delay_count);

	csp_rdp_set_opt(window_size, conn_timeout, packet_timeout,
					delayed_acks, ack_timeout, ack_delay_count);

	return CMD_ERROR_NONE;

}
#endif

int cmd_cmp_ident(struct command_context *ctx) {
    
    int ret, args;
    unsigned int node;
    unsigned int timeout;
    struct csp_cmp_message msg;
   
    args = sscanf(command_args(ctx), "%u %u", &node, &timeout);
    if (args < 1)
        node = my_address;

    if (args < 2)
        timeout = 1000;

    ret = csp_cmp_ident(node, timeout, &msg);
    if (ret != CSP_ERR_NONE) {
        printf("error: %d\r\n", ret);
        return CMD_ERROR_FAIL;
    }

    printf("Hostname: %s\r\n", msg.ident.hostname);
    printf("Model:    %s\r\n", msg.ident.model);
    printf("Revision: %s\r\n", msg.ident.revision);
    printf("Date:     %s\r\n", msg.ident.date);
    printf("Time:     %s\r\n", msg.ident.time);
    
    return CMD_ERROR_NONE;
}

int cmd_cmp_route_set(struct command_context *ctx) {

	if (ctx->argc != 6)
		return CMD_ERROR_SYNTAX;

	uint8_t node = atoi(ctx->argv[1]);
	uint32_t timeout = atoi(ctx->argv[2]);
	printf("Sending route_set to node %"PRIu8" timeout %"PRIu32"\r\n", node, timeout);

	struct csp_cmp_message msg;
	msg.route_set.dest_node = atoi(ctx->argv[3]);
	msg.route_set.next_hop_mac = atoi(ctx->argv[4]);
	strncpy(msg.route_set.interface, ctx->argv[5], 10);
	printf("Dest_node: %u, next_hop_mac: %u, interface %s\r\n", msg.route_set.dest_node, msg.route_set.next_hop_mac, msg.route_set.interface);

	int ret = csp_cmp_route_set(node, timeout, &msg);
	if (ret != CSP_ERR_NONE) {
		printf("CSP Error: %d\r\n", ret);
		return CMD_ERROR_FAIL;
	} else {
		printf("Success\r\n");
	}

	return CMD_ERROR_NONE;
}

int cmd_cmp_ifc(struct command_context *ctx) {

	uint8_t node;
	uint32_t timeout;
	char * interface;

	if (ctx->argc > 4 || ctx->argc < 3)
		return CMD_ERROR_SYNTAX;

	node = atoi(ctx->argv[1]);
	interface = ctx->argv[2];

	if (ctx->argc < 4)
		timeout = 1000;
	else
		timeout = atoi(ctx->argv[3]);

    struct csp_cmp_message msg;
    strncpy(msg.if_stats.interface, interface, CSP_CMP_ROUTE_IFACE_LEN);

    printf("Requesting interface stats for interface %s\r\n", interface);

    int ret = csp_cmp_if_stats(node, timeout, &msg);
    if (ret != CSP_ERR_NONE) {
        printf("Error: %d\r\n", ret);
        return CMD_ERROR_FAIL;
    }

    msg.if_stats.tx = csp_ntoh32(msg.if_stats.tx);
    msg.if_stats.rx = csp_ntoh32(msg.if_stats.rx);
    msg.if_stats.tx_error = csp_ntoh32(msg.if_stats.tx_error);
    msg.if_stats.rx_error = csp_ntoh32(msg.if_stats.rx_error);
    msg.if_stats.drop = csp_ntoh32(msg.if_stats.drop);
    msg.if_stats.autherr = csp_ntoh32(msg.if_stats.autherr);
    msg.if_stats.frame = csp_ntoh32(msg.if_stats.frame);
    msg.if_stats.txbytes = csp_ntoh32(msg.if_stats.txbytes);
    msg.if_stats.rxbytes = csp_ntoh32(msg.if_stats.rxbytes);
    msg.if_stats.irq = csp_ntoh32(msg.if_stats.irq);

	printf("%-5s   tx: %05"PRIu32" rx: %05"PRIu32" txe: %05"PRIu32" rxe: %05"PRIu32"\r\n"
			"		drop: %05"PRIu32" autherr: %05"PRIu32 " frame: %05"PRIu32"\r\n"
			"		txb: %"PRIu32" rxb: %"PRIu32"\r\n\r\n",
			msg.if_stats.interface, msg.if_stats.tx, msg.if_stats.rx, msg.if_stats.tx_error, msg.if_stats.rx_error, msg.if_stats.drop,
			msg.if_stats.autherr, msg.if_stats.frame, msg.if_stats.txbytes, msg.if_stats.rxbytes);

	return CMD_ERROR_NONE;
}

int cmd_cmp_peek(struct command_context *ctx) {

	uint8_t node;
	uint32_t timeout;
	unsigned int addr, len;

	if ((ctx->argc != 4) && (ctx->argc != 5))
		return CMD_ERROR_SYNTAX;

	node = atoi(ctx->argv[1]);
	if (sscanf(ctx->argv[2], "%x", &addr) != 1)
		return CMD_ERROR_SYNTAX;
	if (sscanf(ctx->argv[3], "%u", &len) != 1)
		return CMD_ERROR_SYNTAX;
	if (len > CSP_CMP_PEEK_MAX_LEN) {
		printf("Max length is: %u\r\n", CSP_CMP_PEEK_MAX_LEN);
		return CMD_ERROR_SYNTAX;
	}
	if (ctx->argc < 5)
		timeout = 1000;
	else
		timeout = atoi(ctx->argv[4]);

	printf("Dumping mem from node %u addr 0x%x len %u timeout %"PRIu32"\r\n", node, addr, len, timeout);

	struct csp_cmp_message msg;
	msg.peek.addr = csp_hton32(addr);
	msg.peek.len = len;

    int ret = csp_cmp_peek(node, timeout, &msg);
    if (ret != CSP_ERR_NONE) {
        printf("Error: %d\r\n", ret);
        return CMD_ERROR_FAIL;
    }

    hex_dump(msg.peek.data, len);

	return CMD_ERROR_NONE;
}

int cmd_cmp_poke(struct command_context *ctx) {

	uint8_t node;
	uint32_t timeout;
	unsigned int addr, len;
	unsigned char data[CSP_CMP_POKE_MAX_LEN];

	if ((ctx->argc != 4) && (ctx->argc != 5))
		return CMD_ERROR_SYNTAX;

	node = atoi(ctx->argv[1]);
	if (sscanf(ctx->argv[2], "%x", &addr) != 1)
		return CMD_ERROR_SYNTAX;

	len = base16_decode(ctx->argv[3], data);
	if (len > CSP_CMP_PEEK_MAX_LEN) {
		printf("Max length is: %u\r\n", CSP_CMP_PEEK_MAX_LEN);
		return CMD_ERROR_SYNTAX;
	}

	if (ctx->argc < 5)
		timeout = 1000;
	else
		timeout = atoi(ctx->argv[4]);

	printf("Writing to mem at node %u addr 0x%x len %u timeout %"PRIu32"\r\n", node, addr, len, timeout);
	hex_dump(data, len);

	struct csp_cmp_message msg;
	msg.poke.addr = csp_hton32(addr);
	msg.poke.len = len;
	memcpy(msg.poke.data, data, CSP_CMP_POKE_MAX_LEN);

    int ret = csp_cmp_poke(node, timeout, &msg);
    if (ret != CSP_ERR_NONE) {
        printf("Error: %d\r\n", ret);
        return CMD_ERROR_FAIL;
    }

    printf("Done\r\n");
	return CMD_ERROR_NONE;

}

command_t __sub_command cmp_commands[] = {
    {
        .name = "ident",
        .help = "Node id",
        .usage = "[node] [timeout]",
        .handler = cmd_cmp_ident,
    },{
		.name = "route_set",
		.help = "Update table",
		.usage = "<node> <timeout> <addr> <mac> <ifstr>",
		.handler = cmd_cmp_route_set,
	},{
		.name = "ifc",
		.help = "Remote IFC",
		.usage = "<node> <interface> [timeout]",
		.handler = cmd_cmp_ifc,
	},{
		.name = "peek",
		.help = "Show remote memory",
		.usage = "<node> <addr> <len> [timeout]",
		.handler = cmd_cmp_peek,
	},{
		.name = "poke",
		.help = "Modify remote memory",
		.usage = "<node> <addr> <base16_data> [timeout]",
		.handler = cmd_cmp_poke,
	}
};

command_t __root_command csp_commands[] = {
    {
		.name = "ping",
		.help = "Send CSP ping",
		.usage = "<node> [timeout] [size] [opt]",
		.handler = cmd_ping,
	},{
		.name = "rps",
		.help = "Remote ps",
		.usage = "<node> <timeout>",
		.handler = cmd_ps,
	},{
		.name = "memfree",
		.help = "Memory free",
		.usage = "<node> <timeout>",
		.handler = cmd_memfree,
	},{
		.name = "buffree",
		.help = "Buffers",
		.usage = "<node> <timeout>",
		.handler = cmd_buf_free,
	},{
		.name = "reboot",
		.help = "a subsystem",
		.usage = "<node>",
		.handler = cmd_reboot,
	},{
		.name = "uptime",
		.help = "of subsystem",
		.usage = "<node> <timeout>",
		.handler = cmd_uptime,
	},{
		.name = "cmp",
		.help = "CSP management protocol",
		.chain = INIT_CHAIN(cmp_commands),
	},
#ifdef CSP_DEBUG
	{
		.name = "route",
		.help = "Show routing table",
		.handler = cmd_csp_route_print_table,
	},{
		.name = "ifc",
		.help = "Show interfaces",
		.handler = cmd_csp_route_print_interfaces,
	},{
		.name = "conn",
		.help = "Show connection table",
		.handler = cmd_csp_conn_print_table,
	},{
		.name = "debug",
		.help = "Toggle CSP debug levels ON/OFF",
		.handler = cmd_csp_debug,
	},
#endif
#if CSP_USE_RDP
	{
		.name = "rdpopt",
		.help = "Set RDP options",
		.handler = cmd_csp_rdp_set_opt,
		.usage = "<window size> <conn timeout> <packet timeout> <delayed ACKs> <ACK timeout> <ACK delay count>"
	},
#endif
};

void csp_console_init(void) {
	command_register(csp_commands);
}
