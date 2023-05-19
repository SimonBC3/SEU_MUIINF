/*
 * tasks.c
 *
 *  Created on: May 17, 2023
 *      Author: sbrot
 */

#include "tasks.h"
#include "HW.h"


void Task_HW( void *pvParameters ) {
	for(;;)
	  {
		printf("in HWTask");
		runHW();
	    osDelay(1);
	  }
}
void Task_WIFI( void *pvParameters ) {
	for(;;)
	  {

		//ESP8266_Boot();
	    osDelay(1);
	  }
}
void Task_Send( void *pvParameters ) {
	for(;;)
	  {
	    osDelay(1);
	  }
}
void Task_Receive( void *pvParameters ) {
	for(;;)
	  {
	    osDelay(1);
	  }
}

void CONFIGURACION_INICIAL(void){
 BaseType_t res_task;
 res_task=xTaskCreate(Task_HW,"HW",1024,NULL,NORMAL_PRIORITY,NULL);
 		if( res_task != pdPASS ){
 				printf("PANIC: Error al crear Tarea Visualizador\r\n");
 				fflush(NULL);
 				while(1);
 		}

 res_task=xTaskCreate(Task_WIFI,"WIFI",1024,NULL,NORMAL_PRIORITY ,NULL);
 			if( res_task != pdPASS ){
 					printf("PANIC: Error al crear Tarea Visualizador\r\n");
 					fflush(NULL);
 					while(1);
 			}

}
