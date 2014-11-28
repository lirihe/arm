#include <math.h>
#include <inttypes.h>
#include <io/bpx.h>
#include <util/console.h>
#include <util/log.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int cmd_bpx_counters_reset(struct command_context *ctx) {
	bpx_counters_reset();
	return CMD_ERROR_NONE;
}


int cmd_bpx_node_set(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	uint8_t node = atoi(ctx->argv[1]);
	bpx_node_set(node);
	return CMD_ERROR_NONE;
}

int cmd_bpx_calib_vbat(struct command_context *ctx) {
	unsigned int V1 = 6000;
	unsigned int V2 = 10000;
	unsigned int Vm1, Vm2;
	bpx_adc_t adc;
	bpx_calibration_t calib;
	char dummy;

	calib.id = BPX_CAL_VBATT;

	printf("Calibrate Vbatt:\r\nSet vbat to %u mV\r\nPress key when done (q to quit)\r\n",V1);
	dummy = getchar();
	if (dummy == 'q') 	return CMD_ERROR_NONE;
	if (bpx_adc_get(&adc) < 0) 	return CMD_ERROR_FAIL;
	Vm1 = adc.vbatt;

	printf("Set vbat to %u mV\r\nPress key when done (q to quit)\r\n",V2);
	dummy = getchar();
	if (dummy == 'q') 	return CMD_ERROR_NONE;
	if (bpx_adc_get(&adc) < 0) 	return CMD_ERROR_FAIL;
	Vm2 = adc.vbatt;

	// V = aVm+b
	calib.a = ((float)V1 - (float)V2)/((float)Vm1 - (float)Vm2);
	calib.b = V1-calib.a*Vm1;
	if (bpx_cal_set(&calib) < 0) return CMD_ERROR_FAIL;
	printf("Set vbat gain=%f and offset=%f\r\n",calib.a, calib.b);

	return CMD_ERROR_NONE;
}


int cmd_bpx_calib_idischarge(struct command_context *ctx) {
	unsigned int I1 = 30;
	unsigned int I2 = 500;
	unsigned int Im1, Im2;
	bpx_adc_t adc;
	bpx_calibration_t calib;
	char dummy;

	calib.id = BPX_CAL_DISCHARGE;

	printf("Calibrate I discharge:\r\nSet discharge to %u mA\r\nPress key when done (q to quit)\r\n",I1);
	dummy = getchar();
	if (dummy == 'q') 	return CMD_ERROR_NONE;
	if (bpx_adc_get(&adc) < 0) 	return CMD_ERROR_FAIL;
	Im1 = adc.cur_discharge;

	printf("Set I discharge to %u mA\r\nPress key when done (q to quit)\r\n",I2);
	dummy = getchar();
	if (dummy == 'q') 	return CMD_ERROR_NONE;
	if (bpx_adc_get(&adc) < 0) 	return CMD_ERROR_FAIL;
	Im2 = adc.cur_discharge;

	// V = aVm+b
	calib.a = ((float)I1 - (float)I2)/((float)Im1 - (float)Im2);
	calib.b = I1-calib.a*Im1;
	if (bpx_cal_set(&calib) < 0) return CMD_ERROR_FAIL;
	printf("Set i discharge gain=%f and offset=%f\r\n",calib.a, calib.b);

	return CMD_ERROR_NONE;
}


int cmd_bpx_calib_icharge(struct command_context *ctx) {
	unsigned int I1 = 30;
	unsigned int I2 = 500;
	unsigned int Im1, Im2;
	bpx_adc_t adc;
	bpx_calibration_t calib;
	char dummy;

	calib.id = BPX_CAL_CHARGE;

	printf("Calibrate I charge:\r\nSet charge to %u mA\r\nPress key when done (q to quit)\r\n",I1);
	dummy = getchar();
	if (dummy == 'q') 	return CMD_ERROR_NONE;
	if (bpx_adc_get(&adc) < 0) 	return CMD_ERROR_FAIL;
	Im1 = adc.cur_charge;

	printf("Set I charge to %u mA\r\nPress key when done (q to quit)\r\n",I2);
	dummy = getchar();
	if (dummy == 'q') 	return CMD_ERROR_NONE;
	if (bpx_adc_get(&adc) < 0) 	return CMD_ERROR_FAIL;
	Im2 = adc.cur_charge;

	// V = aVm+b
	calib.a = ((float)I1 - (float)I2)/((float)Im1 - (float)Im2);
	calib.b = I1-calib.a*Im1;
	if (bpx_cal_set(&calib) < 0) return CMD_ERROR_FAIL;
	printf("Set i charge gain=%f and offset=%f\r\n",calib.a, calib.b);

	return CMD_ERROR_NONE;
}


