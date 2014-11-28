#ifndef LM70_H_
#define LM70_H_

#include <dev/spi.h>

/**
 * Helper function to setup a spi_chip_t.
 * This will setup the chip select in the correct SPI mode and speed
 * to communicate with the LM70.
 * @param spi_dev pointer to a spi device (must be initialised)
 * @param spi_chip pointer to a spi chip where the settings are stored
 * @param cs the chip select of the current lm70
 */
void lm70_spi_setup_cs(spi_dev_t * spi_dev, spi_chip_t * spi_chip, uint8_t cs);

/**
 * Get LM70 temperature
 * @param chip
 * @return temperature in deg. C
 */
float lm70_read_temp(spi_chip_t * chip);

/**
 * Get LM70 temperature in RAW
 * @param chip
 * @return temperature in RAW
 */
int16_t lm70_read_raw(spi_chip_t * chip);

#endif /* LM70_H_ */
