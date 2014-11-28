/**
 * Low-level flash driver for AT49BV320DT
 *
 * @author Karl Kaas Laursen and Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <dev/arm/flash.h>

#include "flash_at49bv320dt.h"

typedef unsigned short int flash_data_t;

/**
 * Initialise flash device by sending abort and read mode command
 * @return Error code
 */
int dev_flash_hwr_init(void) {

	volatile flash_data_t * addr = (volatile flash_data_t *) 0x48000000;

	/* Wait for pending operation and init in read mode */
	*addr = 0x50; 		// Clear status
	*addr = 0x70;		// Read Status

	/* Wait until ready */
	volatile int timeout = 100000;
	while (timeout--)
		if (*addr & 0x80)
			break;

	if (timeout <= 0)
		return FLASH_ERR_NOT_INIT;

	*addr = 0xFF; 		// Read mode

	return FLASH_ERR_OK;

}

/**
 * Erase flash from block_addr size bytes forward.
 * If block_addr is not block aligned, it will automatically be truncated to the
 * nearest block base address.
 * @param block_addr Block address
 * @param size Size in bytes to erase
 * @return Error code
 */
int dev_flash_erase(void * block_addr, unsigned int size) {

	volatile flash_data_t* b_p = (flash_data_t*) block_addr;
	volatile flash_data_t* b_end = (flash_data_t*) (block_addr + size);
	volatile flash_data_t state;
	int timeout;
	int block_size;

	/**
	 * b_p is our block-pointer which is incremented as
	 * we go forward. We loop until we pass the end pointer, b_end.
	 */
	while (b_p < b_end) {

		/* Calculate block size */
		if (((unsigned int) b_p % FLASH_SIZE) < FLASH_8_OFFSET) {
			block_size = 0x10000;
		} else {
			block_size = 0x2000;
		}

		/* Quantize to nearest block start address */
		b_p = (flash_data_t *) ((unsigned int) b_p - ((unsigned int) b_p % block_size));

		/* Unlock sector command */
		*b_p = 0x60;
		*b_p = 0xD0;
		*b_p = 0xFF;

		/* Erase command */
		*b_p = 0x20;
		*b_p = 0xD0;

		//printf("Erase at %p\r\n", b_p);

		/* Now poll for the completion of the sector erase timer (up to 5 seconds!) */
		timeout = 0.5 * configTICK_RATE_HZ;
		while (--timeout) {
			state = *b_p;
			if (state & 0x80)
				break;
			vTaskDelay(10);
		}

		//printf("Done in %lu ticks\r\n", 5 * configTICK_RATE_HZ - timeout);

		/* Check full error */
		if (state & 0x19) {
			*b_p = 0x50;	// Clear status
			if (state & 0x08)
				return FLASH_ERR_LOW_VOLTAGE;
			if (state & 0x10)
				return FLASH_ERR_PROGRAM;
			if (state & 0x01)
				return FLASH_ERR_PROTECT;
		}

		/* Check timeout value */
		if (timeout <= 0)
			return FLASH_ERR_ERASE;

		/* Verify erase operation byte for byte.
		 * This also increments the block-pointer */
		*b_p = 0xFF;
		while (block_size > 0) {
			if (*b_p++ != (unsigned short int) 0xFFFF)
				return FLASH_ERR_DRV_VERIFY;
			block_size -= sizeof(*b_p);
		}

	}
	return FLASH_ERR_OK;
}

/**
 * Program flash
 * @param addr destination address in flash
 * @param data source address in RAM
 * @param len length in bytes
 * @return Error code
 */
int dev_flash_program(void * addr, void * data, int len) {

	volatile flash_data_t * data_ptr = (volatile flash_data_t *) data;
	volatile flash_data_t * addr_p = (volatile flash_data_t *) addr;
	volatile flash_data_t state;
	volatile int timeout;
	int res = FLASH_ERR_OK;

	/* Prepare page */
	*addr_p = 0x60;		// Sector (un)locking
	*addr_p = 0xD0;		// Set unlock at addr_p
	*addr_p = 0xFF;		// Set read

	while (len > 0) {

		/* Program data [byte] - 4 step sequence */
		*addr_p = 0x40;
		*addr_p = *data_ptr;

		/* Wait until ready */
		timeout = 10000;
		while (--timeout) {
			state = *addr_p;
			if (state & 0x80)
				break;
		}

		/* Enable Read mode (to verify) */
		*addr_p = 0xFF;

		/* Check full error */
		if (state & 0x19) {
			*addr_p = 0x50;	// Clear status
			if (state & 0x08)
				return FLASH_ERR_LOW_VOLTAGE;
			if (state & 0x10)
				return FLASH_ERR_PROGRAM;
			if (state & 0x01)
				return FLASH_ERR_PROTECT;
		}

		if (!timeout)
			return FLASH_ERR_DRV_TIMEOUT;

		if (*addr_p != *data_ptr)
			return FLASH_ERR_DRV_VERIFY;

		/* This is VERY verbose */
		//if ((unsigned int) addr_p % 16 == 0)
		//	printf("\r\n%p", addr_p);
		//printf(" %02hhx ", *addr_p);

		len -= sizeof(flash_data_t);
		addr_p++;
		data_ptr++;
	}

	return res;

}

char * dev_flash_errmsg(int err) {
	switch (err) {
	case FLASH_ERR_OK:
		return "No error - operation complete";
	case FLASH_ERR_ERASE_SUSPEND:
		return "Device is in erase suspend state";
	case FLASH_ERR_PROGRAM_SUSPEND:
		return "Device is in program suspend state";
	case FLASH_ERR_INVALID:
		return "Invalid FLASH address";
	case FLASH_ERR_ERASE:
		return "Error trying to erase";
	case FLASH_ERR_LOCK:
		return "Error trying to lock/unlock";
	case FLASH_ERR_PROGRAM:
		return "Error trying to program";
	case FLASH_ERR_PROTOCOL:
		return "Generic error";
	case FLASH_ERR_PROTECT:
		return "Device/region is write-protected";
	case FLASH_ERR_NOT_INIT:
		return "FLASH sub-system not initialized";
	case FLASH_ERR_DRV_VERIFY:
		return "Data verify failed after operation";
	case FLASH_ERR_DRV_TIMEOUT:
		return "Driver timed out waiting for device";
	case FLASH_ERR_DRV_WRONG_PART:
		return "Driver does not support device";
	case FLASH_ERR_LOW_VOLTAGE:
		return "Device reports low voltage";
	default:
		return "Unknown error";
	}
}
