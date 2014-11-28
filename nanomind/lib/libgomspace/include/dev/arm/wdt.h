/*
 * wdt.h
 *
 *  Created on: 29/11/2010
 *      Author: johan
 */

#ifndef WDT_ARM_H_
#define WDT_ARM_H_

#include <dev/arm/at91sam7.h>

typedef struct {
	AT91_REG	 Reserved0[24];
	AT91_REG	 WD_CR; 	// Control Register
	AT91_REG	 WD_MR; 	// Mode Register
	AT91_REG	 WD_OMR; 	// Overflow MOde Register
	AT91_REG	 WD_CSR; 	// Clear Status Register
	AT91_REG	 WD_SR; 	// Status Register
	AT91_REG	 WD_IER; 	// Interrupt Enable Register
	AT91_REG	 WD_IDR; 	// Interrupt Disable Register
	AT91_REG	 WD_IMR; 	// Interrupt Mask Register
	AT91_REG	 WD_TR; 	// Test Register
	AT91_REG	 WD_CTR; 	// Counter Test Register
	AT91_REG	 WD_PTR; 	// Preload Test Register
} at91_wdt_t, *at91_wdt_pt;

#define AT91_WDT ((at91_wdt_pt) 0xFFFA0000)

#define WDPEND 	0x01
#define WDOVF	0x02

#define WDEN	0x01
#define RSTEN	0x02

#endif /* WDT_ARM_H_ */
