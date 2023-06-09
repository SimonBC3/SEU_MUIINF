################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Task_HW.c \
../Core/Src/Task_Operativa.c \
../Core/Src/Task_Orion.c \
../Core/Src/Task_leds_alarm.c \
../Core/Src/Task_temp.c \
../Core/Src/Task_time.c \
../Core/Src/cJSON.c \
../Core/Src/cJSON_Utils.c \
../Core/Src/freertos.c \
../Core/Src/jsmn.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/tareas.c \
../Core/Src/tareas_auxiliares.c \
../Core/Src/utility.c \
../Core/Src/utility_hal.c 

OBJS += \
./Core/Src/Task_HW.o \
./Core/Src/Task_Operativa.o \
./Core/Src/Task_Orion.o \
./Core/Src/Task_leds_alarm.o \
./Core/Src/Task_temp.o \
./Core/Src/Task_time.o \
./Core/Src/cJSON.o \
./Core/Src/cJSON_Utils.o \
./Core/Src/freertos.o \
./Core/Src/jsmn.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/tareas.o \
./Core/Src/tareas_auxiliares.o \
./Core/Src/utility.o \
./Core/Src/utility_hal.o 

C_DEPS += \
./Core/Src/Task_HW.d \
./Core/Src/Task_Operativa.d \
./Core/Src/Task_Orion.d \
./Core/Src/Task_leds_alarm.d \
./Core/Src/Task_temp.d \
./Core/Src/Task_time.d \
./Core/Src/cJSON.d \
./Core/Src/cJSON_Utils.d \
./Core/Src/freertos.d \
./Core/Src/jsmn.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/tareas.d \
./Core/Src/tareas_auxiliares.d \
./Core/Src/utility.d \
./Core/Src/utility_hal.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DPASCU_PRJ="Proy_SEU_IoT22_00PROF06.elf"' -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Task_HW.cyclo ./Core/Src/Task_HW.d ./Core/Src/Task_HW.o ./Core/Src/Task_HW.su ./Core/Src/Task_Operativa.cyclo ./Core/Src/Task_Operativa.d ./Core/Src/Task_Operativa.o ./Core/Src/Task_Operativa.su ./Core/Src/Task_Orion.cyclo ./Core/Src/Task_Orion.d ./Core/Src/Task_Orion.o ./Core/Src/Task_Orion.su ./Core/Src/Task_leds_alarm.cyclo ./Core/Src/Task_leds_alarm.d ./Core/Src/Task_leds_alarm.o ./Core/Src/Task_leds_alarm.su ./Core/Src/Task_temp.cyclo ./Core/Src/Task_temp.d ./Core/Src/Task_temp.o ./Core/Src/Task_temp.su ./Core/Src/Task_time.cyclo ./Core/Src/Task_time.d ./Core/Src/Task_time.o ./Core/Src/Task_time.su ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/cJSON_Utils.cyclo ./Core/Src/cJSON_Utils.d ./Core/Src/cJSON_Utils.o ./Core/Src/cJSON_Utils.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/jsmn.cyclo ./Core/Src/jsmn.d ./Core/Src/jsmn.o ./Core/Src/jsmn.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/tareas.cyclo ./Core/Src/tareas.d ./Core/Src/tareas.o ./Core/Src/tareas.su ./Core/Src/tareas_auxiliares.cyclo ./Core/Src/tareas_auxiliares.d ./Core/Src/tareas_auxiliares.o ./Core/Src/tareas_auxiliares.su ./Core/Src/utility.cyclo ./Core/Src/utility.d ./Core/Src/utility.o ./Core/Src/utility.su ./Core/Src/utility_hal.cyclo ./Core/Src/utility_hal.d ./Core/Src/utility_hal.o ./Core/Src/utility_hal.su

.PHONY: clean-Core-2f-Src

