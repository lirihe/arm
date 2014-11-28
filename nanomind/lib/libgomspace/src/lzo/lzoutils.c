/* Utilities for LZO decompression */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>

#include <lzo/minilzo.h>
#include <lzo/lzoutils.h>
#include <lzo/lzoconf.h>
#include <util/log.h>

#ifdef FREERTOS_CONFIG
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

#include <util/byteorder.h>

/*
 * All fields are big endian byte order.

 * Magic: 9 bytes (0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a)
 * Version: 2 bytes
 * Lib version: 2 bytes
 * Version needed: 2 bytes
 * Method: 1 byte
 * Level: 1 byte
 * Flags: 4 byte
 * Filter: 4 bytes (only if flags & F_H_FILTER)
 * Mode: 4 bytes
 * Mtime: 4 bytes
 * GMTdiff: 4 bytes
 * File name length: 1 byte
 * Name: 0-255 bytes
 * Checksum, original data: 4 bytes (CRC32 if flags & F_H_CRC32 else Adler32)
 * Uncompressed size: 4 bytes
 * Compressed size: 4 bytes
 * Checksum, uncompressed data: 4 bytes
 * Checksum, compressed data: 4 bytes (only if flags & F_ADLER32_C or flags & F_CRC32_C)
 * Compressed data: 0-x bytes
 */

static const char const magic[] = {0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a};

int lzo_match_magic(void * add) {
	return !memcmp(add, magic, 9);
}

/* Test if path is a valid LZO image from lzop */
int lzo_is_lzop_image(char * path) {

	int ret = 0;
	char rmagic[9];

	FILE *fp = fopen(path, "r");
	if (!fp) {
		/* Failed to open file */
		goto out_open;
	} else if (fread(rmagic, 1, 9, fp) != 9) {
		/* Failed to read magic sequence */
		goto out;
	} else if (!lzo_match_magic(rmagic)) {
		/* Magic sequence did not match */
		goto out;
	}

	/* Magic found */
	ret = 1;
out:
	fclose(fp);
out_open:

	return ret;

}

int lzo_header(char * path, uint32_t * header_length, uint32_t * decomp_len, uint32_t * comp_len) {

	int ret = -1;
	uint32_t dlen, clen;
	uint8_t fname_len;

	if (!header_length || !decomp_len || !comp_len) {
		printf("length pointer was NULL\r\n");
		goto err_open;
	}

	/* Reset size counters */
	*decomp_len = 0;
	*comp_len = 0;

	/* Open file */
	FILE *fp = fopen(path, "r");
	if (!fp) {
		printf("lzo: cannot open %s\r\n", path);
		goto err_open;
	}

	/* Read file name length */
	if (fseek(fp, 33, SEEK_SET) != 0) {
		printf("Failed to seek file pos 33\r\n");
		goto err_read;
	} else {
		if (fread(&fname_len, 1, 1, fp) != 1) {
			printf("Failed to read file name length\r\n");
			goto err_read;
		}
	}

	/* Skip file name and checksum */
	if (fseek(fp, fname_len + 4, SEEK_CUR) != 0) {
		printf("Failed to skip file name\r\n");
		goto err_read;
	}

	/* Store header length */
	*header_length = 34 + fname_len + 16;

	/* Read block sizes */
	while (1) {
		if (fread(&dlen, 1, sizeof(dlen), fp) != sizeof(dlen)) {
			printf("Failed to read uncompressed file length\r\n");
			goto err_read;
		} else {
			dlen = util_ntohl(dlen);
			*decomp_len += dlen;
		}

		if (dlen == 0) {
			ret = 0;
			break;
		}

		if (fread(&clen, 1, sizeof(clen), fp) != sizeof(clen)) {
			printf("Failed to read compressed file length\r\n");
			goto err_read;
		} else {
			clen = util_ntohl(clen);
			*comp_len += clen;
		}

		/* Skip checksum and seek forward to next block */
		fseek(fp, 4 + clen, SEEK_CUR);
	}

err_read:
	fclose(fp);
err_open:

	return ret;

}

int lzo_decompress_buffer(uint8_t * src, uint32_t srclen, uint8_t * dst, uint32_t dstlen, uint32_t * decomp_len_p) {

	uint32_t comp_len, decomp_len, decomp = 0, overhead;
	int ret = -1, lzoret;

	uint8_t fname_len;
	uint32_t skip = 0;

	/* Validate buffer */
	if (!src || !dst || srclen < 1 || dstlen < 1) {
		printf("Buffer error\r\n");
		goto out;
	}

	/* Read file name length */
	fname_len = src[33];

	/* Skip file name and checksum */
	skip = 34 + fname_len + sizeof(uint32_t);

	/* Decompress blocks */
	while (1) {
		/* Read decompressed block size */
		memcpy(&decomp_len, &src[skip], sizeof(decomp_len));
		decomp_len = util_ntohl(decomp_len);

		/* End of file is indicated when decompressed length equals zero */
		if (decomp_len == 0) {
			ret = 0;
			break;
		}

		skip += sizeof(uint32_t);

		/* Read compressed block size */
		memcpy(&comp_len, &src[skip], sizeof(comp_len));
		comp_len = util_ntohl(comp_len);
		skip += sizeof(uint32_t);

		/* Skip checksum */
		skip += sizeof(uint32_t);

		/* Validate buffer size */
		overhead = decomp_len / 16 + 64 + 3;
		if (dstlen < (decomp + decomp_len + overhead)) {
			printf("Buffer not large enough for decompressed data\r\n");
			goto out;
		}

		/* Perform in-place decompression */
		lzoret = lzo1x_decompress_safe(&src[skip], comp_len, dst + decomp, (unsigned long int *) &decomp_len, NULL);
		if (lzoret != LZO_E_OK) {
			printf("Decompression failed (ret %d)\n", lzoret);
			goto out;
		} else {
			decomp += decomp_len;
			skip += comp_len;
		}
	}

	if (decomp_len_p != NULL)
		*decomp_len_p = decomp;

out:
	return ret;

}

