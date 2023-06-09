/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);
void StartTask02(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		HAL_UART_Transmit(&huart2, (uint8_t*) ptr++, 1, 1000);
	}
	return len;
}

void setOffAlarm(float value, float trigger) {
	if (value > trigger ) {
	 	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_SET);
	} else {
	 	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_RESET);
	}
}

void turnOnLeds(float value) {
	float div = 3.3/8;
	if (value <= div) {
		put_leds(1);
	} else if (value <= div*2) {
		put_leds(3);
	} else if (value <= div*3) {
		put_leds(7);
	} else if (value <= div*4) {
		put_leds(15);
	} else if (value <= div*5) {
		put_leds(31);
	} else if (value <= div*6) {
		put_leds(63);
	} else if (value <= div*7) {
		put_leds(127);
	} else {
		put_leds(255);
	}
}

void turnOnLedsTemp(float initValue, float currentValue) {
	float div = 0.04;
	printf(" div value %.2f\n", (initValue + (div*2)));
	if (currentValue <= (initValue + div)) {
		put_leds(1);
	} else if (currentValue <= (initValue + (div*2))) {
		put_leds(3);
	} else if (currentValue <= (initValue + (div*3))) {
		put_leds(7);
	} else if (currentValue <= (initValue + (div*4))) {
		put_leds(15);
	} else if (currentValue <= (initValue + (div*5))) {
		put_leds(31);
	} else if (currentValue <= (initValue + (div*6))) {
		put_leds(63);
	} else if (currentValue <= (initValue + (div*7))) {
		put_leds(127);
	} else {
		put_leds(255);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  printf("PRJ: %s, Date: %s\r\n", "mi proyecto", __TIME__);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  GPIO_PinState leftButtonState = GPIO_PIN_SET;
  GPIO_PinState rightButtonState = GPIO_PIN_SET;
  int sensor = 0; //0 = LDR || 1 = NTC
  uint32_t valuePot = ConvertidorA_D(4);
  float lastPot = ((valuePot * 3.3)/4095.0);
  uint32_t tempInit = ConvertidorA_D(1);
  float initTempValue = ((4096-tempInit)*3.3)/4095;
  float trigger = lastPot;
  while (1)
  {
	 //Choose sensor
	 GPIO_PinState lbCurrentState = HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin);
	 if(leftButtonState != lbCurrentState && lbCurrentState == GPIO_PIN_RESET) {
		  leftButtonState = GPIO_PIN_RESET;
	 }

	 if(leftButtonState != lbCurrentState && lbCurrentState == GPIO_PIN_SET) {
		 leftButtonState = GPIO_PIN_SET;
		 if (sensor == 0) {
			 printf("changing to NTC\r\n");
			 sensor = 1;
		 } else {
			 printf("changing to LDR\r\n");
			 sensor = 0;
		 }
	 }



	 //LIGHT SENSOR
	 if (sensor == 0) {
		 uint32_t valueADLum = ConvertidorA_D(0);
		 float valueBigLum = 4096-valueADLum;
		 float lumValue = (valueBigLum*3.3)/4095;
		 printf("Value de la luz %.2f \r\n", lumValue);
		 turnOnLeds(lumValue);

		 //set trigger
		 uint32_t valuePot = ConvertidorA_D(4);
		 float currentPot = ((valuePot * 3.3)/4095.0);
		 printf("Value del pot %.2f \r\n", currentPot);
		 if (currentPot > (lastPot+0.35) || currentPot < (lastPot - 0.35)) {
			 printf("pot changed %.2f\r\n", currentPot);
			 lastPot = currentPot;
		 	 trigger = currentPot;
		 	 for(int i = 0; i<8;i++) {
		 		 turnOnLeds(currentPot);
		 		 for (int j = 0; j < 1000000; j++) {}
		 		 put_leds(0);
		 		 for (int j = 0; j < 1000000; j++) {}
		 	 }
		 	}
		 setOffAlarm(lumValue, trigger);
	 }

	 //TEMP SENSOR
 	 if (sensor == 1) {
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
	 GPIO_PinState rbCurrentState = HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin);
	 if(rightButtonState != rbCurrentState && rbCurrentState== GPIO_PIN_RESET) {
	 	rightButtonState = GPIO_PIN_RESET;
	 	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_RESET);
	 	for (int j = 0; j < 70000000; j++) {}
	 }

	 if(rightButtonState != rbCurrentState && rbCurrentState == GPIO_PIN_SET) {
	 	rightButtonState = GPIO_PIN_SET;
	 }

	 {int c;for (c = 0; c < 1000000; c++);}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED8_Pin|LED6_Pin|BUZZER_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED7_Pin|LED2_Pin|LED5_Pin|LED1_Pin
                          |LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 BTN1_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_13|BTN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : POT_Pin */
  GPIO_InitStruct.Pin = POT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(POT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED8_Pin LED6_Pin BUZZER_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED8_Pin|LED6_Pin|BUZZER_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED7_Pin LED2_Pin LED5_Pin LED1_Pin
                           LED4_Pin */
  GPIO_InitStruct.Pin = LED7_Pin|LED2_Pin|LED5_Pin|LED1_Pin
                          |LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN2_Pin */
  GPIO_InitStruct.Pin = BTN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

