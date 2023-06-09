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


int Task_Orion_TimeOut(TickType_t timeout,char *src,char * resp, char msg1,char * msg, uint8_t * try_change_mode){


	TickType_t localtimeout=xTaskGetTickCount();
	int ct;


	localtimeout=xTaskGetTickCount();
	while ((strstr(src,resp)==NULL)&&((xTaskGetTickCount()-localtimeout)<(timeout/portTICK_RATE_MS)))
		if (*try_change_mode)
			return 2;

	if ((xTaskGetTickCount()-localtimeout)>=(timeout/portTICK_RATE_MS)){
		 print_buffer(msg1, "TIMEOUT");
  		 print_buffer(msg1, msg);

  		 ESP8266_RESET();
   		 localtimeout=xTaskGetTickCount();
  		 while ((xTaskGetTickCount()-localtimeout)<(6000/portTICK_RATE_MS))
  			if (*try_change_mode) return 2;
  		// verifica si hay IP

  		    HAL_UART_Init(UART_ESP8266);
  			for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
  			HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
  			HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
  			osDelay(1000);
  			HAL_UART_DMAStop(UART_ESP8266);
  			print_buffer("***1",buff_recv);
  			osDelay(2000);
  			HAL_UART_Init(UART_ESP8266);
  			  			for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
  			  			HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
  			  			HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
  			  			osDelay(1000);
  			  			HAL_UART_DMAStop(UART_ESP8266);
  			  			print_buffer("****2",buff_recv);
  			  			osDelay(2000);


  		 return 1;
       	}
	else
		return 0;
}



