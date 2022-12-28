/*
 * hw.h
 *
 *  Created on: May 16, 2021
 *      Author: lukgd
 */

#ifndef HW_H_
#define HW_H_

//LEDs RGB functions
void hw_set_state_led_rgb(bool state_r, bool state_g, bool state_b);
void hw_set_state_led_red(bool state_r);
void hw_set_state_led_green(bool state_g);
void hw_set_state_led_blue(bool state_b);
void hw_set_state_user_led(bool state_user);
void hw_toggle_led_red(void);
void hw_toggle_led_green(void);
void hw_toggle_led_blue(void);
void hw_toggle_user_led(void);
void hw_pwm_SSR1_start();
void hw_pwm_SSR1_set(uint16_t PWM);
void hw_uart_tx(uint8_t *buffer, uint32_t size);
uint32_t hw_time_ms_get(void);

#endif /* HW_H_ */
