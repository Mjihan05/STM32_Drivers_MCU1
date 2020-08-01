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
	volatile  GPIO_RegTypes * pReg = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t moduleNo = 0U;
	uint8_t pinNo = 0U;

	if(ConfigPtr == NULL_PTR)
	{
		gu8_PortInitStatus = MODULE_INIT_FAILED;
		return;
	}

	for(loopItr0=0U; loopItr0<TOTAL_NO_OF_PINS; loopItr0++)
	{
		if((ConfigPtr[loopItr0]).PinMode != PIN_UNUSED)
		{
			moduleNo = loopItr0/16U;
			pinNo = loopItr0%16U;
			RCC_AHB1PeripheralClkEnable(moduleNo);	/** Turns on the Clock for the configured Port */
			pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];

			/** Configure the Pull Up/Down settings */
			REG_RMW32(&pReg->PUPDR.R,MASK_BITS(0x2U,pinNo*2U),((ConfigPtr[loopItr0]).PinPupDown)<<(pinNo*2U));

			/** Select the output settings */
			if(((ConfigPtr[loopItr0]).PinDirection)==PORT_PIN_OUT)
			{
				REG_RMW32(&pReg->OTYPER.R,MASK_BITS(0x1U,pinNo),((ConfigPtr[loopItr0]).PinOutputMode)<<(pinNo));

				/** Configure output speed settings */
				REG_RMW32(&pReg->OSPEEDR.R,MASK_BITS(0x2U,pinNo*2U),((ConfigPtr[loopItr0]).PinSlewRate)<<(pinNo*2U));
			}

			/** Set the o/p default value before setting the pin direction */
			REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo),((ConfigPtr[loopItr0]).PinInitValue)<<(pinNo));

			/** Set the mode of the GPIO Pins */
			if( ((ConfigPtr[loopItr0]).PinMode)==PIN_MODE_GPIO && ((ConfigPtr[loopItr0]).PinDirection==PORT_PIN_IN) )
			{
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),(PORT_PIN_IN)<<(pinNo*2U));
			}
			else if (((ConfigPtr[loopItr0]).PinMode)>=PIN_MODE_ALT_FUNC_0)
			{
				/** Select the Alternate Functionality in AFRL */
				if( (((ConfigPtr[loopItr0]).PinMode)<=PIN_MODE_ALT_FUNC_7))
				{
					REG_RMW32(&pReg->AFRL.R,MASK_BITS(0xFU,pinNo*4U),(((ConfigPtr[loopItr0]).PinMode-4U))<<(pinNo*4U));
				}
				else /** Select the Alternate Functionality in AFRH */
				{
					REG_RMW32(&pReg->AFRH.R,MASK_BITS(0xFU,pinNo*4U),(((ConfigPtr[loopItr0]).PinMode-4U))<<(pinNo*4U));
				}
			}
			else
			{
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),((ConfigPtr[loopItr0]).PinMode)<<(pinNo*2U));
			}

			gu8_PortInitStatus = MODULE_INITIALIZED;

		}
	}


}




















