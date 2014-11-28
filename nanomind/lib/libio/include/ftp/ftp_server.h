/*
 * task_upload.h
 *
 *  Created on: 14/07/2010
 *      Author: oem
 */

#ifndef TASK_UPLOAD_H_
#define TASK_UPLOAD_H_

#include <stdint.h>
#include <ftp/ftp_types.h>

enum {
	BACKEND_RAM			= 0,
	BACKEND_FAT 		= 1,
	BACKEND_UFFS		= 2,
};

typedef enum {
	FTP_STATE_IDLE 		= 0,
	FTP_STATE_UPLOAD 	= 1,
	FTP_STATE_DOWNLOAD 	= 2,
} ftp_state_t;

/* Backend operations */
typedef struct {
	/* Initialize new backend state */
	ftp_return_t (*init)(void **state);
	/* Release backend state */
	ftp_return_t (*release)(void * state);
	/* Start new upload */
	ftp_return_t (*upload)(void * state, const char const * path, uint32_t memaddr, uint32_t size, uint32_t chunk_size);
	/* Start new download */
	ftp_return_t (*download)(void * state, const char const * path, uint32_t memaddr, uint32_t memsize, uint32_t chunk_size, uint32_t * size, uint32_t * crc);
	/* Write chunk */
	ftp_return_t (*chunk_write)(void * state, uint32_t chunk, uint8_t * data, uint32_t size);
	/* Read chunk */
	ftp_return_t (*chunk_read)(void * state, uint32_t chunk, uint8_t * data, uint32_t size);
	/* Get chunk status */
	ftp_return_t (*status_get)(void * state, uint32_t chunk, int * status);
	/* Set chunk status */
	ftp_return_t (*status_set)(void * state, uint32_t chunk);
	/* List files */
	ftp_return_t (*list)(void * state, char * path, uint16_t * entries);
	/* Next list entry */
	ftp_return_t (*entry)(void * state, ftp_list_entry_t * ent);
	/* Remove file */
	ftp_return_t (*remove)(void * state, char * path);
	/* Move file */
	ftp_return_t (*move)(void * state, char * from, char * to);
	/* Get CRC of file */
	ftp_return_t (*crc)(void * state, uint32_t * crc);
	/* End transfer */
	ftp_return_t (*done)(void * state);
	/* Abort transfer */
	ftp_return_t (*abort)(void * state);
	/* Timeout of transfer */
	ftp_return_t (*timeout)(void * state);
} ftp_backend_t;

extern ftp_backend_t backend_ram;
extern ftp_backend_t backend_fat;
extern ftp_backend_t backend_uffs;

int ftp_register_backend(uint8_t id, ftp_backend_t *backend);

#endif /* TASK_UPLOAD_H_ */
