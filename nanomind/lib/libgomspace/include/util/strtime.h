/*
 * strtime.h
 *
 *  Created on: 22-08-2009
 *      Author: Administrator
 */

#ifndef STRTIME_H_
#define STRTIME_H_

#include <stdint.h>

/**
 * Convert Signed 64 bit time value to string time
 */
int strtime(char *str, int64_t utime, int year_base);

#endif /* STRTIME_H_ */
