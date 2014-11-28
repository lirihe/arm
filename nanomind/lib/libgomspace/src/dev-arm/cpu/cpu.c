/*
 * cpu.c
 *
 *  Created on: Aug 13, 2009
 *      Author: karl
 */
#include <stdio.h>
#include <inttypes.h>
#include <dev/cpu.h>
#include <dev/arm/wdt.h>
#include <dev/arm/at91sam7.h>
#include <util/crc32.h>
#include <freertos/FreeRTOS.h>

/* Usart baud setting */
static AT91PS_USART USARTbase = AT91C_BASE_USART0;

/* SPI buad setting */
#include <dev/spi.h>
extern spi_chip_t * current_chip;

uint32_t cpu_core_clk = 40000000;

/* Persistant store for reset cause */
static unsigned int __attribute__((section(".ram_persist"))) cpu_reset_cause;
static unsigned int __attribute__((section(".ram_persist"))) cpu_reset_cause_crc;

void __attribute__((weak)) cpu_set_reset_cause(cpu_reset_cause_t cause) {
	cpu_reset_cause = cause;
	cpu_reset_cause_crc = chksum_crc32((unsigned char *) &cpu_reset_cause, sizeof(unsigned int));
	//printf("CAUSE %p, CRC %p\r\n", &cpu_reset_cause, &cpu_reset_cause_crc);
	//printf("CAUSE %u, CRC %u\r\n", cpu_reset_cause, cpu_reset_cause_crc);
}

cpu_reset_cause_t __attribute__((weak)) cpu_read_reset_cause(void) {
	//printf("CAUSE %p, CRC %p\r\n", &cpu_reset_cause, &cpu_reset_cause_crc);
	//printf("CAUSE %u, CRC %u\r\n", cpu_reset_cause, cpu_reset_cause_crc);
	unsigned int cause = cpu_reset_cause;
	if (chksum_crc32((unsigned char *) &cause, sizeof(unsigned int)) == cpu_reset_cause_crc) {
		cpu_reset_cause = 0;
		return cause;
	}
	return CPU_RESET_NONE;
}

void cpu_reset(void) {
	AT91_WDT->WD_MR = 0x37000000 | (1 << 3);
    AT91_WDT->WD_OMR = 0x00002343;
    AT91_WDT->WD_IDR = ~0UL;
}

void cpu_clock_info(void) {
	printf("Clock info %08"PRIx32"\r\n", (uint32_t) AT91C_BASE_CM->CM_CS);
}

void cpu_rtc_enable(void) {
	AT91C_BASE_CM->CM_CE = 0x230500C0;
}

void cpu_rtc_disable(void) {
	AT91C_BASE_CM->CM_CD = 0x180700C0;
}

void cpu_pm_pll40(void) {
	AT91C_BASE_CM->CM_CD = 0x18070024;
	AT91C_BASE_CM->CM_PDIV = 0x762d0000 | 5; /* PLL Multiplier*/
	AT91C_BASE_CM->CM_CE = 0x23050004;
	USARTbase->BRGR = 5;
	cpu_core_clk = 40000000;
	current_chip = NULL;
}

void cpu_pm_pll16(void) {
	AT91C_BASE_CM->CM_CD = 0x18070024;
	AT91C_BASE_CM->CM_PDIV = 0x762d0000 | 2; /* PLL Multiplier */
	AT91C_BASE_CM->CM_CE = 0x23050004;
	USARTbase->BRGR = 2;
	cpu_core_clk = 16000000;
	current_chip = NULL;
}

void cpu_pm_mck(void) {
	AT91C_BASE_CM->CM_CD = 0x18070024;
	USARTbase->BRGR = 13;
	cpu_core_clk = 8000000;
	current_chip = NULL;
}

void cpu_pm_slm(void) {
	AT91C_BASE_CM->CM_CD = 0x1807006C;
	AT91C_BASE_CM->CM_CE = 0x23050020;
	USARTbase->BRGR = 13;
	cpu_core_clk = 2000000;
	current_chip = NULL;
}

