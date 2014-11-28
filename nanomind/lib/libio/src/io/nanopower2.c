/**
 * NanoPower firmware
 *
 * @author Morten Bisgaard
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <string.h>
#include <inttypes.h>

#include <io/nanopower2.h>

#include <util/error.h>
#include <util/log.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

static uint8_t eps_node = NODE_EPS;
static uint32_t eps_timeout = 1000;

void eps_set_node(uint8_t node) {
	eps_node = node;
}

void eps_set_timeout(uint32_t timeout) {
	eps_timeout = timeout;
}

void eps_hk_vi_pack(eps_hk_vi_t * hk) {
	int i;

	/* arrays with 3 */
	for (i = 0; i < 3; i++) {
		hk->vboost[i] = csp_hton16(hk->vboost[i]);
		hk->curin[i] = csp_hton16(hk->curin[i]);
	}

	hk->vbatt = csp_hton16(hk->vbatt);
	hk->cursun = csp_hton16(hk->cursun);
	hk->cursys = csp_hton16(hk->cursys);
}


void eps_hk_out_pack(eps_hk_out_t * hk) {
	int i;
	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		hk->curout[i] = csp_hton16(hk->curout[i]);
		hk->latchup[i] = csp_hton16(hk->latchup[i]);
	}
	/* arrays with 8 */
	for (i = 0; i < 8; i++) {
		hk->output[i] = hk->output[i];
		hk->output_on_delta[i] = csp_hton16(hk->output_on_delta[i]);
		hk->output_off_delta[i] = csp_hton16(hk->output_off_delta[i]);
	}
}


void eps_hk_wdt_pack(eps_hk_wdt_t * hk) {
	hk->wdt_i2c_time_left = csp_hton32(hk->wdt_i2c_time_left);
	hk->wdt_gnd_time_left = csp_hton32(hk->wdt_gnd_time_left);
	hk->counter_wdt_i2c = csp_hton32(hk->counter_wdt_i2c);
	hk->counter_wdt_gnd = csp_hton32(hk->counter_wdt_gnd);
	hk->counter_wdt_csp[0] = csp_hton32(hk->counter_wdt_csp[0]);
	hk->counter_wdt_csp[1] = csp_hton32(hk->counter_wdt_csp[1]);
}


void eps_hk_basic_pack(eps_hk_basic_t * hk) {
	int i;
	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		hk->temp[i] = csp_hton16(hk->temp[i]);
	}
	hk->counter_boot = csp_hton32(hk->counter_boot);
}




void eps_hk_pack(eps_hk_t * hk) {
	int i;

	/* arrays with 3 */
	for (i = 0; i < 3; i++) {
		hk->vboost[i] = csp_hton16(hk->vboost[i]);
		hk->curin[i] = csp_hton16(hk->curin[i]);
	}

	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		hk->curout[i] = csp_hton16(hk->curout[i]);
		hk->temp[i] = csp_hton16(hk->temp[i]);
		hk->latchup[i] = csp_hton16(hk->latchup[i]);
	}

	/* arrays with 8 */
	for (i = 0; i < 8; i++) {
		hk->output[i] = hk->output[i];
		hk->output_on_delta[i] = csp_hton16(hk->output_on_delta[i]);
		hk->output_off_delta[i] = csp_hton16(hk->output_off_delta[i]);
	}

	hk->vbatt = csp_hton16(hk->vbatt);
	hk->cursun = csp_hton16(hk->cursun);
	hk->cursys = csp_hton16(hk->cursys);
	hk->wdt_i2c_time_left = csp_hton32(hk->wdt_i2c_time_left);
	hk->wdt_gnd_time_left = csp_hton32(hk->wdt_gnd_time_left);
	hk->counter_boot = csp_hton32(hk->counter_boot);
	hk->counter_wdt_i2c = csp_hton32(hk->counter_wdt_i2c);
	hk->counter_wdt_gnd = csp_hton32(hk->counter_wdt_gnd);
	hk->counter_wdt_csp[0] = csp_ntoh32(hk->counter_wdt_csp[0]);
	hk->counter_wdt_csp[1] = csp_ntoh32(hk->counter_wdt_csp[1]);

}


void eps_hk_vi_unpack(eps_hk_vi_t * hk) {
	int i;

	/* arrays with 3 */
	for (i = 0; i < 3; i++) {
		hk->vboost[i] = csp_ntoh16(hk->vboost[i]);
		hk->curin[i] = csp_ntoh16(hk->curin[i]);
	}

	hk->vbatt = csp_ntoh16(hk->vbatt);
	hk->cursun = csp_ntoh16(hk->cursun);
	hk->cursys = csp_ntoh16(hk->cursys);
}