int cmd_bpx_calib_pt100(struct command_context *ctx) {
	signed int T1 = 30;
	signed int T2 = 500;
	signed int Tm1, Tm2;
	bpx_hk_t hk;
	bpx_calibration_t calib;
	calib.id = BPX_CAL_PT100;
	calib.a = 1;
	calib.b = 0;
	if (bpx_cal_set(&calib) < 0) return CMD_ERROR_FAIL;


	printf("Calibrate PT100:\r\n Input first (lowest) temperature (deg C)\r\n");
	scanf("%d",&T1);
	if (bpx_hk_get(&hk) < 0)
		return CMD_ERROR_FAIL;
	Tm1 =  hk.pt100;

	printf("\r\nInput second (higher) temperature (deg C)\r\n");
	scanf("%d",&T2);
	if (bpx_hk_get(&hk) < 0)
		return CMD_ERROR_FAIL;
	Tm2 =  hk.pt100;

	// V = aVm+b
	calib.a = ((float)T1 - (float)T2)/((float)Tm1 - (float)Tm2);
	calib.b = T1-calib.a*Tm1;
	if (bpx_cal_set(&calib) < 0) return CMD_ERROR_FAIL;
	printf("Set pt100 gain=%f and offset=%f\r\n",calib.a, calib.b);

	return CMD_ERROR_NONE;
}


int cmd_bpx_adc_get(struct command_context *ctx) {
	bpx_adc_t adc;
	if (bpx_adc_get(&adc) < 0)
		return CMD_ERROR_FAIL;
	printf("-- RAW ADC VALUES --\r\n");
	printf("Charge    %u mV\r\n", adc.cur_charge);
	printf("Discharge %u mV\r\n", adc.cur_discharge);
	printf("Batt      %u mV\r\n", adc.vbatt);
	printf("PT100     %u mV\r\n", adc.pt100);
	printf("LM60      %u mV\r\n", adc.lm60);

	printf("Converted PT100\r\n");

	float gain = 11;
	float vcc = 3.300;
	float ref = 2000;
	float r0 = 100;
	float a = 3.95E-3;
	float b = -5.8E-7;
	float v = adc.pt100 / 1000.0;
	float r = (v / (gain * vcc)) * ref / (1 - v / (gain * vcc));
	float t = (-r0 * a + sqrtf((r0 * a) * (r0 * a) - 4 * r0 * b * (r0 - r))) / (2 * r0 * b);

	printf("v %f\r\n", v);
	printf("r %f\r\n", r);
	printf("t %f\r\n", t);

	printf("Converted LM60\r\n");

	float t2 = (adc.lm60 - 424) / 6.25;
	printf("t2 %f\r\n", t2);

	return CMD_ERROR_NONE;
}

int cmd_bpx_hk_get(struct command_context *ctx) {
	bpx_hk_t hk;
	if (bpx_hk_get(&hk) < 0)
		return CMD_ERROR_FAIL;
	printf("Charge    %"PRIu16" mA\r\n", hk.cur_charge);
	printf("Discharge %"PRIu16" mA\r\n", hk.cur_discharge);
	printf("Batt      %"PRIu16" mV\r\n", hk.vbatt);
	printf("PT100     %"PRId16" degC\r\n", hk.pt100);
	printf("LM60      %"PRId16" degC\r\n", hk.lm60);
	printf("bootcause %"PRIu8" \r\n", hk.bootcause);
	printf("bootcount %"PRIu32" \r\n", hk.counter_boot);
	return CMD_ERROR_NONE;
}

command_t __sub_command bpx_calib_subcommands[] = {
	{
		.name = "vbat",
		.help = "Calib vbat",
		.handler = cmd_bpx_calib_vbat,
	},{
		.name = "idischarge",
		.help = "Calib idischarge",
		.handler = cmd_bpx_calib_idischarge,
	},{
		.name = "icharge",
		.help = "Calib icharge",
		.handler = cmd_bpx_calib_icharge,
	},{
			.name = "pt100",
			.help = "Calib pt100",
			.handler = cmd_bpx_calib_pt100,
		}
};

command_t __sub_command bpx_subcommands[] = {
	{
		.name = "adc",
		.help = "Get ADC values",
		.handler = cmd_bpx_adc_get,
	},{
		.name = "resetcounters",
		.help = "Reset all counters to zero",
		.handler = cmd_bpx_counters_reset,
	},{
		.name = "hk",
		.help = "Get HK values",
		.handler = cmd_bpx_hk_get,
	},{
		.name = "calib",
		.help = "Calibrate",
		.chain = INIT_CHAIN(bpx_calib_subcommands),
	},{
		.name = "node",
		.help = "Set client BPX csp/i2c add",
		.usage = "<add>",
		.handler = cmd_bpx_node_set,
	}
};

command_t __root_command bpx_commands[] = {
	{
		.name = "bpx",
		.help = "BPX subsystem",
		.chain = INIT_CHAIN(bpx_subcommands),
	}
};

void cmd_bpx_setup(void) {
	command_register(bpx_commands);
}
