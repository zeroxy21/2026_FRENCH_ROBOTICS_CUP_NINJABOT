################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/VL53L0X/core/src/vl53l0x_api.c \
../Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.c \
../Core/Src/VL53L0X/core/src/vl53l0x_api_core.c \
../Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.c \
../Core/Src/VL53L0X/core/src/vl53l0x_api_strings.c 

OBJS += \
./Core/Src/VL53L0X/core/src/vl53l0x_api.o \
./Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.o \
./Core/Src/VL53L0X/core/src/vl53l0x_api_core.o \
./Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.o \
./Core/Src/VL53L0X/core/src/vl53l0x_api_strings.o 

C_DEPS += \
./Core/Src/VL53L0X/core/src/vl53l0x_api.d \
./Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.d \
./Core/Src/VL53L0X/core/src/vl53l0x_api_core.d \
./Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.d \
./Core/Src/VL53L0X/core/src/vl53l0x_api_strings.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/VL53L0X/core/src/%.o Core/Src/VL53L0X/core/src/%.su Core/Src/VL53L0X/core/src/%.cyclo: ../Core/Src/VL53L0X/core/src/%.c Core/Src/VL53L0X/core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/Src/VL53L0X/core/inc -I../Core/Src/VL53L0X/platform/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-VL53L0X-2f-core-2f-src

clean-Core-2f-Src-2f-VL53L0X-2f-core-2f-src:
	-$(RM) ./Core/Src/VL53L0X/core/src/vl53l0x_api.cyclo ./Core/Src/VL53L0X/core/src/vl53l0x_api.d ./Core/Src/VL53L0X/core/src/vl53l0x_api.o ./Core/Src/VL53L0X/core/src/vl53l0x_api.su ./Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.cyclo ./Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.d ./Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.o ./Core/Src/VL53L0X/core/src/vl53l0x_api_calibration.su ./Core/Src/VL53L0X/core/src/vl53l0x_api_core.cyclo ./Core/Src/VL53L0X/core/src/vl53l0x_api_core.d ./Core/Src/VL53L0X/core/src/vl53l0x_api_core.o ./Core/Src/VL53L0X/core/src/vl53l0x_api_core.su ./Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.cyclo ./Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.d ./Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.o ./Core/Src/VL53L0X/core/src/vl53l0x_api_ranging.su ./Core/Src/VL53L0X/core/src/vl53l0x_api_strings.cyclo ./Core/Src/VL53L0X/core/src/vl53l0x_api_strings.d ./Core/Src/VL53L0X/core/src/vl53l0x_api_strings.o ./Core/Src/VL53L0X/core/src/vl53l0x_api_strings.su

.PHONY: clean-Core-2f-Src-2f-VL53L0X-2f-core-2f-src