void eps_hk_out_unpack(eps_hk_out_t * hk) {
	int i;
	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		hk->curout[i] = csp_ntoh16(hk->curout[i]);
		hk->latchup[i] = csp_ntoh16(hk->latchup[i]);
	}
	/* arrays with 8 */
	for (i = 0; i < 8; i++) {
		hk->output[i] = hk->output[i];
		hk->output_on_delta[i] = csp_ntoh16(hk->output_on_delta[i]);
		hk->output_off_delta[i] = csp_ntoh16(hk->output_off_delta[i]);
	}
}


void eps_hk_wdt_unpack(eps_hk_wdt_t * hk) {
	hk->wdt_i2c_time_left = csp_ntoh32(hk->wdt_i2c_time_left);
	hk->wdt_gnd_time_left = csp_ntoh32(hk->wdt_gnd_time_left);
	hk->counter_wdt_i2c = csp_ntoh32(hk->counter_wdt_i2c);
	hk->counter_wdt_gnd = csp_ntoh32(hk->counter_wdt_gnd);
	hk->counter_wdt_csp[0] = csp_ntoh32(hk->counter_wdt_csp[0]);
	hk->counter_wdt_csp[1] = csp_ntoh32(hk->counter_wdt_csp[1]);
}


void eps_hk_basic_unpack(eps_hk_basic_t * hk) {
	int i;
	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		hk->temp[i] = csp_ntoh16(hk->temp[i]);
	}
	hk->counter_boot = csp_ntoh32(hk->counter_boot);
}



void eps_hk_unpack(eps_hk_t * hk) {
	int i;

	/* arrays with 3 */
	for (i = 0; i < 3; i++) {
		hk->vboost[i] = csp_ntoh16(hk->vboost[i]);
		hk->curin[i] = csp_ntoh16(hk->curin[i]);
	}

	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		hk->curout[i] = csp_ntoh16(hk->curout[i]);
		hk->temp[i] = csp_ntoh16(hk->temp[i]);
		hk->latchup[i] = csp_ntoh16(hk->latchup[i]);
	}

	/* arrays with 8 */
	for (i = 0; i < 8; i++) {
		hk->output[i] = hk->output[i];
		hk->output_on_delta[i] = csp_ntoh16(hk->output_on_delta[i]);
		hk->output_off_delta[i] = csp_ntoh16(hk->output_off_delta[i]);
	}

	hk->vbatt = csp_ntoh16(hk->vbatt);
	hk->cursun = csp_ntoh16(hk->cursun);
	hk->cursys = csp_ntoh16(hk->cursys);
	hk->wdt_i2c_time_left = csp_ntoh32(hk->wdt_i2c_time_left);
	hk->wdt_gnd_time_left = csp_ntoh32(hk->wdt_gnd_time_left);
	hk->counter_boot = csp_ntoh32(hk->counter_boot);
	hk->counter_wdt_i2c = csp_ntoh32(hk->counter_wdt_i2c);
	hk->counter_wdt_gnd = csp_ntoh32(hk->counter_wdt_gnd);
	hk->counter_wdt_csp[0] = csp_ntoh32(hk->counter_wdt_csp[0]);
	hk->counter_wdt_csp[1] = csp_ntoh32(hk->counter_wdt_csp[1]);
}

