/**
 * @file ftp_client.c
 * FTP Client API for X86
 *
 * @author Jeppe Ledet-Pedersen & Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <io/nanomind.h>
#include <ftp/ftp_types.h>

#include <util/crc32.h>
#include <util/color_printf.h>
#include <util/bytesize.h>
#include <util/log.h>

#include <ftp/ftp_client.h>

#define FTP_TIMEOUT 90000

/* Chunk status markers */
static const char const * packet_missing = "-";
static const char const * packet_ok = "+";

static FILE * fp, * fp_map;
static csp_conn_t * conn = NULL;

static int ftp_chunk_size = 100;
static uint32_t ftp_file_size = 0;
static uint32_t ftp_chunks = 0;
static uint32_t ftp_checksum = 0xABCD0123;
char ftp_file_name[FTP_PATH_LENGTH];

static ftp_status_element_t last_status[FTP_STATUS_CHUNKS];
static uint16_t last_entries = 0;
static ftp_progress_handler progress_handler = NULL;
static void* progress_handler_data = NULL;

static double timespec_diff(struct timespec * start, struct timespec * end) {
	struct timespec temp;
	if ((end->tv_nsec - start->tv_nsec) < 0) {
		temp.tv_sec = end->tv_sec-start->tv_sec - 1;
		temp.tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
	} else {
		temp.tv_sec = end->tv_sec - start->tv_sec;
		temp.tv_nsec = end->tv_nsec - start->tv_nsec;
	}
	return (double)(temp.tv_sec + (double)temp.tv_nsec/1000000000);
}

static int ftp_perror(ftp_return_t ret) {
	switch (ret) {
	case FTP_RET_OK:
		color_printf(COLOR_GREEN, "No error\r\n");
		break;
	case FTP_RET_PROTO:
		color_printf(COLOR_RED, "Protocol error\r\n");
		break;
	case FTP_RET_NOENT:
		color_printf(COLOR_YELLOW, "No such file or directory\r\n");
		break;
	case FTP_RET_INVAL:
		color_printf(COLOR_YELLOW, "Invalid argument\r\n");
		break;
	case FTP_RET_NOSPC:
		color_printf(COLOR_RED, "No space left on device\r\n");
		break;
	case FTP_RET_IO:
		color_printf(COLOR_RED, "IO error\r\n");
		break;
	case FTP_RET_FBIG:
		color_printf(COLOR_RED, "File to large\r\n");
		break;
	case FTP_RET_EXISTS:
		color_printf(COLOR_YELLOW, "File exists\r\n");
		break;
	case FTP_RET_NOTSUP:
		color_printf(COLOR_RED, "Operation not supported\r\n");
		break;
	default:
		color_printf(COLOR_RED, "Unknown %d\r\n", ret);
		break;
	}
	return 0;
}

static char *sec_to_time(int total) {
	char *fmt;
	static char buf[20];
	int hour, min, sec;

	hour = total / 3600;
	total -= hour * 3600;

	min = total / 60;
	total -= min * 60;

	sec = total;
	if (hour) fmt = "%3$uh %2$um %1$us";
	else if (min) fmt =  "%2$um %1$us";
	else if (sec) fmt = "%1$us";
	else fmt = "";
	sprintf(buf, fmt, sec, min, hour);
	return buf;
}

void ftp_set_progress_handler(ftp_progress_handler handler, void *data)
{
	progress_handler = handler;
	progress_handler_data = data;
}

