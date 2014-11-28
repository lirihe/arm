#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <util/crc32.h>
#include <util/log.h>

#include <ftp/ftp_server.h>

#define FTP_MAX_BACKENDS 5

struct backend_container {
	uint8_t id;					//! Backend ID
	ftp_backend_t *backend;		//! Backend operations
} backends[FTP_MAX_BACKENDS] = {{0}};
int backends_count = 0;

int ftp_register_backend(uint8_t id, ftp_backend_t *backend) {

	if (backends_count < FTP_MAX_BACKENDS) {
		backends[backends_count].id = id;
		backends[backends_count].backend = backend;
		backends_count++;
		return 0;
	} else {
		printf("No remaining backend\r\n");
		return -1;
	}

}

ftp_backend_t * ftp_get_backend(uint8_t backend) {

	int i;
	ftp_backend_t * ret = NULL;

	for (i = 0; i < backends_count; i++) {
		if (backends[i].id == backend) {
			ret = backends[i].backend;
			break;
		}
	}

	return ret;
}

void task_ftp(void * conn_param) {

	/* Delete task if passed an invalid connection */
	if (!conn_param) {
		printf("NULL connection passed to FTP worker task\r\n");
		vTaskDelete(NULL);
	}

	/* Cast input parameter */
	csp_conn_t * conn = conn_param;
	csp_packet_t * packet = NULL;

	ftp_backend_t * backend = NULL;

	ftp_state_t state = FTP_STATE_IDLE;
	ftp_upload_request_t ul;
	ftp_download_request_t dl;

	uint32_t ftp_size;
	uint32_t ftp_checksum;

	void * backend_state = NULL;

	while(1) {

		/* Read packet from stream */
		packet = csp_read(conn, 60 * configTICK_RATE_HZ);
		if (!packet) {
			printf("Timeout during transfer, aborting\r\n");
			if (backend && backend->timeout)
				backend->timeout(backend_state);
			goto out;
		}

		/* Switch on packet type */
		ftp_packet_t * ftp_packet = (ftp_packet_t *) packet->data;

		switch(ftp_packet->type) {

		case FTP_UPLOAD_REQUEST: {
			/* Validate state */
			if (state != FTP_STATE_IDLE) {
				printf("Upload request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			state = FTP_STATE_UPLOAD;

			/* Get file information, and store it in local structure */
			ftp_upload_request_t * upload = (ftp_upload_request_t *) &ftp_packet->up;
			upload->size = csp_ntoh32(upload->size);
			upload->crc32 = csp_ntoh32(upload->crc32);
			upload->chunk_size = csp_ntoh16(upload->chunk_size);
			upload->mem_addr = csp_ntoh32(upload->mem_addr);
			memcpy(&ul, upload, sizeof(ftp_upload_request_t));
			printf("Upload begin: size %"PRIu32", path %s, type %"PRIu8", addr 0x%"PRIX32" chunk size %u\r\n",
													ul.size, ul.path, ul.backend, ul.mem_addr, ul.chunk_size);

			backend = ftp_get_backend(ul.backend);
			if (backend == NULL) {
				printf("No backend available for %u\r\n", ul.backend);
				goto out_free;
			}

			/* Initialize backend state */
			if (backend->init) {
				if (backend->init(&backend_state) != FTP_RET_OK) {
					printf("Could not init backend\r\n");
					goto out_free;
				}
			}

			ftp_packet->type = FTP_UPLOAD_REPLY;
			if (backend->upload) {
				ftp_packet->uprep.ret = backend->upload(backend_state,
						ul.path, ul.mem_addr, ul.size, ul.chunk_size);
			} else {
				ftp_packet->uprep.ret = FTP_RET_NOTSUP;
			}

			packet->length = sizeof(ftp_type_t) + sizeof(ftp_upload_reply_t);

			if (!csp_send(conn, packet, 0))
				goto out_free;

			break;
		}

		case FTP_DOWNLOAD_REQUEST: {
			/* Validate state */
			if (state != FTP_STATE_IDLE) {
				printf("Download request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			state = FTP_STATE_DOWNLOAD;

			/* Get file information, and store it in local structure */
			ftp_download_request_t * download = (ftp_download_request_t *) &ftp_packet->down;
			download->chunk_size = csp_ntoh16(download->chunk_size);
			download->mem_addr = csp_ntoh32(download->mem_addr);
			download->mem_size = csp_ntoh32(download->mem_size);
			memcpy(&dl, download, sizeof(ftp_download_request_t));
			printf("Download begin: path %s, type %"PRIu8", addr 0x%"PRIX32", size %"PRIu32", chunk size %u\r\n", dl.path, dl.backend, dl.mem_addr, dl.mem_size, dl.chunk_size);

			backend = ftp_get_backend(dl.backend);
			if (backend == NULL) {
				printf("No backend available for type %u\r\n", dl.backend);
				goto out_free;
			}

			/* Initialize backend state */
			if (backend->init) {
				if (backend->init(&backend_state) != FTP_RET_OK) {
					printf("Could not init backend\r\n");
					goto out_free;
				}
			}

			ftp_packet->type = FTP_DOWNLOAD_REPLY;

			if (backend->download) {
				ftp_packet->downrep.ret = backend->download(backend_state, dl.path, dl.mem_addr, dl.mem_size, dl.chunk_size, &ftp_size, &ftp_checksum);
				ftp_packet->downrep.size = csp_hton32(ftp_size);
				ftp_packet->downrep.crc32 = csp_hton32(ftp_checksum);
			} else {
				ftp_packet->downrep.ret = FTP_RET_NOTSUP;
			}

			packet->length = sizeof(ftp_type_t) + sizeof(ftp_download_reply_t);

			if (!csp_send(conn, packet, 0))
				goto out_free;

			break;
		}

		case FTP_DATA: {
			/* Validate state */
			if (state != FTP_STATE_UPLOAD) {
				printf("Data received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			if (backend == NULL)
				goto out_free;

			ftp_data_t * data = (ftp_data_t *) &ftp_packet->data;

			/* NOTE: The network traffic is in this case Little-Endian!
			 * Therefore csp_letoh32 is used instead of csp_ntoh32 */
			data->chunk = csp_letoh32(data->chunk);

			if (data->chunk >= (ul.size + ul.chunk_size - 1)/ ul.chunk_size)
				goto out_free;

			unsigned int remain = ul.size - data->chunk * ul.chunk_size;
			unsigned int size = remain > ul.chunk_size ? ul.chunk_size : remain;

			if (!backend->chunk_write ||
					backend->chunk_write(backend_state, data->chunk, data->bytes, size) != FTP_RET_OK)
				goto out_free;

			if (!backend->status_set ||
					backend->status_set(backend_state, data->chunk) != FTP_RET_OK)
				goto out_free;

			csp_buffer_free(packet);
			break;
		}

		case FTP_STATUS_REQUEST: {
			/* Validate state */
			if (state != FTP_STATE_UPLOAD) {
				printf("Status request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			if (backend == NULL)
				goto out_free;

			ftp_status_reply_t * status = (ftp_status_reply_t *) &ftp_packet->statusrep;
			ftp_packet->type = FTP_STATUS_REPLY;

			if (backend->status_get) {
				/* Build status reply */
				int i = 0, next = 0, count = 0;
				int chunks = (ul.size + ul.chunk_size - 1) / ul.chunk_size;

				status->entries = 0;
				status->complete = 0;
				for (i = 0; i < chunks; i++) {
					int s;

					/* Read chunk status */
					status->ret = backend->status_get(backend_state, i, &s);
					if (status->ret != FTP_RET_OK) {
						printf("Status get failed\r\n");
						goto out_free;
					}

					/* Increase complete counter if chunk was received */
					if (s) status->complete++;

					/* Add chunk status to packet */
					if (status->entries < FTP_STATUS_CHUNKS) {
						if (!s) {
							if (!count) next = i;
							count++;
						}

						if (count > 0 && (s || i == chunks - 1)) {
							status->entry[status->entries].next = csp_hton32(next);
							status->entry[status->entries].count = csp_hton16(count);
							status->entries++;
						}
					}
				}

				status->entries = csp_hton16(status->entries);
				status->complete = csp_hton32(status->complete);
				status->total = csp_hton32(chunks);
				status->ret = FTP_RET_OK;
			} else {
				status->ret = FTP_RET_NOTSUP;
			}

			/* Send reply */
			packet->length = sizeof(ftp_type_t) + sizeof(ftp_status_reply_t);
			if (!csp_send(conn, packet, 100))
				goto out_free;

			break;
		}

		case FTP_STATUS_REPLY: {
			/* Validate state */
			if (state != FTP_STATE_DOWNLOAD) {
				printf("Status reply received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			if (backend == NULL)
				goto out_free;

			ftp_status_reply_t * status = (ftp_status_reply_t *) &ftp_packet->statusrep;

			status->entries = csp_ntoh16(status->entries);
			status->complete = csp_ntoh32(status->complete);
			status->total = csp_ntoh32(status->total);

			printf("\r\nTransfer Status: ");
			printf("%"PRIu32" of %"PRIu32" (%.2f%%)\r\n",
					status->complete, status->total,
					(double) status->complete * 100 / (double) status->total);

			if (status->complete != status->total) {
				int i, j;
				for (i = 0; i < status->entries; i++) {
					status->entry[i].next = csp_ntoh32(status->entry[i].next);
					status->entry[i].count = csp_ntoh16(status->entry[i].count);

					for (j = 0; j < status->entry[i].count; j++) {

						unsigned int remain = ftp_size - (status->entry[i].next + j) * dl.chunk_size;
						unsigned int size = remain > dl.chunk_size ? dl.chunk_size : remain;
						unsigned int length = sizeof(ftp_type_t) + sizeof(uint32_t) + size;

						csp_packet_t * data_packet = csp_buffer_get(length);
						if (data_packet == NULL)
							goto out_free;

						ftp_packet_t * ftp_data_packet = (void *) data_packet->data;
						ftp_data_packet->type = FTP_DATA;
						ftp_data_packet->data.chunk = status->entry[i].next + j;

						/* Read chunk */
						if (backend->chunk_read(backend_state, ftp_data_packet->data.chunk, ftp_data_packet->data.bytes, size) != FTP_RET_OK) {
							printf("Failed to read chunk\r\n");
							goto out_free;
						}

						/* Convert */
						ftp_data_packet->data.chunk = csp_hton32(ftp_data_packet->data.chunk);

						/* Send packet */
						data_packet->length = length;
						if (!csp_send(conn, data_packet, 60000)) {
							csp_buffer_free(data_packet);
							goto out_free;
						}

					}
				}
			}

			csp_buffer_free(packet);
			break;
		}

		case FTP_CRC_REQUEST: {
			/* Validate state */
			if ((state != FTP_STATE_UPLOAD) && (state != FTP_STATE_DOWNLOAD)) {
				printf("CRC request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			if (backend == NULL)
				goto out_free;

			ftp_crc_reply_t * crc = (ftp_crc_reply_t *) &ftp_packet->crcrep;

			/* Let the backend do the work */
			int ret = FTP_RET_NOTSUP;
			if (backend->crc) {
				uint32_t c;
				ret = backend->crc(backend_state, &c);
				crc->crc = csp_hton32(c);
			}

			/* Send back reply */
			ftp_packet->type = FTP_CRC_REPLY;
			ftp_packet->crcrep.ret = ret;
			packet->length = sizeof(ftp_type_t) + sizeof(ftp_crc_reply_t);

			if (!csp_send(conn, packet, 0))
				goto out_free;

			break;
		}

		case FTP_ABORT: {
			/* Validate state */
			if (state == FTP_STATE_IDLE) {
				printf("FTP ABORT received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			if (backend == NULL)
				goto out_free;

			/* Let the backend do the work */
			if (backend->abort)
				backend->abort(backend_state);

			goto out_free;
		}

		case FTP_DONE: {
			/* Validate state */
			if (state == FTP_STATE_IDLE) {
				printf("FTP DONE received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			if (backend == NULL)
				goto out_free;

			/* Let the backend do the work */
			if (backend->done)
				backend->done(backend_state);

			goto out_free;
		}

		case FTP_LIST_REQUEST: {
			/* Validate state */
			if (state != FTP_STATE_IDLE) {
				printf("List request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			uint16_t entry = 0;

			backend = ftp_get_backend(ftp_packet->list.backend);
			if (backend == NULL)
				goto out_free;

			if (backend->init) {
				if (backend->init(&backend_state) != FTP_RET_OK) {
					printf("Could not init backend\r\n");
					goto out_free;
				}
			}

			ftp_packet->type = FTP_LIST_REPLY;
			packet->length = sizeof(ftp_type_t) + sizeof(ftp_list_reply_t);

			if (backend && backend->list && backend->entry) {
				/* Read entry count */
				ftp_return_t ret = backend->list(backend_state, ftp_packet->list.path, &entry);
				ftp_packet->listrep.ret = ret;
				ftp_packet->listrep.entries = csp_hton16(entry);

				if (!csp_send(conn, packet, 0))
					goto out_free;

				if (ret == FTP_RET_OK) {
					/* Send entries */
					int i;
					for (i = 0; i < entry; i++) {

						csp_packet_t * entry = csp_buffer_get(sizeof(ftp_type_t) + sizeof(ftp_list_entry_t));
						if (entry == NULL)
							goto out;

						entry->length = sizeof(ftp_type_t) + sizeof(ftp_list_entry_t);
						ftp_packet_t * entry_packet = (void *) entry->data;
						entry_packet->type = FTP_LIST_ENTRY;

						/* Get next entry */
						if (backend->entry(backend_state, &entry_packet->listent) != FTP_RET_OK)
							goto out;

						/* Pack data */
						entry_packet->listent.entry = csp_hton16(i);
						entry_packet->listent.size = csp_hton32(entry_packet->listent.size);

						if (!csp_send(conn, entry, 30000)) {
							printf("Data transaction failed\r\n");
							csp_buffer_free(entry);
							goto out;
						}

					}

					/* We need to call entry once more to free the dirp */
					backend->entry(backend_state, NULL);
				}

			} else {
				ftp_packet->listrep.ret = FTP_RET_NOTSUP;

				if (!csp_send(conn, packet, 0))
					goto out_free;
			}

			goto out;
		}

		case FTP_MOVE_REQUEST: {
			/* Validate state */
			if (state != FTP_STATE_IDLE) {
				printf("Remove request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			backend = ftp_get_backend(ftp_packet->move.backend);

			if (backend && backend->move) {
				char * from = ftp_packet->move.from;
				char * to   = ftp_packet->move.to;
				ftp_packet->moverep.ret = backend->move(backend_state, from, to);
			} else {
				ftp_packet->moverep.ret = FTP_RET_NOTSUP;
			}

			/* Send back reply */
			ftp_packet->type = FTP_MOVE_REPLY;
			packet->length = sizeof(ftp_type_t) + sizeof(ftp_move_reply_t);

			if (!csp_send(conn, packet, 0))
				goto out_free;

			goto out;
		}

		case FTP_REMOVE_REQUEST: {
			/* Validate state */
			if (state != FTP_STATE_IDLE) {
				printf("Remove request received in state %"PRIu8"\r\n", state);
				goto out_free;
			}

			backend = ftp_get_backend(ftp_packet->remove.backend);

			if (backend && backend->remove) {
				ftp_packet->removerep.ret = backend->remove(backend_state, ftp_packet->remove.path);
			} else {
				ftp_packet->removerep.ret = FTP_RET_NOTSUP;
			}

			/* Send back reply */
			ftp_packet->type = FTP_REMOVE_REPLY;
			packet->length = sizeof(ftp_type_t) + sizeof(ftp_remove_reply_t);

			if (!csp_send(conn, packet, 0))
				goto out_free;

			goto out;
		}

		default:
			printf("FTP received invalid type %"PRIu8"\r\n", ftp_packet->type);
			goto out_free;
		}
	}

out_free:
	csp_buffer_free(packet);
out:
	csp_close(conn);
	if (backend && backend->timeout)
		backend->timeout(backend_state);
	if (backend_state && backend && backend->release)
		backend->release(backend_state);
	vTaskDelete(NULL);
}
