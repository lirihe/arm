#ifndef MAX6675_H_
#define MAX6675_H_

#include <dev/spi.h>

/**
 * Helper function to setup a spi_chip_t.
 * This will setup the chip select in the correct SPI mode and speed
 * to communicate with the MAX6675.
 * @param spi_dev pointer to a spi device (must be initialised)
 * @param spi_chip pointer to a spi chip where the settings are stored
 * @param cs the chip select of the current device
 */
void max6675_spi_setup_cs(spi_dev_t * spi_dev, spi_chip_t * spi_chip, uint8_t cs);

/**
 * Get MAX6675 temperature
 * @param chip
 * @return temperature in deg. C
 */
float max6675_read_temp(spi_chip_t * chip);

#endif /* MAX6675_H_ */
