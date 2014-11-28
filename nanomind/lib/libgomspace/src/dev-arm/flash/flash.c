/**
 * High-level flash driver for AT49BV320DT
 *
 * @author Karl Kaas Laursen and Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

#include <util/error.h>
#include <util/crc32.h>
#include <dev/arm/flash.h>

#include "flash_at49bv320dt.h"

/**
 * Locking
 * Uses FreeRTOS semaphores
 */
static xSemaphoreHandle flashmux = NULL;
static inline void flash_lock() {
	xSemaphoreTake(flashmux, portMAX_DELAY);
}
static inline void flash_unlock() {
	xSemaphoreGive(flashmux);
}

/**
 * Get absolute byte address given block number
 * @param block number
 * @return absolute byte address
 */
char * flash_addr(int block, unsigned int base) {

	if (block < FLASH_16_COUNT) {
		return (char *) (base + block * FLASH_16_SIZE);

	} else if (block < (FLASH_8_COUNT + FLASH_16_COUNT)) {
		return (char *) ((base + FLASH_16_COUNT * FLASH_16_SIZE) + (block - FLASH_16_COUNT) * FLASH_8_SIZE);

	} else {
		return NULL;
	}

}

/**
 * Read from source address to preallocated buffer
 * @param src Source address in flash.
 * @param data Pointer to data buffer. Must be len bytes or more.
 * @param len Number of bytes to read.
 * @return length of read data (memcpy has no error check)
 */
inline int flash_read(void * src, char * data, int len) {

	flash_lock();
	memcpy(data, src, len);
	flash_unlock();
	return len;

}

/**
 * Program Flash data with optional erase
 * @param dst address of flash destination
 * @param data src pointer of data
 * @param len length to program
 * @param erase Erase flash page before program (1 = yes, 0 = no)
 * @return Error code according to error.h
 */
int flash_program(void * dst, char * data, int len, int erase) {

	uint32_t before_csum, after_csum;
	void *before = NULL, *after = NULL, *start, *stop;
	int before_size, after_size, total_size;
	int block_size;
	int err, ret = E_NO_ERR;

	if (erase) {
		/* Calculate block size */
		if (((unsigned int) dst % FLASH_SIZE) < FLASH_8_OFFSET) {
			block_size = 0x10000;
		} else {
			block_size = 0x2000;
		}

		/* Calculate current page base address = start, and the added length */
		start = (void *) ((unsigned int) dst & ~(block_size - 1));
		stop = (void *) ((unsigned int) (dst + len + block_size) & ~(block_size - 1));
		before_size = dst - start;
		after_size = stop - (dst + len);
		total_size = before_size + len + after_size;

		/* Allocate temporary storage */
		before = pvPortMalloc(before_size);
		if (before == NULL) {
			printf("Failed to allocate 'before' buffer\r\n");
			ret = E_MALLOC_FAIL;
			goto out;
		}

		after = pvPortMalloc(after_size);
		if (after == NULL) {
			printf("Failed to allocate 'after' buffer");
			ret = E_MALLOC_FAIL;
			goto out;
		}

		/* Read out data that should not be erased */
		before_csum = flash_checksum(start, before_size);
		after_csum = flash_checksum(dst + len, after_size);
		memcpy(before, start, before_size);
		memcpy(after, dst + len, after_size);

		/* Lock before touching flash */
		flash_lock();

		/* Erase from this address and onwards */
		err = dev_flash_erase(start, total_size);
		if (err != FLASH_ERR_OK) {
			ret = E_FLASH_ERROR;
			flash_unlock();
			goto out;
		}

		err = dev_flash_program(start, before, before_size);
		if (err != FLASH_ERR_OK) {
			ret = E_FLASH_ERROR;
			flash_unlock();
			goto out;
		}

		err = dev_flash_program(start + before_size, data, len);
		if (err != FLASH_ERR_OK) {
			ret = E_FLASH_ERROR;
			flash_unlock();
			goto out;
		}
		
		err = dev_flash_program(start + before_size + len, after, after_size);
		if (err != FLASH_ERR_OK) {
			ret = E_FLASH_ERROR;
			flash_unlock();
			goto out;
		}
		
		/* Unlock flash before calculating checksums */
		flash_unlock();

		if (before_csum != flash_checksum(start, before_size))
			printf("Flash checksum mismatch for 'before'\r\n");

		if (after_csum != flash_checksum(dst + len, after_size))
			printf("Flash checksum mismatch for 'after'\r\n");
out:
		if (before)
			vPortFree(before);
		if (after)
			vPortFree(after);
	} else {
		/* UFFS handles erasing necessary pages */
		flash_lock();
		err = dev_flash_program((void *) dst, (void *) data, len);
		flash_unlock();

		if (err != FLASH_ERR_OK) {
			printf("%s\n\r", dev_flash_errmsg(err));
			ret = E_FLASH_ERROR;
		}
	}

	return ret;

}

/**
 * Wrapper for erasing a block
 * @param block number
 * @return Error code according to error.h
 */
int flash_erase_block(int block, unsigned int base) {

	int err;

	flash_lock();
	err = dev_flash_erase((void *) flash_addr(block, base), 1);
	flash_unlock();

	if (err != FLASH_ERR_OK) {
		printf("Failed to write to flash: %s\n\r", dev_flash_errmsg(err));
		return E_FLASH_ERROR;
	}

	return E_NO_ERR;

}

/**
 * Perform a chip-erase
 * @return Error code according to error.h
 */
int flash_erase_chip(unsigned int base) {

	int err;

	/* Call low level driver */
	flash_lock();
	err = dev_flash_erase((void *) base, FLASH_SIZE);
	flash_unlock();

	if (err != FLASH_ERR_OK) {
		printf("%s\n\r", dev_flash_errmsg(err));
		return E_FLASH_ERROR;
	}

	return E_NO_ERR;

}

/**
 * Calculate checksum of area of flash memory
 * @param addr pointer to first address
 * @param size number of bytes
 * @return uint32 checksum
 */
unsigned int flash_checksum(unsigned char * addr, unsigned int size) {
	flash_lock();
	unsigned int result = chksum_crc32(addr, size);
	flash_unlock();
	return result;
}

/**
 * Initialise flash driver, no arguments needed.
 * @return Error code according to error.h
 */
int flash_init(void) {

	int err;

	/* Flash mutex */
	if (flashmux == NULL)
		flashmux = xSemaphoreCreateMutex();

	/* Call low level driver */
	flash_lock();
	err = dev_flash_hwr_init();
	flash_unlock();

	/* Check error */
	if (err != FLASH_ERR_OK) {
		printf("%s\n\r", dev_flash_errmsg(err));
		return E_FLASH_ERROR;
	}

	return E_NO_ERR;

}

