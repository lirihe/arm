/*
 * server.c
 *
 *  Created on: 29/01/2010
 *      Author: johan
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <fcntl.h>

#include <conf_nanomind.h>
#ifdef WITH_STORAGE
#include <conf_storage.h>
#endif

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <supervisor/supervisor.h>

#include <dev/cpu.h>
#include <dev/arm/wdt.h>
#include <dev/arm/flash.h>
#include <util/crc32.h>
#include <util/error.h>
#include <util/timestamp.h>
#include <util/vermagic.h>
#include <io/nanomind.h>

#include <util/clock.h>

#include <dev/bootcounter.h>

#if ENABLE_RTC
#include <dev/arm/ds1302.h>
#endif

void vWork(csp_conn_t * conn, int timeout) {

	csp_packet_t * packet = NULL;

	/* Read incoming data */
	while ((packet = csp_read(conn, timeout)) != NULL) {

		//printf("Worker reading port %u\r\n", csp_conn_dport(conn));

		switch (csp_conn_dport(conn)) {

		case OBC_PORT_TIMESYNC: {

			/* Get incoming time data */
			timestamp_t * time = (timestamp_t *) packet->data;
			time->tv_sec = csp_ntoh32(time->tv_sec);
			time->tv_nsec = csp_ntoh32(time->tv_nsec);

			/* Sync */
			if (time->tv_sec != 0) {

				clock_set_time(time);

#if ENABLE_RTC
				struct ds1302_clock clock;
				time_t tt = time->tv_sec;
				ds1302_time_to_clock(&tt, &clock);
				ds1302_clock_write_burst(&clock);
#endif

			}

			/* Return */
			clock_get_time(time);

			/* Send reply */
			time->tv_sec = csp_hton32(time->tv_sec);
			time->tv_nsec = csp_hton32(time->tv_nsec);

			packet->length = sizeof(timestamp_t);
			if (!csp_send(conn, packet, 0))
				csp_buffer_free(packet);

			break;
		}

#if ENABLE_CDH_HK
		case OBC_PORT_HK: {
			extern rrstore_handle hk_store;
			int res;

			uint8_t bool_newest = packet->data[0];
			uint8_t bool_peek = packet->data[1];
			uint16_t index = csp_ntoh16(packet->data16[1]);
			uint16_t count = csp_ntoh16(packet->data16[2]);

			if (count > hk_store.header.count)
				count = hk_store.header.count;

			while(count > 0) {

				//printf("HK Reg N: %u, P: %u, I: %u, C: %u\r\n", bool_newest, bool_peek, index, count);

				if (packet == NULL)
					packet = csp_buffer_get(sizeof(obc_beacon_t));

				if (bool_newest) {
					if (bool_peek) {
						res = rrstore_peek_newest(&hk_store, packet->data, index++);
					} else {
						res = rrstore_get_newest(&hk_store, packet->data);
					}
				} else {
					if (bool_peek) {
						res = rrstore_peek_oldest(&hk_store, packet->data, index++);
					} else {
						res = rrstore_get_oldest(&hk_store, packet->data);
					}
				}

				if (res >= 0) {
					packet->length = sizeof(obc_beacon_t);
				} else {
					packet->length = 0;
				}

				if (!csp_send(conn, packet, 5000))
					csp_buffer_free(packet);

				packet = NULL;
				count--;
			}

			return;
			break;
		}

#endif

		case OBC_PORT_BOOT_COUNT: {
			uint32_t tmp_boot_counter;
			// Empty packet is only get request
			if (packet->length != 0) {
				// Data cmd == 0 is reset counter
				if (packet->data[0] == 0) {
					boot_counter_reset();
				}
			}

			/* return boot counter */
			packet->length = sizeof(uint32_t);
			tmp_boot_counter = csp_hton32(boot_counter_get());
			memcpy(packet->data, &tmp_boot_counter, sizeof(uint32_t));
			if (!csp_send(conn, packet, 0))
				csp_buffer_free(packet);
			break;
		}


#ifdef ENABLE_UFFS


		case OBC_PORT_BOOT_CONF: {

			uint32_t checksum;
			uint32_t boot_counts;
			memcpy(&checksum, packet->data, sizeof(checksum));
			checksum = csp_ntoh32(checksum);
			memcpy(&boot_counts, packet->data+sizeof(checksum), sizeof(boot_counts));
			boot_counts = csp_ntoh32(boot_counts);

			printf("Got boot config, checksum %#"PRIX32" and boot counts %"PRIu32"\r\n", checksum, boot_counts);

			/* If checksum is magic, delete boot conf */
			if (checksum == 0x80078007) {
				remove("/boot/boot.conf");
				printf("Deleted boot config\r\n");
				csp_buffer_free(packet);
				break;
			}

			char * path = (char *) packet->data + sizeof(checksum) + sizeof(boot_counts);

			if (!strnlen(path, 100)) {
				csp_buffer_free(packet);
				break;
			}

			/* If boot_counts > 100 limit boot_counts to 100 */
			if (boot_counts > 100)
				boot_counts = 100;

			printf("Inserting path %s to /boot/boot.conf\r\n", path);

			int fd = open("/boot/boot.conf", O_WRONLY | O_CREAT);
			if (fd < 0) {
				csp_buffer_free(packet);
				break;
			}

			if (write(fd, path, strlen(path)) < (int) strlen(path)) {
				close(fd);
				csp_buffer_free(packet);
				break;
			}

			char pbuf[20];
			/* Lines in /boot/boot.conf must end with newline and no linefeed! */
			sprintf(pbuf, "\n%"PRIX32"\n", checksum);
			printf("Checksum: %#"PRIX32"\r\n", checksum);
			if (write(fd, pbuf, strlen(pbuf)) < (int) strlen(pbuf)) {
				close(fd);
				csp_buffer_free(packet);
				break;
			}

			/* Lines in /boot/boot.conf must end with newline and no linefeed! */
			sprintf(pbuf, "%"PRIu32"\n", boot_counts);
			printf("Boot counts: %"PRIu32"\r\n", boot_counts);
			if (write(fd, pbuf, strlen(pbuf)) < (int) strlen(pbuf)) {
				close(fd);
				csp_buffer_free(packet);
				break;
			}

			close(fd);
			csp_buffer_free(packet);
			break;
		}

#if defined(OBC_ROM)
		case OBC_PORT_LOAD_IMG: {

			char * path = (char *) packet->data;

			if (!strnlen(path, 100)) {
				csp_buffer_free(packet);
				break;
			}

			int fd = open(path, O_RDONLY);
			if (fd < 0) {
				printf("Failed to open file %s\r\n", path);
				csp_buffer_free(packet);
				break;
			}

			printf("File opened\r\n");

			void * dst = (void *) 0x50000000;

			/* Read file size */
			unsigned int size;
			struct stat st;
			if (stat(path, &st) != 0) {
				printf("Failed to stat image file\r\n");
				close(fd);
				csp_buffer_free(packet);
				break;
			} else {
				size = st.st_size;
			}
			unsigned int copied = 0;

			printf("Copying %u bytes to %p\r\n", size, dst);

			copied = read(fd, dst, size);
			if (copied != size) {
				printf("Failed to copy image\r\n");
				close(fd);
				csp_buffer_free(packet);
				break;
			}

			printf("Copied %u bytes\r\n", copied);

			/* Checking checksum */
			unsigned int checksum_ram = chksum_crc32((unsigned char *) dst, size);
			printf("Checksum RAM: 0x%x\r\n", checksum_ram);

			close(fd);

			checksum_ram = csp_hton32(checksum_ram);
			memcpy(packet->data, &checksum_ram, sizeof(uint32_t));
			packet->length = 4;

			if (!csp_send(conn, packet, 0))
				csp_buffer_free(packet);

			break;
		}
#endif // OBC_ROM
#endif // ENABLE_UFFS

		case OBC_PORT_JUMP: {

			uint32_t addr;
			memcpy(&addr, packet->data, 4);
			addr = csp_ntoh32(addr);

			/* Check for valid address */
			if ((addr < 0x50000000) || (addr > 0x50200000)) {
				printf("Invalid jump addr %p\r\n", (void *) addr);
				csp_buffer_free(packet);
				break;
			}

			/* Jump to address */
			printf("Jumping to addr %p\r\n", (void *) addr);

			void (*jump) (void) = (void *) addr;
			jump();

			break;
		}

		case OBC_PORT_RAM_TO_ROM: {

			unsigned int checksum_ram, checksum_rom;

			obc_ram_to_rom_t rxbuf;
			memcpy(&rxbuf, packet->data, sizeof(obc_ram_to_rom_t));
			rxbuf.size = csp_ntoh32(rxbuf.size);
			rxbuf.checksum = csp_ntoh32(rxbuf.checksum);
			rxbuf.src = csp_ntoh32(rxbuf.src);
			rxbuf.dst = csp_ntoh32(rxbuf.dst);

			/* No need for the packet anymore */
			csp_buffer_free(packet);

			printf("Checksum IMG: %#"PRIx32", Size %"PRIu32", src %p, dst %p\r\n", rxbuf.checksum, rxbuf.size, (void *) rxbuf.src, (void *) rxbuf.dst);

			checksum_ram = chksum_crc32((unsigned char *)rxbuf.src, rxbuf.size);
			printf("Checksum RAM: %#x\r\n", checksum_ram);

			/* Validate RAM image */
			if (checksum_ram != rxbuf.checksum) {
				printf("Failed to validate image in RAM. Aborting\r\n");
				break;
			}

			/* Init flash */
			if (flash_init() != E_NO_ERR) {
				printf("Failed to init flash\r\n");
				break;
			}

			/* Write RAM image to ROM */
			int ret = flash_program((void *) rxbuf.dst, (void *) rxbuf.src, rxbuf.size, 1);
			if (ret != E_NO_ERR) {
				printf("Failed to copy image from RAM to ROM (err: %d)\r\n", ret);
				break;
			}

			/* Validate ROM image */
			checksum_rom = flash_checksum((unsigned char *)rxbuf.dst, rxbuf.size);
			printf("Checksum ROM: %#x\r\n", checksum_rom);
			if (checksum_rom != rxbuf.checksum) {
				/* Oh crap... */
				printf("Failed to validate image in ROM. Aborting\r\n");
				break;
			}

			/* Everything went well */
			printf("Successfully copied image from RAM to ROM\r\n");
			break;
		}

		default:
			/* If no port has matched yet, pass the call to CSP service handler */
			csp_service_handler(conn, packet);
			return;

		}

	}

}

