/*
 * pwm.c
 *
 *  Created on: Aug 24, 2009
 *      Author: karl
 */
#include <stdint.h>
#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <dev/arm/at91sam7.h>
#include <dev/pwm.h>


// Global variable: indicates the run status of the pwm
// [x x x x  x pwm2 pwm1 pwm0]
// 0=off, 1=on
uint8_t arm_pwm_running_status;
uint8_t arm_pwm_duty_notzero = 7;

xSemaphoreHandle pwm_fall_sem;

static void __attribute__((noinline)) pwm_DSR_oneshot() {
	unsigned int pwm_status;
	pwm_status = PWM_SR;
	if(pwm_status&0x01){ // PWM0 generated interrupt
		PWM_CSR = 0x01; //Clear interrupt
		pwm_disable(0); //Disable the channel
		//printf("Fe 0\r\n");
		arm_pwm_running_status &= ~(1<<0);  //Set pwm status low
	}
	if(pwm_status&0x04){ // PWM0 generated interrupt
		PWM_CSR = 0x04; //Clear interrupt
		pwm_disable(1); //Disable the channel
		//printf("Fe 1\r\n");
		arm_pwm_running_status &= ~(1<<1);  //Set pwm status low
	}
	if(pwm_status&0x10){ // PWM0 generated interrupt
		PWM_CSR = 0x10; //Clear interrupt
		//pwm_disable(2); //Disable the channel
		//printf("Fe 2\r\n");
		arm_pwm_running_status &= ~(1<<2);  //Set pwm status low
	}


}


static void __attribute__((noinline)) pwm_DSR_fall() {
	unsigned int pwm_status;
	static signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	pwm_status = PWM_SR;
	if(pwm_status&0x01){ // PWM0 generated interrupt
		PWM_CSR = 0x01; //Clear interrupt
		arm_pwm_running_status &= ~(1<<0);  //Set pwm status low
	}
	if(pwm_status&0x04){ // PWM0 generated interrupt
		PWM_CSR = 0x04; //Clear interrupt
		arm_pwm_running_status &= ~(1<<1);  //Set pwm status low
	}
	if(pwm_status&0x10){ // PWM0 generated interrupt
		PWM_CSR = 0x10; //Clear interrupt
		arm_pwm_running_status &= ~(1<<2);  //Set pwm status low
	}

	if (arm_pwm_running_status == 0)
	{
		arm_pwm_running_status = 7;
		xSemaphoreGiveFromISR( pwm_fall_sem, &xHigherPriorityTaskWoken );
		printf("l");
	}
}

static void __attribute__((noinline)) pwm_DSR_rise() {
	unsigned int pwm_status;
	pwm_status = PWM_SR;
	if(pwm_status&0x02){ // PWM0 generated interrupt
		PWM_CSR = 0x02; //Clear interrupt
		arm_pwm_running_status |= (1<<0);  //Set pwm status high
	}
	if(pwm_status&0x08){ // PWM0 generated interrupt
		PWM_CSR = 0x08; //Clear interrupt
		arm_pwm_running_status |= (1<<1);  //Set pwm status high
	}
	if(pwm_status&0x20){ // PWM0 generated interrupt
		PWM_CSR = 0x20; //Clear interrupt
		arm_pwm_running_status |= (1<<2);  //Set pwm status high
	}
}

static void __attribute__((naked)) pwm_ISR_oneshot(void) {
	portSAVE_CONTEXT();
	pwm_DSR_oneshot();
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

static void __attribute__((naked)) pwm_ISR_fall(void) {
	portSAVE_CONTEXT();
	pwm_DSR_fall();
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}


static void __attribute__((naked)) pwm_ISR_rise(void) {
	portSAVE_CONTEXT();
	pwm_DSR_rise();
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

// Interrupt on fall flanke (ie when pwm is OFF)
void pwm_init_interrupt_fall(void) {
	__attribute__ ((unused)) unsigned int pwm_status;
	pwm_status = PWM_SR;
	PWM_IDR = 0xFF; // Disable all interrupts
	PWM_IER |= 0x15; // Enable PSTA2, PSTA1, PSTA0 int
	// Register interupt service routine
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_PWM,
			AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,
			(void(*)(void)) pwm_ISR_fall);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_PWM);
	// setup semaphore
	vSemaphoreCreateBinary(pwm_fall_sem);
}

// Interrupt on rising flanke (ie when pwm is ON)
void pwm_init_interrupt_rise(void) {
	__attribute__ ((unused)) unsigned int pwm_status;
	pwm_status = PWM_SR;
	PWM_IDR = 0xFF; // Disable all interrupts
	PWM_IER |= 0x2A; // Enable PEND2, PEND1, PEND0 int
	// Register interupt service routine
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_PWM,
			AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,
			(void(*)(void)) pwm_ISR_rise);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_PWM);
}


void pwm_init_autooff(void) {
	__attribute__ ((unused)) unsigned int pwm_status;
	pwm_status = PWM_SR;
	PWM_IDR = 0xFF; // Disable all interrupts
	PWM_IER = 0x15; // Enable PSTA2, PSTA1, PSTA0 int
	// Register interupt service routine
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_PWM,
			AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,
			(void(*)(void)) pwm_ISR_oneshot);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_PWM);
}

