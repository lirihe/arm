/*
 * hmc5843.c
 *
 *  Created on: Aug 31, 2009
 *      Author: karl
 */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <dev/arm/at91sam7.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <dev/i2c.h>
#include <dev/usart.h>
#include <dev/magnetometer.h>

#include <util/error.h>
#include <util/console.h>
#include <util/hexdump.h>
#include <util/clock.h>
#include <util/csp_buffer.h>



#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <util/clock.h>
timestamp_t start_time,new_time[20];
#define START_TIMER		clock_get_time(&start_time)
#define GET_TIME(a)		clock_get_time(&new_time[a])
#define FLOATTIME(a)    ((a.tv_sec-1293861600)*1.0+a.tv_nsec/1000000000.0)



typedef struct mag_reg_data_s {
	uint8_t id1;
	uint8_t id2;
	uint8_t id3;
	uint8_t conf_a;
	uint8_t conf_b;
	uint8_t mode;
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint8_t status;
} mag_reg_data_t;

/** Default values */
static int is_initialised = pdFALSE;
static int meas = MAG_MEAS_NORM;
static int gain = MAG_GAIN_1_0;
static int rate = MAG_RATE_10;
static int defaultmode = MAG_MODE_IDLE;
static float scale = 1/1.3;

// Write register "reg" with value "val"
static int mag_write_reg(uint8_t reg, uint8_t val) {
  uint8_t txdata[2];
  txdata[0] = reg;
  txdata[1] = val;

  return i2c_master_transaction(1,0x1E, txdata, 2,NULL,0,2);
}

// Read register "reg" into value "val"
// Returns -1 on error and 0 on no-error
static int mag_read_reg(uint8_t reg, uint8_t *val) {
  uint8_t txdata[1];
  txdata[0] = reg;
  return i2c_master_transaction(1,0x1E, txdata, 1,val,1,2);
}

#if 0
static int mag_is_bsy(void) {
  // Check busy bit, this is here done by the mode byte: 
  // If we are in sleep-mode, conversion is complete.
  // NOTE: the actual RDY bit seems to be broken
  uint8_t mode;
  mag_read_reg(0x02, &mode);
  if ((mode) == 1) {
    return 1;
  }
  else {
	 return 0;
  }
}
#endif

// Return. current values of rate, meas and gain:
mag_gain_t mag_get_gain(){
  return gain;
}
mag_rate_t mag_get_rate(){
  return rate;
}
mag_meas_t mag_get_meas(){
  return meas;
}


// Write config, ie. rate, meas and gain to hmc5843
int mag_set_conf(mag_rate_t new_rate, mag_meas_t new_meas, mag_gain_t new_gain) {  
  rate = new_rate;
  meas = new_meas;
  gain = new_gain;
  
  switch(new_gain)
    { 
    case MAG_GAIN_0_7:
      scale = 1/1.65;
      break;
    case MAG_GAIN_1_0:
      scale = 1/1.3;
      break;
    case MAG_GAIN_1_5:
      scale = 1/0.97;
      break;

    default: // Here is default
    case MAG_GAIN_2_0:
      scale = 1/0.78;
      break;

    case MAG_GAIN_3_2:
      scale = 1/0.53;
      break;
    case MAG_GAIN_3_8:
      scale = 1/0.46;
      break;
    case MAG_GAIN_4_5:
      scale = 1/0.39;
      break;
    case MAG_GAIN_6_5:
      scale = 1/0.28;
      break;
 }
  
  mag_write_reg(0x00, (rate << 2) | meas);
  return mag_write_reg(0x01, (gain << 5));
}

// Write mode to hmc5843
int mag_set_mode(mag_mode_t mode) {  
  return mag_write_reg(0x02, mode);
}

// Setup i2c to hmc5843
int mag_init(void) {

	if(is_initialised)
	  return E_NO_ERR;

	/* Turn-on-time: 200 us */
	UPIO_ECR = 1;
	UPIO_MDDR = 0;
	UPIO_OER = 0x00000001;
	if (UPIO_PDSR & 0x00000001) {
		UPIO_CODR = 0x00000001;
		vTaskDelay(configTICK_RATE_HZ * 0.5);
	}
	UPIO_SODR = 0x00000001;
	vTaskDelay(configTICK_RATE_HZ * 0.5);
	
	/* Setup I2C */
	i2c_init(1, I2C_MASTER, 0x06, 60, 5, 5, NULL);

	vTaskDelay(configTICK_RATE_HZ * 0.2);

	is_initialised = pdTRUE;

	// set default conf and mode
	mag_set_conf(rate, meas, gain);
	return mag_set_mode(defaultmode);
}

