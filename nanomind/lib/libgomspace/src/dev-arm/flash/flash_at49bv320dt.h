/**
 * Low-level flash driver for AT49BV320DT
 *
 * @author Karl Kaas Laursen and Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef FLASH_DEV_H
#define FLASH_DEV_H

#define FLASH_SIZE     0x00400000
#define FLASH_16_COUNT 63
#define FLASH_16_SIZE  0x10000
#define FLASH_8_COUNT  8
#define FLASH_8_SIZE   0x2000
#define FLASH_8_OFFSET (FLASH_16_SIZE * FLASH_16_COUNT)

#define FLASH_ERR_OK              0x00  // No error - operation complete
#define FLASH_ERR_INVALID         0x01  // Invalid FLASH address
#define FLASH_ERR_ERASE           0x02  // Error trying to erase
#define FLASH_ERR_LOCK            0x03  // Error trying to lock/unlock
#define FLASH_ERR_PROGRAM         0x04  // Error trying to program
#define FLASH_ERR_PROTOCOL        0x05  // Generic error
#define FLASH_ERR_PROTECT         0x06  // Device/region is write-protected
#define FLASH_ERR_NOT_INIT        0x07  // FLASH info not yet initialized
#define FLASH_ERR_HWR             0x08  // Hardware (configuration?) problem
#define FLASH_ERR_ERASE_SUSPEND   0x09  // Device is in erase suspend mode
#define FLASH_ERR_PROGRAM_SUSPEND 0x0a  // Device is in in program suspend mode
#define FLASH_ERR_DRV_VERIFY      0x0b  // Driver failed to verify data
#define FLASH_ERR_DRV_TIMEOUT     0x0c  // Driver timed out waiting for device
#define FLASH_ERR_DRV_WRONG_PART  0x0d  // Driver does not support device
#define FLASH_ERR_LOW_VOLTAGE     0x0e  // Not enough juice to complete job

int dev_flash_hwr_init(void);
int dev_flash_erase(void* block_addr, unsigned int size);
int dev_flash_program(void* addr, void* data, int len);
char * dev_flash_errmsg(int err);

#endif
