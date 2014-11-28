/*
 * timestamp.c
 *
 *  Created on: Jan 17, 2011
 *      Author: jeppe
 */

#include <util/timestamp.h>

int timestamp_add(timestamp_t * base, timestamp_t * add) {

	if (!base || !add)
		return -1;

	base->tv_sec += add->tv_sec;
	if (base->tv_nsec + add->tv_nsec >= NSEC_SEC) {
		base->tv_sec++;
		base->tv_nsec = (base->tv_nsec + add->tv_nsec) % NSEC_SEC;
	} else {
		base->tv_nsec += add->tv_nsec;
	}

	return 0;
}

/* Test is timestamp is greater or equal */
int timestamp_ge(timestamp_t * base, timestamp_t * test) {

	if (!base || !test)
		return -1;

	if (test->tv_sec > base->tv_sec ||
			(test->tv_sec == base->tv_sec &&
					test->tv_nsec > base->tv_nsec)) {
		return 1;
	} else {
		return 0;
	}

}

int timestamp_copy(timestamp_t * from, timestamp_t * to) {

	if (!from || !to)
		return -1;

	to->tv_sec = from->tv_sec;
	to->tv_nsec = from->tv_nsec;

	return 0;
}