int ftp_upload(uint8_t host, uint8_t port, const char * path, uint8_t backend, int chunk_size, uint32_t addr, const char * remote_path, uint32_t * size, uint32_t * checksum) {

	int req_length, rep_length;
	ftp_packet_t req, rep;

	/* Open file and read size */
	fp = fopen(path, "rb");
	if (fp == NULL) {
		color_printf(COLOR_RED, "Failed to open file %s\r\n", path);
		return -1;
	}
	struct stat statbuf;
	stat(path, &statbuf);
	color_printf(COLOR_GREEN, "File size is %u\r\n", (unsigned int) statbuf.st_size);

	/* Calculate CRC32 */
	char byte;
	int i;
	fseek(fp, 0, SEEK_SET);
	uint32_t crc = 0xFFFFFFFF;
	for (i = 0; i < statbuf.st_size; i++) {
		if (!fread(&byte, 1, 1, fp))
			break;
		crc = chksum_crc32_step(crc, byte);
	}
	crc = (crc ^ 0xFFFFFFFF);
	ftp_checksum = crc;
	color_printf(COLOR_GREEN, "Checksum is %#010"PRIx32"\r\n", crc);

	/* Copy checksum and length */
	if (checksum != NULL)
		*checksum = crc;
	if (size != NULL)
		*size = statbuf.st_size;

	ftp_chunk_size = chunk_size;
	ftp_file_size = (uint32_t) statbuf.st_size;
	ftp_chunks = (ftp_file_size + ftp_chunk_size - 1) / ftp_chunk_size;
	strncpy(ftp_file_name, path, FTP_PATH_LENGTH);

	/* Assemble upload request */
	req.type = FTP_UPLOAD_REQUEST;
	req.up.chunk_size = csp_hton16(ftp_chunk_size);
	req.up.size = csp_hton32(ftp_file_size);
	req.up.crc32 = csp_hton32(crc);
	req.up.mem_addr = csp_hton32(addr);
	req.up.backend = backend;
	strncpy(req.up.path, remote_path, FTP_PATH_LENGTH);

	req_length = sizeof(ftp_type_t) + sizeof(ftp_upload_request_t);
	rep_length = sizeof(ftp_type_t) + sizeof(ftp_upload_reply_t);

	conn = csp_connect(CSP_PRIO_NORM, host, port, FTP_TIMEOUT, CSP_O_RDP);
	if (conn == NULL)
		return -1;

	if (csp_transaction_persistent(conn, FTP_TIMEOUT, &req, req_length, &rep, rep_length) != rep_length) {
		color_printf(COLOR_RED, "No reply to upload request received\r\n");
		return -1;
	}

	if (rep.type != FTP_UPLOAD_REPLY || rep.uprep.ret != FTP_RET_OK) {
		color_printf(COLOR_RED, "Reply was not UPLOAD_REPLY\r\n");
		return -1;
	}

	return 0;

}

int ftp_download(uint8_t host, uint8_t port, const char * path, uint8_t backend, int chunk_size, uint32_t memaddr, uint32_t memsize, const char * remote_path, uint32_t * size) {

	int req_length, rep_length;
	ftp_packet_t req, rep;

	ftp_chunk_size = chunk_size;

	req.type = FTP_DOWNLOAD_REQUEST;
	req.down.chunk_size = csp_hton16(ftp_chunk_size);
	req.down.mem_addr = csp_hton32(memaddr);
	req.down.mem_size = csp_hton32(memsize);
	req.down.backend = backend;

	if (remote_path != NULL)
		strncpy(req.down.path, remote_path, FTP_PATH_LENGTH);
	else
		memset(req.down.path, 0, FTP_PATH_LENGTH);

	req.down.path[FTP_PATH_LENGTH - 1] = '\0';
	strncpy(ftp_file_name, path, FTP_PATH_LENGTH);
	ftp_file_name[FTP_PATH_LENGTH - 1] = '\0';

	req_length = sizeof(ftp_type_t) + sizeof(ftp_download_request_t);
	rep_length = sizeof(ftp_type_t) + sizeof(ftp_download_reply_t);

	conn = csp_connect(CSP_PRIO_NORM, host, port, FTP_TIMEOUT, CSP_O_RDP);
	if (conn == NULL)
		return -1;

	if (csp_transaction_persistent(conn, FTP_TIMEOUT, &req, req_length, &rep, rep_length) != rep_length) {
		color_printf(COLOR_RED, "Length mismatch\r\n");
		return -1;
	}

	if (rep.type != FTP_DOWNLOAD_REPLY || rep.downrep.ret != FTP_RET_OK) {
		ftp_perror(rep.downrep.ret);
		return -1;
	}

	ftp_file_size = csp_ntoh32(rep.downrep.size);
	ftp_checksum = csp_ntoh32(rep.downrep.crc32);
	ftp_chunks = (ftp_file_size + ftp_chunk_size - 1) / ftp_chunk_size;
	*size = ftp_file_size;

	color_printf(COLOR_GREEN, "File size is %"PRIu32"\r\n", ftp_file_size);
	color_printf(COLOR_GREEN, "Checksum is %#010"PRIx32"\r\n", ftp_checksum);

	/* Map file name */
	char map[100];

	/* Try to open file */
	fp = fopen((const char *) path, "r+");
	if (fp == NULL) {
		/* Create new file */
		fp = fopen((const char *) path, "w+");
		if (fp == NULL) {
			color_printf(COLOR_RED, "Client: Failed to create data file\r\n");
			return -1;
		}
	}

	/* Try to create a new bitmap */
	sprintf(map, "%s.map", path);

	/* Check if file already exists */
	fp_map = fopen(map, "r+");
	if (fp_map == NULL) {
		int i;

		/* Create new file */
		fp_map = fopen(map, "w+");
		if (fp_map == NULL) {
			color_printf(COLOR_RED, "Failed to create bitmap\r\n");
			fclose(fp);
			return FTP_RET_IO;
		}

		/* Clear contents */
		for (i = 0; i < ftp_chunks; i++) {
			if (fwrite(packet_missing, 1, 1, fp_map) < 1) {
				color_printf(COLOR_RED, "Failed to clear bitmap\r\n");
				fclose(fp_map);
				fclose(fp);
				return -1;
			}
		}

		fflush(fp_map);
		fsync(fileno(fp_map));
	}

	return 0;

}

