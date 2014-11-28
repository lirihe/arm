/**
 * Supervisor Task
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <dev/cpu.h>
#include <dev/wdt.h>
#include <util/log.h>
#include <util/console.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

typedef struct sv_element_s {
	char * name;				/**< Task name */
	unsigned int id;			/**< Task id */
	uint32_t timer;				/**< Task timer value */
	uint32_t timeout;			/**< Task timeout value */
	struct sv_element_s * next;	/**< Pointer to next task in list */
} sv_task_t;

static uint32_t interval = 1000;
static int sv_next_id = 0;
static xSemaphoreHandle sv_sem;

static sv_task_t * sv_head = NULL;
static sv_task_t * sv_tail = NULL;

/* Calculate greatest common divisor of a and b
 * using the Euclidean algorithm */
uint32_t gcd(uint32_t a, uint32_t b) {
	uint32_t t;
	while (b) {
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}

/* Return current time */
uint32_t time_now(void) {
	return (uint32_t)(xTaskGetTickCount() * (1000/configTICK_RATE_HZ));
}

void sv_task(void * param) {

	uint32_t now, last = time_now();
	while (1) {
		/* Clear watchdog timer */
		wdt_clear();

		/* Check timeouts */
		if (xSemaphoreTake(sv_sem, 1 * configTICK_RATE_HZ) == pdPASS) {
			sv_task_t * t = sv_head;
			while (t) {
				now = time_now();
				t->timer += now - last;
				if (t->timer >= t->timeout) {
					printf("Supervisor timeout for %s - Rebooting system!\r\n", t->name);
					if (cpu_set_reset_cause)
						cpu_set_reset_cause(CPU_RESET_SUPERVISOR);
					cpu_reset();
				}
				t = t->next;
			}
			xSemaphoreGive(sv_sem);
		}

		/* Sleep quarter of interval */
		last = time_now();
		vTaskDelay((interval * configTICK_RATE_HZ / 4) / 1000);
	}
}

int sv_init(uint32_t initial_timeout) {

	/* Init list semaphore */
	vSemaphoreCreateBinary(sv_sem);
	if (sv_sem == NULL) {
		printf("Failed to create supervisor semaphore\r\n");
		return -1;
	}

	/* Set maximum timeout */
	interval = initial_timeout;

	/* Start task */
	if (xTaskCreate(&sv_task, (signed char *)"SV", 1024, NULL, configMAX_PRIORITIES - 1, NULL) != pdPASS) {
		printf("Failed to create supervisor task\r\n");
		return -1;
	}

	return 0;

}

int sv_add(char * name, uint32_t timeout) {
	sv_task_t * t = malloc(sizeof(sv_task_t));
	if (t == NULL) {
		printf("Failed to allocate memory for supervisor entry\r\n");
		return -1;
	}

	if (xSemaphoreTake(sv_sem, 1 * configTICK_RATE_HZ) != pdPASS) {
		free(t);
		return -1;
	}

	/* Set task properties */
	t->name = name;
	t->id = sv_next_id++;
	t->timer = 0;
	t->timeout = timeout;
	t->next = NULL;

	/* Adjust interval */
	interval = gcd(interval, timeout);

	if (interval < 100) {
		printf("GCD of intervals too small, changed to 100 ms\r\n");
		interval = 100;
	}

	/* Insert task */
	if (sv_head == NULL && sv_tail == NULL) {
		sv_head = t;
		sv_tail = t;
	} else {
		sv_tail->next = t;
		sv_tail = t;
	}

	xSemaphoreGive(sv_sem);

	return t->id;
}

int sv_reset(unsigned int id) {

	if (xSemaphoreTake(sv_sem, 1 * configTICK_RATE_HZ) != pdPASS)
		return -1;

	/* Loop task list */
	sv_task_t * i = sv_head;
	while (i) {
		if (i->id == id) {
			i->timer = 0;
			break;
		}
		i = i->next;
	}

	xSemaphoreGive(sv_sem);

	return 0;
}

int sv_print(struct command_context *ctx) {

	/* Loop task list */
	sv_task_t * i = sv_head;
	if (i) {
		printf("ID\tTask\tTimer\tTimeout\r\n");
		while (i) {
			printf("%u\t%s\t%"PRIu32"\t%"PRIu32"\r\n",
					i->id, i->name, i->timer, i->timeout);
			i = i->next;
		}
	} else {
		printf("No tasks being supervised\r\n");
	}

	return CMD_ERROR_NONE;

}