void vTaskWorker(void * conn_param) {
	vWork((csp_conn_t *) conn_param, portMAX_DELAY);
	csp_close((csp_conn_t *) conn_param);
	vTaskDelete(NULL);
}

void vTaskServer(void * pvParameters) {

	csp_conn_t * conn, * socket = csp_socket(0);
	csp_listen(socket, 10);
	csp_bind(socket, CSP_ANY);

#if ENABLE_SUPERVISOR
	/* Get supervisor ID */
	int sv = sv_add("Server", 60000);
	if (sv < 0)
		printf("Failed to get supervisor ID!\r\n");
#endif

	while (1) {

		/* Wait for incoming connection, or timeout */
		conn = csp_accept(socket, 0.2 * configTICK_RATE_HZ);

#if ENABLE_SUPERVISOR
		/* Reset supervisor */
		sv_reset(sv);
#endif

		if (conn == NULL)
			continue;

		/* Spawn new task for SW upload */
		if (csp_conn_dport(conn) == OBC_PORT_FTP) {
			extern void task_ftp(void * conn_param);
			if (xTaskCreate(task_ftp, (signed char *) "FTP", 1024*4, conn, 2, NULL) != pdTRUE) {
				printf("Failed to allocate memory for RAM upload task\r\n");
				csp_close(conn);
			}
			continue;
		}

		/* Spawn new task for RDP */
		if ((csp_conn_flags(conn) & CSP_FRDP) ||
			(csp_conn_dport(conn) == OBC_PORT_LOAD_IMG) ||
			(csp_conn_dport(conn) == OBC_PORT_RAM_TO_ROM)) {
			if (xTaskCreate(vTaskWorker, (signed char *) "WORKER", 1024*4, conn, 2, NULL) != pdTRUE) {
				printf("Failed to allocate memory for WORKER task\r\n");
				csp_close(conn);
			}
			continue;
		}

		/* Otherwise execute work directly in server task! */
		vWork(conn, 0);

		/* Close connection when no more data */
		csp_close(conn);

	}

}
