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




extern BUFF_BUFFER_t * IObuff;
extern BUFF_BUFFER_t * IObuff_rx;




void Task_HW( void *pvParameters ){


	int t,hw_two_buttons;
	uint32_t valueAD;
	ADC_ChannelConfTypeDef sConfig = { 0 };
	GPIO_PinState switch_1, switch_2, switch_1_ant, switch_2_ant;
	char buff[80];
    float aux;
    TickType_t last_time_global_mode_change;

    hw_two_buttons=0;
    last_time_global_mode_change=0;
	switch_1_ant = GPIO_PIN_SET;
	switch_2_ant = GPIO_PIN_SET;
	global_hw_it = 0;

	while (1) {
		global_hw_it++;

		//leer los microinterruptores
		switch_1 = HAL_GPIO_ReadPin(PULSADOR1_GPIO_Port, PULSADOR1_Pin);
		switch_2 = HAL_GPIO_ReadPin(PULSADOR2_GPIO_Port, PULSADOR2_Pin);

		global_switch_1 = switch_1;
		global_switch_2 = switch_2;

//	if ((switch_1==GPIO_PIN_RESET)&&(switch_2==GPIO_PIN_RESET))
//		HAL_NVIC_SystemReset();

// con el boton 2 pulsado, el 1 cambia de modo
        //TODO
		// En todos los eventos poner un temporizador para validarlos no pueden cambiar a menos de 100 milisegundos
        // se evitan los rebotes.

	    //TODO

		//fusionar codigo
		//Consola ANSI
		//cambiar de modo conectado a modo clon si protección crear condiciones de carrera en las tareas de comunicacion.

		// detectar situaciones de bloqueo y realizar un reset del micro.

		if ((switch_1 == GPIO_PIN_RESET) && (switch_2 == GPIO_PIN_RESET))
		{
			hw_two_buttons++;
			if (hw_two_buttons>20000){ // sin retardos
				 global_mode=10;
				 IObuff->puts(IObuff,(BUFF_ITEM_t *)"\r\n*** RESETTING ***\r\n      ",20);
				 ESP8266_Boot();
				 HAL_NVIC_SystemReset();
				 hw_two_buttons=0;
			}
		}
		else{

			hw_two_buttons=0;
		}


		if (switch_2 == GPIO_PIN_RESET) {
			if ((switch_1_ant == GPIO_PIN_SET) && (switch_1 == GPIO_PIN_RESET)) {
				if ((xTaskGetTickCount()-last_time_global_mode_change)>(10/portTICK_RATE_MS)){
				last_time_global_mode_change = xTaskGetTickCount();
				if ((global_mode!=2)&&(global_mode!=3))
					global_mode = (global_mode + 1) % 5; // 0 a 4
				else
					global_try_change_mode=1;
				}
			}
		}

		if (switch_2 == GPIO_PIN_SET) {
			if ((switch_1_ant == GPIO_PIN_SET) && (switch_1 == GPIO_PIN_RESET)) {
				global_sensor = (global_sensor + 1) % 2; // 0 a 1

			}
		}

		if (global_mode==4) // modo local cambia global_sensor
				if (switch_2 == GPIO_PIN_SET) {
					global_sensor_level_alarm[global_sel_alarm_sensor]=map(global_pot,0.0,3.3,global_sensor_level_max[global_sel_alarm_sensor],global_sensor_level_min[global_sel_alarm_sensor]);
					if ((switch_1_ant == GPIO_PIN_SET) && (switch_1 == GPIO_PIN_RESET)) {
						global_sel_alarm_sensor = (global_sel_alarm_sensor  + 1) % 2; // 0 a 1
					}
				}


		for (t = 0; t < 8; t++)
			switch (t) {
			case 0:
				HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, global_LEDS[t]);
				break;
			case 1:
				HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, global_LEDS[t]);
				break;
			case 2:
				HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, global_LEDS[t]);
				break;
			case 3:
				HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, global_LEDS[t]);
				break;
			case 4:
				HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, global_LEDS[t]);
				break;
			case 5:
				HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, global_LEDS[t]);
				break;
			case 6:
				HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, global_LEDS[t]);
				break;
			case 7:
				HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, global_LEDS[t]);
				break;
			}

	 		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin, global_buzzer);

	 		if (global_mode!=3)//clon no toma medidas de sensores
	 		{
			// POT

			// configuracion del canal de entrada A/D
			sConfig.Channel = ADC_CHANNEL_4;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

			HAL_ADC_ConfigChannel(&hadc1, &sConfig);
			// Disparo la conversion
			HAL_ADC_Start(&hadc1);
			// Espero la finalización
			HAL_ADC_PollForConversion(&hadc1, 10000);
			// Leo el valor de la conversión 4095 y 0
			valueAD = HAL_ADC_GetValue(&hadc1);

			global_pot=valueAD * 3.3 / 4095.0;

			// LDR

			// configuracion del canal de entrada A/D
			sConfig.Channel = ADC_CHANNEL_0;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

			HAL_ADC_ConfigChannel(&hadc1, &sConfig);
			// Disparo la conversion
			HAL_ADC_Start(&hadc1);
			// Espero la finalización
			HAL_ADC_PollForConversion(&hadc1, 10000);
			// Leo el valor de la conversión 4095 y 0
			valueAD = HAL_ADC_GetValue(&hadc1);

			aux=(3.1-valueAD * 3.3 / 4095.0)*100.0/3.1;
					if (aux<0.0)
						aux=0.0;
					if (aux>100.0)
						aux=100.0;

			global_sensor_level[1]=aux;   	   // nivel de sensor

			// Temperatura

			sConfig.Channel = ADC_CHANNEL_1;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

			HAL_ADC_ConfigChannel(&hadc1, &sConfig);
			// Disparo la conversion
			HAL_ADC_Start(&hadc1);
			// Espero la finalización
			HAL_ADC_PollForConversion(&hadc1, 10000);
			// Leo el valor de la conversión 4095 y 0
			valueAD = HAL_ADC_GetValue(&hadc1);

			aux = BETA
					/ (log(
							(-10000.0 * 3.3 / (valueAD * 3.3 / 4095.9 - 3.3)
									- 10000.0) / R25) + BETA / T25) - 273.18;
			global_sensor_level[0]=aux;   	   // nivel de sensor
	 		}

	// gestión de alarmas
		if (global_alarm_trigger_value=='1'){
				global_alarm_trigger_value='0'; // borra el disparo
				if (((xTaskGetTickCount()-global_alarm_trigger_time)/portTICK_RATE_MS)>10000)
				/* solo se dispara si han pasado más de 10 segundos despues de la desconexion */
					global_buzzer=1;
			}

		if (global_buzzer==1){
			if ((switch_2_ant == GPIO_PIN_SET) && (switch_2 == GPIO_PIN_RESET)) {
				/*al pulsar boton derecha se apaga y se toma el tiempo, no se puede encender si han pasado menos de 10 segundos*/
				global_alarm_trigger_time=xTaskGetTickCount();
				global_buzzer=0;
				if (global_mode==3)
					global_alarm_remote_disconnect=1;
			}
		}




		//detección de flancos
			switch_2_ant = switch_2;
			switch_1_ant = switch_1;
	//vTaskDelay(1/portTICK_RATE_MS );
	}
}


