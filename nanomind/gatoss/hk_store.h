/*
 * hk_store.h
 *
 *  Created on: Aug 23, 2012
 *      Author: johan
 */

#ifndef HK_STORE_H_
#define HK_STORE_H_

/**
 * Returns status of hk file write system
 * @return <0 if err
 */
int hk_store_file_status(void);

/**
 * Add beacon to HK store
 * @param beacon pointer to beacon
 * @return <0 if err
 */
int hk_store_add(cdh_beacon * beacon);

/**
 * Iterator function
 * @param beacon pointer to element
 * @return -1 if you wish to cancel iteration, 0 otherwise
 */
typedef int (*hk_store_iterate_callback)(cdh_beacon * beacon);

void hk_store_iterate(hk_store_iterate_callback callback, unsigned int from, unsigned int to);
void hk_store_purge(unsigned int max_file_count, unsigned int max_age_sec);

/**
 * Init hk store
 * @return <0 if err
 */
int hk_store_init(void);

/**
 * Save from memory to file
 * @param path filename
 * @param from timestamp
 * @param to timestamp
 * @param interval in seconds
 * @return -1 if err, 0 if ok
 */
int hk_store_file(char * path, uint32_t from, uint32_t to, uint32_t interval);

/**
 * Read contents of stored HK file
 * @param path
 * @return -1 if err, 0 if ok
 */
int hk_store_file_read(char * path);

#endif /* HK_STORE_H_ */
