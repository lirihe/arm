#include <string.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>

#include <util/crc32.h>
#include <util/log.h>
#include <lzo/lzoutils.h>
#include <lzo/minilzo.h>

#include <csp/csp_endian.h>

#include <ftp/ftp_server.h>

#define UPLOAD_ADDR 0x50000000
#define UPLOAD_SIZE 0x00100000

static uint32_t file_mem_addr;
static uint32_t file_size;
static uint32_t file_chunk_size;
static uint32_t file_chunks;

static unsigned char * ram_map = NULL;

ftp_return_t ftp_ram_get_crc(void * state, uint32_t * crc_arg) {

	if (!crc_arg)
		return FTP_RET_INVAL;

	/* Calculate CRC32 */
	unsigned int i;

	uint32_t crc = 0xFFFFFFFF;

	unsigned char * start = (unsigned char *) file_mem_addr;

	for (i = 0; i < file_size; i++)
		crc = chksum_crc32_step(crc, *(start + i));
	crc = (crc ^ 0xFFFFFFFF);
	*crc_arg = crc;

	return FTP_RET_OK;

}

ftp_return_t ftp_ram_upload(void * state, const char const * path, uint32_t memaddr, uint32_t size, uint32_t chunk_size) {

	file_mem_addr = memaddr;
	file_size = size;
	file_chunk_size = chunk_size;
	file_chunks = (size + chunk_size - 1) / chunk_size;

	/* Allocate map */
	ram_map = pvPortMalloc(file_chunks);
	if (ram_map == NULL)
		return FTP_RET_NOSPC;

	memset(ram_map, 0, file_chunks);

	return FTP_RET_OK;

}

ftp_return_t ftp_ram_download(void * state, const char const * path, uint32_t memaddr, uint32_t memsize, uint32_t chunk_size, uint32_t * size, uint32_t * crc) {

	file_mem_addr = memaddr;
	file_size = memsize;
	file_chunk_size = chunk_size;
	file_chunks = (memsize + chunk_size - 1) / chunk_size;

	*size = memsize;

	/* Calculate CRC */
	return ftp_ram_get_crc(state, crc);

}

ftp_return_t ftp_ram_write_chunk(void * state, uint32_t chunk, uint8_t * src, uint32_t size) {

	/* Write to mem */
	void * dst = (void *) (file_mem_addr + chunk * file_chunk_size);

	memcpy(dst, src, size);

	return FTP_RET_OK;

}

ftp_return_t ftp_ram_read_chunk(void * state, uint32_t chunk, uint8_t * dst, uint32_t size) {

	/* Read from mem */
	void * src = (void *) (file_mem_addr + chunk * file_chunk_size);

	memcpy(dst, src, size);

	return FTP_RET_OK;

}

ftp_return_t ftp_ram_get_status(void * state, uint32_t chunk, int * status) {

	if (status) {
		*status = ram_map[chunk];
		return FTP_RET_OK;
	} else {
		return FTP_RET_INVAL;
	}

}

ftp_return_t ftp_ram_set_status(void * state, uint32_t chunk) {

	/* Write to map */
	ram_map[chunk] = 1;

	return FTP_RET_OK;

}

ftp_return_t ftp_ram_abort(void * state) {

	vPortFree(ram_map);
	ram_map = NULL;
	return FTP_RET_OK;

}

ftp_return_t ftp_ram_done(void * state) {

#ifdef __arm__
	if (lzo_match_magic((void *)file_mem_addr)) {
		if (lzo_decompress_buffer((void *)file_mem_addr, UPLOAD_SIZE, (void *)(UPLOAD_ADDR), UPLOAD_SIZE, NULL) != 0) {
			printf("Unpacking LZO compressed RAM image failed\r\n");
			ftp_ram_abort(NULL);
			return FTP_RET_IO;
		}
	}
#endif

	return ftp_ram_abort(NULL);

}

ftp_backend_t backend_ram = {
	.upload 		= ftp_ram_upload,
	.download		= ftp_ram_download,
	.chunk_write	= ftp_ram_write_chunk,
	.chunk_read 	= ftp_ram_read_chunk,
	.status_get 	= ftp_ram_get_status,
	.status_set 	= ftp_ram_set_status,
	.crc 			= ftp_ram_get_crc,
	.abort 			= ftp_ram_abort,
	.done 			= ftp_ram_done,
	.timeout		= ftp_ram_abort,
};