void eps_hk_print(eps_hk_t * hk) {

	unsigned int p_in_1 = (float) hk->vboost[0] * (float) hk->curin[0] / 1000.0;
	unsigned int p_in_2 = (float) hk->vboost[1] * (float) hk->curin[1] / 1000.0;
	unsigned int p_in_3 = (float) hk->vboost[2] * (float) hk->curin[2] / 1000.0;
	unsigned int p_sun = (float) hk->cursun * (float) hk->vbatt / 1000.0;
	unsigned int p_sys = (float) hk->cursys * (float) hk->vbatt / 1000.0;
	unsigned int eff_in = (100.0 * (float) p_sun) / ((float) p_in_1 + (float) p_in_2 + (float) p_in_3);
	if (eff_in >= 100)
		eff_in = 99;


	printf("                    _________\r\n");
	printf("                   |         |                           I(mA),  lup,Ton(s),Toff(s)\r\n");
	printf("              +-------------------+  1 (H1-47) --> EN:%u [ %4u, %4u,  %4u,   %4u]\r\n", hk->output[0], hk->curout[0], hk->latchup[0],hk->output_on_delta[0],hk->output_off_delta[0]);
	printf("  1:          |                   |\r\n");
	printf("%5u mV ->   |  Voltage          |  2 (H1-49) --> EN:%u [ %4u, %4u,  %4u,   %4u]\r\n", hk->vboost[0], hk->output[1], hk->curout[1], hk->latchup[1],hk->output_on_delta[1],hk->output_off_delta[1]);
	printf("%5u mA ->   | %05u mV          |\r\n", hk->curin[0], hk->vbatt);
	printf("%5u mW ->   |                   |  3 (H1-51) --> EN:%u [ %4u, %4u,  %4u,   %4u]\r\n",  p_in_1, hk->output[2], hk->curout[2], hk->latchup[2],hk->output_on_delta[2],hk->output_off_delta[2]);
	printf("  2:          |  Input            |\r\n");
	printf("%5u mV ->   | %05u mA %05u mW |  4 (H1-48) --> EN:%u [ %4u, %4u,  %4u,   %4u]\r\n",  hk->vboost[1], hk->cursun, p_sun, hk->output[3], hk->curout[3], hk->latchup[3],hk->output_on_delta[3],hk->output_off_delta[3]);
	printf("%5u mA ->   |                   |\r\n", hk->curin[1]);
	printf("%5u mW ->   |  Output           |  5 (H1-50) --> EN:%u [ %4u, %4u,  %4u,   %4u]\r\n", p_in_2,  hk->output[4], hk->curout[4], hk->latchup[4],hk->output_on_delta[4],hk->output_off_delta[4]);
	printf("  3:          | %05u mA %05u mW |\r\n", hk->cursys, p_sys);
	printf("%5u mV ->   |                   |  6 (H1-52) --> EN:%u [ %4u, %4u,  %4u,   %4u]\r\n", hk->vboost[2],hk->output[5], hk->curout[5], hk->latchup[5],hk->output_on_delta[5],hk->output_off_delta[5]);
	printf("%5u mA ->   |  Efficiency:      |\r\n", hk->curin[2]);
	printf("%5u mW ->   |  In: %2u %%         |  7         --> EN:%u\r\n", p_in_3, eff_in, hk->output[6]);
	printf("              |                   |\r\n");
	printf("              |  ");
	switch(hk->battmode) {
	case 1: printf("CRITICAL"); break;
	case 2: printf("SAFEMODE"); break;
	case 3: printf("Normal  "); break;
	case 4: printf("Full    "); break;
	}
	printf("         |  8         --> EN:%u\r\n", hk->output[7]);
	printf("              +-------------------+\r\n");
	printf("\r\n");
	printf("            1       2       3       4       5       6\r\n");
	printf(" Temp:   %+4d    %+4d    %+4d    %+4d    %+4d    %+4d\r\n", hk->temp[0], hk->temp[1], hk->temp[2], hk->temp[3], hk->temp[4], hk->temp[5]);
	printf("\r\n");
	printf("          Boot   Cause    PPTm\r\n");
	printf(" Count:  %5"PRIu32"   %5x   %5u\r\n", hk->counter_boot, hk->bootcause, hk->pptmode);
	printf("\r\n");
	printf("        WDTi2c  WDTgnd WDTcsp0 WDTcsp1\r\n");
	printf(" Count:  %5"PRIu32"   %5"PRIu32"   %5"PRIu32"   %5"PRIu32"\r\n", hk->counter_wdt_i2c, hk->counter_wdt_gnd, hk->counter_wdt_csp[0], hk->counter_wdt_csp[1]);
	printf("  Left:  %5"PRIu32"   %5"PRIu32"   %5"PRIu8"   %5"PRIu8"\r\n", hk->wdt_i2c_time_left, hk->wdt_gnd_time_left, hk->wdt_csp_pings_left[0], hk->wdt_csp_pings_left[1]);
	printf("\r\n");

}

void eps_hk_vi_print_text(eps_hk_vi_t * hk) {

	unsigned int i;

	printf("Vbatt %"PRIu16"mV\r\n", hk->vbatt);
	printf("Isun %"PRIu16"mA (%.2f mW)\r\n", hk->cursun, (float) hk->cursun * (float) hk->vbatt / 1000.0);
	printf("Isys %"PRIu16"mA (%.2f mW)\r\n", hk->cursys, (float) hk->cursys * (float) hk->vbatt / 1000.0);

	/* arrays with 3 */
	float p_in, p_in_sum = 0;
	for (i = 0; i < 3; i++) {
		p_in = (float) hk->vboost[i] * (float) hk->curin[i] / 1000.0;
		p_in_sum += p_in;
		printf("boost[%u] %"PRIu16"mV %"PRIu16"mA (%.2f mW)\r\n", i + 1, hk->vboost[i], hk->curin[i], p_in);
	}

	float p_out = (float) hk->cursun * (float) hk->vbatt / 1000.0;
	printf("Input Eff: %.2f %%\r\n", (float) ((100.0 * p_out) / p_in_sum));
}

