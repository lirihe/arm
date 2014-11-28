/**
 * Network Interface and Client API for NanoCam
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef NANOCAM_H_
#define NANOCAM_H_

#include <stdint.h>

/* CSP Address */
#define NODE_CAM				6

/* CSP Port numbers */
#define CAM_PORT_SNAP			8
#define CAM_PORT_IMG_LIST		7
#define CAM_PORT_REG_READ		9
#define CAM_PORT_REG_WRITE		10
#define CAM_PORT_STORE			11
#define CAM_PORT_CODEC			12
#define CAM_PORT_TEMP			13
#define CAM_PORT_LOGD			14
#define CAM_PORT_FTP			15
#define CAM_PORT_RECOVER_FS		16
#define CAM_PORT_AUTOFOCUS		17
#define CAM_PORT_IMGSTAT		18
#define CAM_CONF_CMD_SAVE		19
#define CAM_CONF_CMD_RESTORE	20

/**
 * Network data types
 */

typedef struct __attribute__((packed)) {
	uint8_t input;
	uint8_t sensor_id;
} cam_snap_t;

typedef struct __attribute__((packed)) {
	uint8_t input;
	uint8_t format;
	uint8_t scale_down;
	char filename[40];
} cam_store_t;

typedef struct __attribute__((packed)) {
	uint8_t result;
	uint32_t image_ptr;
	uint32_t image_size;
} cam_store_reply_t;

typedef struct __attribute__((packed)) {
	uint8_t reg_addr;
	uint16_t reg_value;
} cam_reg_t;

typedef struct __attribute__((packed)) {
	uint32_t buffer_size;
	uint32_t chunk_size;
} cam_download_t;

typedef struct __attribute__((packed)) {
	uint32_t ptr;
	uint8_t id;
	uint8_t format;
	uint32_t size;
	uint32_t time;
} cam_list_element_t;

typedef struct __attribute__((packed)) {
	uint8_t count;
	cam_list_element_t images[];
} cam_list_t;

typedef struct __attribute__((packed)) {
	uint8_t light[5];
	uint8_t hist[16];
} cam_stat_t;


/**
 * API
 */

/* Two different input paths in the ISI */
enum cam_input_s {
	CAM_INPUT_PREVIEW,//!< CAM_INPUT_PREVIEW
	CAM_INPUT_CODEC,  //!< CAM_INPUT_CODEC
};

/* Storage format */
enum cam_store_format_s {
	CAM_STORE_RAW,
	CAM_STORE_BMP,
	CAM_STORE_JPG,
};

/** Set node */
void nanocam_set_node(uint8_t node);

/**
 * Take a picture
 * @param input: select PREVIEW or CODEC path (Preview is normal)
 * @param raw_capture: buffer where first 64 bytes of capture data is returned
 * @param timeout: typical value should be 1 second + network latency.
 * @return -1 if err or remote status variable
 */
int nanocam_snap(uint8_t input, uint8_t raw_capture[], int timeout);

/**
 * Convert image in memory and store to file
 * @param input: select PREVIEW or CODEC path (Preview is normal)
 * @param format: select format (RAW, BMP or JPG)
 * @param scale_down: image downscale factor
 * @param filename: path to store
 * @param timeout: 10s store, RAW->BMP, 10s, RAW->JPG, 20s + network latency. (30 seconds is typical)
 * @param reply: pointer to reply structure (for returning size and ptr of image)
 * @return -1 if err or 0 if ok
 */
int nanocam_store(uint8_t input, uint8_t format, uint8_t scale_down, const char * filename, int timeout, cam_store_reply_t * reply);

/**
 * Get temperature on CAM board.
 * @param return_temp: pointer to a float that will be updated with temeprature value
 * @param timeout: network timeout
 * @return -1 if err, 0 otherwise
 */
int nanocam_temp(float * return_temp, int timeout);

/**
 * Auto focus routine - calculates "entropy" of a small region in the image
 * @param input: select PREVIEW or CODEC path (Preview is normal)
 * @param timeout: typical value should be 1 second + network latency.
 * @return Entropy magic number
 */
uint32_t nanocam_autofocus(uint8_t input, int timeout);

/**
 * Image statistics routine
 * @param timeout: typical value should be 1 second + network latency.
 * @return -1 if err, 0 otherwise
 */
uint8_t nanocam_imgstat(cam_stat_t * stat, int timeout);

/**
 * Read a camera register
 * @param reg: address
 * @param return_val: value retuned to this pointer
 * @param timeout: network timeout + 100ms
 * @return -1 if err, 0 otherwise
 */
int nanocam_twi_read(uint8_t reg, uint16_t * return_val, int timeout);

/**
 * Program a camera register
 * @param reg: address
 * @param val: value to write
 * @param timeout: network timeout + 100ms
 * @return -1 if err, 0 otherwise
 */
int nanocam_twi_write(uint8_t reg, uint16_t val, int timeout);

/**
 * Request an image list from RAM
 * @param list pointer to list element
 * @param timeout: network timeout + 100ms
 * @return < 0 if err, 0 if ok
 */
int nanocam_img_list(cam_list_t * list, int timeout);

/**
 * Format the FS on NanoCAM
 * @return < 0 if err, 0 if ok
 */
int nanocam_format_fs(void);

/**
 * Restore configuration from file or defaults
 * @return < 0 if err, 0 if ok
 */
int nanocam_conf_restore(void);

/**
 * Save configuration to file
 * @return < 0 if err, 0 if ok
 */
int nanocam_conf_save(void);


#endif /* NANOCAM_H_ */
