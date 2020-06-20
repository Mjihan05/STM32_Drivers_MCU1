################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include MCAL/Reset_and_Clocks/Src/subdir.mk
-include MCAL/Reset_and_Clocks/Cfg/subdir.mk
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
Reset_and_Clock_Drivers.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Reset_and_Clock_Drivers.list \

OBJCOPY_BIN += \
Reset_and_Clock_Drivers.bin \


# All Target
all: Reset_and_Clock_Drivers.elf secondary-outputs

# Tool invocations
Reset_and_Clock_Drivers.elf: $(OBJS) $(USER_OBJS) /home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/Reset_and_Clock_Drivers/STM32F407VGTX_FLASH.ld
	arm-none-eabi-gcc -o "Reset_and_Clock_Drivers.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/Reset_and_Clock_Drivers/STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Reset_and_Clock_Drivers.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Reset_and_Clock_Drivers.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Reset_and_Clock_Drivers.list"
	@echo 'Finished building: $@'
	@echo ' '

Reset_and_Clock_Drivers.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Reset_and_Clock_Drivers.bin"
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
