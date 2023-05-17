/*
 * comunicaciones.c
 *
 *  Created on: Apr 27, 2023
 *      Author: pperez
 */

#include <stdio.h>
#include <string.h>
#include "comunicaciones.h"
#include "main.h"

int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
	    //ITM_SendChar( *ptr++ );
	   HAL_UART_Transmit(&huart2, (uint8_t*)ptr++,1,1000);
	}

	return len;
}


void ESP8266_RESET(void){
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

	 for (ct=0;ct<10000000;ct++);
	 HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
   	 HAL_UART_DMAStop(&huart1);
	 printf("XXXXX %s",buff_recv);
	 printf("XXXXX\r\n\n\n\n");
}


void ESP8266_Boot(void)

{
	unsigned int ct;

 	printf("Reseting...\r\n");

 	ESP8266_RESET();
 	printf("Init...\r\n");

 	HAL_UART_Init(&huart1);

 	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_RESET);

 	// version
 	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
 	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
 	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT\r\n",strlen("AT\r\n"),10000);
 	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
 	for (ct=0;ct<2000000;ct++);
 	HAL_UART_DMAStop(&huart1);
 	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
	printf("1: %s",buff_recv);


	// version
	for (ct=0;ct<2048;ct++) buff_recv[ct]=0;
	HAL_UART_Receive_DMA(&huart1, buff_recv,2048);
	HAL_UART_Transmit(&huart1, ( unsigned char *)"AT+GMR\r\n",strlen("AT+GMR\r\n"),10000);
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
	for (ct=0;ct<2000000;ct++);
	HAL_UART_DMAStop(&huart1);
	HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);

	printf("2: %s",buff_recv);


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


