/*
 * HW.h
 *
 *  Created on: May 17, 2023
 *      Author: sbrot
 */

#ifndef SRC_HW_H_
#define SRC_HW_H_

#include "main.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

void put_leds(uint8_t dato);

uint32_t ConvertidorA_D(uint8_t channel);

int checkSensorChange();
void setOffAlarm(float value, float trigger);
void turnOnLedsLight(float value);
void turnOnLedsTemp(float initValue, float currentValue);
void lightSensorBehaviour();
void tempSensorBehaviour();
void restartAlarm();
void runHW();

#endif /* SRC_HW_H_ */
