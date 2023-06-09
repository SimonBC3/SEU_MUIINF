/*
 * Task_COMM.h
 *
 *  Created on: 24 may. 2023
 *      Author: pperez
 */

#ifndef INC_TASK_COMM_H_
#define INC_TASK_COMM_H_


#include "FreeRTOS.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>

#include "semphr.h"

// internal

void cleanResponse(uint8_t * data,int maxlen);

typedef struct REQUEST_DUMMY {
								int32_t  command;
								int32_t  result;
								int32_t  dst_port;
								uint8_t * 	 dst_address;
								uint8_t * 	 HTTP_request;
								uint8_t *   HTTP_response;
} scomm_request_t;



extern scomm_request_t COMM_request;
extern SemaphoreHandle_t COMM_xSem;
extern uint32_t global_wifi_ready;

void Task_COMM_init(void);
void Task_COMM( void *pvParameters );


#endif /* INC_TASK_COMM_H_ */
