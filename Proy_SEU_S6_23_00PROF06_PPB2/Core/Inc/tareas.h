#ifndef __TAREAS_H__
#define __TAREAS_H__
#include "FreeRTOS.h"

#define HIGH_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)+1
#define NORMAL_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)
#define LOW_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)-1

void Task_HW( void *pvParameters );
void Task_WIFI( void *pvParameters );
void Task_COMM( void *pvParameters );
void Task_TIME( void *pvParameters );

#endif