int ftp_status_request(void) {

	/* Request */
	int req_length;
	ftp_packet_t req, rep;
	req.type = FTP_STATUS_REQUEST;

	req_length = sizeof(ftp_type_t);

	/* Transaction */
	if (csp_transaction_persistent(conn, FTP_TIMEOUT, &req, req_length, &rep, -1) == 0) {
		color_printf(COLOR_RED, "Failed to receive status reply\r\n");
		return -1;
	}

	if (rep.type != FTP_STATUS_REPLY || rep.statusrep.ret != FTP_RET_OK) {
		color_printf(COLOR_RED, "Reply was not STATUS_REPLY\r\n");
		return -1;
	}

	rep.statusrep.complete = csp_ntoh32(rep.statusrep.complete);
	rep.statusrep.total = csp_ntoh32(rep.statusrep.total);
	rep.statusrep.entries = csp_ntoh16(rep.statusrep.entries);

	color_printf(COLOR_BLUE, "\r\nTransfer Status: ");
	color_printf(COLOR_BLUE, "%u of %u (%.2f%%)\r\n",
			rep.statusrep.complete, rep.statusrep.total,
			(double) rep.statusrep.complete * 100 / (double) rep.statusrep.total);

	if (rep.statusrep.complete != rep.statusrep.total) {
		int i;
		for (i = 0; i < rep.statusrep.entries; i++) {
			rep.statusrep.entry[i].next = csp_ntoh32(rep.statusrep.entry[i].next);
			rep.statusrep.entry[i].count = csp_ntoh16(rep.statusrep.entry[i].count);
			last_status[i].next = rep.statusrep.entry[i].next;
			last_status[i].count = rep.statusrep.entry[i].count;
		}

		last_entries = rep.statusrep.entries;
	} else {
		last_entries = 0;
	}

	return 0;

}

