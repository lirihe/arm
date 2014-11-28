/**
 * Network Interface and Client API for NanoCam
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>
#include <io/nanocam.h>
#include <string.h>
#include <util/hexdump.h>
#include <util/log.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

static uint8_t node_cam = 22;

/**
 * Set the node
 * @param CSP node address
 */
void nanocam_set_node(uint8_t node) {
	node_cam = node;
}

int nanocam_snap(uint8_t input, uint8_t raw_capture[], int timeout) {
	int8_t reply[1+64];
	cam_snap_t snap;
	snap.input = input;
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_SNAP, timeout, &snap, sizeof(cam_snap_t), &reply, sizeof(reply));
	if (status < 1)
		return -2;
	memcpy(raw_capture, &reply[1], 64);
	for(int i = 0; i < 16; i++) {
		((uint32_t *)raw_capture)[i] = csp_ntoh32(((uint32_t *)raw_capture)[i]);
	}
	return reply[0];
}

uint32_t nanocam_autofocus(uint8_t input, int timeout) {
	cam_snap_t snap;
	uint32_t reply;
	snap.input = input;
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_AUTOFOCUS, timeout, &snap, sizeof(cam_snap_t), &reply, sizeof(reply));
	if (status < 1)
		return -2;
	reply = csp_ntoh32(reply);

	return reply;
}

uint8_t nanocam_imgstat(cam_stat_t * stat, int timeout) {
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_IMGSTAT, timeout, NULL, 0, stat, sizeof(cam_stat_t));
	if (status < 1)
		return -2;

	return 0;
}


int nanocam_store(uint8_t input, uint8_t format, uint8_t scale_down, const char * filename, int timeout, cam_store_reply_t * reply) {

	cam_store_t store;
	store.input = input;
	store.format = format;
	store.scale_down = scale_down;
	strncpy((char *) store.filename, filename, 40);
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_STORE, timeout, &store, sizeof(cam_store_t), reply, sizeof(*reply));

	if (status < 0)
		return -2;

	reply->image_ptr = csp_ntoh32(reply->image_ptr);
	reply->image_size = csp_ntoh32(reply->image_size);

	return 0;
}

int nanocam_temp(float * return_temp, int timeout) {

	int16_t reply;
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_TEMP, timeout, NULL, 0, &reply, sizeof(int16_t));
	if (status < 0)
		return -2;
	reply = csp_ntoh16(reply);
	*return_temp = (float) (reply >> 5) / 4.0;
	return 0;

}

int nanocam_twi_read(uint8_t reg, uint16_t * return_val, int timeout) {

	cam_reg_t req;
	req.reg_addr = reg;
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_REG_READ, timeout, &req, sizeof(cam_reg_t), &req, sizeof(cam_reg_t));
	if (status < 0)
		return -2;
	*return_val = csp_ntoh16(req.reg_value);
	return 0;

}

int nanocam_twi_write(uint8_t reg, uint16_t val, int timeout) {

	uint8_t reply;
	cam_reg_t req;
	req.reg_addr = reg;
	req.reg_value = csp_hton16(val);
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_REG_WRITE, timeout, &req, sizeof(cam_reg_t), &reply, sizeof(reply));
	if (status < 0)
		return -2;
	return reply;

}

int nanocam_img_list(cam_list_t * list, int timeout) {

	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_IMG_LIST, timeout, NULL, 0, list, -1);
	if (status < 0)
		return -2;

	unsigned int i;
	for (i = 0; i < list->count; i++) {
		list->images[i].size = csp_ntoh32(list->images[i].size);
		list->images[i].time = csp_ntoh32(list->images[i].time);
		list->images[i].ptr = csp_ntoh32(list->images[i].ptr);
	}

	return 0;

}

int nanocam_format_fs(void) {

	unsigned char result;
	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_PORT_RECOVER_FS, 25000, NULL, 0, &result, 1);
	if (status != 1)
		return -2;

	return result;

}

int nanocam_conf_restore(void) {

	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_CONF_CMD_RESTORE, 25000, NULL, 0, NULL, 0);
	if (status != 1)
		return -2;

	return 0;

}

int nanocam_conf_save(void) {

	int status = csp_transaction(CSP_PRIO_HIGH, node_cam, CAM_CONF_CMD_SAVE, 25000, NULL, 0, NULL, 0);
	if (status != 1)
		return -2;

	return 0;

}
