/*
 * time.h
 *
 *  Created on: 20/05/2010
 *      Author: Johan Christiansen
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <util/timestamp.h>

void clock_uptime(timestamp_t * time);
void clock_get_time(timestamp_t *);
void clock_set_time(timestamp_t *);
void clock_get_monotonic(timestamp_t * time);

#endif /* CLOCK_H_ */
