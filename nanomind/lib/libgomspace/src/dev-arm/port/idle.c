/*
 * idle.c
 *
 *  Created on: 22-08-2009
 *      Author: Administrator
 */

#include <stdio.h>
#include <dev/cpu.h>

void vApplicationIdleHook(void) {
	/** Idle hook is disabled */
	//extern inline void cpu_pm_slm(void);
	//cpu_pm_slm();
}