int ftp_status_reply(void) {

	double sec = 0.0, bps = 0.0;
	struct timespec now = {0,0}, last = {0,0};
	uint32_t last_chunk = 0;

	/* Clear line buffer */
	char cl[80];
	memset(cl, ' ', 80);
	cl[0] = '\r';
	cl[79] = '\0';

	ftp_packet_t ftp_packet;
	ftp_status_reply_t * status = (ftp_status_reply_t *) &ftp_packet.statusrep;
	ftp_packet.type = FTP_STATUS_REPLY;

	/* Build status reply */
	int i = 0, next = 0, count = 0;

	status->entries = 0;
	status->complete = 0;
	for (i = 0; i < ftp_chunks; i++) {
		int s;
		char cstat;

		/* Read chunk status */
		if (fseek(fp_map, i, SEEK_SET) != 0) {
			color_printf(COLOR_RED, "fseek failed\r\n");
			return -1;
		}
		if (fread(&cstat, 1, 1, fp_map) != 1) {
			color_printf(COLOR_RED, "fread byte %u failed\r\n", i);
			return -1;
		}

		s = (cstat == *packet_ok);

		/* Increase complete counter if chunk was received */
		if (s) status->complete++;

		/* Add chunk status to packet */
		if (status->entries < FTP_STATUS_CHUNKS) {
			if (!s) {
				if (!count)
					next = i;
				count++;
			}

			if (count > 0 && (s || i == ftp_chunks - 1)) {
				status->entry[status->entries].next = csp_hton32(next);
				status->entry[status->entries].count = csp_hton16(count);
				status->entries++;
			}
		}
	}

	status->entries = csp_hton16(status->entries);
	status->complete = csp_hton32(status->complete);
	status->total = csp_hton32(ftp_chunks);
	status->ret = FTP_RET_OK;

	/* Send reply */
	if (csp_transaction_persistent(conn, FTP_TIMEOUT, &ftp_packet, sizeof(ftp_type_t) + sizeof(ftp_status_reply_t), NULL, 0) != 1) {
		color_printf(COLOR_RED, "Failed to send status reply\r\n");
		return -1;
	}

	/* Read data */
	csp_packet_t * packet;
	while (1) {
		packet = csp_read(conn, FTP_TIMEOUT);

		if (!packet) {
			color_printf(COLOR_RED, "Timeout while waiting for data\r\n");
			return -1;
		}

		ftp_packet_t * ftp_packet = (ftp_packet_t *) &packet->data;

		ftp_packet->data.chunk = csp_ntoh32(ftp_packet->data.chunk);
		unsigned int size;

		if (ftp_packet->data.chunk >= ftp_chunks) {
			color_printf(COLOR_RED, "Bad chunk number %u > %u\r\n", ftp_packet->data.chunk, ftp_chunks);
			csp_buffer_free(packet);
			continue;
		}

		if (ftp_packet->data.chunk == ftp_chunks - 1) {
			size = ftp_file_size % ftp_chunk_size;
			if (size == 0)
				size = ftp_chunk_size;
		} else {
			size = ftp_chunk_size;
		}

		if (fseek(fp, ftp_packet->data.chunk * ftp_chunk_size, SEEK_SET) != 0) {
			color_printf(COLOR_RED, "Seek error\r\n");
			csp_buffer_free(packet);
			return -1;
		}
		if (fwrite(ftp_packet->data.bytes, 1, size, fp) != size) {
			color_printf(COLOR_RED, "Write error\r\n");
			csp_buffer_free(packet);
			return -1;
		}

		if (fseek(fp_map, ftp_packet->data.chunk, SEEK_SET) != 0) {
			color_printf(COLOR_RED, "Map Seek error\r\n");
			return -1;
		}

		if (fwrite(packet_ok, 1, 1, fp_map) != 1) {
			color_printf(COLOR_RED, "Map write error\r\n");
			return -1;
		}

		/* Print progress bar */
		if (!(ftp_packet->data.chunk % 25) || ftp_packet->data.chunk == ftp_chunks - 1) {
			clock_gettime(CLOCK_MONOTONIC, &now);

			if (last.tv_sec > 0) {
				sec = timespec_diff(&last, &now);
				bps = (((ftp_packet->data.chunk - last_chunk) * ftp_chunk_size)/sec)*0.3 + bps*0.7;
			} else {
				bps = 1;
			}

			if (progress_handler) {
				progress_handler(ftp_packet->data.chunk, ftp_chunks - 1, bps,
					progress_handler_data);
			}

			double p = ((double)ftp_packet->data.chunk / (double)(ftp_chunks - 1));
			int k, l = (int)(40 * p);
			/* Clear line */
			color_printf(COLOR_BLUE, "%s", cl);
			color_printf(COLOR_BLUE, "\r%5.1f%% [", p * 100.0);
			for (k = 0; k < l; k++)
				color_printf(COLOR_BLUE, "#");
			for (k = 0; k < 40 - l; k++)
				color_printf(COLOR_BLUE, " ");
			color_printf(COLOR_BLUE, "] %4.1f kB/s eta %s",  bps/1024, sec_to_time(((ftp_chunks - ftp_packet->data.chunk)*ftp_chunk_size)/(int)bps+1));
			fflush(stdout);

			/* Update last time */
			last.tv_sec = now.tv_sec;
			last.tv_nsec = now.tv_nsec;
			last_chunk = ftp_packet->data.chunk;
		}

		/* Free buffer element */
		csp_buffer_free(packet);

		/* Break if all packets were received */
		if (ftp_packet->data.chunk == ftp_chunks - 1)
			break;
	}

	color_printf(COLOR_NONE, "\r\n");

	/* Sync file to disk */
	fflush(fp);
	fsync(fileno(fp));

	return 0;
}

