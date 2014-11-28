#include <inttypes.h>
#include <stdint.h>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <util/clock.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* Local time offset */
static timestamp_t offset = {0, 0};

void clock_get_time(timestamp_t * time) {

	uint32_t clocks = xTaskGetTickCount();

	time->tv_sec = offset.tv_sec + clocks / configTICK_RATE_HZ;
	time->tv_nsec = (offset.tv_nsec + (clocks % configTICK_RATE_HZ) * 1000000) % 1000000000;

}

void clock_set_time(timestamp_t * time) {

	uint32_t clocks = xTaskGetTickCount();

	offset.tv_sec = time->tv_sec - clocks / configTICK_RATE_HZ;
	offset.tv_nsec = (time->tv_nsec - (clocks % configTICK_RATE_HZ) * 1000000) % 1000000000;

}

void clock_get_monotonic(timestamp_t * time) {

	uint32_t clocks = xTaskGetTickCount();

	time->tv_sec = clocks / configTICK_RATE_HZ;
	time->tv_nsec = (clocks % configTICK_RATE_HZ) * (1000000000/configTICK_RATE_HZ);

}
