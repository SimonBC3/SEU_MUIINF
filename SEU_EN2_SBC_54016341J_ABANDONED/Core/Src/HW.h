/*
 * HW.h
 *
 *  Created on: Mar 29, 2023
 *      Author: sbrot
 */

#ifndef __HW_h__
#define __HW_h__
#include "main.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

void put_leds(uint8_t dato);

uint32_t ConvertidorA_D(uint8_t channel);
#endif
