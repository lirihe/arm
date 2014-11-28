/**
 * @file delay.c
 * Busy-wait utility functions.
 *
 * @note The delay is performed with *interrupts enabled*
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>

#include <util/console.h>

/* Init to 8 MHz to at least get some delay
 * if delay_init is not called */
static uint32_t fcpu = 40000000UL;
static const uint32_t us_per_s = 1000000UL;

#if defined(__ARMEL__)
/* Measured value. This will change if you update the wait states */
static const uint32_t cycles_per_loop = 30;
static inline void __attribute__ ((always_inline)) delay_cycles(uint32_t cycles) {
	asm volatile (
		"L_%=:					\n\t"
		"	nop					\n\t" 
		"	subs %[c], %[c], #1	\n\t"
		"	bne L_%=			\n\t"
		: /* No output */
		: [c] "r" (cycles)
		: /* No clobbered registers */
	);
}
#elif defined(__AVR__)
#include <util/delay_basic.h>
/* http://www.nongnu.org/avr-libc/user-manual/group__util__delay__basic.html */
static const uint32_t cycles_per_loop = 4;
static inline void __attribute__ ((always_inline)) delay_cycles(uint32_t cycles) {
	_delay_loop_2((uint16_t) cycles);
}
#elif defined(__AVR32__)
/* NOTE: This value has not been measured */
static const uint32_t cycles_per_loop = 6;
static inline void __attribute__ ((always_inline)) delay_cycles(uint32_t cycles) {
	__asm__ (
		"L_%=:				\n\t"
		"	nop				\n\t" 
		"	sub %[c], 1		\n\t"
		"	brne L_%=		\n\t"
		: /* No output */
		: [c] "r" (cycles)
	);
}
#else
static const uint32_t cycles_per_loop = 1;
static inline void delay_cycles(uint32_t cycles) {
	printf("delay_us is not implemented for this platform. Since you need it, you get to implement it!\r\n");
}
#endif

void delay_us(uint32_t us) {
	/* Round up so we loop at least once */
	uint32_t cycles = (((uint64_t)us * (uint64_t)fcpu) / us_per_s) / cycles_per_loop + 1;
	delay_cycles(cycles);
}

void delay_init(uint32_t freq) {
	fcpu = freq;
}

#if 0
int cmd_delay(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	unsigned long time = atoi(ctx->argv[1]);
	delay_us(time);

	return CMD_ERROR_NONE;
}

command_t __root_command delay_commands[] = {
	{
		.name = "delay",
		.help = "Test busy wait delay loop",
		.usage = "<microseconds>",
		.handler = cmd_delay,
	},
};

void cmd_setup_delay(void) {
	command_register(delay_commands);
}
#endif
