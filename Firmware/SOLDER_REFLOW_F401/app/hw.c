/*
 * hw.c
 *
 *  Created on: May 16, 2021
 *      Author: lukgd
 */


#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "hw.h"
#include "app.h"

//Defines SSR1
//PWM - TIMER E CHANNELS
extern TIM_HandleTypeDef htim3;
#define HW_TIMER_PORT      htim3
#define TIM_CHANNEL      TIM_CHANNEL_4

//Defines Serial
extern UART_HandleTypeDef huart1;
//##############################################################################################//
//Button configuration
//started app
bool hw_started = false;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BUTTON_Pin)
	{
		hw_started = !hw_started;
		app_started_go(hw_started);
	}

}

//##############################################################################################//
//LEDs RGB configuration

void hw_set_state_led_rgb(bool state_r, bool state_g, bool state_b)
{
	GPIO_PinState led_r_state = state_r ? GPIO_PIN_SET : GPIO_PIN_RESET;
	GPIO_PinState led_g_state = state_g ? GPIO_PIN_SET : GPIO_PIN_RESET;
	GPIO_PinState led_b_state = state_b ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, led_r_state);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, led_g_state);
	HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, led_b_state);
}

void hw_set_state_led_red(bool state_r)
{
	GPIO_PinState led_r_state = state_r ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, led_r_state);
}

void hw_set_state_led_green(bool state_g)
{
	GPIO_PinState led_g_state = state_g ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, led_g_state);
}

void hw_set_state_led_blue(bool state_b)
{
	GPIO_PinState led_b_state = state_b ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, led_b_state);
}

void hw_set_state_user_led(bool state_user)
{
	GPIO_PinState led_user_state = state_user ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(LED_USER_GPIO_Port, LED_USER_Pin, led_user_state);
}

void hw_toggle_led_red(void)
{
	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}

void hw_toggle_led_green(void)
{
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void hw_toggle_led_blue(void)
{
	HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
}

void hw_toggle_user_led(void)
{
	HAL_GPIO_TogglePin(LED_USER_GPIO_Port, LED_USER_Pin);
}

//##############################################################################################//
//SSR1 (State Solid Relay) configuration

void hw_pwm_SSR1_start()
{
	HAL_TIM_Base_Start(&HW_TIMER_PORT);
	HAL_TIM_PWM_Start(&HW_TIMER_PORT, TIM_CHANNEL);
}

void hw_pwm_SSR1_set(uint16_t PWM)
{
	__HAL_TIM_SET_COMPARE(&HW_TIMER_PORT, TIM_CHANNEL, PWM);
}


//##############################################################################################//
//Serial communication configuration

static void hw_uart_tx_byte(uint8_t c)
{
	USART_TypeDef *h = huart1.Instance;

	// garante que o shift register esta vazio
	while(!(h->SR & UART_FLAG_TXE))
	{}

	// coloca o valor no shift register
	h->DR = c;
}


void hw_uart_tx(uint8_t *buffer, uint32_t size)
{
	for(size_t pos = 0 ; pos < size ; pos++)
		hw_uart_tx_byte(buffer[pos]);
}

//##############################################################################################//
//Timer sistick

uint32_t hw_time_ms_get(void)
{
	return HAL_GetTick();
}
