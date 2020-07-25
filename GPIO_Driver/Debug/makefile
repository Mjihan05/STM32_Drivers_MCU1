################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include MCAL/RCC/Src/subdir.mk
-include MCAL/RCC/Cfg/subdir.mk
-include MCAL/GPIO/Src/subdir.mk
-include MCAL/GPIO/Cfg/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
GPIO_Driver.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
GPIO_Driver.list \

OBJCOPY_BIN += \
GPIO_Driver.bin \


# All Target
all: GPIO_Driver.elf secondary-outputs

# Tool invocations
GPIO_Driver.elf: $(OBJS) $(USER_OBJS) /home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/STM32F407VGTX_FLASH.ld
	arm-none-eabi-gcc -o "GPIO_Driver.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/GPIO_Driver/STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="GPIO_Driver.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

GPIO_Driver.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "GPIO_Driver.list"
	@echo 'Finished building: $@'
	@echo ' '

GPIO_Driver.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "GPIO_Driver.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
