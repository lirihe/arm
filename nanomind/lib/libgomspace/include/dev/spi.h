/*
 * spi.h
 *
 *  Created on: Aug 22, 2009
 *      Author: karl
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * Parameters describing an instance of the SPI hardware
 **/
typedef struct{
	void * base;				/*! The SPI hardware base address */
	int index;					/*! Hardware Index, (0 for SPI0, 1 for SPI1 etc...) */
	int fdiv;					/*! Whether or not to use the frequency divider */
	unsigned char variable_ps;	/*! Target slave is selected in transfer register for every character to transmit */
	unsigned char pcs_decode;	/*! The four chip select lines are decoded externaly; values 0 to 15 can be given to select_chip() */
	xSemaphoreHandle lock;		/*! Lock that is used internally in the SPI driver */
} spi_dev_t;

/**
 * Parameters for a single chip-select line
 **/
typedef struct {
	unsigned char reg;			/*! Which CS [CS0-CS3] register to setup */
	unsigned char cs;			/*! Chip Select index [0-15] or [0-3] depending on pcs_decode */
	unsigned int baudrate;		/*! Preferred baudrate for the SPI */
	unsigned char bits;			/*! Number of bits in each character (8 - 16) */
	unsigned char spck_delay;	/*! Delay before first clock pulse after selecting slave (in MCK) */
	unsigned char trans_delay;	/*! Delay between each transfer/character (in MCK * 32) */
	unsigned char stay_act;		/*! Set this chip to stay active after last transfer to it */
	unsigned char spi_mode;		/*! Which of the four SPI-modes to use when transmitting */
	spi_dev_t * spi_dev;		/*! Pointer to the hardware device */
} spi_chip_t;

/**
 * Initialize SPI device
 * @param spi_dev A SPI module hardware configuration struct
 * @param spi_hw_index Hardware number (0 for SPI0, 1 for SPI1)
 */
int spi_init_dev(spi_dev_t * spi_dev);

/**
 * Lock device
 * @param spi_dev
 * @return 0 if OK, -1 if error
 */
int spi_lock_dev(spi_dev_t * spi_dev);

/**
 * Unlock device
 * @param spi_dev
 */
void spi_unlock_dev(spi_dev_t * spi_dev);

/**
 * After initializing hardware using spi_init_dev, this function must be called to setup the CS line
 * @param spi_chip a chip configuration struct, this must be pre-configured with spi_dev and all options.
 */
int spi_setup_chip(spi_chip_t * spi_chip);

/**
 * Send a byte/word via SPI
 * Depending on the transfer-size specified in spi_setup_chip, 8 to 16 bits
 * will be transferred from the 16 bit data-word
 * @param spi_chip The chip to send to
 * @param 8-16 bits to send
 */
void spi_write(spi_chip_t * spi_chip, uint16_t data);

/**
 * Read a byte/word from SPI
 * Depending on the transfer-size specified in spi_setup_chip, 8 to 16
 * bits will be received from the SPI device.
 * @param spi_chip Chip to read from
 * @return bits received
 */
uint16_t spi_read(spi_chip_t * spi_chip);

/**
 * DMA based block transfer to and from SPI devices
 * For RX, a buffer full of 0xFF must be transmitted to create the clock
 * For TX, an empty RX buffer must be provided for dummy RX data.
 * This only works with fixed chip-select
 * @param outbuf pointer to TX buffer
 * @param inbuf  pointer to RX buffer
 * @param length Amount of bytes to transfer
 * @return return 0 if the SPI device was not ready, 1 otherwise.
 */
int spi_dma_transfer(spi_chip_t * spi_chip, uint8_t * outbuf, uint8_t * inbuf, uint32_t length);

#endif /* SPI_H_ */
