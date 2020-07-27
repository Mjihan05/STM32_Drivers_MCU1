/**********************************************
 * File - Gpio.c
 * Author - Michael Jihan
 * Created on 25-Jul-2020
 *
 * Purpose - This files holds the Global functions
 * 			 that will configure and control
 * 			 Gpio settings
 *
 * ********************************************/


#include <stdint.h>
#include <Reg_Macros.h>

#include "RCC.h"
#include "Gpio.h"

InitStatusType gu8_PortInitStatus;


void Port_Init (const Port_ConfigType* ConfigPtr)
{
	gu8_PortInitStatus = MODULE_UNINITIALIZED;
	uint8_t loopItr0 = 0U;

	if(ConfigPtr == NULL_PTR)
	{
		gu8_PortInitStatus = MODULE_INIT_FAILED;
		return;
	}

	for(loopItr0=0U; loopItr0<TOTAL_NO_OF_PINS; loopItr0++)
	{
		if((ConfigPtr[loopItr0]).PinMode != PIN_UNUSED)
		{
			RCC_AHB1PeripheralClkEnable((loopItr0/16U));	/** Turns on the Clock for the configured Port */
			(ConfigPtr[loopItr0]).pGPIOX->(MODER.R) = (ConfigPtr[loopItr0])->PinMode;
		}
	}

	/** Set the op defalult value befire settunf the pin direction */
}




















