/*
 * utility.h
 *
 *  Created on: Apr 30, 2021
 *      Author: pperez
 */

#ifndef INC_UTILITY_H_
#define INC_UTILITY_H_


#include "FreeRTOS.h"
#include "task.h"

#include <FreeRTOS.h>
#include <stdint.h>
#include <semphr.h>

unsigned portBASE_TYPE makeFreeRtosPriority (osPriority priority);
int _write(int file, char *ptr, int len);
void pon_Nivel(uint8_t sensor_sel, uint32_t Hz);
float map(float x, float in_min, float in_max, float out_min, float out_max);



typedef uint8_t BUFF_ITEM_t;
typedef struct BUFFER_CIRCULAR_dummy {

	uint8_t    (*get)(void *, BUFF_ITEM_t *);
	uint8_t    (*put)(void *, BUFF_ITEM_t);
	uint32_t   (*isempty)(void *);
	uint32_t   (*isfull)(void *);
	uint32_t   (*puts)( void *,BUFF_ITEM_t * cad,uint32_t length);
	//uint32_t   (*gets)(uint_t * cad,uint32_t length);

	SemaphoreHandle_t xSem;

	uint32_t 	cabeza;
	uint32_t	cola;
	uint32_t    size;
	BUFF_ITEM_t*	buff;
	uint32_t	n_elem;
} BUFF_BUFFER_t;

// API
BUFF_BUFFER_t * bufferCreat(int size);




#endif /* INC_UTILITY_H_ */
