/*
 * pwm.h
 *
 *  Created on: Aug 24, 2009
 *      Author: karl
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

/**
 * @file
 * @brief Handles PWM
 * \addtogroup PWM
 *
 * @{
 */


// Global variable: indicates the run status of the pwm
// [x x x x  x pwm2 pwm1 pwm0]
// 0=off, 1=on
extern uint8_t arm_pwm_running_status;
extern uint8_t arm_pwm_duty_notzero;
/**
 * initialise PWM outputs
 */
void pwm_init(void);

/**
 * Enable oneshot PWM driver
 */
void pwm_init_autooff(void);

/**
 * Enable falling interrupt PWM driver
 */
void pwm_init_interrupt_fall(void);

/**
 * Enable rising interrupt PWM driver
 */
void pwm_init_interrupt_rise(void);

/**
 * get status of pwm pins [x x x x  x 2 1 0]
 */
unsigned int pwm_status(void);

/**
 * get status of pwm pins [x x x x  x 2 1 0]
 */
unsigned int pwm_status2(void);

/**
 * Set changing frequency for PWM channel
 * NOTE: This is actually the 1/2 frequency of the signal
 * ie. if set to 2Hz, the signal with change 2 times pr. second and have a timeperiod of 1s
 * @param channel Channel number
 * @param freq Requested frequency in Hz
 * @param core_clk CPU core clock in Hz
 * @return Actual achieved PWM frequency
 */
int pwm_set_freq(int channel, int freq, int core_clk);

/**
 * Set duty cycle for channel
 * @param Channel number
 * @param duty Duty cycle, 0=0%, 65535=100%
 */
void pwm_set_duty(int channel, uint16_t duty);

/**
 * Set polarity for PWM channel
 * @param Channel number
 * @param dir Polarity, 0=positive, 1=negative
 */
void pwm_set_dir(int channel, uint8_t dir);

/**
 * Enable PWM output
 * @param channel Channel number
 */
void pwm_enable(int channel);

/**
 * Enable PWM output
 * @param channel Channel number
 */
void pwm_disable(int channel);

/**
 * }@
 */
void pwm_enable_duty(int channel, float duty);


void pwm_set_duty_fromzero(int channel, float duty);


#endif /* PWM_H_ */