void print_bufferln(char * name,char * cad){
int res;
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)name,strlen(name));
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)": ",2);
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)cad,strlen(cad));
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"\r\n",2);

}

void print_buffer(char * name,char * cad){
int res;
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)name,strlen(name));
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)": ",2);
	 res=IObuff->puts(IObuff,(BUFF_ITEM_t *)cad,strlen(cad));
}



//*****************************************************************
// funcion test
// ****************************************************************


void test_function ( void ){
int ct,res;
static int it;
float aux;
ADC_ChannelConfTypeDef sConfig = {0};
uint32_t valueAD;
char buffer[80];
GPIO_PinState switch_1,switch_2,switch_1_ant,switch_2_ant;

	if (global_mode!=global_mode_ant){
		global_mode_ant=global_mode;
	    res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"Modo TEST\r\n",11);
	    it=1;
	}

	 if ((it%5)==0) ESP8266_Boot();
	 it++;
	 // D3-D4-D5-D6-D7-D12-A3-D13
	 // PB3 PB5 PB4 PB10 PA8 PA6 PB0 PA5
	 int t,h,j;
     j=0;
	 while(j++<1)
	 for (t=0;t<9;t++){

     switch (t) {
			    	case 0:
			     	case 1:
			     	case 2:
			     	case 3:
			     	case 4:
			     	case 5:
			     	case 6:
			       	case 7:  global_LEDS[t]=1; break;
			       	case 8:  global_buzzer=1;
			     }
     for (h=0;h<500000;h++);
			    for (h=0;h<8;h++) global_LEDS[h]=0;
			    global_buzzer=0;

			    }

		// POT
	    res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"Modo TEST\r\n",11);
		sprintf(buffer, "POT(TEMP): %3.2f %cC",global_sensor_level_alarm[0],0xf8);
		print_bufferln("TEST", buffer);
		sprintf(buffer, "POT(LDR).: %3.2f %%",global_sensor_level_alarm[1]);
		print_bufferln("TEST", buffer);

	    // LDR
		sprintf(buffer, "LDR: %3.2f %%",global_sensor_level[1] );
		print_bufferln("TEST", buffer);

		// Temperatura
		sprintf(buffer, "TMP: %3.2f %cC", global_sensor_level[0],0xf8);
		print_bufferln("TEST", buffer);

		//leer los microinterruptores

		sprintf(buffer, "PUL: %d %d ", global_switch_1,global_switch_2);
    		print_bufferln("TEST", buffer);

    		 //ESP8266_TEST();
    	/*	 {int t;
    		  for (t=32;t<255;t++)
    			  {
    			  sprintf(buffer, "%x %c : ",t,t);
    			  res=IObuff->puts(IObuff,(BUFF_ITEM_t *)buffer,strlen(buffer));

    		  }
			}
			*/
}



