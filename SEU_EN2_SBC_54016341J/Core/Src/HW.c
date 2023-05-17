/*
 * HW.c
 *
 *  Created on: May 17, 2023
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

