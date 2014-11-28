/**
 * @file delay.c
 * Busy-wait utility functions.
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef _DELAY_H_
#define _DELAY_H_

#include <stdint.h>

/**
 * Init delay system
 * @param freq CPU frequency in Hz
 */
void delay_init(uint32_t freq);

/**
 * Delay for number of microseconds
 * @param us Number of microseconds to wait
 */
void delay_us(uint32_t us);

/**
 * Delay until condition is true or timeout_us microseconds
 * have passed.
 * @return 0 if condition was met before the timeout, -1 otherwise
 */
#define delay_until(condition, timeout_us) 						\
	({															\
		volatile unsigned int counter = timeout_us; 			\
		while (!(condition) && counter) {						\
			delay_us(1);										\
			counter--;											\
		}														\
		counter ? 0 : -1;										\
	})

#endif // _DELAY_H_