void eps_hk_out_print_text(eps_hk_out_t * hk) {

	unsigned int i;

	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		printf("OUTPUT[%u] En: %"PRIu8", Iout: %"PRIu16"mA, latchups: %"PRIu16", Ton:%"PRIu16", Toff: %"PRIu16"\r\n", i + 1, hk->output[i], hk->curout[i], hk->latchup[i], hk->output_on_delta[i], hk->output_off_delta[i]);
	}

	/* arrays with 8 */
	for (i = 6; i < 8; i++) {
		printf("OUTPUT[%u] En: %"PRIu8", Ton:%"PRIu16", Toff: %"PRIu16"\r\n", i + 1, hk->output[i], hk->output_on_delta[i], hk->output_off_delta[i]);
	}
}

void eps_hk_wdt_print_text(eps_hk_wdt_t * hk) {

	printf("        WDTi2c  WDTgnd WDTcsp0 WDTcsp1\r\n");
	printf(" Count:  %5"PRIu32"   %5"PRIu32"   %5"PRIu32"   %5"PRIu32"\r\n", hk->counter_wdt_i2c, hk->counter_wdt_gnd, hk->counter_wdt_csp[0], hk->counter_wdt_csp[1]);
	printf("  Left:  %5"PRIu32"   %5"PRIu32"   %5"PRIu8"   %5"PRIu8"\r\n", hk->wdt_i2c_time_left, hk->wdt_gnd_time_left, hk->wdt_csp_pings_left[0], hk->wdt_csp_pings_left[1]);
	printf("\r\n");
}


void eps_hk_basic_print_text(eps_hk_basic_t * hk) {
	printf("            1       2       3       4       5       6\r\n");
	printf(" Temp:   %+4d    %+4d    %+4d    %+4d    %+4d    %+4d\r\n", hk->temp[0], hk->temp[1], hk->temp[2], hk->temp[3], hk->temp[4], hk->temp[5]);
	printf("\r\n");
	printf("          Boot   Cause    PPTm\r\n");
	printf(" Count:  %5"PRIu32"   %5x   %5u\r\n", hk->counter_boot, hk->bootcause, hk->pptmode);
	printf("\r\n");
}


void eps_hk_print_text(eps_hk_t * hk) {

	unsigned int i;

	printf("Vbatt %"PRIu16"mV\r\n", hk->vbatt);
	printf("Isun %"PRIu16"mA (%.2f mW)\r\n", hk->cursun, (float) hk->cursun * (float) hk->vbatt / 1000.0);
	printf("Isys %"PRIu16"mA (%.2f mW)\r\n", hk->cursys, (float) hk->cursys * (float) hk->vbatt / 1000.0);

	/* arrays with 3 */
	float p_in, p_in_sum = 0;
	for (i = 0; i < 3; i++) {
		p_in = (float) hk->vboost[i] * (float) hk->curin[i] / 1000.0;
		p_in_sum += p_in;
		printf("boost[%u] %"PRIu16"mV %"PRIu16"mA (%.2f mW)\r\n", i + 1, hk->vboost[i], hk->curin[i], p_in);
	}

	float p_out = (float) hk->cursun * (float) hk->vbatt / 1000.0;
	printf("Input Eff: %.2f %%\r\n", (float) ((100.0 * p_out) / p_in_sum));

	/* arrays with 6 */
	for (i = 0; i < 6; i++) {
		printf("OUTPUT[%u] En: %"PRIu8", Iout: %"PRIu16"mA, latchups: %"PRIu16", Ton:%"PRIu16", Toff: %"PRIu16"\r\n", i + 1, hk->output[i], hk->curout[i], hk->latchup[i], hk->output_on_delta[i], hk->output_off_delta[i]);
	}

	/* arrays with 8 */
	for (i = 6; i < 8; i++) {
		printf("OUTPUT[%u] En: %"PRIu8", Ton:%"PRIu16", Toff: %"PRIu16"\r\n", i + 1, hk->output[i], hk->output_on_delta[i], hk->output_off_delta[i]);
	}

	printf("WDT i2c:%"PRIu32", gnd:%"PRIu32"\r\n", hk->wdt_i2c_time_left, hk->wdt_gnd_time_left);
	printf("COUNT boot:%"PRIu32", wdt_i2c_fail:%"PRIu32", wdt_gnd_fail:%"PRIu32"\r\n", hk->counter_boot, hk->counter_wdt_i2c, hk->counter_wdt_gnd);
	printf("TEMP 1:%"PRIi16", 2:%"PRIi16", 3:%"PRIi16", 4:%"PRIi16", 5:%"PRIi16", 6:%"PRIi16"\r\n", hk->temp[0], hk->temp[1], hk->temp[2], hk->temp[3], hk->temp[4], hk->temp[5]);

}

