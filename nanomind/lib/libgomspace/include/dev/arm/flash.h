/**
 * @file
 * @brief Wrapper for the parallel FLASH used on NanoMind.
 */

#ifndef _FLASH_H_
#define _FLASH_H_

int flash_init(void);

/** Address based functions */
int flash_read(void * dst, char * data, int len);
int flash_program(void* dst, char *data, int len, int erase);
unsigned int flash_checksum(unsigned char * addr, unsigned int size);

/** Block based functions */
int flash_erase_chip(unsigned int base);
int flash_erase_block(int block, unsigned int base);
char * flash_addr(int block, unsigned int base);

/** Other */
char * flash_errmsg(int err);

#endif
