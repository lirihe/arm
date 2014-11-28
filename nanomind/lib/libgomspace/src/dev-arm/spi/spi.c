/**
 * spi.c
 *
 *  Created on: Aug 19, 2009
 *      Author: karl
 *
 */

#include <dev/arm/at91sam7.h>
#include <dev/arm/cpu_pm.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <util/hexdump.h>
#include <util/log.h>
#include <dev/spi.h>

#define SPI_TIMEOUT	(10 * configTICK_RATE_HZ)

/**
 * Keep a record of the current, or latest used SPI device
 */
spi_chip_t * current_chip = NULL;

/**
 * Interrupt Service Routine and DMA semaphore
 */
static xSemaphoreHandle spi_sem_dma;
static void __attribute__((noinline)) spi_DSR(void) {
	portBASE_TYPE xTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(spi_sem_dma, &xTaskWoken);
	if (xTaskWoken == pdTRUE)
		portYIELD_FROM_ISR();
	AT91C_BASE_AIC->AIC_EOICR = 1;
}

static void __attribute__((naked)) spi_ISR(void) {
	portSAVE_CONTEXT();
	spi_DSR();
	portRESTORE_CONTEXT();
}


/**
 * Wait for SPI hardware to become ready, with simple timeout.
 * @return positive number if ready, zero if not.
 */
static inline int spi_busy_wait_tx(void) {
	volatile unsigned int j = 100000;
	while (!(SPI_SR & SPI_SR_TDRE_MASK) && --j)
		continue;
	return j;
}

/**
 * Wait for SPI hardware to become ready, with simple timeout.
 * @return positive number if ready, zero if not.
 */
static inline int spi_busy_wait_rx(void) {
	volatile unsigned int j = 100000;
	while (!(SPI_SR & SPI_SR_RDRF_MASK) && --j)
		continue;
	return j;
}

/**
 * Initialize SPI device
 * @param spi_dev A SPI module hardware configuration struct
 * @param spi_hw_index Hardware number (0 for SPI0, 1 for SPI1)
 */
int spi_init_dev(spi_dev_t * spi_dev) {

	/* Reset the peripheral, twice */
	SPI_CR = SPI_CR_SWRST_MASK;
	SPI_CR = SPI_CR_SWRST_MASK;

	/* Enable the SPI clock */
	SPI_ECR = SPI_ECR_PIO_MASK | SPI_ECR_SPI_MASK;

	/* Disable PIO on the SPI pins (enable SPI) */
	SPI_PDR = SPI_PDSR_SPCK_MASK | SPI_PDSR_MISO_MASK | SPI_PDSR_MOSI_MASK
			| SPI_PDSR_NPCS0_MASK | SPI_PDSR_NPCS1_MASK | SPI_PDSR_NPCS2_MASK
			| SPI_PDSR_NPCS3_MASK;

	/* Enable the SPI module */
	SPI_CR = SPI_CR_SPIEN_MASK;

	/* Configure for master mode */
	SPI_MR = SPI_MR_MSTR_MASK | (spi_dev->pcs_decode << SPI_MR_PCSDEC_BIT) | (spi_dev->variable_ps << SPI_MR_PS_BIT);

	/* Also, bind SPI ISR to AIC for DMA transfers */
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_SPI, AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, (void(*)(void)) spi_ISR);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_SPI);

	/* Create signalling semaphore for DMA transfers */
	vSemaphoreCreateBinary(spi_sem_dma);
	xSemaphoreTake(spi_sem_dma, 0);

	/* Create device lock semaphore */
	spi_dev->lock = NULL;
	spi_dev->lock = xSemaphoreCreateMutex();

	/* Configure PDC Lines for DMA transfers */
	PDC_PRA6 = (uint32_t) & (SPI_RDR);			// Pointer to data register RX
	PDC_PRA7 = (uint32_t) & (SPI_TDR);			// Pointer to data register TX
	PDC_CR6 = 0;								// Size = 8 bit, Direction = RX
	PDC_CR7 = 1;								// Size = 8 bit, Direction = TX
	PDC_TCR6 = 0;								// Disable PDC by setting size = 0
	PDC_TCR7 = 0;								// Disable PDC by setting size = 0

	return 1;

}

/**
 * @brief After initializing hardware using spi_init_dev, this function must be called to setup the CS line
 * @param spi_chip a chip configuration struct, this must be pre-configured with spi_dev and all options.
 */
