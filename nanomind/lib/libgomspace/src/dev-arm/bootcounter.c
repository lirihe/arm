/**
 * Boot counter for NanoMind
 *
 * @author Morten Bisgaard
 * Copyright 2013 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <unistd.h>

static unsigned int obc_boot_count;


unsigned int boot_counter_increment(void) {
	FILE * fp;

	/* Check for a boot counter */
	if( access("/boot/boot.count", F_OK ) != -1 ) {
		/* open existing */
		fp = fopen("/boot/boot.count", "r+");
	} else {
		/* create new */
		fp = fopen("/boot/boot.count", "w+");
	}

	if (fp) {
		/* read boot count, if empty set to zero */
		int ret = fscanf(fp,"%u",&obc_boot_count);
		if (ret < 1) {
			obc_boot_count = 0;
		}
		rewind(fp);
		obc_boot_count ++;
		fprintf(fp, "%u",obc_boot_count);
		fclose(fp);
	}
	else
		obc_boot_count = 0;

	return obc_boot_count;
}

unsigned int boot_counter_get(void) {
	return obc_boot_count;
}


void boot_counter_reset(void) {
	FILE * fp = NULL;
	/* Check for a boot counter */
	if( access("/boot/boot.count", F_OK ) != -1 ) {
		/* open existing */
		fp = fopen("/boot/boot.count", "r+");
		if (fp) {
			rewind(fp);
			obc_boot_count = 1;
			fprintf(fp, "%u",obc_boot_count);
			fclose(fp);
		}
	}

}
