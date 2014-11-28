/*
 * boot.c
 *
 *  Created on: Aug 11, 2009
 *      Author: johan
 */

#include <dev/arm/at91sam7.h>
#include <dev/arm/cpu_pm.h>
#include <dev/wdt.h>

#include <conf_gomspace.h>

/* These functions are weak linked, which means that if you provide an implementation
 * it will be called by the interrupt routine. Otherwise the system will crash upon
 * unhandled IRQ's or Spurious interrupts...
 */
extern void AT91F_Default_FIQ_handler(void) __attribute__ ((weak));
extern void AT91F_Default_IRQ_handler(void) __attribute__ ((weak));
extern void AT91F_Spurious_handler(void) __attribute__ ((weak));
extern void AT91F_Soft_Reset_Handler(void) __attribute__ ((weak));
extern void AT91F_Undefined_Instruction_Handler(void) __attribute__ ((weak));
extern void AT91F_Swi_Handler(void) __attribute__ ((weak));
extern void AT91F_Prefetch_Abort_Handler(void) __attribute__ ((weak));
extern void AT91F_Data_Abort_Handler(void) __attribute__ ((weak));

/* This function is executed by the boot-script before the main() routine is called
 * A simple stack has been established to store local variables, but no static or global
 * variables are allowed.
 */

#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xE51FFF20) // ldr, pc, [pc, #-&F20]

inline static void set_isr_vector(unsigned int * vector, void (*handler)(void)) {
	if ((unsigned int) handler == (unsigned int) AT91C_AIC_BRANCH_OPCODE) {
		*vector = (unsigned int) AT91C_AIC_BRANCH_OPCODE;
	} else {
		*(vector + 8) = (unsigned int) handler;
		*vector = 0xe59ff000 + 0x18;
	}
}

void low_level_init(void) {

	int * src, * dst;

	/** SYSTEM CLOCK / PLL INIT **/
	cpu_pm_pll40();
	AT91C_BASE_CM->CM_MDIV= 0xacdc0000 | 1;				/* LFCLK = MCK / 2 * (MDIV + 1) */

	/** Relocate DATA */
	extern int _data_start, _data_end, _data_lma;
	src = &_data_lma;
	dst = &_data_start;
	while (dst < &_data_end)
		*dst++ = *src++;

	/** Relocate SRAM */
	extern int _sram_start, _sram_end, _sram_lma;
	src = &_sram_lma;
	dst = &_sram_start;
	while (dst < &_sram_end)
		*dst++ = *src++;

	/** Clear BSS */
	extern int _szero, _ezero;
	for (dst = &_szero; dst < &_ezero;)
		*dst++ = 0;

	/** INTERRUPT INIT **/
	int i;

	/* Disable and clear all interrupts */
	AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
	AT91C_BASE_AIC->AIC_ICCR = 0xFFFFFFFF;

	/* Unstack a level by writting in AIC_EOICR */
	/* Value written does not matter */
	for (i = 0 ; i < 8 ; i++)
		AT91C_BASE_AIC->AIC_EOICR = i;

	/* Set up the default interrupts handler vectors in the GIC_SVR table */
	AT91C_BASE_AIC->AIC_SPU = (int) AT91F_Spurious_handler;
	AT91C_BASE_AIC->AIC_SVR[0] = (int) AT91F_Default_FIQ_handler;
	for (i = 1; i < 31; i++)
	    AT91C_BASE_AIC->AIC_SVR[i] = (int) AT91F_Default_IRQ_handler;

	/** Setup interrupt vectors */
	set_isr_vector((unsigned int *) 0x00, AT91F_Soft_Reset_Handler);
	set_isr_vector((unsigned int *) 0x04, AT91F_Undefined_Instruction_Handler);
	set_isr_vector((unsigned int *) 0x08, AT91F_Swi_Handler);
	set_isr_vector((unsigned int *) 0x0C, AT91F_Prefetch_Abort_Handler);
	set_isr_vector((unsigned int *) 0x10, AT91F_Data_Abort_Handler);
	set_isr_vector((unsigned int *) 0x14, AT91F_Prefetch_Abort_Handler);
	set_isr_vector((unsigned int *) 0x18, AT91C_AIC_BRANCH_OPCODE);
	set_isr_vector((unsigned int *) 0x1C, AT91C_AIC_BRANCH_OPCODE);

#if defined(ENABLE_SUPERVISOR)
	/** Enable Watchdog Timer */
	wdt_init();
#endif

	/** C++ exception handling */
	//extern char __eh_frame_start[];
	//void __register_frame(void *);
	//__register_frame(&__eh_frame_start);

}

