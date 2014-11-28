/**
 * GATOSS init script
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>
#include <stddef.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "io/cdh.h"
#include <io/nanomind.h>
#include <io/gatoss.h>
#include <sns/sns.h>

#include <fp/fp.h>
#include <fp/fp_timer.h>

void gatoss_init(void) {

	/** Flightplanner */

	/* Table init */
	fp_init();

	/* Server */
	fp_server_start(1024*4, OBC_PORT_FP);

	/* Timer */
	extern void fp_timer_task(void *pvParameters);
	xTaskCreate(fp_timer_task, (const signed char*) "FPT", 1024*4, NULL, 2, NULL);

	/* Client */
	void cmd_fp_setup(void);
	cmd_fp_setup();

	void gatoss_cmd_setup(void);
	gatoss_cmd_setup();
	gatoss_set_node(SNS("gatoss"));

	void cdh_cmd_setup(void);
	cdh_beacon_set_node(SNS("obc"));
	cdh_conf_set_node(SNS("obc"));
	cdh_cmd_setup();

	void conf_restore(void);
	conf_restore();

	extern void task_hk(void *pvParameters);
	xTaskCreate(task_hk, (const signed char*) "HK", 1024*4, NULL, 3, NULL);

	extern void task_cdh_server(void *pvParameters);
	xTaskCreate(task_cdh_server, (const signed char*) "CDH", 1024*4, NULL, 2, NULL);

}
