#include <tareas.h>
#include <main.h>
#include "FreeRTOS.h"
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>

#include "semphr.h"
#include <string.h>

#include <task.h>
#include <math.h>


#include "task_CONSOLE.h"
#include "task_WIFI.h"
#include "task_TIME.h"
#include "task_COMM.h"
#include "task_HW.h"



//char  global_buzzer;
char  	global_alarm;
char  	global_alarm_src[20];
uint8_t global_alarm_trigger;
uint8_t global_alarm_trigger_value;


uint8_t global_sel_alarm_sensor;

uint8_t buff_recv[2048];



void CONFIGURACION_INICIAL(void){

	BaseType_t res_task;



 	IObuff=bufferCreat(128);

 	if (!IObuff) return;

	res_task=xTaskCreate( Task_HW,"Tarea_HW",2048,NULL,	NORMAL_PRIORITY,NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}

	res_task=xTaskCreate(Task_CONSOLE,"CONSOLA",2048,NULL,	NORMAL_PRIORITY,NULL);
 		if( res_task != pdPASS ){
 				printf("PANIC: Error al crear Tarea Visualizador\r\n");
 				fflush(NULL);
 				while(1);
 		}

 		Task_WIFI_init();
 		Task_TIME_init();
 		Task_COMM_init();


}


