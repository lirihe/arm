#ifndef _CRC32_H
#define _CRC32_H

/**
 * Calculate single step of crc32
 */
uint32_t chksum_crc32_step(uint32_t crc, uint8_t byte);

/**
 * Caluclate crc32 of a block
 */
uint32_t chksum_crc32(uint8_t *block, unsigned int length);

#endif
