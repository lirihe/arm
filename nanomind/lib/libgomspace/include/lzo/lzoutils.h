/* Utilities for LZO decompression */

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
 * Compressed data: 0-xxx bytes
 */

#include <stdint.h>

typedef struct __attribute__((packed)) {
	uint8_t magic[9];
	uint16_t version;
	uint16_t libversion;
	uint16_t versionneeded;
	uint8_t method;
	uint8_t level;
	uint32_t flags;
	uint32_t mode;
	uint32_t mtime_low;
	uint32_t mtime_high;
	uint8_t fnamelen;
	uint32_t checksum_header;
	uint8_t data[];
} lzo_header_t;

int lzo_match_magic(void * add);
int lzo_is_lzop_image(char * path);
int lzo_header(char * path, uint32_t * header_length, uint32_t * decomp_len, uint32_t * comp_len);
int lzo_decompress_image(char * path, void * dst, uint32_t dstlen);
int lzo_decompress_buffer(uint8_t * src, uint32_t srclen, uint8_t * dst, uint32_t dstlen, uint32_t * decomp_len_p);
int lzo_write_header(lzo_header_t * head);
int lzo_compress_buffer(void * src, uint32_t src_len, void *out, uint32_t *out_len, uint32_t out_size);
