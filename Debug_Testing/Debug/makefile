################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
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
Debug_Testing.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Debug_Testing.list \

OBJCOPY_BIN += \
Debug_Testing.bin \


# All Target
all: Debug_Testing.elf secondary-outputs

# Tool invocations
Debug_Testing.elf: $(OBJS) $(USER_OBJS) /home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/Debug_Testing/STM32F407VGTX_FLASH.ld
	arm-none-eabi-gcc -o "Debug_Testing.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/michael/Desktop/MCU1/STM32CubeIDE_Workspace/Debug_Testing/STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Debug_Testing.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Debug_Testing.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Debug_Testing.list"
	@echo 'Finished building: $@'
	@echo ' '

Debug_Testing.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Debug_Testing.bin"
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
