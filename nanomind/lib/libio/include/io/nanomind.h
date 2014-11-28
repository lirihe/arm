/*
 * nanomind.h
 *
 *  Created on: 14/04/2010
 *      Author: oem
 */

#ifndef NANOMIND_H_
#define NANOMIND_H_

#include <stdint.h>
#include <util/timestamp.h>
#include <util/vermagic.h>
#include <io/nanocom.h>

/** Default node address */
#define NODE_OBC				1

/** Beacon Downlink address and port */
#define OBC_HK_DOWN_NODE		10
#define OBC_HK_DOWN_PORT		14

/** Port numbers */
#define OBC_PORT_FTP				7
#define OBC_PORT_TIMESYNC			8
#define OBC_PORT_LOAD_IMG			9
#define OBC_PORT_JUMP				10
#define OBC_PORT_BOOT_CONF			11
#define OBC_PORT_FS_TO_FLASH		12
#define OBC_PORT_RAM_TO_ROM			13
#define OBC_PORT_LOGD				14
#define OBC_PORT_PARAM				15
#define OBC_PORT_HK					16
#define OBC_PORT_BOOT_COUNT			17

#define OBC_PORT_FP					18
#define OBC_PORT_CONN_LESS_TEST		19
#define OBC_PORT_ADCS				20

#define OBC_PORT_RSH				22

typedef struct __attribute__((packed)) obc_beacon_s {
	uint32_t time_sec;			// 4 bytes
	struct __attribute__((packed)) {
		uint16_t temp1;
		uint16_t temp2;
		int16_t panel_temp[6];
	} obc;						// 4 bytes
	struct __attribute__((packed)) {
		uint16_t levels[4];
	} log;
	nanocom_data_t com;			// 36 bytes packed
	struct __attribute__ ((__packed__)) {
		uint16_t pv[3];			//Photo-voltaic input voltage [mV]
		uint16_t pc;			//Total photo current [mA]
		uint16_t bv;			//Battery voltage [mV]
		uint16_t sc;			//Total system current [mA]
		int16_t  temp[4];	    //Temp. of boost converters and on-board battery [degC] (Conv 1, Conv 2, Conv 3, Board)
		uint16_t bootcount;     //Number of EPS reboots
		uint8_t  channel_status;//Mask of output channel status, 1=on, 0=off [MSB NC NC 5V1 5V2 5V3 3.3V1 3.3V2 3.3V3 LSB]
	} eps;
} obc_beacon_t;

typedef struct obc_ram_to_rom_s {
	uint32_t size;
	uint32_t checksum;
	uint32_t src;
	uint32_t dst;
} obc_ram_to_rom_t;

void obc_set_node(uint8_t node);
void obc_timesync(timestamp_t * time, int timeout);
void obc_jump_ram(uint32_t addr);
void obc_load_image(const char * path);
void obc_boot_conf(uint32_t checksum, uint32_t boot_counts, const char * path);
void obc_boot_del(void);
void obc_fs_to_flash(uint32_t addr, const char * path);
void obc_ram_to_rom(uint32_t size, uint32_t checksum, uint32_t src, uint32_t dst);
int obc_boot_count_get(uint32_t *boot_count, int timeout);
int obc_boot_count_reset(uint32_t *boot_count, int timeout);

#endif /* NANOMIND_H_ */
