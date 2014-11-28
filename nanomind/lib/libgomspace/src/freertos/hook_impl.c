/*
 * hook_impl.c
 *
 *  Created on: Jun 4, 2010
 *      Author: johan
 */

#include <dev/cpu.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <util/log.h>

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {

	printf("STACK OVERFLOW!\r\n");
	printf("In task %p name: %s\r\n", pxTask, pcTaskName);

	volatile unsigned int i = 0xFFFF;
	while(i--);
	if (cpu_set_reset_cause)
		cpu_set_reset_cause(CPU_RESET_STACK_OVERFLOW);
	cpu_reset();

}
