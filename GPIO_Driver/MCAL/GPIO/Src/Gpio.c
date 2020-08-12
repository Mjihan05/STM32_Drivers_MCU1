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

Port_ConfigType* GlobalConfigPtr;


void Port_Init (const Port_ConfigType* ConfigPtr)
{
	gu8_PortInitStatus = MODULE_UNINITIALIZED;
	volatile  GPIO_RegTypes * pReg = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t moduleNo = 0U;
	uint8_t pinNo = 0U;
	uint16_t portsConfigured = 0U;

	if(ConfigPtr == NULL_PTR)
	{
		gu8_PortInitStatus = MODULE_INIT_FAILED;
		return;
	}

	GlobalConfigPtr = (Port_ConfigType*) ConfigPtr;

	for(loopItr0=0U; loopItr0<TOTAL_NO_OF_PINS; loopItr0++)
	{
		if((ConfigPtr[loopItr0]).PinMode != PIN_UNUSED)
		{
			moduleNo = loopItr0/16U;
			pinNo = loopItr0%16U;
			GPIO_Port_Enable(moduleNo);	/** Turns on the Clock for the configured Port */
			pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];

			portsConfigured |= SET_BIT(moduleNo);

			/** Configure the Pull Up/Down settings */
			REG_RMW32(&pReg->PUPDR.R,MASK_BITS(0x3U,pinNo*2U),((ConfigPtr[loopItr0]).PinPupDown)<<(pinNo*2U));

			/** Select the output settings */
			if(((ConfigPtr[loopItr0]).PinDirection)==PORT_PIN_OUT)
			{
				REG_RMW32(&pReg->OTYPER.R,MASK_BITS(0x1U,pinNo),((ConfigPtr[loopItr0]).PinOutputMode)<<(pinNo));

				/** Configure output speed settings */
				REG_RMW32(&pReg->OSPEEDR.R,MASK_BITS(0x3U,pinNo*2U),((ConfigPtr[loopItr0]).PinSlewRate)<<(pinNo*2U));
			}

			/** Set the o/p default value before setting the pin direction */
			if( ((ConfigPtr[loopItr0]).PinInitValue) == SET)
			{
				REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo),(SET)<<(pinNo));
			}
			else
			{
				REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo+16U),(SET)<<(pinNo+16U));
			}

			/** Set the mode of the GPIO Pins */
			if( ((ConfigPtr[loopItr0]).PinMode)==PIN_MODE_GPIO && ((ConfigPtr[loopItr0]).PinDirection==PORT_PIN_IN) )
			{
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),(PORT_PIN_IN)<<(pinNo*2U));
			}
			else if (((ConfigPtr[loopItr0]).PinMode)>=PIN_MODE_ALT_FUNC_0)
			{
				/** Select the Alternate Functionality in AFRL */
				if( pinNo < 8U )
				{
					REG_RMW32(&pReg->AFRL.R,MASK_BITS(0xFU,pinNo*4U),(((ConfigPtr[loopItr0]).PinMode-4U))<<(pinNo*4U));
				}
				else /** Select the Alternate Functionality in AFRH */
				{
					REG_RMW32(&pReg->AFRH.R,MASK_BITS(0xFU,pinNo*4U),(((ConfigPtr[loopItr0]).PinMode-4U))<<(pinNo*4U));
				}
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),(0x2U)<<(pinNo*2U));
			}
			else
			{
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),((ConfigPtr[loopItr0]).PinMode)<<(pinNo*2U));
			}
		}
	}

#if(GPIO_PORT_LOCK == STD_ON)
	for(loopItr0 =0U; (loopItr0 < TOTAL_NO_OF_PORTS); loopItr0++)
	{
		if( ((portsConfigured>>loopItr0)&0x1U) == SET )
		{
			GPIO_Port_Lock(loopItr0,0xFFFF);  /** Locks all the pins of the port until next reset */
		}
	}
#endif

	gu8_PortInitStatus = MODULE_INITIALIZED;

}

#if(PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection (Port_PinType Pin,Port_PinDirectionType Direction)
{
	if((gu8_PortInitStatus != MODULE_INITIALIZED)||(Pin>TOTAL_NO_OF_PINS))
	{
		return;
	}

	if( (GlobalConfigPtr[Pin]).PinDirectionChangeable == FALSE )
	{
		return;
	}

	volatile  GPIO_RegTypes * pReg = 0U;
	uint8_t moduleNo = Pin/16U;

	pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];
	REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,Pin*2U),(Direction)<<(Pin*2U));
}
#endif

