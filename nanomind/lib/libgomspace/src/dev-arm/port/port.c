/*
	FreeRTOS.org V5.3.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.
	**NOTE** The exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  Alternative commercial
	license and support terms are also available upon request.  See the 
	licensing section of http://www.FreeRTOS.org for full details.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.


	***************************************************************************
	*                                                                         *
	* Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/


/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM7 port.
 *
 * Components that can be compiled to either ARM or THUMB mode are
 * contained in this file.  The ISR routines, which can only be compiled
 * to ARM mode are contained in portISR.c.
 *----------------------------------------------------------*/

/*
	Changes from V2.5.2
		
	+ ulCriticalNesting is now saved as part of the task context, as is 
	  therefore added to the initial task stack during pxPortInitialiseStack.
*/


/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>

/* Scheduler includes. */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* Processor constants. */
#include <dev/arm/at91sam7.h>

/* Constants required to setup the task context. */
#define portINITIAL_SPSR				( ( portSTACK_TYPE ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT				( ( portSTACK_TYPE ) 0x20 )
#define portINSTRUCTION_SIZE			( ( portSTACK_TYPE ) 4 )
#define portNO_CRITICAL_SECTION_NESTING	( ( portSTACK_TYPE ) 0 )

/* Constants required to setup the tick ISR. */
#define portENABLE_TIMER			( ( unsigned portCHAR ) 0x01 )
#define portPRESCALE_VALUE			0x00
#define portINTERRUPT_ON_MATCH		( ( unsigned portLONG ) 0x01 )
#define portRESET_COUNT_ON_MATCH	( ( unsigned portLONG ) 0x02 )

/* Constants required to setup the PIT. */
#define portPIT_CLOCK_DIVISOR			( ( unsigned portLONG ) 16 )
#define portPIT_COUNTER_VALUE			( ( ( configCPU_CLOCK_HZ / portPIT_CLOCK_DIVISOR ) / 1000UL ) * portTICK_RATE_MS )

#define portINT_LEVEL_SENSITIVE  0
#define portPIT_ENABLE      	( ( unsigned portSHORT ) 0x1 << 24 )
#define portPIT_INT_ENABLE     	( ( unsigned portSHORT ) 0x1 << 25 )
/*-----------------------------------------------------------*/

/* Setup the timer to generate the tick interrupts. */
void prvSetupTimerInterrupt(uint32_t fcpu);

/* 
 * The scheduler can only be started from ARM mode, so 
 * vPortISRStartFirstSTask() is defined in portISR.c. 
 */
extern void vPortISRStartFirstTask( void );

/*-----------------------------------------------------------*/

/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description. 
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
portSTACK_TYPE *pxOriginalTOS;

	pxOriginalTOS = pxTopOfStack;

	/* Use this to align context to 8 bytes */
	pxTopOfStack--;

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro. */

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	*pxTopOfStack = ( portSTACK_TYPE ) pxCode + portINSTRUCTION_SIZE;		
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0x00000000;	/* R14 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04040404;	/* R4 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x01010101;	/* R1 */
	pxTopOfStack--;	

	/* When the task starts is will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters; /* R0 */
	pxTopOfStack--;

	/* The last thing onto the stack is the status register, which is set for
	system mode, with interrupts enabled. */
	*pxTopOfStack = ( portSTACK_TYPE ) portINITIAL_SPSR;

	#ifdef THUMB_INTERWORK
	{
		/* We want the task to start in thumb mode. */
		*pxTopOfStack |= portTHUMB_MODE_BIT;
	}
	#endif

	pxTopOfStack--;

	/* Some optimisation levels use the stack differently to others.  This 
	means the interrupt flags cannot always be stored on the stack and will
	instead be stored in a variable, which is then saved as part of the
	tasks context. */
	*pxTopOfStack = portNO_CRITICAL_SECTION_NESTING;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler(uint32_t fcpu)
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt(fcpu);

	/* Start the first task. */
	vPortISRStartFirstTask();	

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/

static int counter_clock, counter_value;

/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
void prvSetupTimerInterrupt(uint32_t fcpu) {

	/* Reset simple timer */
	ST0_CR = ST0_CR_SWRST;

	/* Enable the simple timer clock */
	ST0_ECR = ST0_ECR_ST;

	/* Prescaler register */
	int prescalar = 0;
	int syscal = 0;
	do {
		counter_clock = fcpu / (2 * (syscal + 1)) * (1/pow(2, prescalar));
		counter_value = counter_clock / configTICK_RATE_HZ;
		if (counter_value >= pow(2,16)) {
			counter_value = 0;
			syscal += 1;
		}
	} while (counter_value == 0 && syscal < (pow(2, 11) - 1));

	//printf("Clock: %u, syscal: %u, prescaler: %u, counter %u\r\n", counter_clock, syscal, prescalar, counter_value);

	if (syscal == (pow(2, 11) - 1)) {
		printf("CLOCK TOO FAST!\r\n");
	}

#if configUSE_LF_CLOCK
	ST0_PR0 = (syscal << ST0_PR0_SYSCAL_BIT) | ST0_PR0_AUTOREL | ST0_PR0_SELECTCLK | (prescalar << ST0_PR0_PRESCALAR_BIT);
#else
	ST0_PR0 = (syscal << ST0_PR0_SYSCAL_BIT) | ST0_PR0_AUTOREL | (prescalar << ST0_PR0_PRESCALAR_BIT);
#endif
	ST0_CT0 = counter_value;

	/* Reset and enable the channel 0 */
	ST0_CSR = ST0_CSR_CHEND0 | ST0_CSR_CHDIS0 | ST0_CSR_CHLD0;
	ST0_CR = ST0_CR_CHEN0;
	ST0_IER = ST0_IER_CHEND0;

	/* Setup the AIC for PIT interrupts.  The interrupt routine chosen depends
	on whether the preemptive or cooperative scheduler is being used. */
#if configUSE_PREEMPTION == 0
	extern void ( vNonPreemptiveTick ) ( void );
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_ST0, AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, ( void (*)(void) ) vNonPreemptiveTick );
#else
	extern void ( vPreemptiveTick )( void );
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_ST0, AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, ( void (*)(void) ) vPreemptiveTick );
#endif

	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_ST0);
}

int vGetRuntimeCounterValue(void) {

	/* Get the current number of ticks */
	uint32_t time = xTaskGetTickCountFromISR() * 1000 * portTICK_RATE_MS;

	/* Add the fraction to the time */
	time += (((counter_value - ST0_CCV0) * 1000) / (counter_clock / 1000)) * (portTICK_RATE_MS);

	return time;

}

