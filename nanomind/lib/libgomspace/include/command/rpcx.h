/**
 * @file rpcx.h
 * RPC without XML
 *
 * @author Jeppe Ledet-Pedersen
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

/* Get number of arguments in __VA_ARGS__ */
#define VA_NARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define VA_NARGS(...) VA_NARGS_IMPL(,##__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)

/* This is how you make a Python programmer cry... */
#define __rpcx_pack0(_buf, _fields, _count) \
	__rpcx_pack(_buf, _fields, _count)
#define __rpcx_pack1(_buf, _fields, _count, _a1) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1))
#define __rpcx_pack2(_buf, _fields, _count, _a1, _a2) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2))
#define __rpcx_pack3(_buf, _fields, _count, _a1, _a2, _a3) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3))
#define __rpcx_pack4(_buf, _fields, _count, _a1, _a2, _a3, _a4) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4))
#define __rpcx_pack5(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5))
#define __rpcx_pack6(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6))
#define __rpcx_pack7(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6, _a7) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6), _a7, sizeof(*_a7))
#define __rpcx_pack8(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6), _a7, sizeof(*_a7), _a8, sizeof(*_a8))
#define __rpcx_pack9(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9) \
	__rpcx_pack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6), _a7, sizeof(*_a7), _a8, sizeof(*_a8), _a9, sizeof(*_a9))
#define __rpcx_pack_count(_buf, _fields, _count, ...) \
	__rpcx_pack ## _count (_buf, _fields, _count, ##__VA_ARGS__)
#define __rpcx_pack_precount(_buf, _fields, _count, ...) \
	__rpcx_pack_count(_buf, _fields, _count, ##__VA_ARGS__)
#define rpcx_pack(_buf, _fields, ...) \
	__rpcx_pack_precount(_buf, _fields, VA_NARGS(__VA_ARGS__), ##__VA_ARGS__)

int __rpcx_pack(uint8_t *buf, const struct message *message, unsigned int args, ...);

/* ...and Java programmers too */
#define __rpcx_unpack0(_buf, _fields, _count) \
	__rpcx_unpack(_buf, _fields, _count)
#define __rpcx_unpack1(_buf, _fields, _count, _a1) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1))
#define __rpcx_unpack2(_buf, _fields, _count, _a1, _a2) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2))
#define __rpcx_unpack3(_buf, _fields, _count, _a1, _a2, _a3) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3))
#define __rpcx_unpack4(_buf, _fields, _count, _a1, _a2, _a3, _a4) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4))
#define __rpcx_unpack5(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5))
#define __rpcx_unpack6(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6))
#define __rpcx_unpack7(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6, _a7) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6), _a7, sizeof(*_a7))
#define __rpcx_unpack8(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6), _a7, sizeof(*_a7), _a8, sizeof(*_a8))
#define __rpcx_unpack9(_buf, _fields, _count, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9) \
	__rpcx_unpack(_buf, _fields, _count, _a1, sizeof(*_a1), _a2, sizeof(*_a2), _a3, sizeof(*_a3), _a4, sizeof(*_a4), _a5, sizeof(*_a5), _a6, sizeof(*_a6), _a7, sizeof(*_a7), _a8, sizeof(*_a8), _a9, sizeof(*_a9))
#define __rpcx_unpack_count(_buf, _fields, _count, ...) \
	__rpcx_unpack ## _count (_buf, _fields, _count, ##__VA_ARGS__)
#define __rpcx_unpack_precount(_buf, _fields, _count, ...) \
	__rpcx_unpack_count(_buf, _fields, _count, ##__VA_ARGS__)
#define rpcx_unpack(_buf, _fields, ...) \
	__rpcx_unpack_precount(_buf, _fields, VA_NARGS(__VA_ARGS__), ##__VA_ARGS__)

int __rpcx_unpack(uint8_t *buf, const struct message *message, unsigned int args, ...);

int rpcx_prepare_messages(struct message *messages);
int rpcx_pack_from_args(uint8_t *buf, const struct message *message, unsigned int argc, char * argv[]);
void rpcx_print_syntax(const struct message *message);
void rpcx_unpack_to_stdout(uint8_t *buf, const struct message *message);