void Port_RefreshPortDirection (void)
{
	if(gu8_PortInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	volatile  GPIO_RegTypes * pReg = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t moduleNo = 0U;
	uint8_t pinNo = 0U;

	for(loopItr0=0U; loopItr0<TOTAL_NO_OF_PINS; loopItr0++)
	{
		moduleNo = loopItr0/16U;
		pinNo = loopItr0%16U;
		pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];

		if(((GlobalConfigPtr[loopItr0]).PinMode != PIN_UNUSED) && (GlobalConfigPtr[loopItr0]).PinDirectionChangeable == FALSE )
		{
			/** Set the o/p default value before setting the pin direction */
			if( ((GlobalConfigPtr[loopItr0]).PinInitValue) == SET)
			{
				REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo),(SET)<<(pinNo));
			}
			else
			{
				REG_RMW32(&pReg->BSRR.R,MASK_BITS(0x1U,pinNo),(SET)<<(pinNo+16U));
			}

			/** Set the mode of the GPIO Pins */
			if( ((GlobalConfigPtr[loopItr0]).PinMode)==PIN_MODE_GPIO && ((GlobalConfigPtr[loopItr0]).PinDirection==PORT_PIN_IN) )
			{
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),(PORT_PIN_IN)<<(pinNo*2U));
			}
			else if ( ((GlobalConfigPtr[loopItr0]).PinMode)==PIN_MODE_GPIO && ((GlobalConfigPtr[loopItr0]).PinDirection==PORT_PIN_OUT) )
			{
				REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,pinNo*2U),(PORT_PIN_OUT)<<(pinNo*2U));
			}
			else
			{
				/** Do Nothing */;
			}
		}
		else
		{
			/** Do Nothing */;
		}
	}
}

#if(PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode (Port_PinType Pin,Port_PinModeType Mode)
{
	if((gu8_PortInitStatus != MODULE_INITIALIZED)||(Pin>TOTAL_NO_OF_PINS))
	{
		return;
	}

	if( (GlobalConfigPtr[Pin]).PinModeChangeable == FALSE )
	{
		return;
	}

	volatile  GPIO_RegTypes * pReg = 0U;
	uint8_t moduleNo = Pin/16U;
	uint8_t PinNo = Pin%16U;

	pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];
	if( (Mode)==PIN_MODE_GPIO && ((GlobalConfigPtr[Pin]).PinDirection==PORT_PIN_IN) )
	{
		REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,PinNo*2U),(PORT_PIN_IN)<<(PinNo*2U));
	}
	else if ((Mode)>=PIN_MODE_ALT_FUNC_0)
	{
		/** Select the Alternate Functionality in AFRL */
		if( (PinNo) < 8U )
		{
			REG_RMW32(&pReg->AFRL.R,MASK_BITS(0xFU,PinNo*4U),(Mode-4U)<<(PinNo*4U));
		}
		else /** Select the Alternate Functionality in AFRH */
		{
			REG_RMW32(&pReg->AFRH.R,MASK_BITS(0xFU,PinNo*4U),(Mode-4U)<<(PinNo*4U));
		}
		REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,PinNo*2U),(0x2U)<<(PinNo*2U));
	}
	else
	{
		REG_RMW32(&pReg->MODER.R,MASK_BITS(0x3U,PinNo*2U),(Mode)<<(PinNo*2U));
	}
}
#endif

#if(GPIO_PORT_LOCK == STD_ON)
void GPIO_Port_Lock(uint8_t moduleNo, uint16_t lockMask)
{
	const uint32_t lockBit = 0x00010000;
	volatile  GPIO_RegTypes * pReg = 0U;
	pReg = (GPIO_RegTypes *)Gpio_BaseAddress[moduleNo];
	lockMask = lockMask&0x0000FFFF;
	REG_WRITE32(&pReg->LCKR.R,(uint32_t)(lockBit|lockMask));
	REG_WRITE32(&pReg->LCKR.R,(uint32_t)(0x00000000U|lockMask));
	REG_WRITE32(&pReg->LCKR.R,(uint32_t)(lockBit|lockMask));
	REG_READ32(&pReg->LCKR.R);
	while(REG_READ32((&pReg->LCKR.R)>>16U)!=SET);
}

void GPIO_Port_Unlock(uint8_t moduleNo)
{
	GPIO_Reset_Port(moduleNo);
}
#endif

void GPIO_Port_Enable(uint8_t portNo)
{
	RCC_AHB1PeripheralClkEnable(portNo);	/** Turns on the Clock for the configured Port */
}

void GPIO_Port_Disable(uint8_t portNo)
{
	RCC_AHB1PeripheralClkDisable(portNo);	/** Turns off the Clock for the configured Port */
}

void GPIO_Reset_Port(uint8_t portNo)
{
	RCC_ResetAHB1Peripheral(portNo);		/** Resets the configured Port */
}















