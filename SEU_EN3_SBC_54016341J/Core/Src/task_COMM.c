#include <stdint.h>
#include "task_COMM.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>
#include "semphr.h"
#include "tasks.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>

#include "semphr.h"
#include <string.h>

#include <tasks.h>

#include "task_CONSOLE.h"
#include "task_WIFI.h"


scomm_request_t COMM_request;
SemaphoreHandle_t COMM_xSem = NULL;
uint32_t global_comm_it;


void Task_COMM_init(void){
	BaseType_t res_task;

	global_comm_it=0;

	COMM_xSem=xSemaphoreCreateMutex();
	if( COMM_xSem == NULL ){
		printf("PANIC: Error al crear el Semáforo ORION\r\n");
		fflush(NULL);
		while(1);
	}

 	res_task=xTaskCreate(Task_COMM,"COMMUNICATION",2048,NULL,	NORMAL_PRIORITY,NULL);
 	if( res_task != pdPASS ){
 	 				printf("PANIC: Error al crear Tarea Comunicaciones\r\n");
 	 				fflush(NULL);
 	 				while(1);
 	}
}

void Task_COMM( void *pvParameters ){

	int signal;

	while (global_wifi_ready==0) osDelay(1);

	while (1) {

		signal=1;
		do {
			if (xSemaphoreTake(COMM_xSem, 20000/portTICK_RATE_MS  ) != pdTRUE ){// si en 20 segundos no he continuado entrado en orion mmm mal rollito harakiri
				bprintf("\r\n\n\nHARAKIRI!!\n\n\n");
		   		HAL_NVIC_SystemReset();
		}

		// aquí tengo la exclusión mutua asegurada.
		if (COMM_request.command==0){ //nada quiere nada
			xSemaphoreGive(COMM_xSem); // i’m going out critical section
			osDelay(100);
		}else signal=0;

		}while(signal);

		xSemaphoreGive(COMM_xSem); // i’m going out critical section

		// Here is safe access to COMM_request because other thread has put COMM_request.command to 1 so before write any other thread must read 0 in this item.

		COMM_request.HTTP_response=ESP_Send_Request(COMM_request.dst_address,COMM_request.dst_port,COMM_request.HTTP_request);

		// remove headers and +IPD messages from ESP32 as a result you have json pure string IF it is completely recover from server in a unique +IPD message

		cleanResponse(COMM_request.HTTP_response,strlen((char *)COMM_request.HTTP_response));



		if (xSemaphoreTake(COMM_xSem, 20000/portTICK_RATE_MS  ) != pdTRUE ){// si en 20 segundos no he continuado entrado en orion mmm mal rollito harakiri
			bprintf("\r\n\n\nHARAKIRI!!\n\n\n");
		    HAL_NVIC_SystemReset();
		}

		// if ESP does not receive response or there are any errors result must be different to 1
		COMM_request.result=1;
		xSemaphoreGive(COMM_xSem); // i’m going out critical section

		vTaskDelay(10/portTICK_RATE_MS );
		}

	global_comm_it++;
}





void cleanResponse(uint8_t * data,int maxlen)
{
	int t,i;




	uint8_t * j,*from, *pc;


	 i=0;
	 while ((j=(uint8_t *)strstr((char *)data,"+IPD"))){

		 from=(uint8_t *)strstr((char *)j,":");
		 from++;

		 	 for (pc=from;pc<(data+2048);pc++)
			 	*(j++)=*(from++);
	 };
	 	 do
			                  {
			                  }while(data[i++]!='{');
			                  i--;
	for (t=0;t<(2048-i);t++)
		data[t]=data[t+i];

	i=0;

    for (t=0;t<2048;t++)
    	if (data[t]=='}')
    		i=t;
    data[i+1]=0;




}


