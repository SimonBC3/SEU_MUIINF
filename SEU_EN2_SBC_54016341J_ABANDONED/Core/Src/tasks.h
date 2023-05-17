#include "FreeRTOS.h"

#define HIGH_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)+1
#define NORMAL_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)
#define LOW_PRIORITY (( configMAX_PRIORITIES - 1 )>>1)-1

void CONFIGURACION_INICIAL(void);
void Task_HW( void *pvParameters );
void Task_WIFI( void *pvParameters );
void Task_Send( void *pvParameters );
void Task_Receive( void *pvParameters );
unsigned portBASE_TYPE makeFreeRtosPriority (osPriority priority);
