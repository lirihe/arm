/**
 * @file color_printf.c
 * Prints with nice colors
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdarg.h>

#include <util/log.h>
#include <util/color_printf.h>

void color_printf(color_printf_t color_arg, const char * format, ...) {

	va_list args;
	va_start(args, format);

	switch(color_arg) {
	case COLOR_RED:
		printf("\E[1;31m");
		break;
	case COLOR_YELLOW:
		printf("\E[0;33m");
		break;
	case COLOR_GREEN:
		printf("\E[0;32m");
		break;
	case COLOR_BLUE:
		printf("\E[0;34m");
		break;
	default:
		break;
	}

	vprintf(format, args);
	printf("\E[0m");

    va_end(args);

}
