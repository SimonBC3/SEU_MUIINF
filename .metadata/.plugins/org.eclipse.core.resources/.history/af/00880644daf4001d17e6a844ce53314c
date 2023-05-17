#include "FreeRTOS.h"
#include "main.c"

unsigned portBASE_TYPE makeFreeRtosPriority(osPriority priority) {
	unsigned portBASE_TYPE fpriority = tskIDLE_PRIORITY;

	if (priority != osPriorityError) {
		fpriority += (priority - osPriorityIdle);
	}

	return fpriority;
}

void CONFIGURACION_INICIAL(void){
 BaseType_t res_task;

 res_task = xTaskCreate(Task_HW,"HW",1024,NULL,	32,NULL);
 if( res_task != pdPASS ){
	 printf("PANIC: Error al crear Tarea Visualizador\r\n");
 	 fflush(NULL);
 	 while(1);
 }

 res_task=xTaskCreate(Task_WIFI,"WIFI",1024,NULL,	32 ,NULL);
 if( res_task != pdPASS ){
	 printf("PANIC: Error al crear Tarea Visualizador\r\n");
	 fflush(NULL);
	 while(1);
 }

}

void Task_HW( void *pvParameters ) {
	for( ;; ){
		printf("Estoy en tasks");
		osDelay(1);
	}
}

void Task_WIFI( void *pvParameters ) {

}

void Task_Send( void *pvParameters ) {

}

void Task_Receive( void *pvParameters ) {

}
