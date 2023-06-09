/*
 * Task_time.c
 *
 *  Created on: Jun 7, 2021
 *      Author: pperez
 */
#include <tareas.h>
#include <main.h>
#include "FreeRTOS.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>
#include <utility.h>
#include "semphr.h"
#include <string.h>
#include <jsmn.h>
#include <task.h>
#include <math.h>

void Task_time( void *pvParameters )
{
	int i,res;

	global_time_it=0;

	char * buffer_response=malloc(1024);
	if (buffer_response==NULL) while(1);
	for (i=0;i<JSON_HOPS_LIMIT-1;i++)
	        		    {
	        		        json_tree[i]= pvPortMalloc(JSON_NAME_LENGTH);
	        		        if (json_tree[i]==NULL) {
	        		        	sprintf(buff_sprintf100,"Error in MALOC_2 inside LEDSP\r\n");
	        		        	HAL_UART_Transmit(UART_ESP8266,(unsigned char *)buff_sprintf100, strlen(buff_sprintf100),100000); // cabecera
	        		        	while(1);
	        		        }
	        		    }

	while(1){
		global_time_it++;
		while (xSemaphoreTake(ORION_xSem, 100000/portTICK_RATE_MS  ) != pdTRUE );
        if (ORION_req.status==0){
        	//print_buffer("TIME","REQ=0\r\n");
            sprintf(buffer_response,"%s\r\n\r\n",TIME_QUERY);
            ORION_req.HTTP_request=buffer_response;
            ORION_req.machine_dest=TIME_ADDRESS;
			ORION_req.port_dest=TIME_PORT;
			ORION_req.HTTP_response=buffer_response;
			ORION_req.status=1; // solicitud
			xSemaphoreGive(ORION_xSem);
			vTaskDelay(10/portTICK_RATE_MS );
        }
        else{
        	if (ORION_req.status==2){
            		// si es !=1 y no es null es porque es la respuesta.
            		// procesarla
            		// se asume una escritura correcta y no se procesa
        			//print_buffer("TIME_2",ORION_req.HTTP_response);

        			strcpy(json_tree[0],"currentDateTime");
        			strcpy(json_tree[1],"");
        			json_search(p,tok,(char *)ORION_req.HTTP_response, json_tree,json_value,JSON_VALUE_LIMIT);
        			ORION_req.status=0;

        			if (json_value[0]!=0) {
        				xSemaphoreTake(VGLOBAL_xSem, 100000/portTICK_RATE_MS  );

        				if (global_time_init[0]==0) // si es la primera vez
      			    		strcpy(global_time_init,json_value);
      			    	else
      			    		strcpy(global_time,json_value); // si no actualiza la hora

        				xSemaphoreGive(VGLOBAL_xSem);

        			    json_value[0]=0;

        			    xSemaphoreGive(ORION_xSem);
        			    vTaskDelay(10000/portTICK_RATE_MS );

        			}
      			    else{
      			    	xSemaphoreGive(ORION_xSem);
      			    	vTaskDelay(1000/portTICK_RATE_MS );
      			    }
        	}else{
                xSemaphoreGive(ORION_xSem);
        	vTaskDelay(10/portTICK_RATE_MS );
        	}
        }
	}
}
