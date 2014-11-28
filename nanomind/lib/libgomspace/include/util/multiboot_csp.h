/*
 * multiboot_obc.h
 *
 *  Created on: 14/04/2010
 *      Author: oem
 */

#ifndef MULTIBOOT_OBC_H_
#define MULTIBOOT_OBC_H_

#include <csp/csp.h>
#include <stdint.h>

#define FLASH_BLOCK_BOOT_TAB1 67
#define FLASH_BLOCK_BOOT_TAB2 68
#define FLASH_BLOCK_BOOT_TAB3 69
#define FLASH_BLOCK_JUMP_CODE 70
#define FLASH_SECRET_BOOT_CODE 0x12345678
#define FLASH_NO_SW_IMAGES 16
#define DEFAULT_SW_IMAGE_SIZE 0x40000

/** Software image information */
typedef struct {
    unsigned int   base;
    unsigned int   size;
    unsigned int   boots;
    unsigned int   checksum;
} __attribute__((__packed__)) multiboot_info_t;

/** Software image table */
typedef struct {
    unsigned int boot_vector;
    multiboot_info_t images[FLASH_NO_SW_IMAGES];
} __attribute__((__packed__)) multiboot_tab_t;

/** Upload request */
typedef struct {
	uint32_t len;
	uint32_t crc32;
	uint32_t chunk_size;
	uint32_t address;
	uint8_t version;
} __attribute__((__packed__)) multiboot_upload_image_t;

typedef struct {
	uint32_t len;
	uint32_t crc32;
	uint16_t chunk_size;
	uint8_t path[80];
} __attribute__((__packed__)) multiboot_upload_file_t;

typedef struct {
	uint16_t chunk;
	uint8_t bytes[256];
} __attribute__((__packed__)) multiboot_data_t;

typedef struct {
	uint8_t type;
	union {
		multiboot_upload_image_t image;
		multiboot_upload_file_t file;
		multiboot_data_t data;
	};
} __attribute__((__packed__)) multiboot_packet_t;

typedef struct {
	uint16_t packets;
	uint16_t completed;
	uint16_t next[40];
} __attribute__((__packed__)) multiboot_status_t;

enum {
	MULTIBOOT_UPLOAD_BEGIN_FILE 	= 0,
	MULTIBOOT_UPLOAD_DATA 			= 1,
	MULTIBOOT_UPLOAD_STATUS			= 2,
	MULTIBOOT_UPLOAD_END			= 3,
	MULTIBOOT_UPLOAD_RESTART		= 4,
	MULTIBOOT_UPLOAD_CRC			= 5
} multiboot_upload_types;

/**
 * Worker task to receive software upload
 * @param conn pointer to incoming connection, first packet must not be extracted.
 */
void vTaskMultiboot(void * conn_param);

#endif /* MULTIBOOT_OBC_H_ */