void pwm_init(void) {
	UPIO_ECR = 1;
	//UPIO_CR =  1;
	UPIO_MDDR = 0;
	UPIO_OER = 0x0000007E;

	PWM_ECR = 0x00000003;
	PWM_CR = 0x00000001;
	PWM_PDR = 0x00070000;
	PWM_DCR = 0x00000001;

	PWM_CSR = 0xFFFFFFFF;

	PWM_DLY_0 = 0;
	PWM_PUL_0 = 65535;
	PWM_DLY_1 = 0;
	PWM_PUL_1 = 65535;
	PWM_DLY_2 = 0;
	PWM_PUL_2 = 65535;

	PWM_MR = 0x101F1F1F;

	arm_pwm_running_status = 0;
}

int pwm_set_freq(int channel, int freq, int core_clk) {
	int tmp;
	int pres = 0;
	int mask = 0x0F;
	if (channel > 2)
		return -1;
	if (freq<=0)
		return -1;

	tmp = (core_clk / (freq * 65535));

	while (tmp > (1 << pres)) {
		pres++;
		if (pres > 16)
			return -1;
	}

	pres--;
	pres &= 0x0F;
	PWM_MR = (PWM_MR & ~(mask << (8 * channel))) | (pres << (8 * channel));// | (0x1 << (4+8*channel));
	PWM_MR = PWM_MR & 0xFFEFEFEF;
	return (core_clk / (1 << (pres-1))) / 65535;
}

void pwm_set_duty(int channel, uint16_t duty) {
	if (channel > 2)
		return;
/*    unsigned int tmp;
	tmp = 65535-duty;
*/

	//Set pwm status low if duty cycle is zero, as it will not create a pulse
	if (duty==0)
	{
		arm_pwm_duty_notzero &= ~(1<<channel);
		arm_pwm_running_status &= ~(1<<channel);
	}
	else
		arm_pwm_duty_notzero |= (1<<channel);


	*((&PWM_PUL_0) + (2 * channel)) = 65535 - duty;
	*((&PWM_DLY_0) + (2 * channel)) = duty;
}

void pwm_set_dir(int channel, uint8_t dir) {
	uint16_t pwm_tmp1, pwm_tmp2;
	if (channel > 2)
		return;
	// first take backup of current duty cycle
	pwm_tmp1 = *((&PWM_DLY_0) + (2 * channel));
	pwm_tmp2 = *((&PWM_PUL_0) + (2 * channel));
	// Set duty to zero (this is to avoid shoot-through)
	*((&PWM_PUL_0) + (2 * channel)) = 65535;
	*((&PWM_DLY_0) + (2 * channel)) = 0;
	// and wait a bit
	vTaskDelay(1);
	// Now we can change direction
	if (dir) {
		UPIO_SODR = 1 << ((channel << 1) + 2);
	} else {
		UPIO_CODR = 1 << ((channel << 1) + 2);
	}
	// and restore old dutycycle
	*((&PWM_DLY_0) + (2 * channel)) = pwm_tmp1;
	*((&PWM_PUL_0) + (2 * channel)) = pwm_tmp2;
}

void pwm_set_dir_fromzero(int channel, uint8_t dir) {
	if (channel > 2)
		return;
	if (dir) {
		UPIO_SODR = 1 << ((channel << 1) + 2);
	} else {
		UPIO_CODR = 1 << ((channel << 1) + 2);
	}
}


void pwm_enable(int channel) {
	if (channel > 2)
		return;

	UPIO_SODR = (1 << ((channel << 1) + 1));
//	PWM_CR |= (1 << ((channel << 1) + 1));
	PWM_CR = (1 << ((channel << 1) + 1));
}

void pwm_disable(int channel) {
	if (channel > 2)
		return;

	UPIO_CODR = (1 << ((channel << 1) + 1));
//	PWM_CR |= (1 << ((channel << 1) + 1));
	PWM_CR = (1 << ((channel << 1) + 2));
}

unsigned int pwm_status(void)
{	unsigned int tmp;
	tmp = PWM_MR;

	return ((tmp>>4)&1)|((tmp>>12)&1)|((tmp>>20)&1);
}
unsigned int pwm_status2(void)
{
	unsigned int pwm_status;
	pwm_status = PWM_SR;
	if(pwm_status&0x01){ // PWM0 generated interrupt
		PWM_CSR = 0x01; //Clear interrupt
		arm_pwm_running_status &= ~(1<<0);  //Set pwm status low
	}
	if(pwm_status&0x04){ // PWM0 generated interrupt
		PWM_CSR = 0x04; //Clear interrupt
		arm_pwm_running_status &= ~(1<<1);  //Set pwm status low
	}
	if(pwm_status&0x10){ // PWM0 generated interrupt
		PWM_CSR = 0x10; //Clear interrupt
		arm_pwm_running_status &= ~(1<<2);  //Set pwm status low
	}
	return 0;
}


void pwm_enable_duty(int channel, float duty){
	int iduty;
	if(duty>0.00){

		pwm_set_dir(channel, 0);
	} else {
		pwm_set_dir(channel, 1);
	}
	if (abs(duty)>100.0)
		iduty = 65535;
	else
		iduty = abs(duty)/100.0*65535;
	pwm_set_duty(channel,iduty);
 }

void pwm_set_duty_fromzero(int channel, float duty){
	int iduty;
	if(duty>0.00){

		pwm_set_dir_fromzero(channel, 0);
	} else {
		pwm_set_dir_fromzero(channel, 1);
	}
	if (abs(duty)>100.0)
		iduty = 65535;
	else
		iduty = abs(duty)/100.0*65535;
	pwm_set_duty(channel,iduty);
 }
