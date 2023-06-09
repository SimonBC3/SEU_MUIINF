#include <stdint.h>
#include "task_COMM.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>
#include "semphr.h"
#include "tareas.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>

#include "semphr.h"
#include <string.h>

#include <task.h>
#include "task_CONSOLE.h"
#include "task_WRITE.h"
#include "task_TIME.h"


#include "cJSON.h"


void Task_WRITE_init(void){

	BaseType_t res_task;
	global_time_it=0;
	global_wifi_ready=0;
	res_task=xTaskCreate( Task_WRITE,"WRITE",2048,NULL,	NORMAL_PRIORITY,NULL);
	if( res_task != pdPASS ){
		bprintf("PANIC: Error al crear Tarea WRITE\r\n");
		fflush(NULL);
		while(1);
	}

}


void Task_WRITE( void *pvParameters ){

	int signal;
	CJSON_PUBLIC(cJSON *) jsons1;

 	cJSON *name = NULL;

 	cJSON *root, *contextResponses, *contextElement, *attributes, *alarma, *alarma_src, *intensidadLuz, *temperatura, *statusCode;
 	root = cJSON_CreateObject();
 	contextResponses = cJSON_CreateArray();

 	cJSON_AddItemToObject(root, "contextResponse", contextResponses);

 	contextElement = cJSON_CreateObject();
 	cJSON_AddItemToArray(contextResponses, contextElement);
 	cJSON_AddItemToObject(contextElement, "type", cJSON_CreateString("Sensor"));
 	cJSON_AddItemToObject(contextElement, "isPattern", cJSON_CreateString("false"));
 	cJSON_AddItemToObject(contextElement, "id", cJSON_CreateString("SensorSEU_SBC00"));

 	attributes = cJSON_CreateArray();
 	cJSON_AddItemToObject(atributes, "attributes", contextElement);

 	alarma = cJSON_CreateObject();
 	cJSON_AddItemToObject(alarma, "name", cJSON_CreateString("Alarma"));
 	cJSON_AddItemToObject(alarma, "type", cJSON_CreateString("boolean"));
 	cJSON_AddItemToObject(alarma, "value", cJSON_CreateString("F")); //Need to get current Value
 	cJSON_AddItemToArray(attributes, alarma);

 	alarma_src = cJSON_CreateObject();
 	cJSON_AddItemToObject(alarma_src, "name", cJSON_CreateString("Alarma_src"));
 	cJSON_AddItemToObject(alarma_src, "type", cJSON_CreateString("string"));
 	cJSON_AddItemToObject(alarma_src, "value", cJSON_CreateString("SensorSEU_SBC003")); //Need to get current Value
 	cJSON_AddItemToArray(attributes, alarma);

 	intensidadLuz = cJSON_CreateObject();
 	cJSON_AddItemToObject(intensidadLuz, "name", cJSON_CreateString("IntensidadLuz"));
 	cJSON_AddItemToObject(intensidadLuz, "type", cJSON_CreateString("floatArray"));
 	cJSON_AddItemToObject(intensidadLuz, "value", cJSON_CreateString("72.96, 100.0,0.0,79.60")); //Need to get current Value
 	cJSON_AddItemToArray(attributes, intensidadLuz);

 	temperatura = cJSON_CreateObject();
 	cJSON_AddItemToObject(temperatura, "name", cJSON_CreateString("Temperatura"));
 	cJSON_AddItemToObject(temperatura, "type", cJSON_CreateString("floatArray"));
 	cJSON_AddItemToObject(temperatura, "value", cJSON_CreateString("72.96, 100.0,0.0,79.60"));
 	cJSON_AddItemToArray(attributes, temperatura);

 	statusCode = cJSON_CreateObject();
 	cJSON_AddItemToObject(statusCode, "code", cJSON_CreateString("200"));
 	cJSON_AddItemToObject(statusCode, "reasonPhrase", cJSON_CreateString("OK"));
 	cJSON_AddItemToObject(contextResponses, "statusCode", statusCode);

 	cJSON_Print(root);

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
				COMM_request.dst_address=(uint8_t *)"pperez-seu-or.disca.upv.es:1026";
				string request = "GET /v1/updateContext -H 'Content-Type: application/json' -d '" + cJSON_Print(root) +"' HTTP/1.1\r\n\r\n";
				COMM_request.HTTP_request=(uint8_t *)request;
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
		bprintf("Response error task_WRITE\r\n");

////

		COMM_request.command=0;
		COMM_request.result=0;

		xSemaphoreGive(COMM_xSem); // i’m going out critical section

  		vTaskDelay(10/portTICK_RATE_MS );
		global_time_it++;
	}
}