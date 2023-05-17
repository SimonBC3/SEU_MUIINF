#include "tareas.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void CONFIGURACION_INICIAL(void){
	BaseType_t res_task;

 res_task=xTaskCreate(Task_HW,"HW",1024,NULL,	 NORMAL_PRIORITY,NULL);
 		if( res_task != pdPASS ){
 				printf("PANIC: Error al crear Tarea Visualizador\r\n");
 				fflush(NULL);
 				while(1);
 		}

 res_task=xTaskCreate(Task_WIFI,"WIFI",1024,NULL,	 NORMAL_PRIORITY ,NULL);
 			if( res_task != pdPASS ){
 					printf("PANIC: Error al crear Tarea Visualizador\r\n");
 					fflush(NULL);
 					while(1);
 			}

}

void Task_HW( void *pvParameters ){



	for(;;)
	  {
	    osDelay(1);
	  }
}

void Task_WIFI( void *pvParameters ){

	ESP8266_Boot();

	for(;;)
	  {
		// aqui ir preguntando si tienes IP
	    osDelay(1);
	  }
}

void Task_COMM( void *pvParameters ){

	for(;;)
	  {
		// while (no IP)?
		// Hay paquetes que transmitir
		// Connect
		// SEND
		// receive

	    osDelay(1);
	  }
}

void Task_TIME( void *pvParameters ){

	for(;;)
	  {
		// crea una solicitud TIME
		// solicita a la tarea COMM su transmisión.
		// espera al resultado
		// procesa el resultado

	    osDelay(1);
	  }
}


