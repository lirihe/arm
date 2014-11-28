/**
 * cmd_periph.c
 *
 *  Created on: 16/12/2009
 */

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <dev/i2c.h>
#include <dev/pwm.h>
#include <dev/adc.h>
#include <dev/cpu.h>
#include <dev/usart.h>
#include <dev/arm/flash.h>
#include <dev/magnetometer.h>
#include <dev/gyro.h>
#include <dev/arm/cpu_pm.h>

#include <util/console.h>
#include <util/error.h>
#include <util/test_malloc.h>

#include <io/nanomind.h>

int adc_test(struct command_context *ctx) {
	uint16_t * adc_buffer;
	int i;
	adc_buffer = adc_start_blocking(1);

	printf("Temperature 0: %u %.0fmv %.1f C\n\r", adc_buffer[0], (adc_buffer[0] * 2493.0) / 1023.0, ((((adc_buffer[0] * 2493.0) / 1023) - 424) / 6.25));
	printf("Temperature 1: %u %.0fmv %.1f C\n\r", adc_buffer[1], (adc_buffer[1] * 2493.0) / 1023.0, ((((adc_buffer[1] * 2493.0) / 1023) - 424) / 6.25));

	for (i = 2; i < 8; i++) {
		printf("Channel %d: %dmV raw %#x\n\r", i, (adc_buffer[i] * 2493) / 1023, adc_buffer[i]);
	}

	return CMD_ERROR_NONE;

}

int adc_test_cont(struct command_context *ctx) {
	uint16_t * adc_buffer;
	int i;
	while (1) {
		if (usart_messages_waiting(USART_CONSOLE) != 0)
			break;

		adc_buffer = adc_start_blocking(1);

		for (i = 0; i < 8; i++) {
			printf(" %d,", adc_buffer[i]);
		}
		printf("\n\r");
		vTaskDelay(configTICK_RATE_HZ * 0.100);
	}

	return CMD_ERROR_NONE;
}

int pwm_test(struct command_context *ctx) {

	char * args = command_args(ctx);
	int dutycycle;
	int frequency;
	int i;
	int f;

	if (sscanf(args, "%u %u", &dutycycle,&frequency) != 2)
		return CMD_ERROR_SYNTAX;

	unsigned int duty16bit;
	for (i = 0; i < 3; i++) {
	if(dutycycle>0){
			pwm_set_dir(i, 0);
		} else {
			pwm_set_dir(i, 1);
		}
	}

	if (dutycycle >= 0)
		duty16bit = 65535 * (dutycycle / 100.0);
	else
		duty16bit = 65535 * (-dutycycle / 100.0);

	printf("Setting dutycycle to %u %% [%u]\r\n", dutycycle, duty16bit);


	pwm_init();
	for (i = 0; i < 3; i++) {
		f = pwm_set_freq(i, frequency, 40000000);
		pwm_enable(i);
		//pwm_init_autooff();
		pwm_set_duty(i, duty16bit);
	}
	printf("PWM frequency requesed %d: got %d Hz\n\r",frequency, f);

	return CMD_ERROR_NONE;

}





 int pwm_test_single(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int dutycycle;
	unsigned int channel;
	if (sscanf(args, "%u %u", &channel, &dutycycle) != 1)
		return CMD_ERROR_SYNTAX;

	unsigned int duty16bit;

	if (channel > 2)
		return CMD_ERROR_SYNTAX;


	if (dutycycle == 0)
		duty16bit = 0;
	else
		duty16bit = 65535 * (dutycycle / 100.0);

	printf("Setting channel %u dutycycle to %u %% [%u]\r\n", channel, dutycycle, duty16bit);

	pwm_init();
	pwm_disable(channel);
	pwm_disable(channel);
	pwm_disable(channel);

	pwm_set_freq(channel, 100, 40000000);
	pwm_enable(channel);
	pwm_set_duty(channel, duty16bit);

	return CMD_ERROR_NONE;

}

int pwm_dir(struct command_context *ctx) {
	int i;
	static uint8_t d = 0;
	d++;
	for (i = 0; i < 3; i++)
		pwm_set_dir(i, d & 0x01);

	return CMD_ERROR_NONE;
}

int stack_overflow(struct command_context *ctx) {

	#include <string.h>
	char buf[4096*4];
	memset(buf, 0, 4096*4);
	printf("Wee\r\n");

	return CMD_ERROR_NONE;

}

int cpu_pll40(struct command_context *ctx) {
	printf("Setting PLL40 goto baud 500000\r\n");
	cpu_pm_pll40();
	return CMD_ERROR_NONE;
}

int cpu_pll16(struct command_context *ctx) {
	printf("Setting PLL16 goto baud 500000\r\n");
	cpu_pm_pll16();
	return CMD_ERROR_NONE;
}

