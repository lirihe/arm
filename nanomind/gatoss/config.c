/**
 * Nanomind config for GATOSS mission
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <freertos/FreeRTOS.h>
#include <util/crc32.h>
#include <util/log.h>
#include "config.h"

/* Global configuration */
conf_t config;

/* Compile-time configuration */
static conf_t config_compiled = {
	.collect_interval = 10,
	.interval[0].store = 10,
	.interval[0].transmit = 10,
	.interval[1].store = 10,
	.interval[1].transmit = 10,
	.force_beacon_b = 0,
	.checksum = 0xABEDDEBA,
};

/* Lazy macro for writing a field to a file pointer */
#define CONF_WRITE_FIELD(fp, field) 									\
	do {																\
		if (fwrite(&(field), 1, sizeof(field), (fp)) != sizeof(field))	\
			return -1;													\
	} while (0)

/* Equally lazy macro for reading a field from a char buffer */
#define CONF_READ_FIELD(buf, field, offset)								\
	do {																\
		memcpy(&(field), buf + *offset, sizeof(field));					\
		*offset += sizeof(field);										\
	} while (0)

/* Config magic sequence */
static char magic[] = "GATOSS-CONF";

static int conf_store_write_magic(FILE * fpfp) {

	if (fwrite(&magic, 1, strlen(magic), fpfp) != strlen(magic))
		return -1;

	return 0;

}

static int conf_store_verify_magic(char * fpbuf, int * offset, unsigned int bufsize) {

	if (fpbuf == NULL || offset == NULL)
		return -1;

	if (bufsize < strlen(magic))
		return -1;

	if (memcmp(magic, fpbuf + *offset, strlen(magic)))
		return -1;

	*offset += strlen(magic);

	return 0;

}

static int conf_store_write_checksum(FILE * fpfp) {

	/* Get current file position */
	long int size = ftell(fpfp);
	if (size < 0) {
		log_error("CONF_SAVE", "Ftell error %ld", size);
		return -1;
	}

	/* Allocate buffer */
	char * fpbuf = pvPortMalloc(size);
	if (fpbuf == NULL) {
		log_error("CONF_SAVE", "Malloc fail %ld", size);
		return -1;
	}

	/* Rewind file and read into buffer */
	rewind(fpfp);
	if (fread(fpbuf, 1, size, fpfp) != (size_t) size) {
		log_error("CONF_SAVE", "Read fail %ld", size);
		vPortFree(fpbuf);
		return -1;
	}

	/* Calculate checksum */
	uint32_t fpcrc = chksum_crc32((unsigned char *)fpbuf, size);

	/* Write to file */
	if (fwrite(&fpcrc, 1, sizeof(fpcrc), fpfp) != sizeof(fpcrc)) {
		log_error("CONF_SAVE", "Write fail");
		vPortFree(fpbuf);
		return -1;
	}

	vPortFree(fpbuf);
	return 0;

}

static int conf_store_verify_checksum(char * fpbuf, unsigned int bufsize) {

	if (fpbuf == NULL)
		return -1;

	if (bufsize < sizeof(uint32_t))
		return -1;

	uint32_t fpcrc_read, fpcrc_calc;
	memcpy(&fpcrc_read, &fpbuf[bufsize - sizeof(fpcrc_read)], sizeof(fpcrc_read));
	fpcrc_calc = chksum_crc32((unsigned char *)fpbuf, bufsize - sizeof(fpcrc_calc));

	if (fpcrc_read != fpcrc_calc)
		return -1;

	return 0;

}

int conf_save(char * path) {

	int ret = -1;

	/* Open file */
	FILE * fpfp = fopen(path, "w+");
	if (fpfp == NULL) {
		log_error("CONF_SAVE", "Open fail %s", path);
		goto out;
	}

	/* Save magic */
	ret = conf_store_write_magic(fpfp);
	if (ret < 0) {
		log_error("CONF_SAVE", "Magic write fail %s", path);
		goto out_close;
	}

	/* Generate config checksum */
	config.checksum = chksum_crc32((unsigned char *) &config, sizeof(config) - sizeof(uint32_t));

	/* Save config */
	CONF_WRITE_FIELD(fpfp, config);

	/* Save checksum */
	ret = conf_store_write_checksum(fpfp);
	if (ret < 0) {
		log_error("CONF_SAVE", "CRC write fail");
		goto out_close;
	}

	ret = 0;
out_close:
	fclose(fpfp);
out:
	return ret;

}

int conf_load(char * path) {

	int ret = -1;
	int offset = 0;

	/* Open file */
	FILE * fpfp = fopen(path, "r");
	if (fpfp == NULL)
		goto out;

	log_info("CONF_LOAD", "Using %s", path);

	/* Read size */
	struct stat st;
	if (stat(path, &st) != 0) {
		log_error("CONF_LOAD", "Failed to stat %s", path);
		goto out_close;
	}
	unsigned int size = st.st_size;

	char * fpbuf = pvPortMalloc(size);
	if (fpbuf == NULL) {
		log_error("CONF_LOAD", "Malloc fail %u", size);
		goto out_free;
	}

	if (fread(fpbuf, 1, size, fpfp) != size) {
		log_error("CONF_LOAD", "Read fail %u %s", size, path);
		goto out_free;
	}

	/* Verify magic */
	ret = conf_store_verify_magic(fpbuf, &offset, size);
	if (ret < 0) {
		log_error("CONF_LOAD", "%s invalid", path);
		goto out_free;
	}

	/* Verify file checksum */
	ret = conf_store_verify_checksum(fpbuf, size);
	if (ret < 0) {
		log_error("CONF_LOAD", "%s invalid crc", path);
		goto out_free;
	}

	/* Load config */
	CONF_READ_FIELD(fpbuf, config, &offset);

	/* Verify config checksum */
	if (config.checksum != chksum_crc32((unsigned char *) &config, sizeof(config) - sizeof(uint32_t))) {
		log_error("CONF_LOAD", "%s invalid conf crc", path);
		goto out_free;
	}

	/* Okay we got this far, config is good */
	ret = 0;

out_free:
	vPortFree(fpbuf);
out_close:
	fclose(fpfp);
out:
	return ret;

}

void conf_restore() {

	/* Restore */
	int ret = conf_load("/boot/default.cnf");

	/* Check checksum */
	if ((ret < 0) || (config.checksum != chksum_crc32((unsigned char *) &config, sizeof(config) - sizeof(uint32_t)))) {
		log_info("CONF_RUN_COMPILED", "");

		/* Restore compile-time configuration */
		memcpy((void *)&config, (void *) &config_compiled, sizeof(conf_t));

	} else {
		log_info("CONF_RUN_STORED", "");
	}

}

void conf_save_as_default() {
	log_info("CONF_SAVE", "Default settings saved");

	/* Generate checksum */
	config.checksum = chksum_crc32((unsigned char *) &config, sizeof(config) - sizeof(uint32_t));

	/* Save to default file */
	conf_save("/boot/default.cnf");

}
