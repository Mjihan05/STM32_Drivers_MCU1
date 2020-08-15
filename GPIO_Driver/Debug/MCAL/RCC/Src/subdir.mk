################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCAL/RCC/Src/RCC.c 

OBJS += \
./MCAL/RCC/Src/RCC.o 

C_DEPS += \
./MCAL/RCC/Src/RCC.d 


# Each subdirectory must supply rules for building sources it contributes
MCAL/RCC/Src/RCC.o: ../MCAL/RCC/Src/RCC.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -DDEBUG -c -I../Inc -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/MCAL/RCC/Inc" -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/MCAL/RCC/Cfg" -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/MCAL/GPIO/Inc" -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/MCAL/GPIO/Cfg" -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/MCAL/DIO/Inc" -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/MCAL/DIO/Cfg" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MCAL/RCC/Src/RCC.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