int cpu_mck(struct command_context *ctx) {
	printf("Setting MCK goto baud 38400\r\n");
	cpu_pm_mck();
	return CMD_ERROR_NONE;
}

int cpu_slm(struct command_context *ctx) {
	printf("Setting slow-motion goto baud 9600\r\n");
	cpu_pm_slm();
	return CMD_ERROR_NONE;
}




int cmd_mag_setgain(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int gain;
	if (sscanf(args, "%u", &gain ) != 1)
	  return CMD_ERROR_SYNTAX;

	if (gain > 7)
	  return CMD_ERROR_SYNTAX;

	if (mag_set_conf(mag_get_rate(), mag_get_meas(), (mag_gain_t)gain) == E_NO_ERR)
		return CMD_ERROR_NONE;
	else
		return CMD_ERROR_FAIL;
}

int cmd_mag_setrate(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int rate;
	if (sscanf(args, "%u", &rate ) != 1)
	  return CMD_ERROR_SYNTAX;

	if (rate > 6)
	  return CMD_ERROR_SYNTAX;

	if (mag_set_conf((mag_rate_t)rate, mag_get_meas(), mag_get_gain()) == E_NO_ERR)
		return CMD_ERROR_NONE;
	else
		return CMD_ERROR_FAIL;
}


int cmd_mag_init(struct command_context *ctx) {
	if (mag_init_force())
		return CMD_ERROR_NONE;
	else
		return CMD_ERROR_FAIL;
}

struct command adc_commands[] = {
	{
		.name = "test",
		.help = "ADC test",
		.handler = adc_test,
	},{
		.name = "cont",
		.help = "ADC test (cont)",
		.handler = adc_test_cont,
	}
};

struct command pwm_commands[] = {
	{
		.name = "init",
		.help = "PWM init",
		.usage = "<dutycycle> <frequency>",
		.handler = pwm_test,
	},{
		.name = "dir",
		.help = "PWM change dir",
		.handler = pwm_dir,
	},
};

struct command mon_commands[] = {
	{
		.name = "read",
		.help = "Magnetometer read",
		.handler = mag_test_single,
	},{
		.name = "read_alt",
		.help = "Magnetometer read alternative",
		.handler = mag_test_single_alt,
	},{
		.name = "loop",
		.help = "Magnetometer read in a loop",
		.handler = mag_loop,
	},{
		.name = "init",
		.help = "Magnetometer init",
		.handler = cmd_mag_init,
	},{
		.name = "loop_noformat",
		.help = "Magnetometer read in a loop (not formatted)",
		.handler = mag_loop_noformat,
	},{
		.name = "loop_fast",
		.help = "Magnetometer read in a loop (fast)",
		.handler = mag_loop_fast,
	},{
		.name = "loop_raw",
		.help = "Magnetometer read in a loop (raw output)",
		.handler = mag_loop_raw,
	},{
		.name = "status",
		.help = "Read magnetometer status registers",
		.handler = mag_get_info,
	},{
		.name = "bias",
		.help = "Test magnetometer bias",
		.handler = mag_test_bias,
	},{
		.name = "setgain",
		.help = "Set magnetometer gain",
		.usage = "GAIN0.7 = 0  ... GAIN6.5 = 7",
		.handler = cmd_mag_setgain,
	},{
		.name = "setrate",
		.help = "Set magnetometer rate",
		.usage = "RATE0.5 = 0  ... RATE50 = 6",
		.handler = cmd_mag_setrate,
	}
};

struct command cpu_commands[] = {
	{
		.name = "slm",
		.help = "CPU speed slow-motion",
		.handler = cpu_slm,
	},{
		.name = "mck",
		.help = "CPU speed MCK",
		.handler = cpu_mck,
	},{
		.name = "pll40",
		.help = "CPU speed PLL40",
		.handler = cpu_pll40,
	},{
		.name = "pll16",
		.help = "CPU speed PLL16",
		.handler = cpu_pll16,
	},
};

struct command __root_command periph_commands[] = {
	{
		.name = "stackoverflow",
		.help = "Test Stackoverflow",
		.handler = stack_overflow,
	},{
		.name = "adc",
		.help = "ADC commands",
		.chain = INIT_CHAIN(adc_commands),
	},{
		.name = "pwm",
		.help = "PWM commands",
		.chain = INIT_CHAIN(pwm_commands),
	},{
		.name = "mag",
		.help = "Magnetometer commands",
		.chain = INIT_CHAIN(mon_commands),
	},{
		.name = "cpu",
		.help = "CPU commands",
		.chain = INIT_CHAIN(cpu_commands),
	},
};

void cmd_periph_setup(void) {
	command_register(periph_commands);
}

