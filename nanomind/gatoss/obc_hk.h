/**
 * OBC HK
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef OBC_HK_H_
#define OBC_HK_H_

typedef struct __attribute__((packed)) {
	uint32_t bootcount;							// OBC boot count
	uint16_t temp1;								// Board temperature * 4 in [C]	(LM60 close to magnetometer)
	uint16_t temp2;								// Board temperature * 4 in [C] (LM60 close to crystal)
	int16_t panel_temp[6];						// Panel temperature * 4 in [C] [A1 A2 A3 A4 A5 A6]
} obc_hk_t;

/**
 * Get latest HK values from OBC
 * @param hk
 */
void obc_hk_get(obc_hk_t * hk);

#endif /* OBC_HK_H_ */
