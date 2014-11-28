/**
 * Boot counter for NanoMind
 *
 * @author Morten Bisgaard
 * Copyright 2013 GomSpace ApS. All rights reserved.
 */

/**
 * Read, increment and write the boot counter (/boot/boot.count)
 * @return boot count
 */
unsigned int boot_counter_increment(void);

/**
 * Outputs boot counter
 * @return boot count
 */
unsigned int boot_counter_get(void);

/**
 * Resets boot counter to 0
 */
void boot_counter_reset(void);
