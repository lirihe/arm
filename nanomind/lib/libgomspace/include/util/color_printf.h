/*
 * color_printf.h
 *
 *  Created on: 11/02/2011
 *      Author: johan
 */

#ifndef COLOR_PRINTF_H_
#define COLOR_PRINTF_H_

#include <stdio.h>

typedef enum color_printf_e {
	COLOR_NONE,
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_GREEN
} color_printf_t;

void color_printf(color_printf_t color_arg, const char * format, ...);

#endif /* COLOR_PRINTF_H_ */
