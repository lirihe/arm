/**
 * @file pca9665_isr.c
 * Low level ISR functions for PCA9665 device on NanoMind boards
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <dev/arm/at91sam7.h>
#include <dev/cpu.h>
#include <util/color_printf.h>

#include "../src/dev/pca9665/pca9665.h"

/* Number of devices present on board */
const int pca9665_device_count = 2;

/* Setup of each device */
pca9665_device_object_t device[2] = {
	{
		.base = (uint8_t *)0x58000004,
		.is_initialised = 0,
		.is_busy = 0,
		.mode = DEVICE_MODE_M_T,
		.callback = NULL,
	},
	{
		.base = (uint8_t *)0x58000000,
		.is_initialised = 0,
		.is_busy = 0,
		.mode = DEVICE_MODE_M_T,
		.callback = NULL,
	},
};

/**
 * Interrupt spam protection on ISR
 */
static void __attribute__((naked)) pca9665_isr(void) {
	static unsigned int irq_spam_count = 0;
	static portBASE_TYPE task_woken;
	portSAVE_CONTEXT();
	task_woken = pdFALSE;
	pca9665_dsr(&task_woken);
	if (task_woken == pdTRUE)
		portYIELD_FROM_ISR();
	if (AT91C_BASE_AIC->AIC_IPR & (1 << AT91C_ID_IRQ0)) {
		irq_spam_count++;
		if (irq_spam_count > 10000) {
			color_printf(COLOR_RED, "I2C IRQ spam detected rebooting\E[0m\r\n");
			AT91F_AIC_DisableIt(AT91C_BASE_AIC, AT91C_ID_IRQ0);
			if (cpu_set_reset_cause)
				cpu_set_reset_cause(CPU_RESET_IRQ_SPAM);
			cpu_reset();
		}
	} else {
		irq_spam_count = 0;
	}
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

void pca9665_isr_init(void) {
	/**
	 * Register intterupt service routine
	 * EDGE vs. LEVEL triggered:
	 * If edge triggering is used, the processor may miss one IRQ and thereby not reset the IRQ status of the PCA9665
	 * since the two PCA9665 devices shares the same interrupt pin. Therefore low level is used, this however
	 * has the drawback that if a hardware malfunction occurs, the interrupt could stay active and block the processor.
	 * Therefore IRQ spam protection must be implemented in order to recover from this.
	 * */
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_IRQ0, AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL, (void(*)(void)) pca9665_isr);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_IRQ0);
}