int spi_setup_chip(spi_chip_t * spi_chip) {

	/* Calculate the value of the SPI mode
	 * 0: CPOL = 0, CPHA = 0;
	 * 1: CPOL = 0, CPHA = 1;
	 * 2: CPOL = 1, CPHA = 0;
	 * 3: CPOL = 1, CPHA = 1;
	 * On the ARM the register order is NCPHA, CPOL, so: 
	 * 0 = 2, 1 = 0, 2 = 3 and 3 = 1;
	 * */
	uint8_t spi_mode_reg;
	switch(spi_chip->spi_mode) {
		case 0: spi_mode_reg = 2; break;
		case 1: spi_mode_reg = 0; break;
		case 2: spi_mode_reg = 3; break;
		case 3: spi_mode_reg = 1; break;
		default: return 0;
	}

	/* Calculate the value of the CSR register */
	uint8_t scbr = (cpu_core_clk/(2*spi_chip->baudrate));
	if (scbr < 2)
		scbr = 2;
	//printf("scbr %u %u\r\n", scbr, spi_chip->baudrate);

	uint32_t csr_reg = spi_mode_reg |
			((spi_chip->bits - 8) << SPI_CSR0_BITS_BIT) |
			(scbr << SPI_CSR0_SCBR_BIT) |
			(spi_chip->spck_delay << SPI_CSR0_DLYBS_BIT) |
			(spi_chip->trans_delay << SPI_CSR0_DLYBCT_BIT);

	/* Set the CSR register depending on the chip location */
	switch (spi_chip->reg) {
	case 0:
		SPI_CSR0 = csr_reg;
		break;
	case 1:
		SPI_CSR1 = csr_reg;
		break;
	case 2:
		SPI_CSR2 = csr_reg;
		break;
	case 3:
		SPI_CSR3 = csr_reg;
		break;
	}

	/* Reset the controller to apply new values */
	SPI_CR = SPI_CR_SPIDIS_MASK;
	SPI_CR = SPI_CR_SPIEN_MASK;
	current_chip = NULL;
	return 1;

}

int spi_lock_dev(spi_dev_t * spi_dev) {
	if ((!spi_dev) || (!spi_dev->lock))
		return -1;
	if (xSemaphoreTake(spi_dev->lock, SPI_TIMEOUT) == pdFALSE) {
		log_error("SD_LOCK", "");
		return -1;
	}
	return 0;
}

void spi_unlock_dev(spi_dev_t * spi_dev) {
	if ((!spi_dev) || (!spi_dev->lock))
		return;
	xSemaphoreGive(spi_dev->lock);
}

/**
 * Send a byte via SPI
 * @param spi_chip The chip to send to
 * @param 16 bits to send
 */
void spi_write(spi_chip_t * spi_chip, uint16_t byte) {

	spi_busy_wait_tx();

	/* Re-initialise new chip in new mode */
	if (current_chip != spi_chip) {
		spi_setup_chip(spi_chip);
		current_chip = spi_chip;
	}

	/* Select chip */
	SPI_MR = (SPI_MR & ~SPI_MR_PCS_MASK) | (spi_chip->cs << SPI_MR_PCS_BIT);
	SPI_TDR = byte;

}

/**
 * Read a byte from SPI
 * @param spi_chip Chip to read from
 * @return 16 bits received
 */
uint16_t spi_read(spi_chip_t * spi_chip) {
	spi_busy_wait_rx();
	uint16_t data = SPI_RDR;
	return data;
}

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
int spi_dma_transfer(spi_chip_t * spi_chip, uint8_t * outbuf, uint8_t * inbuf, uint32_t length) {

	unsigned int bytes_done = 0;

	/* Wait for SPI to become ready */
	if (!spi_busy_wait_tx()) {
		xSemaphoreGive(spi_chip->spi_dev->lock);
		return 0;
	}

	/* Set chip select mode */
	if (current_chip != spi_chip) {
		spi_setup_chip(spi_chip);
		current_chip = spi_chip;
	}

	/* Select chip */
	SPI_MR = (SPI_MR & ~SPI_MR_PCS_MASK) | (spi_chip->cs << SPI_MR_PCS_BIT);

	/* Enable PDC interrupt on SPI */
	SPI_IER = SPI_IER_TEND;

	/* Loop here if length is larger than 0xFFFF */
	while (bytes_done < length) {

		/* Start PDC Transfer */
		if (inbuf != NULL)
			PDC_MPR6 = (uint32_t) &inbuf[bytes_done];
		PDC_MPR7 = (uint32_t) &outbuf[bytes_done];

		/* Number of bytes to read - truncated to 64k */
		if ((length - bytes_done) > 0x0000FFFF) {
			if (inbuf != NULL)
				PDC_TCR6 = 0x0000FFFF;
			PDC_TCR7 = 0x0000FFFF;
			bytes_done += 0x0000FFFF;
		} else {
			if (inbuf != NULL)
				PDC_TCR6 = length - bytes_done;
			PDC_TCR7 = length - bytes_done;
			bytes_done = length;
		}

		/* Wait for the DMA finish signal */
		xSemaphoreTake(spi_sem_dma, 1*configTICK_RATE_HZ);

	}

	/* Disable PDC interrupt on SPI */
	SPI_IER = SPI_IER_TEND;

	return 1;

}
