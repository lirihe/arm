#include <stdint.h>
#ifndef __linux__
#include <freertos/FreeRTOS.h>
#else
#define LITTLE_ENDIAN
#endif

/* Convert 16-bit number from host byte order to network byte order */
extern inline uint16_t __attribute__ ((__const__)) util_hton16(uint16_t h16) {
#ifdef BIG_ENDIAN
	return h16;
#else
	return (((h16 & 0xff00) >> 8) |
			((h16 & 0x00ff) << 8));
#endif
}

/* Convert 16-bit number from network byte order to host byte order */
extern inline uint16_t __attribute__ ((__const__)) util_ntoh16(uint16_t n16) {
	return util_hton16(n16);
}

/* Convert 32-bit number from host byte order to network byte order */
extern inline uint32_t __attribute__ ((__const__)) util_hton32(uint32_t h32) {
#ifdef BIG_ENDIAN
	return h32;
#else
	return (((h32 & 0xff000000) >> 24) |
			((h32 & 0x000000ff) << 24) |
			((h32 & 0x0000ff00) <<  8) |
			((h32 & 0x00ff0000) >>  8));
#endif
}

/* Convert 32-bit number from network byte order to host byte order */
extern inline uint32_t __attribute__ ((__const__)) util_ntoh32(uint32_t n32) {
	return util_hton32(n32);
}

/* Convert 64-bit number from host byte order to network byte order */
extern inline uint64_t __attribute__ ((__const__)) util_hton64(uint64_t h64) {
#ifdef BIG_ENDIAN
	return h64;
#else
	return (((h64 & 0xff00000000000000LL) >> 56) |
			((h64 & 0x00000000000000ffLL) << 56) |
			((h64 & 0x00ff000000000000LL) >> 40) |
			((h64 & 0x000000000000ff00LL) << 40) |
			((h64 & 0x0000ff0000000000LL) >> 24) |
			((h64 & 0x0000000000ff0000LL) << 24) |
			((h64 & 0x000000ff00000000LL) >>  8) |
			((h64 & 0x00000000ff000000LL) <<  8));
#endif
}

/* Convert 64-bit number from host byte order to network byte order */
extern inline uint64_t __attribute__ ((__const__)) util_ntoh64(uint64_t n64) {
	return util_hton64(n64);
}

/* Convert float from host byte order to network byte order */
extern inline float __attribute__ ((__const__)) util_htonflt(float f) {
#ifdef BIG_ENDIAN
	return f;
#else
	union v {
		float       f;
		uint32_t	i;
	};
	union v val;
	val.f = f;
	val.i = util_hton32(val.i);
	return val.f;

#endif
}

/* Convert float from host byte order to network byte order */
extern inline float __attribute__ ((__const__)) util_ntohflt(float f) {
	return util_htonflt(f);
}

/* Convert 16-bit number from host byte order to big endian byte order */
extern inline uint16_t __attribute__ ((__const__)) util_htobe16(uint16_t h16) {
	return util_hton16(h16);
}

/* Convert 16-bit number from host byte order to little endian byte order */
extern inline uint16_t __attribute__ ((__const__)) util_htole16(uint16_t h16) {
#ifdef LITTLE_ENDIAN
	return h16;
#else
	return (((h16 & 0xff00) >> 8) |
			((h16 & 0x00ff) << 8));
#endif
}

/* Convert 16-bit number from big endian byte order to little endian byte order */
extern inline uint16_t __attribute__ ((__const__)) util_betoh16(uint16_t be16) {
	return util_ntoh16(be16);
}

/* Convert 16-bit number from little endian byte order to host byte order */
extern inline uint16_t __attribute__ ((__const__)) util_letoh16(uint16_t le16) {
	return util_htole16(le16);
}

/* Convert 32-bit number from host byte order to big endian byte order */
extern inline uint32_t __attribute__ ((__const__)) util_htobe32(uint32_t h32) {
	return util_hton32(h32);
}

/* Convert 32-bit number from little endian byte order to host byte order */
extern inline uint32_t __attribute__ ((__const__)) util_htole32(uint32_t h32) {
#ifdef LITTLE_ENDIAN
	return h32;
#else
	return (((h32 & 0xff000000) >> 24) |
			((h32 & 0x000000ff) << 24) |
			((h32 & 0x0000ff00) <<  8) |
			((h32 & 0x00ff0000) >>  8));
#endif
}

/* Convert 32-bit number from big endian byte order to host byte order */
extern inline uint32_t __attribute__ ((__const__)) util_betoh32(uint32_t be32) {
	return util_ntoh32(be32);
}

/* Convert 32-bit number from little endian byte order to host byte order */
extern inline uint32_t __attribute__ ((__const__)) util_letoh32(uint32_t le32) {
	return util_htole32(le32);
}

/* Convert 64-bit number from host byte order to big endian byte order */
extern inline uint64_t __attribute__ ((__const__)) util_htobe64(uint64_t h64) {
	return util_hton64(h64);
}

/* Convert 64-bit number from host byte order to little endian byte order */
extern inline uint64_t __attribute__ ((__const__)) util_htole64(uint64_t h64) {
#ifdef LITTLE_ENDIAN
	return h64;
#else
	return (((h64 & 0xff00000000000000LL) >> 56) |
			((h64 & 0x00000000000000ffLL) << 56) |
			((h64 & 0x00ff000000000000LL) >> 40) |
			((h64 & 0x000000000000ff00LL) << 40) |
			((h64 & 0x0000ff0000000000LL) >> 24) |
			((h64 & 0x0000000000ff0000LL) << 24) |
			((h64 & 0x000000ff00000000LL) >>  8) |
			((h64 & 0x00000000ff000000LL) <<  8));
#endif
}

/* Convert 64-bit number from big endian byte order to host byte order */
extern inline uint64_t __attribute__ ((__const__)) util_betoh64(uint64_t be64) {
	return util_ntoh64(be64);
}

/* Convert 64-bit number from little endian byte order to host byte order */
extern inline uint64_t __attribute__ ((__const__)) util_letoh64(uint64_t le64) {
	return util_htole64(le64);
}

/* Convert 16-bit number from host byte order to network byte order */
extern inline uint16_t __attribute__ ((__const__)) util_htons(uint16_t h16) {
	return util_hton16(h16);
}

/* Convert 16-bit number from network byte order to host byte order */
extern inline uint16_t __attribute__ ((__const__)) util_ntohs(uint16_t n16) {
	return util_ntoh16(n16);
}

/* Convert 32-bit number from host byte order to network byte order */
extern inline uint32_t __attribute__ ((__const__)) util_htonl(uint32_t h32) {
	return util_hton32(h32);
}

/* Convert 32-bit number from network byte order to host byte order */
extern inline uint32_t __attribute__ ((__const__)) util_ntohl(uint32_t n32) {
	return util_ntoh32(n32);
}
