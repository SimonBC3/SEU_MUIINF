/*
 * Task_temp.c
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


void Task_TEMP( void *pvParameters ){
int i;

	while(1){
		while (xSemaphoreTake(ORION_xSem, 10000/portTICK_RATE_MS  ) != pdTRUE )
			i=i+1;

		if ((ORION_req.status==0)&&(global_try_change_mode==1)){
						       xSemaphoreGive(ORION_xSem);
						       vTaskDelay(100/portTICK_RATE_MS );
						       continue;
						}

		if ((ORION_req.status==1)&&(ORION_req.src!=2)){ //idle
			switch (global_mode){
        	case 2: 	  global_try_trans_connect++; break;
        	case 3:		  global_try_trans_clone++; break;
			}
			xSemaphoreGive(ORION_xSem);
			continue;
		}



		if (ORION_req.status==0){ //idle
       // print_buffer("TEMP","REQ=0\r\n");
        	switch (global_mode){
        	case 2: // conectado publica
        	sprintf(buff_sprintf_body,ORION_UPDATE_BODY_TEMP,IoT_NAME,global_sensor_level[0],global_sensor_level_max[0],global_sensor_level_min[0],global_sensor_level_alarm[0],global_sensor_level[1],global_sensor_level_max[1],global_sensor_level_min[1],global_sensor_level_alarm[1],global_buzzer?'T':'F');
        	sprintf(buff_sprintf1024,"%s %d\r\n\r\n%s",ORION_UPDATE,strlen(buff_sprintf_body),buff_sprintf_body);
        	ORION_req.HTTP_request=buff_sprintf1024;
            ORION_req.machine_dest=ORION_ADDRESS;
    		ORION_req.port_dest=ORION_PORT;
    		ORION_req.HTTP_response=NULL;
    		ORION_req.src=2;
    		ORION_req.status=1; // solicitud
    		break;
        	case 3:// clon publica si se detectado una desconexión del alarma

         		if (global_alarm_remote_disconnect==1){
         		global_alarm_remote_disconnect=0;
         		global_alarm_remote_disconnect_count++;
         		sprintf(buff_sprintf_body,ORION_UPDATE_ALARM,IoT_NAME_CLONE,IoT_NAME,global_alarm_remote_disconnect_count);
         		sprintf(buff_sprintf1024,"%s %d\r\n\r\n%s",ORION_UPDATE,strlen(buff_sprintf_body),buff_sprintf_body);
         		ORION_req.HTTP_request=buff_sprintf1024;
                    ORION_req.machine_dest=ORION_ADDRESS;
            		ORION_req.port_dest=ORION_PORT;
            		ORION_req.HTTP_response=NULL;
            		ORION_req.src=2;
            		ORION_req.status=1; // solicitud
         		}

         		break;
		    }

    		xSemaphoreGive(ORION_xSem);
    		vTaskDelay(1/portTICK_RATE_MS );
        }
        else{

        	if ((ORION_req.status==2)&&(ORION_req.src==2)){
        		// si es !=1 y no es null es porque es la respuesta.
        		// procesarla
        		// se asume una escritura correcta y no se procesa
        		 switch (global_mode){
        	case 2: // conectado publica
        		    global_trans_connect++;
        			break;
        		case 3: //clone
        		 global_trans_clone++;
        	break;
        		 }
        		ORION_req.status=0;
        		 ORION_req.src=0;
        		 xSemaphoreGive(ORION_xSem);
        		 vTaskDelay((10+rand()%2)/portTICK_RATE_MS );
        		// vTaskDelay((10+rand()%10)/portTICK_RATE_MS );
        	}
        	else
        	{

        	 xSemaphoreGive(ORION_xSem);

        	 //vTaskDelay(10/portTICK_RATE_MS );
        	}
        }
	}
}


