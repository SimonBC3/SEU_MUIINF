/*
 * task_WIFI.h
 *
 *  Created on: 24 may. 2023
 *      Author: pperez
 */

#ifndef INC_TASK_WIFI_H_
#define INC_TASK_WIFI_H_
#include "main.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "task_WIFI.h"


extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

extern uint8_t buff_recv[2048];

// WIFI
#define SSID			  "routerSEU"
#define SSID_PASS		  "00000000"

//#define SSID			  "MiFibra-70DB"
//#define SSID_PASS		  "QSetep44"

//#define SSID			  ""
//#define SSID_PASS		  ""


extern uint32_t global_wifi_ready;

void WIFI_RESET(void);
void WIFI_Boot(void);
int checkWIFI();

uint8_t * ESP_Send_Request(uint8_t * dst_address, uint32_t dst_port, uint8_t * request);

#endif /* INC_TASK_WIFI_H_ */
