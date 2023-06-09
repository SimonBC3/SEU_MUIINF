/*
 * Task_leds_alarm.c
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
#include <cJSON.h>

void Task_LEDS_ALARM( void *pvParameters ){

	int i,res,t;

	cJSON *jsons1 = NULL;
	        				    //char *string = cJSON_Print(json);
	        				 	  cJSON *name = NULL;
	        				 	  cJSON *name1 = NULL;
	        				 	  cJSON *name2 = NULL;
	        				 	  cJSON *name3 = NULL;
	        				 	 cJSON *name4 = NULL;
	        				 	 cJSON *name5 = NULL;
	        				 	 cJSON *name6 = NULL;


	char * buffer_response=malloc(2048);
	char * buffer_response1=malloc(2048);
	if (buffer_response==NULL) while(1);
	for (i=0;i<JSON_HOPS_LIMIT;i++){
        		        json_tree[i]= pvPortMalloc(JSON_NAME_LENGTH);
        		        if (json_tree[i]==NULL) {
        		        	sprintf(buff_sprintf100,"Error in MALOC_2 inside LEDSP\r\n");
        		        	HAL_UART_Transmit(UART_ESP8266,(unsigned char *)buff_sprintf100, strlen(buff_sprintf100),100000); // cabecera
        		        	while(1);
        		        }
        		    }

	while(1){
		while (xSemaphoreTake(ORION_xSem, 10000/portTICK_RATE_MS  ) != pdTRUE )
			i=i+1;


		if ((ORION_req.status==0)&&(global_try_change_mode==1)){
		       xSemaphoreGive(ORION_xSem);
		       vTaskDelay(100/portTICK_RATE_MS );
		       continue;
		}

		if ((ORION_req.status==1)&&(ORION_req.src!=1)){ //idle
			switch (global_mode){
        	case 2: 	  global_try_recv_connect++; break;
        	case 3:		  global_try_recv_clone++; break;
			}
			xSemaphoreGive(ORION_xSem);
			continue;
		}



        if (ORION_req.status==0){
        	//print_buffer("LEDS","REQ=0\r\n");
        	switch (global_mode){
        	case 2:
        		sprintf(buff_sprintf_body,ORION_QUERY_BODY,IoT_NAME);
                sprintf(buff_sprintf1024,"%s %d\r\n\r\n%s",ORION_QUERY,strlen(buff_sprintf_body),buff_sprintf_body);
                ORION_req.HTTP_request=buff_sprintf1024;
                ORION_req.machine_dest=ORION_ADDRESS;
                ORION_req.port_dest=ORION_PORT;
                ORION_req.HTTP_response=buffer_response;
                ORION_req.status=1; // solicitud
                ORION_req.src=1;

                break;
        	case 3:
        	        		sprintf(buff_sprintf_body,ORION_QUERY_BODY,IoT_NAME_CLONE);
        	        		sprintf(buff_sprintf1024,"%s %d\r\n\r\n%s",ORION_QUERY,strlen(buff_sprintf_body),buff_sprintf_body);

        	                ORION_req.HTTP_request=buff_sprintf1024;
        	                ORION_req.machine_dest=ORION_ADDRESS;
        	                ORION_req.port_dest=ORION_PORT;
        	                ORION_req.HTTP_response=buffer_response;
        	            	ORION_req.src=1;
        	                ORION_req.status=1; // solicitud
        	                break;
        	}
        	xSemaphoreGive(ORION_xSem);
        	vTaskDelay(1/portTICK_RATE_MS );
        }
        else{
        	if ((ORION_req.status==2)&&(ORION_req.src==1)){
            		// si es !=1 y no es null es porque es la respuesta.
            		// procesarla
            		// se asume una escritura correcta y no se procesa

        		jsons1 = cJSON_Parse(ORION_req.HTTP_response);

        			switch (global_mode){
        				case 3: // clone


        				    //char *string = cJSON_Print(json);


        				 	 name = cJSON_GetObjectItem(jsons1, "contextResponses");
        				 	 name1 =  cJSON_GetArrayItem(name, 0);
        				 	 name2 = cJSON_GetObjectItem(name1,"contextElement");
        				 	 name3 = cJSON_GetObjectItem(name2,"attributes");
        				 	  for (i = 0 ; i < cJSON_GetArraySize(name3) ; i++)
        				 	  {
        				 	     cJSON * subitem = cJSON_GetArrayItem(name3, i);
        				 	    cJSON * index;
        				 	     name4 = cJSON_GetObjectItem(subitem, "name");
        				 	     if (strcmp(name4->valuestring,"Temperatura")==0){
        				 	    	 index = cJSON_GetObjectItem(subitem, "value");
        				 	    	  char * json_value;
        				 	    	 char * pt1,*pt2,*pt3;
        	        		    		int t,h;
        	        		    		        		    		h=0;
        	        		    		        		    		for (t=0;t<100;t++)
        	        		    		        		    			if (index->valuestring[t]==',') h++;
        	        		    		        		    		if (h>=3){
        	        		    		        		    				json_value=index->valuestring;
        	        		    		        		    				global_recv_clone++;   // se contabilizan como válidas

        	        		    		        		    					pt1=strstr(json_value,",");
        	        		    		        		    				    pt2=strstr(pt1+1,",");
        	        		    		        		    				    	pt3=strstr(pt2+1,",");
        	        		    		        		    				    	global_sensor_level[0]=atof(json_value);
        	        		    		        		    				    	global_sensor_level_max[0]=atof(pt1+1);
        	        		    		        		    				    	global_sensor_level_min[0]=atof(pt2+1);
        	        		    		        		    				    	global_sensor_level_alarm[0]=atof(pt3+1);
        	        		    		        		    		}
        				 	     }
        				 	    if (strcmp(name4->valuestring,"IntensidadLuz")==0){
        				 	            				 	    	 index = cJSON_GetObjectItem(subitem, "value");
        				 	            				 	    	  char * json_value;
        				 	            				 	    	 char * pt1,*pt2,*pt3;
        				 	            	        		    		int t,h;
        				 	            	        		    		        		    		h=0;
        				 	            	        		    		        		    		for (t=0;t<100;t++)
        				 	            	        		    		        		    			if (index->valuestring[t]==',') h++;
        				 	            	        		    		        		    		if (h>=3){
        				 	            	        		    		        		    				json_value=index->valuestring;
        				 	            	        		    		        		    				global_recv_clone++;   // se contabilizan como válidas

        				 	            	        		    		        		    					pt1=strstr(json_value,",");
        				 	            	        		    		        		    				    pt2=strstr(pt1+1,",");
        				 	            	        		    		        		    				    	pt3=strstr(pt2+1,",");
        				 	            	        		    		        		    				    	global_sensor_level[1]=atof(json_value);
        				 	            	        		    		        		    				    	global_sensor_level_max[1]=atof(pt1+1);
        				 	            	        		    		        		    				    	global_sensor_level_min[1]=atof(pt2+1);
        				 	            	        		    		        		    				    	global_sensor_level_alarm[1]=atof(pt3+1);
        				 	            	        		    		        		    		}
        				 	            				 	     }
        				 	    if (strcmp(name4->valuestring,"Alarma")==0){
        				 	            				 	    	 index = cJSON_GetObjectItem(subitem, "value");
        				 	            				 	    	  char * json_value;
        				 	            				 	    	 char * pt1,*pt2,*pt3;
        				 	            	        		    		int t,h;
        				 	            	        		    		if (index->valuestring[0]=='T')
        				 	            	        		    			global_alarm_trigger_value='1';
        				 	            	        	   		    	else
        				 	            	        		    			        		    		global_buzzer=0;

        				 	    }
        				 	  }

        		    	break;
        				case 2: // mode conectado
        					global_recv_connect++;
        									 name = cJSON_GetObjectItem(jsons1, "contextResponses");
        		        				 	 name1 =  cJSON_GetArrayItem(name, 0);
        		        				 	 name2 = cJSON_GetObjectItem(name1,"contextElement");
        		        				 	 name3 = cJSON_GetObjectItem(name2,"attributes");
        		        				 	  for (i = 0 ; i < cJSON_GetArraySize(name3) ; i++)
        		        				 	  {
        		        				 	     cJSON * subitem = cJSON_GetArrayItem(name3, i);
        		        				 	    cJSON * index;
        		        				 	     name4 = cJSON_GetObjectItem(subitem, "name");
        		        				 	     if (strcmp(name4->valuestring,"Alarma_src")==0){
        		        				 	    	 index = cJSON_GetObjectItem(subitem, "value");

        		        				 	    	if (!(strcmp(last_alarm_src_trigger,index->valuestring)==0)) // son iguales no se actua sobre buzzer
        		        				 	    		{
        		        				 	    			strcpy(last_alarm_src_trigger,index->valuestring); // para que no lo desconecte constantemente, sólo una vez
        		        				 	    			global_alarm_trigger_time=xTaskGetTickCount();
        		        				 	    			global_buzzer=0;
        		        				 	    		}
        		        				 	     }
        		        				 	  }
        		    		break;
        			}

                    //sprintf(buffer_response,"%c\r\n",global_alarm);
                    //print_buffer("ALARM","He terminado de procesar\r\n");
        		    cJSON_Delete(jsons1);
                    ORION_req.status=0;
                    ORION_req.src=0;
            		xSemaphoreGive(ORION_xSem);
            		vTaskDelay((1+rand()%2)/portTICK_RATE_MS );
        	}else{
                xSemaphoreGive(ORION_xSem);

        	}
        }
	}
}
