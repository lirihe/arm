/**
 * @file pca9665.h
 * Header for PCA9665 driver
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef PCA9665_H_
#define PCA9665_H_

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <dev/i2c.h>

/** Various definitions */
#define MAX_DEVICES						2
#define DEVICE_MODE_M_T					0
#define DEVICE_MODE_M_R					1

/** Type declarations */
typedef struct pca9665_transmission_object_s {
	xQueueHandle queue;
	i2c_frame_t * frame;
	uint16_t next_byte;
} pca9665_transmission_object_t;

typedef struct pca9665_device_object_s {
	uint8_t * base;
	pca9665_transmission_object_t rx;
	pca9665_transmission_object_t tx;
	uint16_t speed;
	volatile unsigned int is_initialised;
	volatile unsigned int slave_addr;
	volatile unsigned int is_busy;
	volatile unsigned int mode;
	i2c_callback_t callback;
} pca9665_device_object_t;

/** Implemented in board specifc context */
extern pca9665_device_object_t device[];
extern const int pca9665_device_count;
void pca9665_isr_init(void);

/** Implemented in driver */
void pca9665_dsr(portBASE_TYPE * task_woken);

#endif /* PCA9665_H_ */
