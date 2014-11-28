/**
 * @file gatoss_common.c
 * This file contains routines that are usefull both on the server and the client of the GATOSS API
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <io/gatoss.h>
#include <csp/csp_endian.h>
#include <math.h>

void gatoss_position_compress(gatoss_pos_t * in, gatoss_compos_t * out) {

	uint32_t lon_tmp = lround((in->lon + 180.0) * 23300.0) & 0x7FFFFF;
	uint32_t lat_tmp = lround((in->lat + 90.0) * 23300.0) & 0x3FFFFF;
	uint32_t alt_tmp = lround(in->altitude / 25) & 0x7FF;

	out->compos[0] = lon_tmp >> 15;
	out->compos[1] = lon_tmp >> 7;
	out->compos[2] = lon_tmp << 1 | lat_tmp >> 21;
	out->compos[3] = lat_tmp >> 13;
	out->compos[4] = lat_tmp >> 5;
	out->compos[5] = lat_tmp << 3 | alt_tmp >> 8;
	out->compos[6] = alt_tmp & 0xFF;
	out->timestamp = csp_hton32(in->timestamp);

}

void gatoss_positions_decompress(gatoss_compos_t * in, gatoss_pos_t * out) {

	uint32_t lon_tmp = (in->compos[0] << 15) | (in->compos[1] << 7) | (in->compos[2] >> 1);
	uint32_t lat_tmp = ((in->compos[2] & 1) << 21) | (in->compos[3] << 13) | (in->compos[4] << 5) | ((in->compos[5] & 0xF8) >> 3);
	uint32_t alt_tmp = ((in->compos[5] & 0x7) << 8) | in->compos[6];

	out->lat = (lat_tmp / 23300.0) - 90.0;
	out->lon = (lon_tmp / 23300.0) - 180.0;
	out->altitude = (alt_tmp * 25);
	out->timestamp = csp_ntoh32(in->timestamp);

}
