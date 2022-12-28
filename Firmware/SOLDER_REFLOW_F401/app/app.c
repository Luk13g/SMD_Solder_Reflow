/*
 * app.c
 *
 *  Created on: May 16, 2021
 *      Author: lukgd
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "app.h"
#include "hw.h"
#include "simulation_scr.h"
#include "ssd1306.h"

//sensor_temperature
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim5;
#define MAX6675_SPI hspi1
uint8_t data[2];
uint16_t temp;
volatile uint8_t point = 0;

//setpoint
uint16_t set_point[] = {160, 210};
volatile float cont_time = 0;
volatile float pwm = 1;

//TIMES SOAK and REFLOW
#define TIME_SOAK 60//140
#define TIME_REFLOW 30//70

//Serial_buffer
#define APP_UART_BUFFER_MAX   64

//PID
float rk = 0, uk = 0, ek = 0, ek_1 = 0, uk_1 = 0, yk = 0;

//started app
bool app_started = false;

#define APP_DEBOUCING_TIME_MS 60

//STATE MACHINE
typedef enum app_sm_states_e
{
	HEAT_UP1 = 0,
	SOAK,
	HEAT_UP2,
	REFLOW,
	COOL,
} app_sm_states_t;

typedef struct app_sm_s
{
	app_sm_states_t state;
} app_sm_t;

app_sm_t app_sm;


void app_started_go(bool start_state)
{
	static uint32_t deboucing_time_ms = 0;

	//if(!app_started)
		//return;

	if((hw_time_ms_get() - deboucing_time_ms) >= APP_DEBOUCING_TIME_MS)
	{
		app_started = start_state;
		deboucing_time_ms = hw_time_ms_get();
	}
}

void app_init()
{
	hw_pwm_SSR1_start();
	HAL_TIM_Base_Start_IT(&htim5);
    SSD1306_Init();
    SDD1306_Clear_Screen();
	app_sm.state = HEAT_UP1;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim5)
	{
		if(app_started)
		{
		static uint8_t tx_buffer[APP_UART_BUFFER_MAX];
		uint32_t tx_size;
		float temp_float;
		//Read the thermocouple every 500ms:
		hw_toggle_led_red();
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Receive(&MAX6675_SPI, data, 2, 100);
		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin, GPIO_PIN_SET);
		temp = ((((uint16_t) data[1] << 8) | data[2]) >> 3) * 0.249;

		temp_float = (float)temp;
		app_PID(temp);
		cont_time += 0.5;
		tx_size = snprintf((char *)tx_buffer,APP_UART_BUFFER_MAX-1,"%f;%f;%f\n", (float)temp_float, (float)cont_time, (float)pwm);//(unsigned int)temp
		hw_uart_tx(tx_buffer,tx_size);
		}
	}
}

void app_PID(float yk)
{
	ek = rk - yk;//erro calculate

	//controlador

	uk = (1.7*ek_1 + 0.45*uk_1)/0.55;

	//atualizando as variaveis
	ek_1 = ek;
	uk_1 = uk;

	if(uk > 999) uk = 999;

	if(uk < 0) uk = 0;

	hw_pwm_SSR1_set(uk);

}
void gera_degral()
{
	if (point == 0)
	{
		pwm = 1;
			if (temp <= 210 && point == 0)
			{
				hw_pwm_SSR1_set(999);
				if (temp == 210)
				{
					point++;
				}
			}
	}

	//COOL

	if (point == 1)
	{
		pwm = 0;
		hw_set_state_led_green(0);
		hw_set_state_led_blue(1);
		hw_pwm_SSR1_set(0);
	}
}

app_sm_states_t sm_app_heat_up1(app_sm_t *state_sm)
{

	//HEAT_UP1

 	app_sm_states_t next_state = state_sm ->state;

	rk = set_point[0];
	pwm = 0.75;
	if (temp <= set_point[0])
	{
		hw_pwm_SSR1_set(uk);
		if (temp == set_point[0])
		{
			next_state = SOAK;
		}
	}

	return next_state;
}

app_sm_states_t sm_app_soak(app_sm_t *state_sm)
{
	float time_now = cont_time;

	//SOAK

 	app_sm_states_t next_state = state_sm ->state;

	while (true)
	{
		hw_set_state_led_green(1);
		if (temp > set_point[0])
		{
			pwm = 0;
			hw_pwm_SSR1_set(uk);
		}
		else if (temp < set_point[0])
		{
			pwm = 0.5;
			hw_pwm_SSR1_set(uk);
		}

		if (cont_time - time_now >= TIME_SOAK)//200
		{
			next_state = HEAT_UP2;
			break;
		}
	}

	return next_state;
}

app_sm_states_t sm_app_heat_up2(app_sm_t *state_sm) {

	//HEAT_UP2

 	app_sm_states_t next_state = state_sm ->state;

	rk = set_point[1];
	pwm = 1;

	hw_set_state_led_green(0);
	if (temp <= set_point[1])
	{
		hw_pwm_SSR1_set(uk);
		if (temp == set_point[1])
		{
			next_state = REFLOW;
		}
	}

	return next_state;
}

app_sm_states_t sm_app_reflow(app_sm_t *state_sm)
{

	float time_now1 = cont_time;

	//REFLOW

 	app_sm_states_t next_state = state_sm ->state;

	while (true)
	{
		hw_set_state_led_green(1);
		hw_set_state_led_blue(1);
		if (temp > set_point[1])
		{
			pwm = 0;
			hw_pwm_SSR1_set(uk);
		}
		else if (temp < set_point[1])
		{
			pwm = 0.75;
			hw_pwm_SSR1_set(uk);
		}

		if (cont_time - time_now1 >= TIME_REFLOW)//120
		{
			next_state = COOL;
			break;
		}
	}

	return next_state;
}
void sm_app_cool()
{
	//COOL

	pwm = 0;
	hw_set_state_led_green(0);
	hw_set_state_led_blue(1);
	hw_pwm_SSR1_set(0);
}

void routine_reflow_solder_sm(app_sm_t *state_sm)
{
 	switch(state_sm-> state)
	{
	case HEAT_UP1:
		state_sm->state = sm_app_heat_up1(state_sm);
		break;

	case SOAK:
		state_sm->state = sm_app_soak(state_sm);
		break;

	case HEAT_UP2:
		state_sm->state = sm_app_heat_up2(state_sm);
		break;

	case REFLOW:
		state_sm->state = sm_app_reflow(state_sm);
		break;

	case COOL:
		sm_app_cool();
		break;

	default:
		state_sm->state = HEAT_UP1;
		break;
	}
}

void app_loop()
{
	 //gera_degral();
	while (app_started)
	{
		SSD1306_Write_Centered_String((uint8_t*)"PROCESSO INICIADO",0);
		SSD1306_Write_Centered_String((uint8_t*)"SOLDER_REFLOW",2);
		routine_reflow_solder_sm(&app_sm);
	}
}
