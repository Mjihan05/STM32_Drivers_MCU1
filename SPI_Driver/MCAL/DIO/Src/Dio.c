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

uint16_t gu16_Port_IpPinsMask[TOTAL_NO_OF_PORTS]= {0};
uint16_t gu16_Port_OpPinsMask[TOTAL_NO_OF_PORTS]= {0};

/** Local Function Prototypes */
uint16_t Dio_GetIpPortMask(uint8_t PortId);
uint16_t Dio_GetOpPortMask(uint8_t PortId);

Dio_LevelType Dio_ReadChannel (Dio_ChannelType ChannelId)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return MODULE_UNINITIALIZED;
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

	if(GlobalConfigPtr[ChannelId].PinDirection == PORT_PIN_IN)
	{
		return ((REG_READ32(&pReg->IDR.R))>>pinNo)&0x1U;
	}
	else
	{
		return ((REG_READ32(&pReg->ODR.R))>>pinNo)&0x1U;
	}
}

void Dio_WriteChannel (Dio_ChannelType ChannelId,Dio_LevelType Level)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	/** Parameter Checking */
	if(ChannelId== DIO_PIN_UNUSED)
	{
		return;//DIO_E_PARAM_INVALID_CHANNEL_ID; /** TODO- This error needs to be reported in DET
	}

	for(uint8_t loopItr=0; loopItr<DIO_PINS_CONFIGURED; loopItr++)
	{
		if(Dio_ChannelUsed[loopItr] == ChannelId)
		{
			break;
		}
		else if (loopItr == (DIO_PINS_CONFIGURED-1U))
		{
			return; //DIO_E_PARAM_INVALID_CHANNEL_ID; /** TODO- This error needs to be reported in DET   and the return value should be 0*/
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
			REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,(pinNo+16U)),(SET)<<(pinNo+16U));
		}
	}
}

/** NOTE - Call Dio_ConfigPortMasks() first only once */
Dio_PortLevelType Dio_ReadPort (Dio_PortType PortId)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return MODULE_UNINITIALIZED;
	}

	if(PortId>= TOTAL_NO_OF_PORTS)
	{
		return DIO_E_PARAM_INVALID_PORT_ID;
	}

	volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[PortId];

	/** Returns the level of all the DIO channels in the Port */
	return (Dio_PortLevelType)((REG_READ32(&pReg->IDR.R))&((uint16_t)(gu16_Port_IpPinsMask[PortId])));
}

/** NOTE - Call Dio_ConfigPortMasks() first only once */
void Dio_WritePort (Dio_PortType PortId,Dio_PortLevelType Level)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	if(PortId>= TOTAL_NO_OF_PORTS)
	{
		return; //DIO_E_PARAM_INVALID_PORT_ID;/** TODO- This error needs to be reported in DET   and the return value should be 0*/
	}

	volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[PortId];

	REG_RMW32(&pReg->ODR.R,((uint16_t)(gu16_Port_OpPinsMask[PortId])),Level);
}

Dio_PortLevelType Dio_ReadChannelGroup (const Dio_ChannelGroupType* ChannelGroupIdPtr)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return MODULE_UNINITIALIZED;
	}

	if(ChannelGroupIdPtr == NULL_PTR)
	{
		return DIO_E_PARAM_INVALID_GROUP;
	}

	volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[ChannelGroupIdPtr->port];

	/** Returns the level of all the DIO channels in the Group */
	return (Dio_PortLevelType)( ((REG_READ32(&pReg->IDR.R))&(ChannelGroupIdPtr->mask))>>ChannelGroupIdPtr->offset );
}

void Dio_WriteChannelGroup (const Dio_ChannelGroupType* ChannelGroupIdPtr,Dio_PortLevelType Level)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	if(ChannelGroupIdPtr == NULL_PTR)
	{
		return; // DIO_E_PARAM_INVALID_GROUP;/** TODO- This error needs to be reported in DET   and the return value should be 0*/
	}

	volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[ChannelGroupIdPtr->port];

	REG_RMW32(&pReg->ODR.R,(ChannelGroupIdPtr->mask),(Level<<(ChannelGroupIdPtr->offset)));
}

Dio_LevelType Dio_FlipChannel (Dio_ChannelType ChannelId)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return MODULE_UNINITIALIZED;
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

	if(GlobalConfigPtr[ChannelId].PinDirection == PORT_PIN_IN)
	{
		return (Dio_LevelType)((REG_READ32(&pReg->ODR.R)&MASK_BIT(pinNo))>>pinNo);
	}
	else
	{
		REG_RMW32(&pReg->ODR.R,(MASK_BIT(pinNo)),~(REG_READ32(&pReg->ODR.R) ));
		return (Dio_LevelType)((REG_READ32(&pReg->ODR.R)&MASK_BIT(pinNo))>>pinNo);
	}
}

void Dio_MaskedWritePort (Dio_PortType PortId,Dio_PortLevelType Level,Dio_PortLevelType Mask)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	if(PortId>= TOTAL_NO_OF_PORTS)
	{
		return; //DIO_E_PARAM_INVALID_PORT_ID;/** TODO- This error needs to be reported in DET   and the return value should be 0*/
	}

	volatile  GPIO_RegTypes * pReg = (GPIO_RegTypes *)Gpio_BaseAddress[PortId];

	REG_RMW32(&pReg->ODR.R,Mask,Level);
}

void Dio_ConfigPortMasks(void)
{
	/** Check if Port is Initialised */
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	uint16_t channelId;
	uint8_t portId;
	uint8_t pinNo;

	for(uint8_t loopItr=0U; loopItr<DIO_PINS_CONFIGURED; loopItr++)
	{
		channelId = Dio_ChannelUsed[loopItr];
		portId = channelId/16U;
		pinNo = channelId%16U;

		if(GlobalConfigPtr[channelId].PinDirection == PORT_PIN_IN)
		{
			gu16_Port_IpPinsMask[portId] =  ((gu16_Port_IpPinsMask[portId])|(SET<<pinNo));
		}
		else
		{
			gu16_Port_OpPinsMask[portId] =  ((gu16_Port_OpPinsMask[portId])|(SET<<pinNo));
		}
	}
}

uint16_t Dio_GetIpPortMask(uint8_t PortId)
{
	return (uint16_t)(gu16_Port_IpPinsMask[PortId]);
}

uint16_t Dio_GetOpPortMask(uint8_t PortId)
{
	return (uint16_t)(gu16_Port_OpPinsMask[PortId]);
}





