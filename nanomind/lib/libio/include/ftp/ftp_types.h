/*
 * ftp_upload.h
 *
 *  Created on: 07/07/2010
 *      Author: johan
 */

#ifndef _FTP_UPLOAD_H_
#define _FTP_UPLOAD_H_

#include <stdint.h>
#include <csp/csp.h>

/** Maximum path length */
#define FTP_PATH_LENGTH 50

/** Maximum chunk size */
#define FTP_CHUNK_SIZE 256

/** Number of chunks in status message */
#define FTP_STATUS_CHUNKS 10

/** FTP Type enumeration */
typedef enum __attribute__ ((__packed__)) {
	FTP_UPLOAD_REQUEST		= 0, 	/**< New upload request */
	FTP_UPLOAD_REPLY 		= 1, 	/**< New upload reply */
	FTP_DOWNLOAD_REQUEST	= 2, 	/**< New download request */
	FTP_DOWNLOAD_REPLY 		= 3, 	/**< New download reply */
	FTP_STATUS_REQUEST		= 5, 	/**< Status request */
	FTP_STATUS_REPLY		= 6, 	/**< Status reply */
	FTP_LIST_REQUEST		= 7, 	/**< List request */
	FTP_LIST_REPLY			= 8, 	/**< List reply */
	FTP_LIST_ENTRY 			= 9,	/**< List entry */
	FTP_MOVE_REQUEST		= 10,	/**< Move request */
	FTP_MOVE_REPLY 			= 11, 	/**< Move reply */
	FTP_REMOVE_REQUEST		= 12,	/**< Remove request */
	FTP_REMOVE_REPLY		= 13,	/**< Remove reply */
	FTP_CRC_REQUEST			= 14, 	/**< CRC request */
	FTP_CRC_REPLY 			= 15, 	/**< CRC reply */
	FTP_DATA				= 16, 	/**< Data chunk */
	FTP_DONE				= 17,	/**< Transfer done */
	FTP_ABORT				= 18,	/**< Abort transfer */
} ftp_type_t;

/** FTP return codes */
typedef enum __attribute__ ((__packed__)) {
	FTP_RET_OK 				= 0, 	/**< No error */
	FTP_RET_PROTO			= 1, 	/**< Protocol error */
	FTP_RET_NOENT			= 2, 	/**< No such file or directory */
	FTP_RET_INVAL			= 3, 	/**< Invalid argument */
	FTP_RET_NOSPC			= 4, 	/**< No space left on device */
	FTP_RET_IO				= 5, 	/**< IO error */
	FTP_RET_FBIG			= 6, 	/**< File to large */
	FTP_RET_EXISTS			= 7, 	/**< File exists */
	FTP_RET_NOTSUP			= 8,	/**< Operation not supported */
	FTP_RET_NOMEM			= 9,	/**< Not enough memory */
} ftp_return_t;

/** Upload file request */
typedef struct {
	uint32_t size;
	uint32_t crc32;
	uint16_t chunk_size;
	uint8_t backend;
	uint32_t mem_addr;
	char path[FTP_PATH_LENGTH];
} __attribute__ ((__packed__)) ftp_upload_request_t;

/** Upload file reply */
typedef struct {
	uint8_t ret;
} __attribute__ ((__packed__)) ftp_upload_reply_t;

/** Download file request */
typedef struct {
	uint16_t chunk_size;
	uint8_t backend;
	uint32_t mem_addr;
	uint32_t mem_size;
	char path[FTP_PATH_LENGTH];
} __attribute__ ((__packed__)) ftp_download_request_t;

/** Download file reply */
typedef struct {
	uint8_t ret;
	uint32_t size;
	uint32_t crc32;
} __attribute__ ((__packed__)) ftp_download_reply_t;

/** Move file request */
typedef struct {
	uint8_t backend;
	char from[FTP_PATH_LENGTH];
	char to[FTP_PATH_LENGTH];
} __attribute__ ((__packed__)) ftp_move_request_t;

/** Move file reply */
typedef struct {
	uint8_t ret;
} __attribute__ ((__packed__)) ftp_move_reply_t;

/** Remove file request */
typedef struct {
	uint8_t backend;
	char path[FTP_PATH_LENGTH];
} __attribute__ ((__packed__)) ftp_remove_request_t;

/** Remove file reply */
typedef struct {
	uint8_t ret;
} __attribute__ ((__packed__)) ftp_remove_reply_t;

/** Data chunk */
typedef struct {
	uint32_t chunk;
	uint8_t bytes[FTP_CHUNK_SIZE];
} __attribute__ ((__packed__)) ftp_data_t;

typedef struct {
	uint32_t next;
	uint16_t count;
} __attribute__ ((__packed__)) ftp_status_element_t;

/** Status packet */
typedef struct {
	uint8_t ret;
	uint32_t complete;
	uint32_t total;
	uint16_t entries;
	ftp_status_element_t entry[FTP_STATUS_CHUNKS];
} __attribute__ ((__packed__)) ftp_status_reply_t;

/** List files request */
typedef struct {
	uint8_t backend;
	char path[FTP_PATH_LENGTH];
} __attribute__ ((__packed__)) ftp_list_request_t;

/** List files reply */
typedef struct {
	uint8_t ret;
	uint16_t entries;
} __attribute__ ((__packed__)) ftp_list_reply_t;

typedef enum {
	FTP_LIST_FILE 	= 0,
	FTP_LIST_DIR	= 1,
} __attribute__ ((__packed__)) ftp_list_entry_type_t;

/** List files entry */
typedef struct {
	uint16_t entry;
	char path[FTP_PATH_LENGTH];
	uint8_t type;
	uint32_t size;
} __attribute__ ((__packed__)) ftp_list_entry_t;

/** CRC reply */
typedef struct {
	uint8_t ret;
	uint32_t crc;
} __attribute__ ((__packed__)) ftp_crc_reply_t;

/** FTP packet */
typedef struct {
	uint8_t type;
	union {
		/* Download */
		ftp_download_request_t down;
		ftp_download_reply_t downrep;

		/* Upload */
		ftp_upload_request_t up;
		ftp_upload_reply_t uprep;

		/* Data */
		ftp_data_t data;

		/* Status reply */
		ftp_status_reply_t statusrep;

		/* Listing */
		ftp_list_request_t list;
		ftp_list_reply_t listrep;
		ftp_list_entry_t listent;

		/* Remove */
		ftp_remove_request_t remove;
		ftp_remove_reply_t removerep;

		/* Move */
		ftp_move_request_t move;
		ftp_move_reply_t moverep;

		/* CRC */
		ftp_crc_reply_t crcrep;
	};
} __attribute__ ((__packed__)) ftp_packet_t;

double ftp_get_transfer_progress();
double ftp_get_transfer_bps();

#endif /* _FTP_UPLOAD_H_ */
