/**
 * Housekeeping pack
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#ifndef HK_PACK_H_
#define HK_PACK_H_

#include "hk_collect.h"

/**
 * Updates last beacons with freshly packed data and stores to backend
 * @param data pointer to houskeeping data
 */
void hk_pack(hk_cache_t *data);

/**
 * Get latest beacon of a certain type
 * @param beacon_type [0,1,2] = [a,b,c]
 * @return pointer to beacon, ready to transmit
 */
cdh_beacon * hk_pack_get(uint8_t beacon_type);

#endif /* HK_STORE_H_ */
