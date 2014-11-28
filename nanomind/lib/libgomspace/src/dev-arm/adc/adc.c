/*
 * adc.c
 *
 *  Created on: Aug 24, 2009
 *      Author: karl
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <dev/adc.h>

#include <dev/arm/at91sam7.h>

uint16_t adc_buffer[8];

xSemaphoreHandle sem;

void adc_ISR(void);
void adc_DSR(void);


#define ADC_DR    0xFFFC0080
#define PDC_PRA10_DATA ADC_DR

// ADC -> RAM
#define ADC_DIR 0
// half-word
#define ADC_SIZE 1
#define PDC_CR10_DATA ((ADC_DIR<<0)|(ADC_SIZE<<1))

// scale ADC clock to 400kHz at 40MHz CORECLK
#define PRLVAL 31
#define STOPEN 1
// 5 micro-seconds at 40MHz CORECLK
#define STARTUPTIME 200
// number of conversions = NBRCH+1
#define NBRCH 7
#define CONTCV 0
#define ADC_MR ((volatile uint32_t*)0xFFFC0064)
#define ADC_MR_DATA ((PRLVAL<<0)|(STOPEN<<6)|(STARTUPTIME<<8)|(NBRCH<<16)|(CONTCV<<19))

#define ADC_ENABLE_CLOCK() *((volatile uint32_t*)0xFFFC0050) = 2
#define ADC_DISABLE_CLOCK() *((volatile uint32_t*)0xFFFC0054) = 2
#define ADC_PM_REG ((volatile uint32_t*)0xFFFC0058)
#define ADC_CLOCK_ENABLED ((*ADC_PM_REG & 0x2)>>1)

#define ADC_CR ((volatile uint32_t*)0xFFFC0060)
#define ADC_ENABLE() *ADC_CR = (1<<1)
#define ADC_DISABLE() *ADC_CR = (1<<2)
#define ADC_START() *ADC_CR = (1<<3)
#define ADC_STOP() *ADC_CR = (1<<4)

#define CONV_SHIFT(conv_number,conv_port) (conv_port<<((conv_number-1)*4))
#define ADC_CMR ((volatile uint32_t*)0xFFFC0068)

#define ADC_CMR_DATA (\
		CONV_SHIFT(1,0)|\
		CONV_SHIFT(2,1)|\
		CONV_SHIFT(3,2)|\
		CONV_SHIFT(4,3)|\
		CONV_SHIFT(5,4)|\
		CONV_SHIFT(6,5)|\
		CONV_SHIFT(7,6)|\
		CONV_SHIFT(8,7))

#define ADC_SR ((volatile uint32_t*)0xFFFC0070)
#define ADC_READY ((*ADC_SR & 0x2)>>1)

#define ADC_IER ((volatile uint32_t*)0xFFFC0074)
#define ADC_IDR ((volatile uint32_t*)0xFFFC0078)
#define ADC_IMR ((volatile uint32_t*)0xFFFC007C)
#define ADC_CSR ((volatile uint32_t*)0xFFFC006C)
#define ADC_TEND_INT (1<<3)
#define ADC_CLEAR_TEND_INT() *ADC_CSR = (1<<3)
#define ADC_CLEAR_OVR_INT() *ADC_CSR = (1<<2)


void adc_setup(void) {
	// PDC setup
	PDC_PRA10 = PDC_PRA10_DATA;
	PDC_CR10 = PDC_CR10_DATA;
	// ADC setup
	ADC_ENABLE_CLOCK();
	int i = 200;
	while (!ADC_CLOCK_ENABLED && i)
		i--;
	*ADC_MR = ADC_MR_DATA;
	*ADC_CMR = ADC_CMR_DATA;
	*ADC_IDR = *ADC_IMR;
	*ADC_IER = ADC_TEND_INT;
	ADC_DISABLE_CLOCK();

	// Register intterupt service routine
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_ADC0,
			AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,
			(void(*)(void)) adc_ISR);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_ADC0);

	vSemaphoreCreateBinary(sem);
	xSemaphoreTake(sem, 0);
}

void adc_start(void) {
	static int has_been_setup = 0;
	volatile int i;
	if (has_been_setup == 0) {
		adc_setup();
		has_been_setup = 1;
	}

	// Start the
	for (i = 0; i < 8; i++)
		adc_buffer[i] = 0x0000;
	ADC_ENABLE_CLOCK();
	i = 200;
	while (!ADC_CLOCK_ENABLED && i)
		i--;
	ADC_ENABLE();
	i = 200;
	while (!ADC_READY && i)
		i--;
	PDC_MPR10 = (uint32_t)(&adc_buffer[0]);
	PDC_TCR10 = 8;
	ADC_CLEAR_OVR_INT();
	ADC_START();
}

uint16_t * adc_start_blocking(portTickType timeout) {
	adc_start();
	if(xSemaphoreTake(sem, configTICK_RATE_HZ*timeout))
		return adc_buffer;
	return NULL;
}

void adc_stop(void) {
	ADC_DISABLE();
	ADC_DISABLE_CLOCK();
}

__attribute__((naked)) void adc_ISR(void) {
	portSAVE_CONTEXT();
	adc_DSR();
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

void adc_DSR(void) {
	static signed portBASE_TYPE xTaskWoken;
	ADC_CLEAR_TEND_INT();
	ADC_CLEAR_OVR_INT();
	adc_stop();
	xSemaphoreGiveFromISR(sem, &xTaskWoken);

	if (xTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR();
	}
}
