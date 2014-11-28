/*
 * nanomind.c
 *
 *  Created on: 20/05/2010
 *      Author: Johan
 */
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <io/nanomind.h>

#include <util/timestamp.h>
#include <util/vermagic.h>
#include <util/log.h>


static uint8_t node_obc = NODE_OBC;

/**
 * Set the OBC node
 * @param CSP node nr to set the OBC node to
 */
void obc_set_node(uint8_t node) {
	node_obc = node;
}

/**
 * Execute an OBC timesync
 * Sends a timespec to the OBC and returns the value back from the OBC.
 * Use the value tv_sec = 0, if you don't wish to update time but only get time back.
 * @param time timespec_t seconds and nanoseconds
 * @param timeout timeout in [ms]
 */
void obc_timesync(timestamp_t * time, int timeout) {

	time->tv_sec = csp_hton32(time->tv_sec);
	time->tv_nsec = csp_hton32(time->tv_nsec);
	if (csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_TIMESYNC, timeout, time, sizeof(timestamp_t), time, sizeof(timestamp_t)) > 0) {
		time->tv_sec = csp_ntoh32(time->tv_sec);
		time->tv_nsec = csp_ntoh32(time->tv_nsec);
	} else {
		time->tv_sec = 0;
		time->tv_nsec = 0;
	}

}

/**
 * Jump to an address in RAM
 * This function is only useful after the load command has been executed, or an image
 * has been uploaded to RAM.
 * @param addr
 */
void obc_jump_ram(uint32_t addr) {

	addr = csp_hton32(addr);
	csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_JUMP, 0, &addr, sizeof(uint32_t), NULL, 0);

}

void obc_load_image(const char * path) {

	uint32_t remote_crc;
	if (!csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_LOAD_IMG, 10000, (void *) path, strlen(path)+1, &remote_crc, sizeof(uint32_t)))
		return;

	remote_crc = csp_ntoh32(remote_crc);
	printf("Remote CRC %"PRIX32"\r\n", remote_crc);

}

void obc_boot_conf(uint32_t checksum, uint32_t boot_counts, const char * path) {

	char buf[100];

	checksum = csp_hton32(checksum);
	boot_counts = csp_hton32(boot_counts);
	memcpy(buf, &checksum, 4);
	memcpy(&buf[4], &boot_counts, 4);
	strncpy(buf + 8, path, 100-8);

	csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_BOOT_CONF, 0, buf, strlen(path)+1+4+4, NULL, 0);

}

void obc_boot_del(void) {

	uint32_t checksum = csp_hton32(0x80078007);
	csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_BOOT_CONF, 0, &checksum, 4, NULL, 0);

}


int  obc_boot_count_get(uint32_t *boot_count, int timeout) {

	int ret = csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_BOOT_COUNT, timeout, NULL, 0, boot_count, sizeof(uint32_t));
	if (ret > 0)
		*boot_count = csp_ntoh32(*boot_count);
	else
		*boot_count = 0;
	return ret;
}

int obc_boot_count_reset(uint32_t *boot_count, int timeout) {
	uint8_t cmd = 0;
	int ret = csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_BOOT_COUNT, timeout, &cmd, sizeof(uint8_t), boot_count, sizeof(uint32_t));
	if (ret > 0)
		*boot_count = csp_ntoh32(*boot_count);
	else
		*boot_count = 0;
	return ret;
}


/** Copy image from RAM (src) to ROM (typically 0x48000000) */
void obc_ram_to_rom(uint32_t size, uint32_t checksum, uint32_t src, uint32_t dst) {

	obc_ram_to_rom_t txbuf;

	txbuf.size = csp_hton32(size);
	txbuf.checksum = csp_hton32(checksum);
	txbuf.src = csp_hton32(src);
	txbuf.dst = csp_hton32(dst);

	csp_transaction(CSP_PRIO_NORM, node_obc, OBC_PORT_RAM_TO_ROM, 0, &txbuf, sizeof(obc_ram_to_rom_t), NULL, 0);

}

