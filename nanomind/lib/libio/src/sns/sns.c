/**
 * Static Name Service for CSP
 *
 * @author Johan De Claville Christiansen
 * Copyright 2013 GomSpace ApS
 */

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <csp/csp.h>
#include <util/log.h>

#define SNS_DOMAIN_LEN 				3
#define SNS_FQDN_LEN				12

static char sns_domain[SNS_DOMAIN_LEN + 1] = {};

/** This array stores the CSP address and the FQDN's */
static struct {
	unsigned int addr;					//! CSP Address
	char name[SNS_FQDN_LEN + 1];	//! SNS Name
} sns_data[CSP_ID_HOST_MAX] = {
		{1, "obc.fm"},
		{2, "eps.fm"},
		{3, "hub.fm"},
		{4, "gatoss.fm"},
		{5, "com.fm"},
		{6, "cam.fm"},
		{9, "tnc.gnd"},
		{10, "pc.gnd"},
		{17, "obc.em"},
		{18, "eps.em"},
		{19, "hub.em"},
		{20, "gatoss.em"},
		{21, "com.em"},
		{22, "cam.em"},
};

/**
 * This counts the number of allocated elements in the array
 * Since inserted SNS names cannot be deleted we use a simple input counter only
 */
static unsigned int sns_data_counter = 14; //! @warning: This number must match the inital array size of sns_data

int sns(char * name_str, unsigned int name_len) {

	/* Input validation */
	if ((name_len > SNS_FQDN_LEN) || (name_str == NULL)) {
		log_error("SNS", "Invalid name");
		return -1;
	}

	char * dot_pos = strstr(name_str, ".");

	char search_str[SNS_FQDN_LEN] = {};
	unsigned int search_len = name_len;
	strncpy(search_str, name_str, name_len);
	if ((dot_pos == NULL) && (strlen(sns_domain) > 0)) {
		strcat(search_str, ".");
		strcat(search_str, sns_domain);
		search_len += 1 + strlen(sns_domain);
	}

	for (unsigned int i = 0; i < sns_data_counter; i++) {
		if (strncmp(sns_data[i].name, search_str, search_len) == 0)
			return sns_data[i].addr;
	}

	return -1;
}

int sns_set_name(char * name_str, unsigned int name_len, unsigned int addr) {

	/* Input validation */
	if ((name_len > SNS_FQDN_LEN) || (name_str == NULL)) {
		log_error("SNS", "Invalid name");
		return -1;
	}

	/* Copy data to array */
	memcpy(&sns_data[sns_data_counter].name, name_str, name_len);
	sns_data[sns_data_counter].addr = addr;

	/* Increase data counter */
	sns_data_counter++;

	return 0;
}

void sns_print_table(void) {
	printf("      NAME     ADDR\r\n");
	for (unsigned int i = 0; i < sns_data_counter; i++)
		printf("%10s       %2u\r\n", sns_data[i].name, sns_data[i].addr);
}

int sns_search_domain(char * domain_str, unsigned int domain_len) {
	if ((domain_len > SNS_DOMAIN_LEN) || (domain_str == NULL)) {
		log_error("SNS", "Invalid domain name");
		return -1;
	}
	memcpy(sns_domain, domain_str, domain_len);
	log_info("SNS", "Domain set to: %s\r\n", sns_domain);

	return 0;
}

void sns_reset(void) {
	memset(sns_data, 0, sizeof(sns_data[0]) * CSP_ID_HOST_MAX);
	sns_data_counter = 0;
}