void eps_hk_1_pack(eps_hk_1_t * hk) {

	int i;
	for (i = 0; i < 3; i++)
		hk->pv[i] = csp_hton16(hk->pv[i]);
	hk->pc = csp_hton16(hk->pc);
	hk->bv = csp_hton16(hk->bv);
	hk->sc = csp_hton16(hk->sc);
	for (i = 0; i < 6; i++)
		hk->temp[i] = csp_hton16(hk->temp[i]);
	for (i = 0; i < 6; i++)
		hk->latchup[i] = csp_hton16(hk->latchup[i]);
	hk->bootcount = csp_hton16(hk->bootcount);

}

int eps_vboost_set(unsigned int vboost1, unsigned int vboost2, unsigned int vboost3) {

	uint16_t vboost[3];
	vboost[0] = csp_hton16(vboost1);
	vboost[1] = csp_hton16(vboost2);
	vboost[2] = csp_hton16(vboost3);

	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_SET_VBOOST, eps_timeout, &vboost, sizeof(vboost), NULL, 0);

}

int eps_pptmode_set(unsigned char mode) {
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_SET_PPTMODE, eps_timeout, &mode, sizeof(mode), NULL, 0);
}

int eps_hk_get(eps_hk_t * hk) {
	uint8_t req = 0;
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_HK, eps_timeout, &req, 2, hk, sizeof(eps_hk_t));
	if (status != sizeof(eps_hk_t))
		return status;
	eps_hk_unpack(hk);
	return status;
}
int eps_hk_vi_get(eps_hk_vi_t * hk) {
	uint8_t req = 1;
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_HK, eps_timeout, &req, 2, hk, sizeof(eps_hk_vi_t));
	if (status != sizeof(eps_hk_vi_t))
		return status;
	eps_hk_vi_unpack(hk);
	return status;
}
int eps_hk_out_get(eps_hk_out_t * hk) {
	uint8_t req = 2;
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_HK, eps_timeout, &req, 2, hk, sizeof(eps_hk_out_t));
	if (status != sizeof(eps_hk_out_t))
		return status;
	eps_hk_out_unpack(hk);
	return status;
}
int eps_hk_wdt_get(eps_hk_wdt_t * hk) {
	uint8_t req = 3;
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_HK, eps_timeout, &req, 2, hk, sizeof(eps_hk_wdt_t));
	if (status != sizeof(eps_hk_wdt_t))
		return status;
	eps_hk_wdt_unpack(hk);
	return status;
}
int eps_hk_basic_get(eps_hk_basic_t * hk) {
	uint8_t req = 4;
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_HK, eps_timeout, &req, 2, hk, sizeof(eps_hk_basic_t));
	if (status != sizeof(eps_hk_basic_t))
		return status;
	eps_hk_basic_unpack(hk);
	return status;
}

void eps_config2_unpack(eps_config2_t * conf2) {
	conf2->batt_maxvoltage = csp_ntoh16(conf2->batt_maxvoltage);
	conf2->batt_safevoltage = csp_ntoh16(conf2->batt_safevoltage);
	conf2->batt_normalvoltage = csp_ntoh16(conf2->batt_normalvoltage);
	conf2->batt_criticalvoltage = csp_ntoh16(conf2->batt_criticalvoltage);

}

void eps_config_unpack(eps_config_t * conf) {
	int i;

	/* arrays with 3 */
	for (i = 0; i < 3; i++) {
		conf->vboost[i] = csp_ntoh16(conf->vboost[i]);
	}

	/* arrays with 8 */
	for (i = 0; i < 8; i++) {
		conf->output_initial_on_delay[i] = csp_ntoh16(conf->output_initial_on_delay[i]);
		conf->output_initial_off_delay[i] = csp_ntoh16(conf->output_initial_off_delay[i]);
	}
}