int ftp_data(int count) {

	int i, j;
	double sec = 0.0, bps = 0.0;
	struct timespec now = {0,0}, last = {0,0};
	uint32_t last_chunk = 0;
	int ret;

	/* Clear line buffer */
	char cl[80];
	memset(cl, ' ', 80);
	cl[0] = '\r';
	cl[79] = '\0';

	/* Request */
	ftp_packet_t packet;
	packet.type = FTP_DATA;
	for (i = 0; i < last_entries; i++) {
		ftp_status_element_t * n = &last_status[i];

		for (j = 0; j < n->count; j++) {
			/* Calculate chunk number */
			packet.data.chunk = n->next + j;

			/* Print progress bar */
			if (!(packet.data.chunk % 25) || packet.data.chunk == ftp_chunks - 1) {
				clock_gettime(CLOCK_MONOTONIC, &now);

				if (last.tv_sec > 0) {
					sec = timespec_diff(&last, &now);
					if (sec==0) sec = 1;
					bps = (((packet.data.chunk - last_chunk) * ftp_chunk_size)/sec)*0.3 + bps*0.7;
				} else {
					bps = 1;
				}

				if (progress_handler) {
					progress_handler(packet.data.chunk, ftp_chunks - 1, bps,
						progress_handler_data);
				}

				double p = ((double)packet.data.chunk / (double)(ftp_chunks - 1));
				int k, l = (int)(40 * p);
				/* Clear line */
				color_printf(COLOR_BLUE, "%s", cl);
				color_printf(COLOR_BLUE, "\r%5.1f%% [", p * 100.0);
				for (k = 0; k < l; k++)
					color_printf(COLOR_BLUE, "#");
				for (k = 0; k < 40 - l; k++)
					color_printf(COLOR_BLUE, " ");
				color_printf(COLOR_BLUE, "] %4.1f kB/s eta %s",  bps/1024.0, sec_to_time(((ftp_chunks - packet.data.chunk)*ftp_chunk_size)/bps+1));
				fflush(stdout);


				/* Update last time */
				last.tv_sec = now.tv_sec;
				last.tv_nsec = now.tv_nsec;
				last_chunk = packet.data.chunk;
			}

			/* Read chunk */
			fseek(fp, packet.data.chunk * ftp_chunk_size, SEEK_SET);
			ret = fread(packet.data.bytes, ftp_chunk_size, 1, fp);
			if (ret < 0) {
				if (!feof(fp))
					break;
			}

			/* Chunk number MUST be little-endian!
			 * Note: Yes, this is due to an old mistake, and now we are stuck with it! :( */
			packet.data.chunk = htole32(packet.data.chunk);

			/* Send data */
			int length = sizeof(ftp_type_t) + sizeof(uint32_t) + ftp_chunk_size;
			if (csp_transaction_persistent(conn, FTP_TIMEOUT, &packet, length, NULL, 0) != 1) {
				color_printf(COLOR_RED, "Data transaction failed\r\n");
				csp_close(conn);
				break;
			}
		}

		color_printf(COLOR_NONE, "\r\n");

	}

	return 0;

}

int ftp_done(void) {

	/* Delete map file if it exits */
	char map[FTP_PATH_LENGTH];
	snprintf(map, FTP_PATH_LENGTH, "%s.map", ftp_file_name);
	if (access(map, F_OK) == 0)
		if (remove(map) != 0)
			color_printf(COLOR_RED, "Failed to remove %s\r\n", map);

	/* Request */
	ftp_packet_t packet;
	packet.type = FTP_DONE;

	csp_transaction_persistent(conn, FTP_TIMEOUT, &packet, sizeof(ftp_type_t), NULL, 0);
	csp_close(conn);
	conn = NULL;

	last_entries = 0;
	progress_handler = NULL;
	progress_handler_data = NULL;
	return 0;

}

int ftp_crc(void) {

	/* Request */
	ftp_packet_t packet;
	packet.type = FTP_CRC_REQUEST;

	/* Reply */
	int repsiz = sizeof(ftp_type_t) + sizeof(ftp_crc_reply_t);
	if (csp_transaction_persistent(conn, FTP_TIMEOUT, &packet, sizeof(ftp_type_t), &packet, repsiz) != repsiz)
		return -1;
	if (packet.type != FTP_CRC_REPLY)
		return -1;
	if (packet.crcrep.ret != FTP_RET_OK) {
		ftp_perror(packet.crcrep.ret);
		return -1;
	}

	/* Calculate CRC32 */
	char byte;
	int i;
	fseek(fp, 0, SEEK_SET);
	uint32_t crc = 0xFFFFFFFF;
	for (i = 0; i < ftp_file_size; i++) {
		if (!fread(&byte, 1, 1, fp))
			break;
		crc = chksum_crc32_step(crc, byte);
	}
	crc = (crc ^ 0xFFFFFFFF);
	ftp_checksum = crc;

	/* Validate checksum */
	packet.crcrep.crc = csp_ntoh32(packet.crcrep.crc);
	color_printf(COLOR_GREEN, "CRC Remote: 0x%"PRIx32", Local: 0x%"PRIx32"\r\n", packet.crcrep.crc, ftp_checksum);
	if (packet.crcrep.crc != ftp_checksum) {
		printf("CRC Failed!\r\n");
		return -1;
	}

	return 0;

}

