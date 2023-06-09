/*
 * tasks.c
 *
 *  Created on: May 17, 2023
 *      Author: sbrot
 */

#include "tasks.h"
#include "HW.h"
#include "semphr.h"

static SemaphoreHandle_t xSemIP = NULL;

int ip_status;

GPIO_PinState leftButtonState = GPIO_PIN_SET;
GPIO_PinState rightButtonState = GPIO_PIN_SET;
int sensorValue = 0; // 0 = LDR || 1 = NTC
uint32_t valuePot = 0;
float lastPot = 0;
uint32_t tempInit = 0;
float initTempValue = 0;
float trigger = 0;

void Task_HW( void *pvParameters ) {
	for(;;)
	  {
		printf('in HW');
		//runHW(); refactor pending

		//Choose sensor
		GPIO_PinState lbCurrentState = HAL_GPIO_ReadPin(PULSADOR2_GPIO_Port, PULSADOR2_Pin);
		if(leftButtonState != lbCurrentState && lbCurrentState == GPIO_PIN_RESET) {
		leftButtonState = GPIO_PIN_RESET;
		 }

		if(leftButtonState != lbCurrentState && lbCurrentState == GPIO_PIN_SET) {
			leftButtonState = GPIO_PIN_SET;
			if (sensorValue == 0) {
				printf("changing to NTC\r\n");
				sensorValue = 1;
			} else {
				 printf("changing to LDR\r\n");
				 sensorValue = 0;
			}
			 }



			 //LIGHT SENSOR
			 if (sensorValue == 0) {
				 uint32_t valueADLum = ConvertidorA_D(0);
				 float valueBigLum = 4096-valueADLum;
				 float lumValue = (valueBigLum*3.3)/4095;
				 printf("Value de la luz %.2f \r\n", lumValue);
				 turnOnLedsLight(lumValue);

				 //set trigger
				 uint32_t valuePot = ConvertidorA_D(4);
				 float currentPot = ((valuePot * 3.3)/4095.0);
				 printf("Value del pot %.2f \r\n", currentPot);
				 if (currentPot > (lastPot+0.35) || currentPot < (lastPot - 0.35)) {
					 printf("pot changed %.2f\r\n", currentPot);
					 lastPot = currentPot;
				 	 trigger = currentPot;
				 	 for(int i = 0; i<8;i++) {
				 		turnOnLedsLight(currentPot);
				 		 for (int j = 0; j < 1000000; j++) {}
				 		 put_leds(0);
				 		 for (int j = 0; j < 1000000; j++) {}
				 	 }
				 	}
				 setOffAlarm(lumValue, trigger);
			 }

			 //TEMP SENSOR
		 	 if (sensorValue == 1) {
		 		uint32_t valueADTemp = ConvertidorA_D(1);
		 		float valueBigTemp = 4096-valueADTemp;
		 		float tempValue = (valueBigTemp*3.3)/4095;
		 		//float resis = ((3.3*10000)/((3.3-valueBigTemp)/(4095*3.3)))-10000;
		 		printf("Value de la temp %.2f \r\n Value init temp %.2f \n", tempValue, initTempValue);
		 		//printf("Value de la resis %.2f \r\n", resis);
		 		turnOnLedsTemp(initTempValue, tempValue);

		 		//adjust leds maxvalue - minvalue and set leds according to the currentValue
		 		setOffAlarm(tempValue, trigger);
		 	 }

			 //SCREAM
			 /*if (lumValue > trigger ) {
			 	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_SET);
			 } else {
			 	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_RESET);
			 }*/

			 //RESET SCREAM
			 GPIO_PinState rbCurrentState = HAL_GPIO_ReadPin(PULSADOR1_GPIO_Port, PULSADOR1_Pin);
			 if(rightButtonState != rbCurrentState && rbCurrentState== GPIO_PIN_RESET) {
			 	rightButtonState = GPIO_PIN_RESET;
			 	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_RESET);
			 	for (int j = 0; j < 70000000; j++) {}
			 }

			 if(rightButtonState != rbCurrentState && rbCurrentState == GPIO_PIN_SET) {
			 	rightButtonState = GPIO_PIN_SET;
			 }

	    osDelay(1);
	  }
}
void Task_WIFI( void *pvParameters ) {
	for(;;)
	  {
		xSemaphoreTake(xSemIP,100000);
		int ip_status = checkIP();
		{int c;for (c = 0; c < 10000000; c++);}
		if (ip_status == 0) {
			config_WIFI();
		} else {
			xSemaphoreGive(xSemIP);
		}
	    osDelay(1000);
	  }
}
void Task_Send( void *pvParameters ) {
	for(;;)
	  {
		sendHTTP();
	    osDelay(10000);

	  }
}
void Task_Receive( void *pvParameters ) {
	for(;;)
	  {
	    osDelay(1000);
	  }
}

void CONFIGURACION_INICIAL(void){
 BaseType_t res_task;
 res_task=xTaskCreate(Task_HW,"HW",1024,NULL,NORMAL_PRIORITY,NULL);
 		if( res_task != pdPASS ){
 				printf("PANIC: Error al crear Tarea HW\r\n");
 				fflush(NULL);
 				while(1);
 		}

 res_task=xTaskCreate(Task_WIFI,"WIFI",1024,NULL,NORMAL_PRIORITY ,NULL);
 			if( res_task != pdPASS ){
 					printf("PANIC: Error al crear Tarea WIFI\r\n");
 					fflush(NULL);
 					while(1);
 			}

 res_task=xTaskCreate(Task_Send,"SEND",1024,NULL,NORMAL_PRIORITY ,NULL);
 			 if( res_task != pdPASS ){
 				 printf("PANIC: Error al crear Tarea SEND\r\n");
 				 fflush(NULL);
 				 while(1);
 			 }

}
