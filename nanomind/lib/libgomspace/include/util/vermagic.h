/*
 * vermagic.h
 *
 *  Created on: 25/03/2011
 *      Author: johan
 */

#ifndef VERMAGIC_H_
#define VERMAGIC_H_

typedef struct __attribute__((packed)) version_s {
	char type[4];
	char ver[5];
	char date[12];
	char time[9];
} version_t;

/**
 * Get a version_t datastructure filled with the fields;
 * svn_ver, date and time filled out
 * the optional field type is set to empty string "".
 * @param ver pointer to datastructure to fill in fields.
 */
void vermagic_get(version_t * ver);

#endif /* VERMAGIC_H_ */
