#include <io/bpx.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

static uint8_t node_bpx = NODE_BPX;


void bpx_node_set(uint8_t node) {
	node_bpx = node;
}


int bpx_counters_reset(void) {
	uint8_t magic = 0x42;
	return csp_transaction(CSP_PRIO_HIGH, node_bpx, BPX_PORT_RESET_COUNTERS, 0, &magic, 1, NULL, 0);
}

int bpx_adc_get(bpx_adc_t * adc) {

	int status = csp_transaction(CSP_PRIO_NORM, node_bpx, BPX_PORT_ADC, 5000, NULL, 0, adc, sizeof(*adc));
	for (int i = 0; i < 5; i++)
		adc->adc[i] = csp_ntoh16(adc->adc[i]);
	return status;

}

int bpx_hk_get(bpx_hk_t * hk) {

	int status = csp_transaction(CSP_PRIO_NORM, node_bpx, BPX_PORT_HK, 5000, NULL, 0, hk, sizeof(*hk));
	hk->cur_charge = csp_ntoh16(hk->cur_charge);
	hk->cur_discharge = csp_ntoh16(hk->cur_discharge);
	hk->vbatt = csp_ntoh16(hk->vbatt);
	hk->pt100 = csp_ntoh16(hk->pt100);
	hk->lm60 = csp_ntoh16(hk->lm60);
	hk->counter_boot = csp_ntoh32(hk->counter_boot);
	return status;

}

int bpx_cal_set(bpx_calibration_t * cal) {
	return csp_transaction(CSP_PRIO_NORM, node_bpx, BPX_PORT_CAL, 5000, cal, sizeof(*cal), NULL, 0);
}
