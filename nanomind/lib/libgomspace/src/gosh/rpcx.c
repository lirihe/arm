/**
 * @file rpcx.c
 * RPC without XML
 *
 * @author Jeppe Ledet-Pedersen
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include <command/command.h>
#include <command/rpcx.h>
#include <util/byteorder.h>
#include <util/log.h>

/**
 * TODO: all these data accesses are potentially unaligned!
 */
void rpcx_unpack_to_stdout(uint8_t *buf, const struct message *message) {

	unsigned int i;
	unsigned int offset = 0;

	for (i = 0; i < message->fields; i++) {
		printf("%i:%s = ", i, message->field[i].name);
		switch (message->field[i].type.type) {
		case __ARG_STRING:
			printf("%s\r\n", (char *) buf + offset);
			break;
		case __ARG_BOOL:
			printf("%u\r\n", *((unsigned int *) buf + offset));
			break;
		case __ARG_UINT8:
			printf("%hhu\r\n", *((unsigned char *) buf + offset));
			break;
		case __ARG_INT8:
			printf("%d\r\n", *((signed char *) buf + offset));
			break;
		case __ARG_UINT16:
			printf("%hu\r\n", *((unsigned short *) buf + offset));
			break;
		case __ARG_INT16:
			printf("%d\r\n", *((signed short *) buf + offset));
			break;
		case __ARG_UINT32:
			printf("%u\r\n", *((unsigned int *) buf + offset));
			break;
		case __ARG_INT32:
			printf("%d\r\n", *((signed int *) buf + offset));
			break;
		case __ARG_UINT64:
			printf("%lu\r\n", *((unsigned long *) buf + offset));
			break;
		case __ARG_INT64:
			printf("%lu\r\n", *((signed long *) buf + offset));
			break;
		case __ARG_FLOAT:
		case __ARG_DOUBLE:
		default:
			return;
		}
		offset += message->field[i].type.size;
	}

}

int rpcx_pack_from_args(uint8_t *buf, const struct message *message, unsigned int argc, char **argv) {

	if (argc != message->fields + 1)
		return -1;

	unsigned int i;
	uint64_t val_long;
	unsigned int offset = 0;

	for (i = 0; i < message->fields; i++) {
		//printf("Packing field %s size %u\r\n", message->field[i].name, message->field[i].type.size);
		switch (message->field[i].type.type) {
		case __ARG_STRING:
			memcpy(buf + offset, argv[i + 1], message->field[i].type.size);
			break;
		case __ARG_BOOL:
		case __ARG_UINT8:
		case __ARG_INT8:
		case __ARG_UINT16:
		case __ARG_INT16:
		case __ARG_UINT32:
		case __ARG_INT32:
			val_long = atol(argv[i + 1]);
			memcpy(buf + offset, &val_long, message->field[i].type.size);
			break;
#ifndef AVR
		case __ARG_UINT64:
		case __ARG_INT64:
			val_long = atoll(argv[i + 1]);
			memcpy(buf + offset, &val_long, message->field[i].type.size);
			break;
#endif
		case __ARG_FLOAT:
		case __ARG_DOUBLE:
		default:
			return -1;
		}
		offset += message->field[i].type.size;
	}

	return 0;

}

int __va_rpcx_pack(uint8_t *buf, const struct message *message, unsigned int args, va_list ap) {

	unsigned int i;
	void *val;
	unsigned int val_size;
	struct field *field;
	unsigned int offset = 0;

	//printf("Pack called with %u args\r\n", args);

	if (args != message->fields) {
		printf("Incorrect number of arguments for unpack\r\n");
		return -1;
	}

	for (i = 0; i < args; i++) {
		field = &message->field[i];
		val = va_arg(ap, void *);
		val_size = va_arg(ap, unsigned int);

		//printf("Arg %d is at %p and has size %u\r\n", i, val, val_size);
		if (val_size != field->type.size) {
			printf("Arg %d does not match size (has %u should be %u)\r\n", i, val_size, field->type.size);
			return -1;
		}
		memcpy(buf + offset, val, val_size);
		offset += field->type.size;
	}

	return 0;
}

int __va_rpcx_unpack(uint8_t *buf, const struct message *message, unsigned int args, va_list ap) {

	unsigned int i;
	void *val;
	unsigned int val_size;
	struct field *field;
	unsigned int offset = 0;

	//printf("Unpack called with %u args\r\n", args);

	if (args != message->fields) {
		printf("Incorrect number of arguments for unpack\r\n");
		return -1;
	}
	
	for (i = 0; i < args; i++) {
		field = &message->field[i];
		val = va_arg(ap, void *);
		val_size = va_arg(ap, unsigned int);

		//printf("Arg %d is at %p and has size %u\r\n", i, val, val_size);
		if (val_size != field->type.size) {
			printf("Arg %d does not match size (has %u should be %u)\r\n", i, val_size, field->type.size);
			return -1;
		}
		memcpy(val, buf + offset, val_size);
		offset += field->type.size;
	}

	return 0;
}

int __rpcx_pack(uint8_t *buf, const struct message *message, unsigned int args, ...) {
	int ret;
	va_list ap;
	va_start(ap, args);
	ret = __va_rpcx_pack(buf, message, args, ap);
	va_end(ap);
	return ret;
}

int __rpcx_unpack(uint8_t *buf, const struct message *message, unsigned int args, ...) {
	int ret;
	va_list ap;
	va_start(ap, args);
	ret = __va_rpcx_unpack(buf, message, args, ap);
	va_end(ap);
	return ret;
}

void rpcx_print_syntax(const struct message *message) {

	unsigned int i;

	printf("This is an RPC command, with the following protocol:\r\n");

	for (i = 0; i < message->fields; i++) {
		printf("  Field: %s, type: ", message->field[i].name);
		switch (message->field[i].type.type) {
		case __ARG_STRING:
			printf("STRING[%u]", message->field[i].type.size);
			break;
		case __ARG_BOOL:
			printf("Bool");
			break;
		case __ARG_UINT8:
			printf("UINT8");
			break;
		case __ARG_INT8:
			printf("INT8");
			break;
		case __ARG_UINT16:
			printf("UINT16");
			break;
		case __ARG_INT16:
			printf("INT16");
			break;
		case __ARG_UINT32:
			printf("UINT32");
			break;
		case __ARG_INT32:
			printf("INT32");
			break;
		case __ARG_UINT64:
			printf("UINT64");
			break;
		case __ARG_INT64:
			printf("INT64");
			break;
		case __ARG_FLOAT:
			printf("FLOAT");
			break;
		case __ARG_DOUBLE:
			printf("DOUBLE");
			break;
		default:
			printf("UNKNOWN");
			break;
		}
		printf(", size %u - %s\r\n", message->field[i].type.size, message->field[i].help);

	}

}
