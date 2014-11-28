/*
 * cpu_pm.h
 *
 *  Created on: 11/01/2011
 *      Author: johan
 */

#ifndef CPU_PM_H_
#define CPU_PM_H_

#include <stdint.h>

extern uint32_t cpu_core_clk;

void cpu_pm_pll40(void);
void cpu_pm_pll16(void);
void cpu_pm_mck(void);
void cpu_pm_slm(void);

#endif /* CPU_PM_H_ */
