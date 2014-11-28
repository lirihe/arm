/**
 * ISIS AntS Driver
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <dev/i2c.h>
#include <dev/isis_ants.h>

#include <util/console.h>
#include <util/error.h>
#include <util/hexdump.h>
#include <util/csp_buffer.h>
#include <util/log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define ISIS_ANTS_CMD_RESET			0xAA
#define ISIS_ANTS_CMD_ARM			0xAD
#define ISIS_ANTS_CMD_DISARM		0xAC

#define ISIS_ANTS_CMD_DEPLOY_1		0xA1
#define ISIS_ANTS_CMD_DEPLOY_2		0xA2
#define ISIS_ANTS_CMD_DEPLOY_3		0xA3
#define ISIS_ANTS_CMD_DEPLOY_4		0xA4
#define ISIS_ANTS_CMD_DEPLOY_AUTO	0xA5
#define ISIS_ANTS_CMD_DEPLOY_CANCEL	0xA9

#define ISIS_ANTS_CMD_O_DEPLOY_1	0xBA
#define ISIS_ANTS_CMD_O_DEPLOY_2	0xBB
#define ISIS_ANTS_CMD_O_DEPLOY_3	0xBC
#define ISIS_ANTS_CMD_O_DEPLOY_4	0xBD

#define ISIS_ANTS_CMD_TEMP			0xC0
#define ISIS_ANTS_CMD_STATUS_DEPLOY	0xC3

#define ISIS_ANTS_CMD_COUNT_1		0xB0
#define ISIS_ANTS_CMD_COUNT_2		0xB1
#define ISIS_ANTS_CMD_COUNT_3		0xB2
#define ISIS_ANTS_CMD_COUNT_4		0xB3

#define ISIS_ANTS_CMD_TIME_1		0xB4
#define ISIS_ANTS_CMD_TIME_2		0xB5
#define ISIS_ANTS_CMD_TIME_3		0xB6
#define ISIS_ANTS_CMD_TIME_4		0xB7

int isis_ants_status(uint8_t i2c_addr, isis_ants_status_t * status) {

	uint8_t tx[1], rx[2], tx_len, rx_len;

	tx[0] = ISIS_ANTS_CMD_STATUS_DEPLOY;
	tx_len = 1;
	rx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
		
	status->armed = rx[0] & 0x01;
	status->switch_ignore = rx[1] & 0x01;

	status->ant[0].not_deployed = rx[1] & 0x80;
	status->ant[0].time_limit_reached = rx[1] & 0x40;
	status->ant[0].deployment_active = rx[1] & 0x20;

	status->ant[1].not_deployed = rx[1] & 0x08;
	status->ant[1].time_limit_reached = rx[1] & 0x04;
	status->ant[1].deployment_active = rx[1] & 0x02;

	status->ant[2].not_deployed = rx[0] & 0x80;
	status->ant[2].time_limit_reached = rx[0] & 0x40;
	status->ant[2].deployment_active = rx[0] & 0x20;

	status->ant[3].not_deployed = rx[0] & 0x08;
	status->ant[3].time_limit_reached = rx[0] & 0x04;
	status->ant[3].deployment_active = rx[0] & 0x02;

	tx[0] = ISIS_ANTS_CMD_COUNT_1;
	tx_len = 1;
	rx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}		
	status->ant[0].activation_count = rx[0];

	tx[0] = ISIS_ANTS_CMD_COUNT_2;
	tx_len = 1;
	rx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}		
	status->ant[1].activation_count = rx[0];

	tx[0] = ISIS_ANTS_CMD_COUNT_3;
	tx_len = 1;
	rx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}		
	status->ant[2].activation_count = rx[0];

	tx[0] = ISIS_ANTS_CMD_COUNT_4;
	tx_len = 1;
	rx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}		
	status->ant[3].activation_count = rx[0];

	tx[0] = ISIS_ANTS_CMD_TIME_1;
	tx_len = 1;
	rx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}	
	status->ant[0].activation_time = (rx[1] << 8) | rx[0];

	tx[0] = ISIS_ANTS_CMD_TIME_2;
	tx_len = 1;
	rx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	status->ant[1].activation_time = (rx[1] << 8) | rx[0];

	tx[0] = ISIS_ANTS_CMD_TIME_3;
	tx_len = 1;
	rx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	status->ant[2].activation_time = (rx[1] << 8) | rx[0];

	tx[0] = ISIS_ANTS_CMD_TIME_4;
	tx_len = 1;
	rx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	status->ant[3].activation_time = (rx[1] << 8) | rx[0];

	return 0;

}

int isis_ants_temp(uint8_t i2c_addr, uint16_t * temp) {

	uint8_t tx[1], rx[2], tx_len, rx_len;

	tx[0] = ISIS_ANTS_CMD_TEMP;
	tx_len = 1;
	rx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, &rx, rx_len, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}

	*temp = (rx[1] << 8) | rx[0];
	return 0;

}

int isis_ants_deploy_single(uint8_t i2c_addr, int isis_ant_nr, uint8_t time_sec, unsigned int override) {

	uint8_t tx[2], tx_len;

	if (override) {
		switch(isis_ant_nr) {
		case 0: tx[0] = ISIS_ANTS_CMD_O_DEPLOY_1; break;
		case 1: tx[0] = ISIS_ANTS_CMD_O_DEPLOY_2; break;
		case 2: tx[0] = ISIS_ANTS_CMD_O_DEPLOY_3; break;
		case 3: tx[0] = ISIS_ANTS_CMD_O_DEPLOY_4; break;
		default: return -1;
		}
	} else {
		switch(isis_ant_nr) {
		case 0: tx[0] = ISIS_ANTS_CMD_DEPLOY_1; break;
		case 1: tx[0] = ISIS_ANTS_CMD_DEPLOY_2; break;
		case 2: tx[0] = ISIS_ANTS_CMD_DEPLOY_3; break;
		case 3: tx[0] = ISIS_ANTS_CMD_DEPLOY_4; break;
		default: return -1;
		}
	}

	tx[1] = time_sec;
	tx_len = 2;
	
	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, NULL, 0, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}

	return 0;

}

int isis_ants_deploy_auto(uint8_t i2c_addr, uint8_t time_sec) {

	uint8_t tx[2], tx_len;

	tx[0] = ISIS_ANTS_CMD_DEPLOY_AUTO;
	tx[1] = time_sec;
	tx_len = 2;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, NULL, 0, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	
	return 0;

}

int isis_ants_deploy_cancel(uint8_t i2c_addr) {

	uint8_t tx[1], tx_len;

	tx[0] = ISIS_ANTS_CMD_DEPLOY_CANCEL;
	tx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, NULL, 0, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	
	return 0;

}

int isis_ants_disarm(uint8_t i2c_addr) {

	uint8_t tx[1], tx_len;

	tx[0] = ISIS_ANTS_CMD_DISARM;
	tx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, NULL, 0, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	
	return 0;

}

int isis_ants_arm(uint8_t i2c_addr) {

	uint8_t tx[1], tx_len;

	tx[0] = ISIS_ANTS_CMD_ARM;
	tx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, NULL, 0, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	
	return 0;

}

int isis_ants_reset(uint8_t i2c_addr) {

	uint8_t tx[1], tx_len;

	tx[0] = ISIS_ANTS_CMD_RESET;
	tx_len = 1;

	if (i2c_master_transaction(0, i2c_addr, &tx, tx_len, NULL, 0, 50) != E_NO_ERR) {
		printf("I2C transaction error\r\n");
		return -1;
	}
	
	return 0;

}
