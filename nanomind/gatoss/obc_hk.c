/**
 * Housekeeping task
 *
 * @author Morten Jensen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <dev/adc.h>
#include <dev/spi.h>
#include <dev/lm70.h>
#include <dev/bootcounter.h>

#include <io/cdh.h>

#include "obc_hk.h"

#define ADC_TO_MV(adc) ((int) ((adc * 2493.0) / 1023.0))
#define LM60_MV_TO_C(mv) ((mv - 424) / 6.25)

#define LM70_MAP_SIZE 6
extern spi_dev_t spi_dev;
static spi_chip_t lm70_chip[LM70_MAP_SIZE];
static int lm70_map[LM70_MAP_SIZE] = {2, 4, 6, 9, 11, 13};

void obc_hk_get(obc_hk_t * hk) {

	int i;

	/* boot count */
	hk->bootcount = boot_counter_get();

	/* Start sampling */
	uint16_t * adc_buffer = adc_start_blocking(10);

	/* Get board temperatures */
	hk->temp1 = (int16_t) LM60_MV_TO_C(ADC_TO_MV(adc_buffer[0])) * 4;
	hk->temp2 = (int16_t) LM60_MV_TO_C(ADC_TO_MV(adc_buffer[1])) * 4;

	/* Get panel temperatures */
	for (i=0;i < LM70_MAP_SIZE;i++) {
		lm70_spi_setup_cs(&spi_dev, &lm70_chip[i], lm70_map[i]);
		hk->panel_temp[i] = (int16_t) (lm70_read_temp(&lm70_chip[i]) * 4);
	}

}