void eps_config_pack(eps_config_t * conf) {
	int i;

	/* arrays with 3 */
	for (i = 0; i < 3; i++) {
		conf->vboost[i] = csp_hton16(conf->vboost[i]);
	}

	/* arrays with 8 */
	for (i = 0; i < 8; i++) {
		conf->output_initial_on_delay[i] = csp_hton16(conf->output_initial_on_delay[i]);
		conf->output_initial_off_delay[i] = csp_hton16(conf->output_initial_off_delay[i]);
	}
}


void eps_config2_pack(eps_config2_t * conf2) {
	conf2->batt_maxvoltage = csp_hton16(conf2->batt_maxvoltage);
	conf2->batt_safevoltage = csp_hton16(conf2->batt_safevoltage);
	conf2->batt_normalvoltage = csp_hton16(conf2->batt_normalvoltage);
	conf2->batt_criticalvoltage = csp_hton16(conf2->batt_criticalvoltage);

}

void eps_config_print(eps_config_t * conf) {
	int i;

	printf("pptmode: %"PRIu8"\r\n", conf->ppt_mode);
	printf("battheater: %"PRIu8", low: %"PRIi8", high: %"PRIi8"\r\n", conf->battheater_mode, conf->battheater_low, conf->battheater_high);
	for (i = 0; i < 3; i++)
		printf("vboost[%u]: %"PRIu16"\r\n", i, conf->vboost[i]);
	for (i = 0; i < 8; i++)
		printf("output[%u]: ondelay: %"PRIu16", offdelay: %"PRIu16", normal mode: %"PRIu8", safe mode %"PRIu8"\r\n", i, conf->output_initial_on_delay[i], conf->output_initial_off_delay[i], conf->output_normal_value[i], conf->output_safe_value[i]);

}

void eps_config2_print(eps_config2_t * conf2) {

	printf("Batt max volt      %"PRIu16" mV\r\n",conf2->batt_maxvoltage);
	printf("Batt normal volt   %"PRIu16" mV\r\n",conf2->batt_normalvoltage);
	printf("Batt safe volt     %"PRIu16" mV\r\n",conf2->batt_safevoltage);
	printf("Batt critical volt %"PRIu16" mV\r\n",conf2->batt_criticalvoltage);
}

int eps_config_get(eps_config_t * config) {
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_CONFIG_GET, eps_timeout, NULL, 0, config, sizeof(eps_config_t));
	if (status < 0)
		return -1;
	eps_config_unpack(config);
	return status;
}
int eps_config_set(eps_config_t * config) {
	eps_config_pack(config);
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_CONFIG_SET, eps_timeout, config, sizeof(eps_config_t), NULL, 0);
}
int eps_config_cmd(uint8_t cmd) {
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_CONFIG_CMD, eps_timeout, &cmd, 1, NULL, 0);
}


int eps_config2_get(eps_config2_t * config2) {
	int status = csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_CONFIG2_GET, 5000, NULL, 0, config2, sizeof(eps_config2_t));
	if (status < 0)
		return -1;
	eps_config2_unpack(config2);
	return status;
}
int eps_config2_set(eps_config2_t * config2) {
	eps_config2_pack(config2);
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_CONFIG2_SET, 5000, config2, sizeof(eps_config2_t), NULL, 0);
}
int eps_config2_cmd(uint8_t cmd) {
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_CONFIG2_CMD, 5000, &cmd, 1, NULL, 0);
}


int eps_output_set(uint8_t output) {
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_SET_OUTPUT, 0, &output, 1, NULL, 0);
}

int eps_output_set_single(uint8_t channel, uint8_t mode, int16_t delay) {
	eps_output_set_single_req req;
	req.channel=channel;
	req.mode = mode;
	req.delay = csp_hton16(delay);
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_SET_SINGLE_OUTPUT, 0, &req, 4, NULL, 0);
}

int eps_counters_reset(void) {
	uint8_t magic = 0x42;
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_RESET_COUNTERS, 0, &magic, 1, NULL, 0);
}

int eps_hardreset(void) {
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_HARDRESET, 0, NULL, 0, NULL, 0);
}

int eps_wdt_gnd_reset(void) {
	uint8_t magic = 0x78;
	return csp_transaction(CSP_PRIO_HIGH, eps_node, EPS_PORT_RESET_WDT_GND, 0, &magic, 1, NULL, 0);
}
