/*
 * Task_Operativa.c
 *
 *  Created on: Jun 3, 2022
 *      Author: ppere
 */
#include "tareas.h"

void conectado ( void );
void clon ( void );

void Task_Operativa( void *pvParameters ){



while(1){
	switch(global_mode){
	case 0:// local
			local_operativa ();
	 		break;
	case 1:// test
			test_function();
	 		break;
	case 2:// conectado
			conectado();
	 		break;
	case 3: // clon
			clon();


		 	break;
	case 4: // setup alarmas
			local_setpoint ();
			break;
	}
	vTaskDelay(1/portTICK_RATE_MS );
}
}
