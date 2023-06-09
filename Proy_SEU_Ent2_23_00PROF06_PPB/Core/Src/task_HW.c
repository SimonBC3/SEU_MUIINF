#include <tareas.h>
#include "Task_HW.h"
#include <stdint.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task_CONSOLE.h"
#include "task_WIFI.h"
#include "task_HW.h"

// operativa
// operativa
uint8_t global_sensor; // sensor seleccionado en modo local

float  global_sensor_level_max[2];
float  global_sensor_level_min[2];
float  global_sensor_level_alarm[2];   // nivel de alarma por sensor
float  global_sensor_level[2];   	   // nivel de sensor
float global_pot;

char  		global_LEDS[8];
uint8_t 	global_buzzer;
TickType_t global_alarm_trigger_time;


uint32_t global_hw_it;
uint32_t HZ_parpadeo;




void Task_HW( void *pvParameters ){


	int t,hw_two_buttons;
	uint32_t valueAD;
	ADC_ChannelConfTypeDef sConfig = { 0 };
	GPIO_PinState switch_1, switch_2, switch_1_ant, switch_2_ant;

    float aux;

    TickType_t last_time_alarm_change=0;

    hw_two_buttons=0;

	switch_1_ant = GPIO_PIN_SET;
	switch_2_ant = GPIO_PIN_SET;
	global_hw_it = 0;

	HZ_parpadeo=100;

	global_sensor=0;


	global_sensor_level_max[0]=30.0;
	global_sensor_level_min[0]=25.0;
	global_sensor_level_max[1]=100.0;
	global_sensor_level_min[1]=0.0;
	global_sensor_level_alarm[0]=100.0;   // nivel de alarma por sensor
	global_sensor_level_alarm[1]=100.0;   // nivel de alarma por sensor

	global_alarm_trigger_time=xTaskGetTickCount();


	bprintf (PASCU_PRJ " at "__TIME__ "\r\n");
	bprintf ("Module: " IoT_NAME "\r\n\r\n");


	while (1) {
		global_hw_it++;

		//leer los microinterruptores
		switch_1 = HAL_GPIO_ReadPin(PULSADOR1_GPIO_Port, PULSADOR1_Pin);
		switch_2 = HAL_GPIO_ReadPin(PULSADOR2_GPIO_Port, PULSADOR2_Pin);

		if ((switch_1 == GPIO_PIN_RESET) && (switch_2 == GPIO_PIN_RESET))
		{
			hw_two_buttons++;
			if (hw_two_buttons>20000){ // sin retardos

				 bprintf("\r\n*** RESETTING ***\r\n      ",20);
				 WIFI_Boot();
				 HAL_NVIC_SystemReset();
				 hw_two_buttons=0;
			}
		}
		else{

			hw_two_buttons=0;
		}

		// DIGITAL INPUT

		if (switch_2 == GPIO_PIN_SET) {
			if ((switch_1_ant == GPIO_PIN_SET) && (switch_1 == GPIO_PIN_RESET)) {
				global_sensor = (global_sensor + 1) % 2; // 0 a 1

			}
	    }

		// fin alarma
		// si se pulsa el boton derecho para el buzzer y marca el momento
		if ((switch_2_ant == GPIO_PIN_SET) && (switch_2 == GPIO_PIN_RESET)) {

				if (global_buzzer==1){
					/*al pulsar boton derecha se apaga y se toma el tiempo, no se puede encender si han pasado menos de 10 segundos*/
					global_alarm_trigger_time=xTaskGetTickCount();
					global_buzzer=0;

				}
		}

		// DIGITAL_OUTPUT

 		pon_Nivel(global_sensor, HZ_parpadeo);

		for (t = 0; t < 8; t++){
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
		}
	 		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin, global_buzzer);



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
			if (fabs(global_pot-valueAD)>109.0){

				HZ_parpadeo=40;
				last_time_alarm_change=xTaskGetTickCount();
				global_sensor_level_alarm[global_sensor]=global_sensor_level_min[global_sensor]+valueAD / 4095.0*(global_sensor_level_max[global_sensor]-global_sensor_level_min[global_sensor]);
				global_pot=valueAD;
			}


			{
			int value=xTaskGetTickCount();
			value=((value-last_time_alarm_change)/portTICK_RATE_MS);
			if (value>1500)
				HZ_parpadeo=5;
			}

			if (((xTaskGetTickCount()-global_alarm_trigger_time)/portTICK_RATE_MS)>10000){
			if (
					(global_sensor_level[0]>global_sensor_level_alarm[0]) ||
					(global_sensor_level[1]>global_sensor_level_alarm[1])
			   )
			global_buzzer=1;

		}

	// gestión de alarmas

		if (global_buzzer==1){
			if ((switch_2_ant == GPIO_PIN_SET) && (switch_2 == GPIO_PIN_RESET)) {
				/*al pulsar boton derecha se apaga y se toma el tiempo, no se puede encender si han pasado menos de 10 segundos*/
				global_alarm_trigger_time=xTaskGetTickCount();
				global_buzzer=0;
			}
		}




		//detección de flancos
			switch_2_ant = switch_2;
			switch_1_ant = switch_1;
	    vTaskDelay(10/portTICK_RATE_MS );
	}
}



////////////////////////////////////////////////////////////////////////7 LED
void pon_Nivel(uint8_t sensor_sel, uint32_t Hz){

	int nled;
	int nleds;
	int ct;
	static TickType_t last_time=0;

	static int last_value_led=0;
	static int last_pos_led=0;


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

float valor_sensor_LDR() {
	return global_sensor_level[1];
}

float valor_sensor_NTC() {
	return global_sensor_level[0];
}