//*****************************************************************
// funcion set point
// ****************************************************************

void  local_setpoint ( void ){

	int res;
	static uint32_t it;
	static TickType_t last_time_print;
	char buffer[80];

	if (global_mode!=global_mode_ant){
		global_mode_ant=global_mode;
	    res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"\r\n",2);
	    last_time_print=xTaskGetTickCount();
	    it=0;
	}
	it++;

	pon_Nivel(global_sel_alarm_sensor,30);

	if ((xTaskGetTickCount()-last_time_print)>(100/portTICK_RATE_MS)){
		last_time_print = xTaskGetTickCount();
		sprintf(buffer,( unsigned char *)"SETPOINT %8x \r",it);
		res=IObuff->puts(IObuff,(BUFF_ITEM_t *)buffer,strlen(buffer));
	}
}


//*****************************************************************
// funcion local
// ****************************************************************

void local_fase_2(void){

	pon_Nivel(global_sensor,5);

	// comprobar el nivel de alarma

	//temp
	if (global_sensor_level_alarm[0]<global_sensor_level[0])
	{	global_sensor=0; // se selecciona el sensor que está disparando
		global_alarm_trigger_value='1';
	}

	//ldr
	if (global_sensor_level_alarm[1]<global_sensor_level[1]){
		global_sensor=1; // se selecciona el sensor que está disparando
		global_alarm_trigger_value='1';
	}
}

void  local_operativa ( void ){

    int res;
	char buffer[80];
	static TickType_t last_time_print;
	static uint32_t it;

	if (global_mode!=global_mode_ant){
		global_mode_ant=global_mode;
	    res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"\r\n",2);
	    last_time_print=xTaskGetTickCount();
	    it=0;
	}
    it++;


	local_fase_2();

	if ((xTaskGetTickCount()-last_time_print)>(100/portTICK_RATE_MS)){
		last_time_print = xTaskGetTickCount();
		sprintf(buffer,( unsigned char *)"LOCAL %8x \r",it);
		res=IObuff->puts(IObuff,(BUFF_ITEM_t *)buffer,strlen(buffer));
	}

}

//*****************************************************************
// función clon
// ****************************************************************
// TODO

void clon ( void ){
	static uint32_t it;

char buffer[80];
static TickType_t last_time_print;

	int res;
	if (global_mode!=global_mode_ant){
		global_mode_ant=global_mode;
	    res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"\r\n",2);
	    last_time_print=xTaskGetTickCount();
		global_recv_clone=0;
		global_trans_clone=0;
		global_try_trans_clone=0;
		global_try_recv_clone=0;

	    it=0;
	}


	pon_Nivel(global_sensor,30);
	it++;

	if ((xTaskGetTickCount()-last_time_print)>(100/portTICK_RATE_MS)){
		last_time_print = xTaskGetTickCount();
		sprintf(buffer,( unsigned char *)"CLON %8x G:%d S:%d T:%d TT:%d R:%d TR: %d            "
		"\r",it,	global_Orion_it,	global_Orion_success,global_trans_clone,  global_try_trans_clone,   global_recv_clone,   global_try_recv_clone);

		res=IObuff->puts(IObuff,(BUFF_ITEM_t *)buffer,strlen(buffer));
	}

	// TODO falta la LDR
	// TODO falta la ALARMAS
	vTaskDelay(1/portTICK_RATE_MS );

}

//*****************************************************************
// función conectado
// ****************************************************************
// Conectado funciona como en local pero publica los datos y se puede actuar sobre la alarma

void conectado ( void ){
	static uint32_t it;

	int res;
	char buffer[80];

	static TickType_t last_time_print;


	if (global_mode!=global_mode_ant){
		global_mode_ant=global_mode;
	    res=IObuff->puts(IObuff,(BUFF_ITEM_t *)"\r\n",2);
	    last_time_print=xTaskGetTickCount();
	    global_trans_connect=0;
	    global_recv_connect=0;
		global_try_trans_connect=0;
		global_try_recv_connect=0;
	    it=0;
	}
	it++;
	// En modo conectado hace lo mismo que en local +
	local_fase_2();
	// time, Task Time pide a la tarea Orion la hora cada 10000 segundos la tarea time obtiene el tiempo
	// temp, Task temp publica la temperatura
	// alarm, la tarea LEDS_ALARM mira si alguien ha lanzado la alarma.7

	if ((xTaskGetTickCount()-last_time_print)>(100/portTICK_RATE_MS)){
		last_time_print = xTaskGetTickCount();

		sprintf(buffer,( unsigned char *)"CONECTADO %8x G:%d S:%d T:%d TT:%d R:%d TR: %d          "
		"\r",it,	global_Orion_it,	global_Orion_success,global_trans_connect,global_try_trans_connect, global_recv_connect, global_try_recv_connect);
		res=IObuff->puts(IObuff,(BUFF_ITEM_t *)buffer,strlen(buffer));
	}


	vTaskDelay(1/portTICK_RATE_MS );

}