// Setup i2c to hmc5843
int mag_init_force(void) {

	/* Turn-on-time: 200 us */
	UPIO_ECR = 1;
	UPIO_MDDR = 0;
	UPIO_OER = 0x00000001;
	if (UPIO_PDSR & 0x00000001) {
		UPIO_CODR = 0x00000001;
		vTaskDelay(configTICK_RATE_HZ * 0.5);
	}
	UPIO_SODR = 0x00000001;
	vTaskDelay(configTICK_RATE_HZ * 0.5);

	/* Setup I2C */
	i2c_init(1, I2C_MASTER, 0x06, 60, 5, 5, NULL);

	vTaskDelay(configTICK_RATE_HZ * 0.2);

	is_initialised = pdTRUE;

	// set default conf and mode
	mag_set_conf(rate, meas, gain);
	return mag_set_mode(defaultmode);
}

// Perform read of data-registers from hmc5843
int mag_read(mag_data_t * data) {

	int16_t tmpx, tmpy, tmpz;
	uint8_t txdata[1];
	uint8_t rxdata[6];
	int retval;

	txdata[0] = 0x03;
	retval = i2c_master_transaction(1,0x1E, txdata, 1,rxdata,6,5);
	if (retval == E_NO_ERR) {

		/* Data is returned in a slave-frame structure */
		tmpx = rxdata[0] << 8 | rxdata[1];
		tmpy = rxdata[2] << 8 | rxdata[3];
		tmpz = rxdata[4] << 8 | rxdata[5];

		data->x = (float) tmpx * scale ;
		data->y = (float) tmpy * scale;
		data->z = (float) tmpz * scale;

		return E_NO_ERR;
	} else {
		return E_TIMEOUT;
	}
}


// Perform read of data-registers from hmc5843: output raw data
int mag_read_raw(mag_data_t * data) {

	int16_t tmpx, tmpy, tmpz;
	uint8_t txdata[1];
	uint8_t rxdata[6];
	int retval;

	txdata[0] = 0x03;
	retval = i2c_master_transaction(1,0x1E, txdata, 1,rxdata,6,5);
	if (retval == E_NO_ERR) {

		/* Data is returned in a slave-frame structure */
		tmpx = rxdata[0] << 8 | rxdata[1];
		tmpy = rxdata[2] << 8 | rxdata[3];
		tmpz = rxdata[4] << 8 | rxdata[5];

		data->x = (float) tmpx;
		data->y = (float) tmpy;
		data->z = (float) tmpz;

		return E_NO_ERR;
	} else {
		return E_TIMEOUT;
	}
}


// Perform complete single sampling of magnetometer
int mag_read_single(mag_data_t * data) {
	//int timeout = 10;
	//int busy;
	int ret;
	START_TIMER;
	ret = mag_set_mode(MAG_MODE_SINGLE);
	if (ret != E_NO_ERR)
		return ret;

//	vTaskDelay(15);
/*
	while(timeout--) {
		  busy = mag_is_bsy();
	  if (busy == 0)
	    break;
	  if (busy == -1)
	    return E_TIMEOUT;
	}
	if (timeout == 0)
		return E_TIMEOUT;
		*/
//	GET_TIME(0);
	int retval =mag_read(data);

//	printf("t=%f\r\n",FLOATTIME(new_time[0]) - FLOATTIME(start_time));

	return retval;
}



// Perform alternative complete single sampling of magnetometer
int mag_read_single_alt(mag_data_t * data) {
	int timeout=13;
	int ret;
	uint8_t status;
	//START_TIMER;
	ret = mag_set_mode(MAG_MODE_CONTINUOUS);
	if (ret != E_NO_ERR)
		return ret;
	// Wait 5 ms
	vTaskDelay(configTICK_RATE_HZ * 0.005);
	while(timeout--) {
		mag_read_reg(0x09, &status);
		if ( (status&0x01) == 1)
			break;
	}
	//GET_TIME(0);
	mag_set_mode(MAG_MODE_IDLE);
	if (timeout == 0)
		return E_TIMEOUT;


	int retval =mag_read(data);
	//printf("t=%f,%d\r\n",FLOATTIME(new_time[0]) - FLOATTIME(start_time),timeout);
	return retval;
}