void Task_Orion( void *pvParameters ){
	int ct;
int i;
	int localtimeout;



	while(1){

		while (xSemaphoreTake(ORION_xSem, 20000/portTICK_RATE_MS  ) != pdTRUE )
		{   // si en 20 segundos no he entrado en orion mmm mal rollito harakiri
			print_buffer("\r\n\n\nHARAKIRI!!\n\n\n",buff_recv);
			//xSemaphoreGive(ORION_xSem);
			HAL_NVIC_SystemReset();
			i=i+1;
		}

//		if (ORION_req.status==0){ // cambia de modo si no hay ninguna petición en marcha
		   if (global_try_change_mode==1){
			   global_try_change_mode=0;
			   global_mode = (global_mode + 1) % 5; // 0 a 4
			   xSemaphoreGive(ORION_xSem);
			   osDelay(100);
			   continue;
		   }
	//	}

		if (!((global_mode==2)||(global_mode==3))){ // cuenda no esta en modo conectado o. clon no hace nada
			xSemaphoreGive(ORION_xSem);
			osDelay(100);
			continue;
		}

        if (ORION_req.status==1){
        	 global_Orion_it++;
        	 // open connection
        	 //print_buffer("ORION1","REQ=1\r\n");
        	 HAL_UART_DMAStop(UART_ESP8266);
        	 HAL_UART_Init(UART_ESP8266);
        	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
        	 HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
      	   	 sprintf(buff_sprintf100,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ORION_req.machine_dest,ORION_req.port_dest);
      	   	 HAL_UART_Transmit_DMA(UART_ESP8266, ( unsigned char *) buff_sprintf100, strlen( buff_sprintf100));

      	   	 //HAL_UART_Transmit(UART_ESP8266,( unsigned char *) buff_sprintf100, strlen( buff_sprintf100),100000); // cabecera
        	 //osDelay(100);

        	 if (Task_Orion_TimeOut(2000,buff_recv,"CONNECT\r\n", "CONNECT",buff_recv,&global_try_change_mode)){
        		 xSemaphoreGive(ORION_xSem);
        		 continue;
        	 }

        	 HAL_UART_DMAStop(UART_ESP8266);
        	 //print_buffer("ORION2", buff_recv);
        	 HAL_UART_Init(UART_ESP8266);
        	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
        	 HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
             sprintf(buff_sprintf100_1,( unsigned char *)"AT+CIPSEND=%d\r\n",strlen(ORION_req.HTTP_request));
        	 //HAL_UART_Transmit(UART_ESP8266,( unsigned char *) buff_sprintf100_1, strlen( buff_sprintf100_1),100000); // cabecera
             HAL_UART_Transmit_DMA(UART_ESP8266, ( unsigned char *) buff_sprintf100_1, strlen( buff_sprintf100_1));

             osDelay(10);

           	 if (Task_Orion_TimeOut(1000,buff_recv,">", "SEND",buff_recv,&global_try_change_mode)){
           		 xSemaphoreGive(ORION_xSem);
        	     continue;
        	 }

        	 HAL_UART_DMAStop(UART_ESP8266);

        	 //print_buffer("ORION3", buff_recv);
        	 //print_buffer("ORION33", buff_sprintf1024);
        	 HAL_UART_Init(UART_ESP8266);
        	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
        	 HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
        	 HAL_UART_Transmit_DMA(UART_ESP8266,  ORION_req.HTTP_request, strlen(  ORION_req.HTTP_request));
//       	 HAL_UART_Transmit(UART_ESP8266,( unsigned char *)  ORION_req.HTTP_request, strlen(  ORION_req.HTTP_request),100000); // cabecera
        	 //haz un bucle hasta que el buffer no crezca debido al comportamiento de orion
        	 //while(strstr(buff_recv,">")==NULL);


        	 if (Task_Orion_TimeOut(2000,buff_recv,"reasonPhrase", "SEND2",buff_recv,&global_try_change_mode))    	 {
	        	 xSemaphoreGive(ORION_xSem);
	             continue;
	         }
        	 osDelay(50);

			 HAL_UART_DMAStop(UART_ESP8266);
        	 //print_buffer("ORION3", buff_recv);
        	 cleanResponse(( unsigned char *)buff_recv,2048);
     		 //print_buffer("ORION4", buff_recv);

        	 if (ORION_req.HTTP_response!=NULL) {

        		  	 	    strncpy(ORION_req.HTTP_response,buff_recv,2048);
        		  	 	    if (strlen(buff_recv)<500)
        		  	 	    	print_buffer("ORION34", "\r\n\r\n\r\n UPPPPPs\r\n\n\n\n");
        		 	 	    //osDelay(100);
        		 	 	    //print_buffer("ORION4", "He terminado\r\n");
        	      			global_Orion_success++;
        	      		 }

        	 HAL_UART_Init(UART_ESP8266);
        	 sprintf(buff_sprintf100_1,( unsigned char *)"AT+CIPCLOSE\r\n");
        	 //HAL_UART_Transmit(UART_ESP8266,( unsigned char *) buff_sprintf100_1, strlen( buff_sprintf100_1),100000); // cabecera
        	 HAL_UART_Transmit_DMA(UART_ESP8266, ( unsigned char *) buff_sprintf100_1, strlen( buff_sprintf100_1));
        	 osDelay(10);


     		 // close connection
/*        	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
        	 HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
        	 sprintf(buff_sprintf100_1,( unsigned char *)"AT+CIPCLOSE\r\n");
     		 HAL_UART_Transmit(UART_ESP8266,( unsigned char *) buff_sprintf100_1, strlen( buff_sprintf100_1),100000); // cabecera
        	 osDelay(100);

        	 localtimeout=xTaskGetTickCount();
             while ((strstr(buff_recv,"CLOSED")==NULL)&&((xTaskGetTickCount()-localtimeout)<(2000/portTICK_RATE_MS)));

             if ((xTaskGetTickCount() - localtimeout)>= (2000 / portTICK_RATE_MS)) {
                ESP8266_RESET();
            	vTaskDelay(4000 / portTICK_RATE_MS);HAL_UART_DMAStop(UART_ESP8266);
				print_buffer("ORION6", buff_recv);
            	HAL_UART_DMAStop(UART_ESP8266);
				xSemaphoreGive(ORION_xSem);
				continue;
			}
         	 HAL_UART_DMAStop(UART_ESP8266);
*/
     		 ORION_req.status=2;
     		 //print_buffer("ORION4", "He terminado\r\n");
     		 xSemaphoreGive(ORION_xSem);

             }
        else{
            xSemaphoreGive(ORION_xSem);
        	vTaskDelay(10/portTICK_RATE_MS );
        }
	}
}


