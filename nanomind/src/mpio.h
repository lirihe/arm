/**
 * IO header for NanoMind A712-D revision and onwards
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef IO_H_
#define IO_H_

#include <dev/arm/at91sam7.h>

#define IO_PIN_47			0		// T0TIOA0
#define IO_PIN_48			1		// T0TIOB0
#define IO_PIN_49			2		// T0TCLK0
#define IO_PIN_50			3		// T0TIOA1
#define IO_PIN_51			4		// T0TIOB1
#define IO_PIN_52			5		// T0TCLK1
#define IO_PIN_53			6		// T0TIOA2

/**
 * Initialise IO pin
 * @param pin number
 * @param output 1 = set as output, 0 = set as input
 */
void io_init(int pin, int output);

/**
 * Read value of IO pin
 * @param pin number
 * @return value of IO pin
 */
int io_get(int pin);

/**
 * Set IO pin high
 * @param pin number
 */
void io_set(int pin);

/**
 * Set IO pin low
 * @param pin number
 */
void io_clear(int pin);

#endif /* IO_H_ */