// Read in entire status of hmc5843
int mag_get_info(struct command_context *ctx) {

	i2c_frame_t * frame;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;
	
	frame = csp_buffer_get(I2C_MTU);
	frame->dest = 0x1E;
	frame->data[0] = 0x0A;
	frame->len = 1;
	frame->len_rx = 13;
	i2c_send(1, frame, 0);

	if (i2c_receive(1, &frame, 1000) == E_NO_ERR) {
		hex_dump(frame->data, 13);
	} else {
		printf("Timeout\r\n");
		return CMD_ERROR_FAIL;
	}

	mag_reg_data_t * data = (void *) frame->data;

	printf("ID: %c%c%c\r\n", data->id1, data->id2, data->id3);

	uint8_t dorate = (data->conf_a >> 2) & 0x07;
	float dorate_f = 0;
	switch (dorate) {
	case 0: dorate_f = 0.5; break;
	case 1: dorate_f = 1; break;
	case 2: dorate_f = 2; break;
	case 3: dorate_f = 5; break;
	case 4: dorate_f = 10; break;
	case 5: dorate_f = 20; break;
	case 6: dorate_f = 50; break;
	case 7: dorate_f = 0; break;
	}
	printf("Data output rate is %f [Hz]\r\n", dorate_f);

	uint8_t mmode = (data->conf_a & 0x3);
	switch (mmode) {
	case 0: printf("Normal Measurement Mode\r\n"); break;
	case 1: printf("Positive Bias Mode\r\n"); break;
	case 2: printf("Negative Bias Mode\r\n"); break;
	case 3: printf("Invalid mode\r\n"); break;
	}

	uint8_t gain = ((data->conf_b >> 5) & 0x7);
	switch (gain) {
	case 0: printf("Field Range: +- 0.7 [Ga] Gain 1620\r\n"); break;
	case 1: printf("Field Range: +- 1.0 [Ga] Gain 1300\r\n"); break;
	case 2: printf("Field Range: +- 1.5 [Ga] Gain 970\r\n"); break;
	case 3: printf("Field Range: +- 2.0 [Ga] Gain 780\r\n"); break;
	case 4: printf("Field Range: +- 3.2 [Ga] Gain 530\r\n"); break;
	case 5: printf("Field Range: +- 3.8 [Ga] Gain 460\r\n"); break;
	case 6: printf("Field Range: +- 4.5 [Ga] Gain 390\r\n"); break;
	case 7: printf("Field Range: +- 6.5 [Ga] Gain 280 (Not recommended!)\r\n"); break;
	}
	printf("Scale %f\r\n",scale);

	uint8_t mode = data->mode & 3;
	switch (mode) {
	case 0: printf("Continuous-Conversion Mode\r\n"); break;
	case 1: printf("Single-Conversion Mode\r\n"); break;
	case 2: printf("Idle Mode\r\n"); break;
	case 3: printf("Sleep Mode\r\n"); break;
	}
	csp_buffer_free(frame);

	return CMD_ERROR_NONE;

}



// Do single conversion and print it
int mag_test_single(struct command_context *ctx) {

	mag_data_t data;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	if (mag_read_single(&data) == E_NO_ERR) {
		printf("X: %4.1f mG\n\r", data.x);
		printf("Y: %4.1f mG\n\r", data.y);
		printf("Z: %4.1f mG\n\r", data.z);
		printf("Magnitude: %4.1f mG\n\r", sqrt(powf(data.x,	2.0) + powf(data.y, 2.0) + powf(data.z, 2.0)));
	}
	else {
		printf("Error reading from magnetometer\r\n");
		return CMD_ERROR_FAIL;
	}

	return CMD_ERROR_NONE;

}


// Do loop measurements
int mag_test_single_alt(struct command_context *ctx) {

	mag_data_t data;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	if (mag_read_single_alt(&data) == E_NO_ERR) {
		printf("X: %4.1f mG\n\r", data.x);
		printf("Y: %4.1f mG\n\r", data.y);
		printf("Z: %4.1f mG\n\r", data.z);
		printf("Magnitude: %4.1f mG\n\r", sqrt(powf(data.x,	2.0) + powf(data.y, 2.0) + powf(data.z, 2.0)));
	}
	else {
		printf("Error reading from magnetometer\r\n");
		return CMD_ERROR_FAIL;
	}

	return CMD_ERROR_NONE;
}




