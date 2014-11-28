#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <dev/spi.h>
#include <dev/usart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define GYRO_CTRL_GYRO_OFF			0x14
#define GYRO_CTRL_GYRO_SCALE		0x16
#define GYRO_CTRL_ALM_MAG1			0x20
#define GYRO_CTRL_ALM_MAG2			0x22
#define GYRO_CTRL_ALM_SMPL1			0x24
#define GYRO_CTRL_ALM_SMPL2			0x26
#define GYRO_CTRL_ALM_CTRL			0x28
#define GYRO_CTRL_AUX_DAC			0x30
#define GYRO_CTRL_GPIO_CTRL			0x32
#define GYRO_CTRL_MSC_CTRL			0x34
#define GYRO_CTRL_SMPL_PRD			0x36
#define GYRO_CTRL_SENSAVG			0x38
#define GYRO_CTRL_SLP_CNT			0x3A
#define GYRO_CTRL_STATUS			0x3C
#define GYRO_CTRL_COMMAND			0x3E

#define GYRO_DATA_ENDURANCE			0x00
#define GYRO_DATA_SUPPLY_OUT		0x02
#define GYRO_DATA_GYRO_OUT			0x04
#define GYRO_DATA_AUX_ADC			0x0A
#define GYRO_DATA_TEMP_OUT			0x0C
#define GYRO_DATA_ANGL_OUT			0x0E

float gyro_sensitivity_factor = 1;

static void gyro_write_reg(spi_chip_t * chip, uint8_t reg_base, uint16_t value) {
	if (spi_lock_dev(chip->spi_dev) < 0)
		return;
	spi_write(chip, ((reg_base + 1) << 8) | 0x8000 | ((value >> 8) & 0xff));
	//printf("Writing %#x\r\n", ((reg_base + 1) << 8) | 0x8000 | ((value >> 8) & 0xff));
	spi_read(chip);
	spi_write(chip, (reg_base << 8) | 0x8000 | (value & 0xff));
	//printf("Writing %#x\r\n", (reg_base << 8) | 0x8000 | (value & 0xff));
	spi_read(chip);
	spi_unlock_dev(chip->spi_dev);
}

static uint16_t gyro_read_reg(spi_chip_t * chip, uint8_t reg_base) {
	uint16_t val;
	if (spi_lock_dev(chip->spi_dev) < 0)
		return 0;
	spi_write(chip, (reg_base << 8));
	val = spi_read(chip);
	//printf("Read %#x\r\n", val);
	spi_write(chip, (reg_base << 8));
	val = spi_read(chip);
	//printf("Read %#x\r\n", val);
	spi_unlock_dev(chip->spi_dev);
	return val;
}

void gyro_spi_setup_cs(spi_dev_t * spi_dev, spi_chip_t * spi_chip, uint8_t cs) {
	spi_chip->spi_dev = spi_dev;		// A pointer to the physical device SPI0
	spi_chip->baudrate = 1000000;		// This is only the initial baud rate, it will be increased by the driver
	spi_chip->spi_mode = 3;				// SPI mode
	spi_chip->bits = 16;				// Default value for transferring bytes
	spi_chip->cs = cs;					// The chip select register
	spi_chip->reg = cs/4;				// The register bank to use
	spi_chip->spck_delay = 4;			// Delay
	spi_chip->trans_delay = 60;			// Delay
	spi_setup_chip(spi_chip);
}

float gyro_read_temp(spi_chip_t * chip) {
	int16_t val = gyro_read_reg(chip, GYRO_DATA_TEMP_OUT) & 0xFFF;
	if (val & 0x0800)
		val |= 0xF000;
	return val * 0.145348837 + 25.0;
}

float gyro_read_rate(spi_chip_t * chip) {
	int16_t val = gyro_read_reg(chip, GYRO_DATA_GYRO_OUT) & 0x3FFF;
	if (val & 0x2000)
		val |= 0xC000;
	return val * 0.01832 * gyro_sensitivity_factor;
}

float gyro_read_angle(spi_chip_t * chip) {
	return (gyro_read_reg(chip, GYRO_DATA_ANGL_OUT) & 0x3FFF) * 0.03663;
}

float gyro_read_offset(spi_chip_t * chip) {
	int16_t val = gyro_read_reg(chip, GYRO_CTRL_GYRO_OFF) & 0x3FFF;
	if (val & 0x2000)
		val |= 0xC000;
	return val * 0.00458;
}

void gyro_write_offset(spi_chip_t * chip, float offset_float) {
	int16_t offset = offset_float / 0.00458;
	gyro_write_reg(chip, GYRO_CTRL_GYRO_OFF, offset & 0x3FFF);
}

float gyro_read_supply(spi_chip_t * chip) {
	return (gyro_read_reg(chip, GYRO_DATA_SUPPLY_OUT) & 0x0FFF) * 1.8315;
}

