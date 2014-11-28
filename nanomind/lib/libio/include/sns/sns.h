/**
 * Static Name Service for CSP
 *
 * @author Johan De Claville Christiansen
 * Copyright 2013 GomSpace ApS
 */

#ifndef SNS_H_
#define SNS_H_

#include <string.h>

#define SNS_SEARCH_DOMAIN(domain) sns_search_domain(domain, strlen(domain))

/**
 * Initialise the local search domain
 * Please use the SNS_SEARCH_DOMAIN macro in order to enable compile time strlen computation
 */
int sns_search_domain(char * domain_str, unsigned int domain_len);

/**
 * Helper function to print table to stdout
 */
void sns_print_table(void);

#define SNS(name) sns(name, strlen(name))

/**
 * Resolve SNS addr
 *
 * Please use the SNS macro in order to enable compile time strlen computation
 * @param name_str
 * @param name_len
 * @return
 */
int sns(char * name_str, unsigned int name_len);

#define SNS_SET_NAME(name, addr) sns_set_name(name, strlen(name), addr)

/**
 * Setup SNS table
 *
 * Please use the SNS_SET_NAME macro to enable compile time strlen computation
 * @param name_str
 * @param name_len
 * @param addr
 * @return 0 if ok, -1 if error
 */
int sns_set_name(char * name_str, unsigned int name_len, unsigned int addr);

/**
 * Clear the entire SNS name table
 */
void sns_reset(void);

#endif /* SNS_H_ */
