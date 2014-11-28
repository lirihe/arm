/*
 * adc.h
 *
 *  Created on: Aug 24, 2009
 *      Author: karl
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <freertos/FreeRTOS.h>

/**
 * @file
 * @brief Access to analog-to-digital conversion
 * \addtogroup ADC
 *
 * @{
 */

/**
 * Start AD conversion on all 8 channels
 * @param timeout Number of ticks to wait before returning
 * @return Pointer to uint16_t data[8]. Returns NULL if timed out
 */
uint16_t * adc_start_blocking(portTickType timeout);

/**
 * }@
 */

#endif /* ADC_H_ */
