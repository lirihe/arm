#ifndef GYRO_H_
#define GYRO_H_

#include <dev/spi.h>

/**
 * Helper function to setup a spi_chip_t.
 * This will setup the chip select in the correct SPI mode and speed
 * to communicate with the gyro.
 * @param spi_dev pointer to a spi device (must be initialised)
 * @param spi_chip pointer to a spi chip where the settings are stored
 * @param cs the chip select of the current gyro
 */
void gyro_spi_setup_cs(spi_dev_t * spi_dev, spi_chip_t * spi_chip, uint8_t cs);

/**
 * Start a gyro device
 * @param gyro pointer to a gyro chip, already setup by gyro_spi_setup_cs
 * @return 0 if successful
 */
int gyro_setup(spi_chip_t * gyro);

/**
 * Run a gyro test in a loop
 * @param gyro
 */
void gyro_test(spi_chip_t * gyro);

/**
 * Run the autonull procedure
 * @param chip
 */
void gyro_autonull(spi_chip_t * gyro);

/**
 * Get internal temperature
 * @param chip
 * @return temperature in deg. C
 */
float gyro_read_temp(spi_chip_t * chip);

/**
 * Relative angle estimate
 * This number is reset to zero at startup
 * The user is cautioned to fully understand the stability requirements
 * @param chip
 * @return Integration of GYRO_OUT parameter
 */
float gyro_read_angle(spi_chip_t * chip);

/**
 * Get angular rate
 * @param chip
 * @return Angular rate in [deg/sec]
 */
float gyro_read_rate(spi_chip_t * chip);

/**
 * Get gyro offset
 * @param chip
 * @return Offset in [deg/sec]
 */
float gyro_read_offset(spi_chip_t * chip);

/**
 * Get supply voltage
 * @param chip
 * @return Voltage in mV
 */
float gyro_read_supply(spi_chip_t * chip);

/**
 * Get aux voltage
 * @param chip
 * @return Voltage in mV
 */
float gyro_read_aux_voltage(spi_chip_t * chip);

/**
 * Set the offset of the gyro.
 * Used to compensate for bias
 * @param chip
 * @param offset
 */
void gyro_write_offset(spi_chip_t * chip, float offset);

#endif /* GYRO_H_ */