float gyro_read_aux_voltage(spi_chip_t * chip) {
	return (gyro_read_reg(chip, GYRO_DATA_AUX_ADC) & 0x0FFF) * 0.6105;
}

int gyro_setup(spi_chip_t * chip) {

	/* Send software reset */
	gyro_write_reg(chip, GYRO_CTRL_COMMAND, 0x0080);
	vTaskDelay(50);

	/* Set sensitivity to +-20deg/sec */
//	printf("Gyro sensitivity reg %#X\r\n", gyro_read_reg(chip, GYRO_CTRL_SENSAVG));
	gyro_sensitivity_factor = 0.25;
	gyro_write_reg(chip, GYRO_CTRL_SENSAVG, 0x0107);
	vTaskDelay(10);
//	printf("Gyro sensitivity reg %#X\r\n", gyro_read_reg(chip, GYRO_CTRL_SENSAVG));

	//gyro_write_offset(chip, 0.3361);

#if 0
	/* Store config */
	gyro_write_reg(chip, GYRO_CTRL_COMMAND, 0x0008);
	vTaskDelay(100);
	printf("Gyro sensitivity reg %#X\r\n", gyro_read_reg(chip, GYRO_CTRL_SENSAVG));

	/* Run internal self test */
	gyro_write_reg(chip, GYRO_CTRL_MSC_CTRL, 0x0400);

	int i;
	for(i = 0; i < 35; i++) {
		printf("Gyro rate [%u]: %f\r\n", i, gyro_read_rate(chip));
		printf("\t\tStatus reg %#x\r\n", gyro_read_reg(chip, GYRO_CTRL_STATUS));
		vTaskDelay(1);
	}
#endif

	return 0;

}

void gyro_test(spi_chip_t * chip) {

	double avg = 0;
	uint32_t avg_count = 0;

	while(1) {

		if (usart_messages_waiting(USART_CONSOLE) != 0)
			break;

		printf("\E[2KSens reg\t\t %#X\r\n", gyro_read_reg(chip, GYRO_CTRL_SENSAVG));
		printf("\E[2KOffset\t\t\t %.4f [deg/sec]\r\n", gyro_read_offset(chip));
		printf("\E[2KStatus reg\t\t %#X\r\n", gyro_read_reg(chip, GYRO_CTRL_STATUS));
		printf("\E[2KEndurance\t\t %u writes\r\n", gyro_read_reg(chip, GYRO_DATA_ENDURANCE));
		printf("\E[2KPower supply\t\t %.3f [mV]\r\n", gyro_read_supply(chip));
		printf("\E[2KAUX analog input\t %.3f [mV]\r\n", gyro_read_aux_voltage(chip));
		printf("\E[2KTemperature\t\t %.3f [C]\r\n", gyro_read_temp(chip));
		printf("\E[2KAngle\t\t\t %.3f [deg]\r\n", gyro_read_angle(chip));

		float rate = gyro_read_rate(chip);
		avg += rate;
		avg_count++;
		printf("\E[2KRate\t\t\t %.3f [deg/sec] (avg: %.4f)\r\n", rate, avg / avg_count);

		vTaskDelay(30);

		printf("\E[9A\r");

	}

}

void gyro_autonull(spi_chip_t * chip) {

	int16_t gyro_out, gyro_off;

	gyro_out = gyro_read_reg(chip, GYRO_DATA_GYRO_OUT) & 0x3FFF;
	if (gyro_out & 0x2000)
		gyro_out |= 0xC000;
	printf("GYRO_OUT: %d\r\n", gyro_out);
	gyro_off = gyro_read_reg(chip, GYRO_CTRL_GYRO_OFF) & 0x3FFF;
	printf("GYRO_OFF: %d\r\n", gyro_off);
	printf("Setting GYRO_OFF = %d\r\n", -gyro_out);
	gyro_write_reg(chip, GYRO_CTRL_GYRO_OFF, ((uint16_t) -gyro_out) & 0x3FFF);
	gyro_off = gyro_read_reg(chip, GYRO_CTRL_GYRO_OFF) & 0x3FFF;
	printf("GYRO_OFF: %d\r\n", gyro_off);
	printf("Executing auto null\r\n");
	gyro_write_reg(chip, GYRO_CTRL_COMMAND, 0x0001);
	vTaskDelay(100);
	gyro_out = gyro_read_reg(chip, GYRO_DATA_GYRO_OUT) & 0x3FFF;
	if (gyro_out & 0x2000)
		gyro_out |= 0xC000;
	printf("GYRO_OUT: %d\r\n", gyro_out);
	gyro_off = gyro_read_reg(chip, GYRO_CTRL_GYRO_OFF) & 0x3FFF;
	printf("GYRO_OFF: %d\r\n", gyro_off);

}
