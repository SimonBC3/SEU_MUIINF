/*
 * Task_HW.h
 *
 *  Created on: 24 may. 2023
 *      Author: ppere
 */

#ifndef INC_TASK_HW_H_
#define INC_TASK_HW_H_

#include "main.h"

// definiciones


#define R25    10000.0
#define T25    298.15
#define BETA   3900




typedef struct DUMMY_SENSOR_Struct
{
	float value;
	float alarm_level;
	float max;
	float min;
} sensor_t;

extern sensor_t V_SENSOR[2];

extern ADC_HandleTypeDef hadc1;



void Task_HW( void *pvParameters );
void pon_Nivel(uint8_t sensor_sel, uint32_t Hz);


#endif /* INC_TASK_HW_H_ */
