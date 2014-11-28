#ifndef _BYTEORDER_H_
#define _BYTEORDER_H_

#include <stdint.h>

/**
 * Convert 16-bit integer from host byte order to network byte order
 * @param h16 Host byte order 16-bit integer
 */
uint16_t util_htons(uint16_t h16);

/**
 * Convert 16-bit integer from host byte order to host byte order
 * @param n16 Network byte order 16-bit integer
 */
uint16_t util_ntohs(uint16_t n16);

/**
 * Convert 32-bit integer from host byte order to network byte order
 * @param h32 Host byte order 32-bit integer
 */
uint32_t util_htonl(uint32_t h32);

/**
 * Convert 32-bit integer from host byte order to network byte order
 * @param h32 Host byte order 32-bit integer
 */
uint32_t util_ntohl(uint32_t n32);

/**
 * Convert 16-bit integer from host byte order to network byte order
 * @param h16 Host byte order 16-bit integer
 */
uint16_t util_hton16(uint16_t h16);

/**
 * Convert 16-bit integer from host byte order to host byte order
 * @param n16 Network byte order 16-bit integer
 */
uint16_t util_ntoh16(uint16_t n16);

/**
 * Convert 32-bit integer from host byte order to network byte order
 * @param h32 Host byte order 32-bit integer
 */
uint32_t util_hton32(uint32_t h32);

/**
 * Convert 32-bit integer from host byte order to host byte order
 * @param n32 Network byte order 32-bit integer
 */
uint32_t util_ntoh32(uint32_t n32);

/**
 * Convert 64-bit integer from host byte order to network byte order
 * @param h64 Host byte order 64-bit integer
 */
uint64_t util_hton64(uint64_t h64);

/**
 * Convert 64-bit integer from host byte order to host byte order
 * @param n64 Network byte order 64-bit integer
 */
uint64_t util_ntoh64(uint64_t n64);

/**
 * Convert float from network to host byte order
 * @param float in host byte order
 */
float util_htonflt(float f);

/**
 * Convert float from network to host byte order
 * @param float in network byte order
 */
float util_ntohflt(float f);

/**
 * Convert 16-bit integer from host byte order to big endian byte order
 * @param h16 Host byte order 16-bit integer
 */
uint16_t util_htobe16(uint16_t h16);

/**
 * Convert 16-bit integer from host byte order to little endian byte order
 * @param h16 Host byte order 16-bit integer
 */
uint16_t util_htole16(uint16_t h16);

/**
 * Convert 16-bit integer from big endian byte order to little endian byte order
 * @param be16 Big endian byte order 16-bit integer
 */
uint16_t util_betoh16(uint16_t be16);

/**
 * Convert 16-bit integer from little endian byte order to host byte order
 * @param le16 Little endian byte order 16-bit integer
 */
uint16_t util_letoh16(uint16_t le16);

/**
 * Convert 32-bit integer from host byte order to big endian byte order
 * @param h32 Host byte order 32-bit integer
 */
uint32_t util_htobe32(uint32_t h32);

/**
 * Convert 32-bit integer from little endian byte order to host byte order
 * @param h32 Host byte order 32-bit integer
 */
uint32_t util_htole32(uint32_t h32);

/**
 * Convert 32-bit integer from big endian byte order to host byte order
 * @param be32 Big endian byte order 32-bit integer
 */
uint32_t util_betoh32(uint32_t be32);

/**
 * Convert 32-bit integer from little endian byte order to host byte order
 * @param le32 Little endian byte order 32-bit integer
 */
uint32_t util_letoh32(uint32_t le32);

/**
 * Convert 64-bit integer from host byte order to big endian byte order
 * @param h64 Host byte order 64-bit integer
 */
uint64_t util_htobe64(uint64_t h64);

/**
 * Convert 64-bit integer from host byte order to little endian byte order
 * @param h64 Host byte order 64-bit integer
 */
uint64_t util_htole64(uint64_t h64);

/**
 * Convert 64-bit integer from big endian byte order to host byte order
 * @param be64 Big endian byte order 64-bit integer
 */
uint64_t util_betoh64(uint64_t be64);

/**
 * Convert 64-bit integer from little endian byte order to host byte order
 * @param le64 Little endian byte order 64-bit integer
 */
uint64_t util_letoh64(uint64_t le64);

#endif // _BYTEORDER_H_
