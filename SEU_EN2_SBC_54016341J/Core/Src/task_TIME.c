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

#include <task.h>
#include "task_CONSOLE.h"
#include "task_TIME.h"

#include "cJSON.h"


uint32_t global_time_it;

void Task_TIME_init(void){

	BaseType_t res_task;
	global_time_it=0;
	global_wifi_ready=0;
	res_task=xTaskCreate( Task_TIME,"TIME",2048,NULL,	NORMAL_PRIORITY,NULL);
	if( res_task != pdPASS ){
		bprintf("PANIC: Error al crear Tarea TIME\r\n");
		fflush(NULL);
		while(1);
	}

}


void Task_TIME( void *pvParameters ){

	int signal;
	CJSON_PUBLIC(cJSON *) jsons1;

 	cJSON *name = NULL;


	while (1) {

		signal=1;
		do {
			if (xSemaphoreTake(COMM_xSem, 20000/portTICK_RATE_MS  ) != pdTRUE ){// si en 20 segundos no he continuado entrado en orion mmm mal rollito harakiri
				bprintf("\r\n\n\nHARAKIRI!!\n\n\n");
		   		HAL_NVIC_SystemReset();
			}

			// aquí tengo la exclusión mutua asegurada.
			if (COMM_request.command==0){ //nada quiere nada
				COMM_request.command=1;
				COMM_request.result=0;
				COMM_request.dst_port=80;
				COMM_request.dst_address=(uint8_t *)"worldtimeapi.org";
				COMM_request.HTTP_request=(uint8_t *)"GET /api/timezone/Europe/Madrid HTTP/1.1\r\n\r\n";

				signal=0;
			}
		    xSemaphoreGive(COMM_xSem); // i’m going out critical section
		}while(signal);


  		// Here you must parse json response in COMM_request.response item
		do
		{
			if (xSemaphoreTake(COMM_xSem, 20000/portTICK_RATE_MS  ) != pdTRUE ){// si en 20 segundos no he continuado entrado en orion mmm mal rollito harakiri
			bprintf("\r\n\n\nHARAKIRI!!\n\n\n");
			HAL_NVIC_SystemReset();
			}

			while (COMM_request.result==0){
				xSemaphoreGive(COMM_xSem); // i’m going out critical section
				vTaskDelay(100/portTICK_RATE_MS );
				if (xSemaphoreTake(COMM_xSem, 20000/portTICK_RATE_MS  ) != pdTRUE ){// si en 20 segundos no he continuado entrado en orion mmm mal rollito harakiri
					bprintf("\r\n\n\nHARAKIRI!!\n\n\n");
					HAL_NVIC_SystemReset();
				}
			}
		}
		while(COMM_request.result==0);
		xSemaphoreGive(COMM_xSem); // i’m going out critical section

////
	jsons1 = cJSON_Parse((const char *)COMM_request.HTTP_response);
	if (jsons1) {
				name = cJSON_GetObjectItem(jsons1, "utc_datetime");
				bprintf("%s \r\n",name->valuestring);
				cJSON_Delete(jsons1);
	}
	else
		bprintf("Response error \r\n");

////

		COMM_request.command=0;
		COMM_request.result=0;

		xSemaphoreGive(COMM_xSem); // i’m going out critical section

  		vTaskDelay(10/portTICK_RATE_MS );
		global_time_it++;
	}
}
