#include <stdint.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task_WIFI.h"
#include "task_CONSOLE.h"
#include <stdio.h>
#include <string.h>
#include "tareas.h"

uint32_t global_wifi_it;
uint32_t global_wifi_ready;

 uint8_t aux_buff_WIFI[2048];
 uint8_t buff_WIFI_response[2048];

void Task_WIFI_init(void){
	BaseType_t res_task;
	global_wifi_ready=0;
	res_task=xTaskCreate( Task_WIFI,"Tarea_WIFI",2048,NULL,	NORMAL_PRIORITY,NULL);
	if( res_task != pdPASS ){
		printf("PANIC: Error al crear Tarea WIFI\r\n");
		fflush(NULL);
		while(1);
}
}
void Task_WIFI( void *pvParameters ){

	global_wifi_it=0;
	WIFI_Boot();
	global_wifi_ready=1;

	while (1) {
		global_wifi_it++;
	    vTaskDelay(10/portTICK_RATE_MS );
	}
}

void WIFI_RESET(void){
	// RESET
	unsigned int ct;
	 HAL_GPIO_WritePin(ESP8266_RESET_GPIO_Port, ESP8266_RESET_Pin, GPIO_PIN_RESET);
	 for (ct=0;ct<1000000;ct++);
	 HAL_UART_Init(&huart1);
	 for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	 HAL_UART_Receive_DMA(&huart1, buff_recv,2048);

	 HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_RESET);

	 HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(ESP8266_RESET_GPIO_Port, ESP8266_RESET_Pin, GPIO_PIN_SET);

	 vTaskDelay(1000/portTICK_RATE_MS );
	 HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
   	 HAL_UART_DMAStop(&huart1);
	 bprintf("XXXXX %s",buff_recv);
	 bprintf("XXXXX\r\n\n\n\n");
}


void WIFI_Boot(void)

{
	unsigned int ct;

 	bprintf("Reseting...\r\n");

 	WIFI_RESET();
 	bprintf("Init...\r\n");

 	HAL_UART_Init(&huart1);

 	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_RESET);

 	// version
 	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
 	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
 	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT\r\n",strlen("AT\r\n"),10000);
 	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
	 vTaskDelay(100/portTICK_RATE_MS );
 	HAL_UART_DMAStop(&huart1);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
	bprintf("1: %s",buff_recv);


	// version
	/*for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+GMR\r\n",strlen("AT+GMR\r\n"),10000);
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
	 vTaskDelay(1000/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);

	bprintf("2: %s",buff_recv);
*/

	// Pon en modo station=1,  station+access_point=3
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *) "AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),100000);
	 vTaskDelay(100/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	bprintf("3: %s",buff_recv);

	// Programa la contraseña del access-point
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1,( unsigned char *) "AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n",strlen("AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"\r\n"),10000);
	vTaskDelay(5000/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	bprintf("4: %s",buff_recv);
/*
	// verifica si hay IP
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),10000);
	vTaskDelay(5000/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	bprintf("5: %s",buff_recv);*/

}

uint8_t * ESP_Send_Request(uint8_t * dst_address, uint32_t dst_port, uint8_t * request){
    int ct;
	// abrir conexión con
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	sprintf(( char *)aux_buff_WIFI,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",dst_address,(int)dst_port);
	HAL_UART_Transmit(&huart1, ( unsigned char *) aux_buff_WIFI,strlen((const char *)aux_buff_WIFI),10000);
	vTaskDelay(200/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	//bprintf("6e: %s",buff_recv);

	// enviar una peticion HTTP

	int lc=strlen((const char *)request);
	sprintf((char *)aux_buff_WIFI,"AT+CIPSEND=%d\r\n",lc);
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1,( const uint8_t *)aux_buff_WIFI,strlen((const char *)aux_buff_WIFI),10000);
	vTaskDelay(200/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	//printf("7: %s",buff_recv);

	// ahora HTTP

	for (ct=0;ct<2048;ct++) buff_WIFI_response[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_WIFI_response,2048);
	HAL_UART_Transmit(&huart1, request,strlen( (const char *)request),10000);
	vTaskDelay(2000/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	//printf("8: %s",buff_WIFI_response);


	// cerrar conexión

	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *) "AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),10000);
	vTaskDelay(200/portTICK_RATE_MS );
	HAL_UART_DMAStop(&huart1);
	//bprintf("6: %s",buff_recv);


	return buff_WIFI_response;
}

