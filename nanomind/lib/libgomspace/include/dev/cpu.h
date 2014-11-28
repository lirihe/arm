/*
 * cpu.h
 *
 *  Created on: Aug 13, 2009
 *      Author: karl
 */

#ifndef CPU_H_
#define CPU_H_
/**
 * @file
 * @brief Controls CPU speed and reset
 * \addtogroup CPU
 *
 * @{
 */

typedef enum {
	CPU_RESET_NONE = 0,
	CPU_RESET_USER = 1,
	CPU_RESET_SUPERVISOR,
	CPU_RESET_STACK_OVERFLOW,
	CPU_RESET_DATA_ABORT,
	CPU_RESET_PREFETCH_ABORT,
	CPU_RESET_UNDEFINED_INSTRUCTION,
	CPU_RESET_SOFT_RESET,
	CPU_RESET_IRQ_SPAM,
	CPU_RESET_FS_FORMAT,
} cpu_reset_cause_t;

void __attribute__((weak)) cpu_set_reset_cause(cpu_reset_cause_t cause);
cpu_reset_cause_t __attribute__((weak)) cpu_read_reset_cause(void);

void cpu_reset(void);
/**
 * }@
 */
#endif /* CPU_H_ */
