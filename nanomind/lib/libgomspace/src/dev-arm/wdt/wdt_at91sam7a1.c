/*
 * wdt_at91sam7a1.c
 *
 *  Created on: 29/11/2010
 *      Author: johan
 */

#include <stdio.h>

#include <dev/arm/at91sam7.h>
#include <dev/arm/wdt.h>
#include <dev/wdt.h>

#include <freertos/FreeRTOS.h>

static volatile portBASE_TYPE clear_next = 0;

static void __attribute__((noinline)) wdt_dsr(void) {
	if (clear_next) {
		AT91_WDT->WD_CR = 0x0000C071;
		clear_next = 0;
	} else {
		printf("WDT Resetting\r\n");
	}
	AT91_WDT->WD_CSR = WDPEND;
}

static void __attribute__((naked)) wdt_isr (void) {
	portSAVE_CONTEXT();
	wdt_dsr();
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

/**
 * Clear watchdog next time a WD pending interrupt occurs.
 */
inline void wdt_clear(void) {
	clear_next = 1;
}

/**
 * Initialise and enable WDT.
 * Uses Main Clock, typically 40 MHz,
 * Total divider = 40000
 * Divided clock = 1 ms
 * Countdown value = 2048 = 2.048 seconds.
 * Pending interrupt at 0xFF = 255 ms left.
 */
inline void wdt_init(void) {

	/* Clear interrupt */
	AT91_WDT->WD_IDR = ~0UL;

	/* Mode register
	 * Sysclk = 1, main divider = 1000, second divider = 128, WDT timeout 40MHz = 6.5504 sec */
	AT91_WDT->WD_MR = 0x37000000 | (1 << 3) | ((2000/2) << 4) | (1 << 2) | 1;

	/* Overflow Mode Register */
	AT91_WDT->WD_OMR = 0x00002343;

	/* Register intterupt service routine */
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_WD, AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, wdt_isr);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_WD);

	/* Enable interrupt */
	AT91_WDT->WD_IER = WDPEND;

}

/**
 * Enable the watchdog timer. The WDT must be initialized.
 */
inline void wdt_enable(void) {

	/* Set WDEN and RSTEN bit in Overflow Mode Register */
	AT91_WDT->WD_OMR = 0x00002343;

}


/**
 * Disable the watchdog timer. The WDT must be initialized.
 */
inline void wdt_disable(void) {

	/* Clear WDEN and RSTEN bit in Overflow Mode Register */
	AT91_WDT->WD_OMR = 0x00002340;

}
