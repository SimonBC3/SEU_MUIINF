/*
 * HW.c
 *
 *  Created on: Mar 29, 2023
 *      Author: sbrot
 */

#include "HW.h"

void put_leds(uint8_t dato){

	if (dato&0x01)
		  HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_SET);
	else
		  HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_RESET);

	if (dato&0x02)
			  HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
	if (dato&0x04)
				  HAL_GPIO_WritePin(GPIOA, LED3_Pin, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(GPIOA, LED3_Pin, GPIO_PIN_RESET);
	if (dato&0x08)
				  HAL_GPIO_WritePin(GPIOB, LED4_Pin, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(GPIOB, LED4_Pin, GPIO_PIN_RESET);
	if (dato&0x10)
				  HAL_GPIO_WritePin(GPIOB, LED5_Pin, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(GPIOB, LED5_Pin, GPIO_PIN_RESET);
	if (dato&0x20)
				  HAL_GPIO_WritePin(GPIOA, LED6_Pin, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(GPIOA, LED6_Pin, GPIO_PIN_RESET);
	if (dato&0x40)
				  HAL_GPIO_WritePin(GPIOB, LED7_Pin, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(GPIOB, LED7_Pin, GPIO_PIN_RESET);
	if (dato&0x80)
				  HAL_GPIO_WritePin(GPIOA, LED8_Pin, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(GPIOA, LED8_Pin, GPIO_PIN_RESET);
	if (dato == 0) {
		put_leds(1);
		HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_RESET);
	}
}

void pot_leds(float value){
	if (value <= (3.3/8)) {
		put_leds(1);
	} else if (value <= (3.3*2/8)) {
		put_leds(3);
	} else if (value <= (3.3*3/8)) {
		put_leds(7);
	} else if (value <= (3.3*4/8)) {
		put_leds(15);
	} else if (value <= (3.3*5/8)) {
		put_leds(63);
	} else if (value <= (3.3*6/8)) {
		put_leds(127);
	} else if (value <= (3.3*7/8)) {
		put_leds(255);
	}
}

uint32_t ConvertidorA_D(uint8_t channel) {
	uint32_t valueAD;
	ADC_ChannelConfTypeDef sConfig;
	switch(channel) {
	case 0:
		//sensor de luz
		sConfig.Channel = ADC_CHANNEL_0;
				sConfig.Rank = 1;
				sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
				break;
	case 1:
			//sensor de temp
			sConfig.Channel = ADC_CHANNEL_1;
					sConfig.Rank = 1;
					sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
					break;
	case 4:
			sConfig.Channel = ADC_CHANNEL_4;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
			break;
	}
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10000);
	valueAD=HAL_ADC_GetValue(&hadc1);
	return valueAD;
}

/*void turn_leds_on(float value) {
	float div = 3.3/8;
	if (value <= div) {
		put_leds(1);
	} else if (value <= div*2) {
		put_leds(3);
	} else if (value <= div*3) {
		put_leds(7);
	} else if (value <= div*4) {
		put_leds(15);
	} else if (value <= div*5) {
		put_leds(63);
	} else if (value <= div*6) {
		put_leds(127);
	} else if (value <= div*7) {
		put_leds(255);
	}
}*/
