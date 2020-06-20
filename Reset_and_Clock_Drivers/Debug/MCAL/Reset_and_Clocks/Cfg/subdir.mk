################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCAL/Reset_and_Clocks/Cfg/RCC_PbCfg.c 

OBJS += \
./MCAL/Reset_and_Clocks/Cfg/RCC_PbCfg.o 

C_DEPS += \
./MCAL/Reset_and_Clocks/Cfg/RCC_PbCfg.d 


# Each subdirectory must supply rules for building sources it contributes
MCAL/Reset_and_Clocks/Cfg/RCC_PbCfg.o: ../MCAL/Reset_and_Clocks/Cfg/RCC_PbCfg.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -DDEBUG -c -I../Inc -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/Reset_and_Clock_Drivers/MCAL/Reset_and_Clocks/Inc" -I"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/Reset_and_Clock_Drivers/MCAL/Reset_and_Clocks/Cfg" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MCAL/Reset_and_Clocks/Cfg/RCC_PbCfg.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

