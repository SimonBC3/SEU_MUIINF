/*
 * tareas.h
 *
 *  Created on: Apr 29, 2021
 *      Author: pperez
 */

#ifndef __TAREAS_H__
#define __TAREAS_H__

#include <stdint.h>
#include "main.h"
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


// definiciones


#define R25    10000.0
#define T25    298.15
#define BETA   3900



//operativa
#define FREERTOS_5SEG		(5000/portTICK_PERIOD_MS)

// WIFI
//#define SSID			  "Pascual"
//#define SSID_PASS		  "12345678z"

#define SSID			  "Caloret"
#define SSID_PASS		  "12345678z"

//#define SSID			  "DIGIFIBRA-sGYS"
//#define SSID_PASS		  "bftNcKQF7x"


// Orion Context IoT name
#define IoT_NAME   	 	  "SensorSEU_PPB01"
#define IoT_NAME_CLONE    "SensorSEU_PPB00"
// clon 3
// connected 2

#define IoT_MODO_INICIAL  3
// Orion Context Broker
//con VPN
//#define ORION_PORT   	 "1026"
//#define ORION_ADDRESS   "pperez-seu-or.disca.upv.es"
#define ORION_PORT   	 10000
#define ORION_ADDRESS    "pperez2.disca.upv.es"

#define TIME_PORT		 80
#define TIME_ADDRESS	 "worldclockapi.com"

// Orion Operations
#define ORION_UPDATE		"POST /v1/updateContext HTTP/1.1\r\nConnection: close\r\nContent-Type: application/json\r\nAccept: application/json\r\nContent-Length: "
#define ORION_UPDATE_BODY_TEMP   "{ \"contextElements\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"%s\",\"attributes\": [ {\"name\": \"Temperatura\",\"type\": \"floatArray\",\"value\": \"%f,%f,%f,%f\"},{\"name\": \"IntensidadLuz\",\"type\": \"floatArray\",\"value\": \"%f,%f,%f,%f\"},{\"name\": \"Alarma\",\"type\": \"boolean\",\"value\": \"%c\"}]}],\"updateAction\": \"APPEND\"}"
#define ORION_UPDATE_ALARM       "{ \"contextElements\": [{\"type\": \"Sensor\", \"isPattern\": \"false\",\"id\": \"%s\",\"attributes\": [ {\"name\": \"Alarma_src\",\"type\": \"string\",\"value\": \"%s_%d\"}]}],\"updateAction\": \"APPEND\"}"




#define ORION_QUERY 		"POST /v1/queryContext HTTP/1.1\r\nConnection: close\r\nContent-Type: application/json\r\nAccept: application/json\r\nContent-Length: "
#define ORION_QUERY_BODY 	"{\"entities\": [{\"type\": \"Sensor\",\"isPattern\": \"false\",\"id\": \"%s\"}]}"

#define TIME_QUERY   		"GET /api/json/cet/now HTTP/1.1\r\nAccept: text/html\r\nHost: worldclockapi.com"


#define BUFF_MAX_ITEMS		200
#define BUFF_ITEM_VACIO		255



typedef uint8_t BUFF_ITEM_t ;


typedef struct ORION_REQUEST_dummy {

	char    status;
	char 	src;
	char * machine_dest;
	uint32_t port_dest;
	char * 	HTTP_request;
	char *  HTTP_response;
} ORION_REQUEST_t;

// API interna
void BUFF_init(void);

void print_buffer(char * name,char * cad);
void print_bufferln(char * name,char * cad);

// Tareas

void BUFF_Task_Visualizador( void *pvParameters );
void BUFF_Task_Productor( void *pvParameters );
void BUFF_Task_Consumidor( void *pvParameters );
void ESP8266_Task_Configuracion ( void *pvParameters );
void ESP8266_Task_Estresador ( void *pvParameters );
void Task_ADQ( void *pvParameters );
void Task_Display( void *pvParameters );
void Task_Orion( void *pvParameters );
void Task_TEMP( void *pvParameters );
void Task_LEDS_ALARM( void *pvParameters );
void Task_HW( void *pvParameters );
void Task_time( void *pvParameters );
void Task_ALARM( void *pvParameters );
void Task_Operativa( void *pvParameters );

// operativa
void local_operativa ( void );

void  local_setpoint ( void );
void test_function ( void );

// ESP
void ESP8266_RESET(void);
void ESP8266_Boot(void);
void ESP8266_TEST(void);
void ESP8266_TEST_time(void);
void OrionContextBroker_update(void);


// API externa

void seminario_sincronizacion(void);
void ESP8266_CONFIGURACION_INICIAL(void);

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;

#define UART_ESP8266  &huart1


// recursos
extern ORION_REQUEST_t ORION_req;

// statistics

extern uint32_t global_hw_it;
extern uint32_t global_time_it;

extern uint32_t global_recv_clone;
extern uint32_t global_trans_clone;
extern uint32_t global_try_trans_clone;
extern uint32_t global_try_recv_clone;

extern uint32_t global_recv_connect;
extern uint32_t global_trans_connect;
extern uint32_t global_try_trans_connect;
extern uint32_t global_try_recv_connect;

extern uint32_t global_Orion_it;
extern uint32_t global_Orion_success;



// operativa
extern float  global_sensor_level_alarm[2];   // nivel de alarma por sensor
extern float  global_sensor_level[2];
extern float  global_sensor_level_max[2];
extern float  global_sensor_level_min[2];
extern float  global_pot;

extern char  global_LEDS[8];
extern uint8_t global_sensor; // sensor seleccionado en modo local
extern uint8_t 		global_buzzer;
extern uint8_t		global_switch_1;
extern uint8_t		global_switch_2;

extern float  global_sensor_level_alarm[2];
extern uint8_t global_sel_alarm_sensor;

//char  global_buzzer;
extern char  global_alarm;
extern char  global_alarm_src[20];
extern uint8_t global_alarm_trigger;
extern uint8_t global_alarm_trigger_value;
extern TickType_t global_alarm_trigger_time;
extern uint8_t global_alarm_remote_disconnect;
extern uint32_t global_alarm_remote_disconnect_count;
extern char last_alarm_src_trigger[100];



extern char  global_switch[2];
extern uint8_t global_mode;
extern uint8_t global_mode_ant; // detección de camb io de modo
extern uint8_t global_try_change_mode;


extern char global_time[50];
extern char global_time_init[50];

extern SemaphoreHandle_t BUFF_xSem ;
extern SemaphoreHandle_t VGLOBAL_xSem ;
extern SemaphoreHandle_t ORION_xSem;
extern BUFF_BUFFER_t buffer;

extern uint8_t buff_recv[2048];
extern char	buff_sprintf100[100];
extern uint8_t buff_sprintf100_1[100];
extern uint8_t buff_recv[2048];
extern uint8_t buff_sprintf_body[1024];
extern uint8_t buff_sprintf1024[1024];
extern uint8_t buff_response1[2048];


extern uint8_t buff_display[1024];

#define JSON_NAME_LENGTH 30
#define JSON_HOPS_LIMIT 4
#define JSON_VALUE_LIMIT 100
extern char * json_tree[JSON_HOPS_LIMIT]; // suficiente profundidad
extern char json_value[JSON_VALUE_LIMIT]; //suficiente espacion reales de

extern char * * nombre;
extern int level;
extern float val;
extern char buff[2048];
extern char *  value;

extern char buff_j[4000];
extern jsmn_parser p;
extern jsmntok_t tok[300];
extern int level;


#endif
