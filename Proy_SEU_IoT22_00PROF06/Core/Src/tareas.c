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

// statistics

uint32_t global_hw_it;
uint32_t global_time_it;
uint32_t global_recv_clone;
uint32_t global_trans_clone;
uint32_t global_try_trans_clone;
uint32_t global_try_recv_clone;

uint32_t global_recv_connect;
uint32_t global_trans_connect;
uint32_t global_try_trans_connect;
uint32_t global_try_recv_connect;

uint32_t global_Orion_it;
uint32_t global_Orion_success;


// operativa
// operativa
uint8_t global_sensor; // sensor seleccionado en modo local

float  global_sensor_level_max[2];
float  global_sensor_level_min[2];
float  global_sensor_level_alarm[2];   // nivel de alarma por sensor
float  global_sensor_level[2];   	   // nivel de sensor
float global_pot;

char  global_LEDS[8];
uint8_t 		global_buzzer;
uint8_t		global_switch_1;
uint8_t		global_switch_2;

//char  global_buzzer;
char  	global_alarm;
char  	global_alarm_src[20];
uint8_t global_alarm_trigger;
uint8_t global_alarm_trigger_value;
TickType_t global_alarm_trigger_time;

uint8_t global_alarm_remote_disconnect;
uint32_t global_alarm_remote_disconnect_count;
char last_alarm_src_trigger[100];





char  global_switch[2];
uint8_t global_mode;   // modo de funcionamiento
uint8_t global_mode_ant; // detección de camb io de modo
uint8_t global_try_change_mode;

uint8_t global_sel_alarm_sensor;


char  global_time[50];
char  global_time_init[50];
ORION_REQUEST_t ORION_req;


char	buff_sprintf100[100];
uint8_t buff_sprintf100_1[100];
uint8_t buff_recv[2048];
uint8_t buff_sprintf_body[1024];
uint8_t buff_sprintf1024[1024];
uint8_t buff_response1[2048];


uint8_t buff_display[1024];


char * * nombre;
int level=0;
float val;
char buff[2048];
char *  value;

char buff_j[4000];
jsmn_parser p;
jsmntok_t tok[300];
int level;

#define JSON_NAME_LENGTH 30
#define JSON_HOPS_LIMIT 4
char * json_tree[JSON_HOPS_LIMIT]; // suficiente profundidad

char json_value[JSON_VALUE_LIMIT]; //suficiente espacion reales de

SemaphoreHandle_t BUFF_xSem = NULL;
SemaphoreHandle_t VGLOBAL_xSem = NULL;
SemaphoreHandle_t ORION_xSem = NULL;

BUFF_BUFFER_t buffer;

void Task_Display( void *pvParameters );
void Task_DMA( void *pvParameters );
void Task_Send( void *pvParameters );
void Task_Receive( void *pvParameters );

#define buffer_DMA_size 2048
uint8_t buffer_DMA[buffer_DMA_size];
extern UART_HandleTypeDef huart1;
#define UART_ESP_AT_WIFI (&huart1)




BUFF_BUFFER_t * IObuff;
BUFF_BUFFER_t * IObuff_rx;




void ESP8266_CONFIGURACION_INICIAL(void){
//Esta función configura la wifi para que una vez arranque se conecte con el accessPOINT
//una vez configurada la wifi se conectará con el access_point y obtendrá una IP
// y solo será necesario preocuparse de la conexión TCP
	BaseType_t res_task;


 	ORION_req.status=0;
	ORION_req.src=0;

	global_LEDS[0]='0';global_LEDS[1]='0';global_LEDS[2]='0';global_LEDS[3]='0';global_LEDS[4]=0;
	global_LEDS[3]='0';global_LEDS[4]='0';global_LEDS[5]='0';global_LEDS[6]='0';global_LEDS[7]=0;
	global_time[0]=0;
	global_time_init[0]=0;
	global_alarm='0';

	// 3 clone
	// 2 conectado
	global_mode=IoT_MODO_INICIAL;
	global_try_change_mode=0;
	global_mode_ant=1;

	// alarma
	global_alarm_trigger_value='0';
	global_alarm_trigger_time=0;

	global_alarm_remote_disconnect=0;
    global_alarm_remote_disconnect_count=10;
    last_alarm_src_trigger[0]=0;

	global_sensor_level_alarm[0]=10000.0; // imposible que salte la alarma
	global_sensor_level_alarm[1]=10000.0;
	global_sensor_level_max[0]=32.0;
	global_sensor_level_min[0]=25.0;
	global_sensor_level_max[1]=100.0;
	global_sensor_level_min[1]=0.0;
	global_sensor=0;


	// statistics
	global_recv_clone=0;
	global_recv_connect=0;
	global_hw_it=0;
	global_time_it=0;
	global_recv_clone=0;
	global_recv_connect=0;

	global_Orion_it=0;
	global_Orion_success=0;



	global_sel_alarm_sensor=0;			  // sensor temperatura
	//TO-DO mensaje
 	//printf("Proy_SEU_Orion "__TIME__ "\r\n");
	printf (PASCU_PRJ " at "__TIME__ "\r\n");
	printf ("Module: " IoT_NAME "\r\n\r\n");

 	// In/Out
 	fflush(0);

 	IObuff=bufferCreat(128);
 	if (!IObuff) return;

 	IObuff_rx=bufferCreat(512);
 	if (!IObuff_rx) return;

 		res_task=xTaskCreate(Task_Display,"DISPLAY",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
 		if( res_task != pdPASS ){
 				printf("PANIC: Error al crear Tarea Visualizador\r\n");
 				fflush(NULL);
 				while(1);
 		}

 		res_task=xTaskCreate(Task_DMA,"DMA",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
 			if( res_task != pdPASS ){
 					printf("PANIC: Error al crear Tarea Visualizador\r\n");
 					fflush(NULL);
 					while(1);
 			}


	ORION_xSem=xSemaphoreCreateMutex();
	if( ORION_xSem == NULL ){
		printf("PANIC: Error al crear el Semáforo ORION\r\n");
		fflush(NULL);
		while(1);
	}

	BUFF_xSem=xSemaphoreCreateMutex();
	if( BUFF_xSem == NULL ){
		printf("PANIC: Error al crear el Semaforo\r\n");
		fflush(NULL);
		while(1);
	}

	res_task=xTaskCreate( Task_Orion,"ORION",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Orion\r\n");
			fflush(NULL);
			while(1);
	}

	res_task=xTaskCreate( Task_TEMP,"UPDATE",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}

	res_task=xTaskCreate(  Task_LEDS_ALARM,"QUERY",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea LEDS_ALARM\r\n");
			fflush(NULL);
			while(1);
	}
	res_task=xTaskCreate( Task_HW,"Tarea_HW",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}

	res_task=xTaskCreate( Task_Operativa,"Tarea_Operativa",2048,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea Visualizador\r\n");
			fflush(NULL);
			while(1);
	}
/*
	res_task=xTaskCreate( Task_time,"TIME",512,NULL,	makeFreeRtosPriority(osPriorityNormal),NULL);
	if( res_task != pdPASS ){
			printf("PANIC: Error al crear Tarea TIME\r\n");
			fflush(NULL);
			while(1);
	}


	*/
}

 /********************************************************************************** tareas */

void ESP8266_Task_Estresador ( void *pvParameters ){
int ct;
	 while(1){
		 for (ct=0;ct++;ct<1000000){ct++;}
	 }
}




