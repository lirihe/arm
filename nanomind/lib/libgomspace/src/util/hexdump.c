/*
 * util.c
 *
 *  Created on: Aug 12, 2009
 *      Author: johan
 */

#include <string.h>
#include <util/log.h>


//! Dump memory to debugging output
/**
 * Dumps a chunk of memory to the screen
 */
void hex_dump(void *src, int len) {
	int i, j=0, k;
	char text[17];

	text[16] = '\0';
	//printf("Hex dump:\r\n");
	printf("%p : ", src);
	for(i=0; i<len; i++) {
		j++;
		printf("%02X ",((volatile unsigned char *)src)[i]);
		if(j == 8)
			printf(" ");
		if(j == 16) {
			j = 0;
			memcpy(text, &((char *)src)[i-15], 16);
			for(k=0; k<16; k++) {
				if((text[k] < 32) || (text[k] > 126)) {
					text[k] = '.';
				}
			}
			printf(" |");
			printf("%s", text);
			printf("|\n\r");
			if(i<len-1) {
				printf("%p : ", src+i+1);
			}
		}
	}
	if (i % 16)
		printf("\r\n");
}

