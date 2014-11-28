/**
 * Supervisor Task
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#ifndef _SUPERVISOR_H_
#define _SUPERVISOR_H_

#include <stdint.h>
#include <util/console.h>

/**
 * Initialise and create supervisor task
 * This task will wake up to reset the hardware watchdog at a rate of initial_timeout/2.
 * A task can register a software heartbeat using the sv_add function, and decide its own timeout.
 * If a software heartbeat fails, the supervisor will not reset the hardware watchdog, and the system will reboot.
 * @param initial_timeout [ms] must be set to a value lower than the hardware watchdog timeout
 * @return 0 if OK, -1 if ERR
 */
int sv_init(uint32_t initial_timeout);

/**
 * Add a software heartbeat to the watchdog
 * @param name string containing the name of the task or the heartbeat
 * @param timeout [ms] must be set to a value min. 2 times higher than the expected heartbeat rate
 * @return id of software heartbeat (must be used with sv_reset(id) to clear), or -1 if ERR
 */
int sv_add(char * name, uint32_t timeout);

/**
 * Clear the software heartbeat timeout counter
 * @param id of software heartbeat
 * @return 0 if OK, -1 if ERR
 */
int sv_reset(unsigned int id);

/**
 * Print a list of all supervised tasks
 */
int sv_print(struct command_context *ctx);

#endif /* _SUPERVISOR_H_ */
