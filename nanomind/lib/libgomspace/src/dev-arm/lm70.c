#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <dev/spi.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

void lm70_spi_setup_cs(spi_dev_t * spi_dev, spi_chip_t * spi_chip, uint8_t cs) {
	spi_chip->spi_dev = spi_dev;		// A pointer to the physical device SPI0
	spi_chip->baudrate = 2000000;		// This is only the initial baudrate, it will be increased by the driver
	spi_chip->spi_mode = 3;				// SPI Mode (See p. 288 in AP7001 DS)
	spi_chip->bits = 16;				// Default value for transferring bytes
	spi_chip->cs = cs;					// The chip select number
	spi_chip->reg = cs/4;					// The chip select register
	spi_chip->stay_act = 0;				// Should the chip-select stay active until next SPI transmission?
	spi_chip->spck_delay = 0;			// No delays
	spi_chip->trans_delay = 0;			// No delays
	spi_setup_chip(spi_chip);
}

float lm70_read_temp(spi_chip_t * chip) {
	if (spi_lock_dev(chip->spi_dev) < 0)
		return 0;
	spi_write(chip, 0x0000);
	int16_t temp = spi_read(chip);
	spi_unlock_dev(chip->spi_dev);
	return (((double)(temp >> 5)) / 4.0);
}

int16_t lm70_read_raw(spi_chip_t * chip) {
	if (spi_lock_dev(chip->spi_dev) < 0)
		return 0;
	spi_write(chip, 0x0000);
	spi_unlock_dev(chip->spi_dev);
	return spi_read(chip);
}
