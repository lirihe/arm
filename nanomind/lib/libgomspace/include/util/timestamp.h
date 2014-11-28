#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <stdint.h>

#define NSEC_SEC 1000000000

/*
 * timespec_t is non-portable, so this
 * structure must be used instead
 */
typedef struct __attribute__((packed)) {
	uint32_t tv_sec;
	uint32_t tv_nsec;
} timestamp_t;

int timestamp_add(timestamp_t * base, timestamp_t * add);
int timestamp_ge(timestamp_t * base, timestamp_t * test);
int timestamp_copy(timestamp_t * from, timestamp_t * to);

#endif // TIMESTAMP_H_
