/**********************************************
 * File - Dio.c
 * Author - Michael Jihan
 * Created on 08-Aug-2020
 *
 * Purpose - This files holds the Global functions
 * 			 that will Read/Modify the pins
 * 			 configured as GPIO in PORT
 *
 * ********************************************/


#include <stdint.h>
#include <Reg_Macros.h>

#include "Gpio.h"
#include "GPIO_regTypes.h"
#include "Dio.h"



Dio_LevelType Dio_ReadChannel (Dio_ChannelType ChannelId)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	/** Parameter Checking */
	for(uint8_t loopItr=0; loopItr<DIO_PINS_CONFIGURED; loopItr++)
	{
		if(Dio_ChannelUsed[loopItr] == ChannelId)
		{
			break;
		}
		else if (loopItr == (DIO_PINS_CONFIGURED-1U))
		{
			return DIO_E_PARAM_INVALID_CHANNEL_ID; /** TODO- This error needs to be reported in DET   and the return value should be 0*/
		}
	}

	uint8_t moduleNo = ChannelId/16U;
	uint8_t pinNo = ChannelId%16U;
	volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];

	return ((REG_READ32(&pReg->IDR.R))>>pinNo)&0x1U;
}

void Dio_WriteChannel (Dio_ChannelType ChannelId,Dio_LevelType Level)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	/** Parameter Checking */
	for(uint8_t loopItr=0; loopItr<DIO_PINS_CONFIGURED; loopItr++)
	{
		if(Dio_ChannelUsed[loopItr] == ChannelId)
		{
			break;
		}
		else if (loopItr == (DIO_PINS_CONFIGURED-1U))
		{
			return DIO_E_PARAM_INVALID_CHANNEL_ID; /** TODO- This error needs to be reported in DET   and the return value should be 0*/
		}
	}

	/** If the channel is configured as input then this API should not influence the pin */
	if(GlobalConfigPtr[ChannelId].PinDirection == PORT_PIN_IN)
	{
		return;
	}
	else
	{
		uint8_t moduleNo = ChannelId/16U;
		uint8_t pinNo = ChannelId%16U;
		volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];

		if(Level == STD_HIGH)
		{
			REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo),(SET)<<(pinNo));
		}
		else
		{
			REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo+16U),(SET)<<(pinNo+16U));
		}
	}
}