void ESP8266_RESET(void){
	// RESET
	int ct;
	 HAL_UART_DMAStop(UART_ESP8266);
	 HAL_UART_Init(UART_ESP8266);
	 	        	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	 	        	 HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);

		HAL_GPIO_WritePin(ESP8266_RESET_GPIO_Port, ESP8266_RESET_Pin, GPIO_PIN_RESET);
	  	 osDelay(100);
	  	HAL_GPIO_WritePin(ESP8266_RESET_GPIO_Port, ESP8266_RESET_Pin, GPIO_PIN_SET);
		osDelay(3000);
		HAL_UART_DMAStop(UART_ESP8266);
		print_buffer("XXXXX",buff_recv);
		print_buffer("XXXXX","\r\n\n\n\nXXX1");
}
void ESP8266_TEST(void){

	int ct;


	// version

for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
HAL_UART_Init(UART_ESP8266);
HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
ESP8266_RESET();
HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT\r\n",strlen("AT\r\n"),10000);
osDelay(1000);
HAL_UART_DMAStop(UART_ESP8266);
print_buffer("TEST_ESP",buff_recv);

HAL_UART_Init(UART_ESP8266);
for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT+GMR\r\n",strlen("AT+GMR\r\n"),10000);
osDelay(1000);
HAL_UART_DMAStop(UART_ESP8266);
print_buffer("TEST_ESP",buff_recv);
osDelay(500);
}
void ESP8266_Boot(void)
{
	int ct;
 	 HAL_UART_Init(UART_ESP8266);
 	 HAL_UART_DMAStop(UART_ESP8266);

 	 ESP8266_RESET();

		// version
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
	HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT+GMR\r\n",strlen("AT+GMR\r\n"),10000);
	osDelay(500);
	HAL_UART_DMAStop(UART_ESP8266);
	print_buffer("BOOT",buff_recv);


	    // Tiene IP?
		for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
		HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
		HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
		osDelay(500);
		HAL_UART_DMAStop(UART_ESP8266);
		print_buffer("BOOT",buff_recv);
		//HAL_UART_Transmit(&huart2, buff_recv,strlen(buff_recv),10000);


		// Pon en modo station=1,  station+access_point=3
		for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
		HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
		HAL_UART_Transmit(UART_ESP8266, ( unsigned char *) "AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),100000);
		osDelay(4000);
		HAL_UART_DMAStop(UART_ESP8266);
		//HAL_UART_Transmit(&huart2, buff_recv,strlen(buff_recv),10000);
		print_buffer("BOOT",buff_recv);

		// Programa la contraseña del access-point
		for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
		HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);

		HAL_UART_Transmit(UART_ESP8266,( unsigned char *) "AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n",strlen("AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n"),10000);
		print_buffer("BOOT",">>>>>>>>>>>>>>AT+CWJAP\r\n");
		osDelay(7000);
		HAL_UART_DMAStop(UART_ESP8266);
		print_buffer("BOOT",buff_recv);

		//HAL_UART_Transmit(&huart2, buff_recv,strlen(buff_recv),10000);


		// verifica si hay IP
		for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
		HAL_UART_Receive_DMA(UART_ESP8266, buff_recv,2048);
		HAL_UART_Transmit(UART_ESP8266, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
		print_buffer("BOOT",">>>>>>>>>>>>>>AT+CIFSR\r\n");
		osDelay(3000);
		HAL_UART_DMAStop(UART_ESP8266);
		print_buffer("",buff_recv);
		//HAL_UART_Transmit(&huart2, buff_recv,strlen(buff_recv),10000);

}