int ftp_move(uint8_t host, uint8_t port, uint8_t backend, char * from, char * to) {

	/* Request */
	ftp_packet_t packet;
	packet.type = FTP_MOVE_REQUEST;
	strncpy(packet.move.from, from, FTP_PATH_LENGTH);
	packet.move.from[FTP_PATH_LENGTH-1] = '\0';
	strncpy(packet.move.to, to, FTP_PATH_LENGTH);
	packet.move.to[FTP_PATH_LENGTH-1] = '\0';
	packet.move.backend = backend;

	int reqsiz = sizeof(ftp_type_t) + sizeof(ftp_move_request_t);
	int repsiz = sizeof(ftp_type_t) + sizeof(ftp_move_reply_t);

	if (csp_transaction(CSP_PRIO_NORM, host, port,
			FTP_TIMEOUT, &packet, reqsiz, &packet, repsiz) != repsiz)
		return -1;
	if (packet.type != FTP_MOVE_REPLY)
		return -1;
	if (packet.moverep.ret != FTP_RET_OK) {
		ftp_perror(packet.moverep.ret);
		return -1;
	}

	return 0;

}

int ftp_remove(uint8_t host, uint8_t port, uint8_t backend, char * path) {

	/* Request */
	ftp_packet_t packet;
	packet.type = FTP_REMOVE_REQUEST;
	strncpy(packet.remove.path, path, FTP_PATH_LENGTH);
	packet.remove.path[FTP_PATH_LENGTH-1] = '\0';
	packet.remove.backend = backend;

	int reqsiz = sizeof(ftp_type_t) + sizeof(ftp_remove_request_t);
	int repsiz = sizeof(ftp_type_t) + sizeof(ftp_remove_reply_t);

	if (csp_transaction(CSP_PRIO_NORM, host, port,
			FTP_TIMEOUT, &packet, reqsiz, &packet, repsiz) != repsiz)
		return -1;
	if (packet.type != FTP_REMOVE_REPLY)
		return -1;
	if (packet.removerep.ret != FTP_RET_OK) {
		ftp_perror(packet.removerep.ret);
		return -1;
	}

	return 0;

}

int ftp_list(uint8_t host, uint8_t port, uint8_t backend, char * path) {

	/* Request */
	int entries;
	csp_conn_t * c;
	csp_packet_t * p;
	ftp_packet_t packet;
	packet.type = FTP_LIST_REQUEST;
	packet.list.backend = backend;
	strncpy(packet.list.path, path, FTP_PATH_LENGTH);
	packet.list.path[FTP_PATH_LENGTH-1] = '\0';

	int reqsiz = sizeof(ftp_type_t) + sizeof(ftp_list_request_t);
	int repsiz = sizeof(ftp_type_t) + sizeof(ftp_list_reply_t);

	c = csp_connect(CSP_PRIO_NORM, host, port, FTP_TIMEOUT, CSP_O_RDP);
	if (c == NULL)
		return -1;
	if (csp_transaction_persistent(c, FTP_TIMEOUT, &packet, reqsiz, &packet, repsiz) != repsiz)
		return -1;
	if (packet.type != FTP_LIST_REPLY)
		return -1;
	if (packet.listrep.ret != FTP_RET_OK) {
		ftp_perror(packet.listrep.ret);
		return -1;
	}

	entries = csp_ntoh16(packet.listrep.entries);

	/* Read entries */
	ftp_packet_t * f;
	while (entries && ((p = csp_read(c, FTP_TIMEOUT)) != NULL)) {
		f = (ftp_packet_t *) &(p->data);
		uint16_t ent = csp_ntoh16(f->listent.entry);
		uint32_t size = csp_ntoh32(f->listent.size);

		char bytebuf[25];
		bytesize(bytebuf, 25, size);

		color_printf(COLOR_GREEN, "%6s %s%s\r\n", bytebuf, f->listent.path, f->listent.type ? "/" : "");

		csp_buffer_free(p);
		if (ent == entries - 1)
			break;
	}

	csp_close(c);

	return 0;

}
