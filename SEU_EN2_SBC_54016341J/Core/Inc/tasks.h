/*
 * tasks.h
 *
 *  Created on: May 17, 2023
 *      Author: sbrot
 */

#include "FreeRTOS.h"
#include "main.h"
#include "semphr.h"

#define HIGH_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)+1
#define NORMAL_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)
#define LOW_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)-1

extern SemaphoreHandle_t WIFI_xSem;

void Task_HW( void *pvParameters );
void Task_WIFI( void *pvParameters );
void Task_Send( void *pvParameters );
void Task_Receive( void *pvParameters );
void CONFIGURACION_INICIAL(void);


