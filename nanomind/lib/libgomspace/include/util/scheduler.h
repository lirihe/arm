/*
 * scheduler.h
 *
 *  Created on: Sep 1, 2009
 *      Author: karl
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/**
 * @file
 * @brief A simple, interrupt-safe scheduler for dispatching timed execution of
 * functions
 *
 * \addtogroup Scheduler
 *
 * @{
 */

typedef struct event_s {
	void (*func)(void * pvParameter);
	void * pvParameter;
	portTickType deadline;
	portTickType interval;
	struct event_s * next;
} event_t;

/**
 * Add a function pointer, a parameter pointer and a deadline
 * to be scheduled
 *
 * @param func Pointer to function to be executed
 * @param pvParameter Void pointer to the parameter to pass to the function
 * @param deadline Abosolute time of execution
 * @param interval Request the event to be scheduled at regular intervals. Set to 0 for "one-shot".
 * @return pointer to the event
 */
event_t * scheduler_add(void(*func)(void * pvParameter), void * pvParameter,
		portTickType deadline, portTickType interval);

int scheduler_remove(event_t * event);

/**
 * Initialize the scheduler. Must be called before any other scheduler functions
 */
void scheduler_init(void);

/**
 * Print a list of scheduled event deadlines
 */
void scheduler_show_list(void);


/**
 * }@
 */

#endif /* SCHEDULER_H_ */