// Do bias testing
int mag_test_bias(struct command_context *ctx) {

	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	mag_set_mode(MAG_MODE_SINGLE);
	mag_set_conf(mag_get_rate(), MAG_MEAS_POS, mag_get_gain() );

	printf("Positive bias mode\r\n");
	vTaskDelay(configTICK_RATE_HZ * 0.1);

	mag_test_single(NULL);
	mag_test_single(NULL);
	mag_test_single(NULL);

	mag_set_conf(mag_get_rate(), MAG_MEAS_NEG, mag_get_gain() );

	printf("Negative bias mode\r\n");
	vTaskDelay(configTICK_RATE_HZ * 0.1);

	mag_test_single(NULL);
	mag_test_single(NULL);
	mag_test_single(NULL);

	mag_set_conf(mag_get_rate(), MAG_MEAS_NORM, mag_get_gain() );

	printf("Normal bias mode\r\n");
	vTaskDelay(configTICK_RATE_HZ * 0.1);

	mag_test_single(NULL);
	mag_test_single(NULL);
	mag_test_single(NULL);

	mag_set_mode(MAG_MODE_CONTINUOUS);

	return CMD_ERROR_NONE;
}



// Do loop measurements
int mag_loop(struct command_context *ctx) {

	mag_data_t data;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	mag_set_mode(MAG_MODE_CONTINUOUS);

	while (1) {

		if (usart_messages_waiting(USART_CONSOLE) != 0)
			break;

		if (mag_read(&data) == E_NO_ERR) {
			console_clear();
			printf("X: %4.1f mG\n\r", data.x);
			printf("Y: %4.1f mG\n\r", data.y);
			printf("Z: %4.1f mG\n\r", data.z);
			printf("Magnitude: %4.1f mG\n\r", sqrt(powf(data.x,	2.0) + powf(data.y, 2.0) + powf(data.z, 2.0)));
		}

		vTaskDelay(configTICK_RATE_HZ * 0.100);
	}

	mag_set_mode(MAG_MODE_IDLE);

	return CMD_ERROR_NONE;
}


// Do loop measurements
int mag_loop_noformat(struct command_context *ctx) {

	mag_data_t data;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	ret = mag_set_mode(MAG_MODE_CONTINUOUS);
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;
;

	while (1) {

		if (usart_messages_waiting(USART_CONSOLE) != 0)
			break;

		if (mag_read(&data) == E_NO_ERR) {
			printf(" %4.1f, ", data.x);
			printf(" %4.1f, ", data.y);
			printf(" %4.1f, ", data.z);
			printf(" %4.1f \n\r", sqrt(powf(data.x,	2.0) + powf(data.y, 2.0) + powf(data.z, 2.0)));
		}

		vTaskDelay(configTICK_RATE_HZ * 0.100);

	}

	mag_set_mode(MAG_MODE_IDLE);

	return CMD_ERROR_NONE;
}

// Do loop measurements
int mag_loop_fast(struct command_context *ctx) {

	mag_data_t data;
	int t = 0;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	ret = mag_set_mode(MAG_MODE_CONTINUOUS);
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;
;

	while (1) {

		if (usart_messages_waiting(USART_CONSOLE) != 0)
			break;
		  uint8_t status;
		  mag_read_reg(0x09, &status);
		  printf("%d",status);
		  t++;
		  if (t>50)
		  {
			  printf("\r\n");
			  t=0;
			  mag_read(&data);
		  }
		  /*	if (mag_read(&data) == E_NO_ERR) {
			printf("%d ", (int)data.x);
			printf("%d ", (int)data.y);
			printf("%d\r\n", (int)data.z);
		}
*/
		vTaskDelay(configTICK_RATE_HZ * 0.001);

	}


	mag_set_mode(MAG_MODE_IDLE);

	return CMD_ERROR_NONE;
}



// Do loop measurements (raw values)
int mag_loop_raw(struct command_context *ctx) {

	mag_data_t data;
	int ret;
	ret = mag_init();
	if (ret != E_NO_ERR)
		return CMD_ERROR_FAIL;

	mag_set_mode(MAG_MODE_CONTINUOUS);

	while (1) {

		if (usart_messages_waiting(USART_CONSOLE) != 0)
			break;

		if (mag_read_raw(&data) == E_NO_ERR) {
			printf(" %4.1f, ", data.x);
			printf(" %4.1f, ", data.y);
			printf(" %4.1f, ", data.z);
			printf(" %4.1f \n\r", sqrt(powf(data.x,	2.0) + powf(data.y, 2.0) + powf(data.z, 2.0)));
		}

		vTaskDelay(configTICK_RATE_HZ * 0.100);

	}

	mag_set_mode(MAG_MODE_IDLE);

	return CMD_ERROR_NONE;
}

