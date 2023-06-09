/*
 * utility.c
 *
 *  Created on: Apr 30, 2021
 *      Author: pperez
 */

#include "main.h"
#include "tareas.h"
#include "cmsis_os.h"

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


float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned portBASE_TYPE makeFreeRtosPriority (osPriority priority)
{
unsigned portBASE_TYPE fpriority = tskIDLE_PRIORITY;

if (priority != osPriorityError) {
fpriority += (priority - osPriorityIdle);
}

return fpriority;
}

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



/////////////////////////////////////////////////////////////////////// Procesar

void cleanResponse(char * data,int maxlen)
{
	int t,h,l,o,i;
	char * res;
	int indexbk;
	char bytes[4];

	char * j,*from, *pc;

	res=data;


	 if (strlen(data)<300)
		 res=1;
	 else
		 res=0;
	 i=0;
	 while (j=strstr(data,"+IPD")){

		 from=strstr(j,":");
		 from++;

		 	 for (pc=from;pc<(data+2048);pc++)
			 	*(j++)=*(from++);
	 };
	 	 do
			                  {
			                  }while(data[i++]!='{');
			                  i--;
	for (t=0;t<(2048-i);t++)
		data[t]=data[t+i];

	i=0;

    for (t=0;t<2048;t++)
    	if (data[t]=='}')
    		i=t;
    data[i+1]=0;




}



////////////////////////////////////////////////////////////////////////7 LED
void pon_Nivel(uint8_t sensor_sel, uint32_t Hz){

	int nled;
	int nleds;
	int ct;
	static TickType_t last_time=0;
	static TickType_t ttime;
	static int last_value_led=0;
	static int last_pos_led=0;
    float alarm_aux;

    global_LEDS[last_pos_led]=0;
	for (ct=0;ct<8;ct++) global_LEDS[ct]=0; // borra el led alarma

	// pon nivel de señal
	nleds=trunc((global_sensor_level[sensor_sel]-global_sensor_level_min[sensor_sel])/(global_sensor_level_max[sensor_sel]-global_sensor_level_min[sensor_sel])*7.4);
	for (ct=0;ct<8;ct++)
	if (ct<nleds)
			global_LEDS[ct]=1;
	else
			global_LEDS[ct]=0;

	// este nivel tiene prioridad sobre el valor act, por eso va al final

	global_LEDS[last_pos_led]=last_value_led;

 	if (((float)(xTaskGetTickCount()-last_time)/portTICK_RATE_MS)>(float)1000.0/(float)Hz){
		last_value_led=!last_value_led;
		last_time= xTaskGetTickCount();

		nled=trunc((global_sensor_level_alarm[sensor_sel]-global_sensor_level_min[sensor_sel])/(global_sensor_level_max[sensor_sel]-global_sensor_level_min[sensor_sel])*7.4);
		last_pos_led=nled;
		if (last_value_led==1)
			global_LEDS[nled]=0;
		else
			global_LEDS[nled]=1;
	}
}




/////////////////////////////////////////////////////////////////////////////////////// BUFFER sin proteccion

uint32_t BUFF_is_empty(BUFF_BUFFER_t * buffer){
	return (buffer->n_elem==0);
}

uint32_t BUFF_is_full(BUFF_BUFFER_t * buffer){
	return (buffer->n_elem==buffer->size);
}

uint32_t BUFF_inserta(BUFF_BUFFER_t * buffer,BUFF_ITEM_t item){

	if (!buffer->isfull(buffer)){
		buffer->buff[buffer->cabeza]=item;
		buffer->cabeza=((buffer->cabeza)+1)%(buffer->size);
		buffer->n_elem++;
		return 1;
	} else return 0;
}

uint32_t BUFF_extrae(BUFF_BUFFER_t * buffer,BUFF_ITEM_t *item){

	if (!buffer->isempty(buffer)){
			*item=buffer->buff[buffer->cola];
			buffer->cola=(buffer->cola+1)%(buffer->size);
			buffer->n_elem--;

	} else return 0;

   return 1;
}

uint32_t BUFF_extrae_prot(BUFF_BUFFER_t * buffer,BUFF_ITEM_t *item)
{ //variable condicion
	int it;

	it=0;
	while (xSemaphoreTake(buffer->xSem, 10000/portTICK_RATE_MS  ) != pdTRUE );
	 // lock
	while(!BUFF_extrae(buffer,item)){
			 xSemaphoreGive(buffer->xSem);
	 		 vTaskDelay(100/portTICK_RATE_MS );
	 		 while (xSemaphoreTake(buffer->xSem, 10000/portTICK_RATE_MS  ) != pdTRUE );
	 		 it++;
	}
	xSemaphoreGive(buffer->xSem);
	return 1; // siempre tiene éxito
}
uint32_t BUFF_inserta_prot(BUFF_BUFFER_t * buffer,BUFF_ITEM_t item)
{ //variable condicion
	int it;

	it=0;
	while (xSemaphoreTake(buffer->xSem, 10000/portTICK_RATE_MS  ) != pdTRUE );
	 // lock
	while(!BUFF_inserta(buffer,item)){
	 		 xSemaphoreGive(buffer->xSem);
	 		 vTaskDelay(100/portTICK_RATE_MS );
	 		 while (xSemaphoreTake(buffer->xSem, 10000/portTICK_RATE_MS  ) != pdTRUE );
	 		 it++;
	}
	xSemaphoreGive(buffer->xSem);
	return 1; // siempre tiene éxito
}

uint32_t BUFF_inserta_cad(BUFF_BUFFER_t * buffer,BUFF_ITEM_t * cad,uint32_t nitems)
{ //variable condicion

	int it;
	uint32_t res;
	for (it=0;it<nitems;it++)
	{
		res=BUFF_inserta_prot(buffer,cad[it]);
		if (!res) return it;
	}
	return 1; // siempre tiene éxito
}


/*
typedef struct BUFFER_CIRCULAR_dummy {


	uint8_t    (*get)(uint8_t *);
	uint8_t    (*put)(uint8_t);
	uint32_t   (*BUFF_is_empty)(void);
	uint32_t   (*BUFF_is_full)(void);
	uint32_t   (*puts)(uint_t * cad,uint32_t length);
	uint32_t   (*gets)(uint_t * cad,uint32_t length);

	enum status{NOINIT,INIT};
	uint32_t 	cabeza;
	uint32_t	cola;
	uint8_t		buff[BUFF_MAX_ITEMS];
	uint32_t	n_elem;
} BUFF_BUFFER_t;
*/


// API
BUFF_BUFFER_t * bufferCreat(int size){
	BUFF_BUFFER_t * tmp;
	void * bff;

	tmp=malloc(sizeof(BUFF_BUFFER_t));
	if (tmp){ // enought for structure

		bff=malloc(sizeof(uint8_t)*size);
		if (bff){ //enought for buffer
			tmp->xSem=xSemaphoreCreateMutex();
			if (!tmp->xSem){
				free(bff);
				free(tmp);
				return NULL;
			};
            tmp->buff=bff;
			tmp->cabeza=0;
			tmp->cola=0;
			tmp->size=size;
			tmp->n_elem=0;
			tmp->get=(void*)BUFF_extrae_prot;
			tmp->put=(void*)BUFF_inserta_prot;
			tmp->puts=(void*)BUFF_inserta_cad;

			tmp->isfull=(void*)BUFF_is_full;
			tmp->isempty=(void*)BUFF_is_empty;
			return tmp;
		}
		else
			free(tmp);
	}
	return NULL;

}


