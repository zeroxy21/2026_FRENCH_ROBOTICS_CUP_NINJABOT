################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/VL53L0X/platform/src/vl53l0x_platform.c 

OBJS += \
./Core/Src/VL53L0X/platform/src/vl53l0x_platform.o 

C_DEPS += \
./Core/Src/VL53L0X/platform/src/vl53l0x_platform.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/VL53L0X/platform/src/%.o Core/Src/VL53L0X/platform/src/%.su Core/Src/VL53L0X/platform/src/%.cyclo: ../Core/Src/VL53L0X/platform/src/%.c Core/Src/VL53L0X/platform/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/Src/VL53L0X/core/inc -I../Core/Src/VL53L0X/platform/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-VL53L0X-2f-platform-2f-src

clean-Core-2f-Src-2f-VL53L0X-2f-platform-2f-src:
	-$(RM) ./Core/Src/VL53L0X/platform/src/vl53l0x_platform.cyclo ./Core/Src/VL53L0X/platform/src/vl53l0x_platform.d ./Core/Src/VL53L0X/platform/src/vl53l0x_platform.o ./Core/Src/VL53L0X/platform/src/vl53l0x_platform.su

.PHONY: clean-Core-2f-Src-2f-VL53L0X-2f-platform-2f-src