int lzo_decompress_image(char * path, void * dst, uint32_t dstlen) {

	int ret = -1;
	uint32_t header_len, comp_len, decomp_len;

	/* Validate buffer */
	if (dst == NULL || dstlen < 1) {
		printf("Buffer error\r\n");
		goto err_open;
	}

	/* Read decompressed size */
	if (lzo_header(path, &header_len, &decomp_len, &comp_len) != 0) {
		printf("Failed to read header\r\n");
		goto err_open;
	}

	/* Open file */
	FILE * fp = fopen(path, "r");
	if (!fp) {
		printf("Cannot open %s\r\n", path);
		goto err_open;
	}

	struct stat statbuf;
	stat(path, &statbuf);

	/* Allocate buffer */
	if (dstlen < (decomp_len + 0x40000 / 16 + 64 + 3)) {
		printf("Buffer error\r\n");
		goto err_read;
	}

	/* Read file into buffer */
	if (fread(dst + dstlen - statbuf.st_size, 1, statbuf.st_size, fp) != (size_t) statbuf.st_size) {
		printf("Failed to read complete file\r\n");
		goto err_read;
	}

	/* Decompress */
	if (lzo_decompress_buffer(dst + dstlen - statbuf.st_size, statbuf.st_size, dst, dstlen, NULL) != 0) {
		printf("Failed to decompress buffer\r\n");
		goto err_read;
	}

	ret = decomp_len;

err_read:
	fclose(fp);
err_open:
	return ret;

}

int lzo_write_header(lzo_header_t * head) {

	#define F_H_CRC32       	0x00001000L
	#define CRC32_INIT_VALUE    0

	/* Init header */
	memcpy(head->magic, magic, 9);
	head->version = util_htons(0x1030);
	head->libversion = util_htons(0x2030);
	head->versionneeded = util_htons(0x0940);
	head->method = 1;
	head->level = 5;
	head->flags = util_htonl(F_H_CRC32);
	head->mode = 0;
	head->mtime_low = 0;
	head->mtime_high = 0;
	head->fnamelen = 0;

	/* Calculate checksum */
	lzo_uint32 lzo_crc32(lzo_uint32 c, const lzo_bytep buf, lzo_uint len);
	lzo_uint32 crc = CRC32_INIT_VALUE;
	char * in = (char *) &head->version;
	char * out = (char *) &head->checksum_header;
	while(in != out) {
		crc = lzo_crc32((lzo_uint32) crc, (const lzo_bytep) in, 1);
		in++;
	}
	head->checksum_header = util_htonl(crc);

	return 0;
}

#ifdef FREERTOS_CONFIG
int lzo_compress_buffer(void * src, uint32_t src_len, void *out, uint32_t *out_len, uint32_t out_size) {

	void write32(char * buf, uint32_t *offset, uint32_t data) {
	    uint32_t data_be = util_hton32(data);
	    memcpy(buf + *offset, &data_be, sizeof(uint32_t));
	    *offset += sizeof(uint32_t);
	}

	/* Init lzo */
	lzo_init();

	/* Allocate working memory */
	void * wrkmem = pvPortMalloc(LZO1X_MEM_COMPRESS);
	if (wrkmem == NULL)
		return -1;

	/* Destination */
	unsigned long dst_len = 0;
	void * dst = pvPortMalloc(BLOCK_SIZE);
	if (dst == NULL) {
		vPortFree(wrkmem);
		return -1;
	}

	*out_len = 0;

	/* Write header */
	lzo_write_header((void *) out);
	*out_len += sizeof(lzo_header_t);

	/* Compress blocks */
	unsigned int src_read = 0;
	unsigned int block_size;
	while(src_read < src_len) {

		/* Calculate block size */
		block_size = (src_len - src_read);
		if (block_size > BLOCK_SIZE)
			block_size = BLOCK_SIZE;

		/* Compress */
		int result = lzo1x_1_compress(src + src_read, block_size, dst, &dst_len, wrkmem);
		if (result < 0) {
			vPortFree(dst);
			vPortFree(wrkmem);
			return -1;
		}

		log_debug("LZO_BLOCK", "Compressed %u to %lu (%.0f)", block_size, dst_len, (float) (dst_len * 100) / block_size);

		/* If compression worked */
		if (dst_len < src_len) {

			/* Write lengths */
			write32(out, out_len, block_size);
			write32(out, out_len, dst_len);

			/* Write data */
			memcpy(out + *out_len, dst, dst_len);
			*out_len += dst_len;

		/* Otherwise write original block */
		} else {

			/* Write lengths */
			write32(out, out_len, block_size);
			write32(out, out_len, block_size);

			/* Write data */
			memcpy(out + *out_len, src + src_read, block_size);
			*out_len += block_size;

		}

		src_read += block_size;

	}

	/* Write EOF */
	write32(out, out_len, 0);

	/* Free mem */
	vPortFree(wrkmem);
	vPortFree(dst);

	return 0;

}
#endif
