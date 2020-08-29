/**********************************************
 * File - Nvic.c
 * Author - Michael Jihan
 * Created on 22-Aug-2020
 *
 * Purpose - This files contains the definition of
 * 			 functions and data structures that
 * 			 configures NVIC.
 *
 * ********************************************/

#include <stdint.h>
#include <Reg_Macros.h>

#include "NVIC_regTypes.h"
#include "Nvic.h"


void NVIC_EnableIRQ(IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return;
	}

	uint8_t regNo = IRQn/32;
	uint8_t regPos = IRQn%32;

	NVIC.ISER[regNo] |= SET_BIT(regPos);

}

void NVIC_DisableIRQ(IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return;
	}

	uint8_t regNo = IRQn/32;
	uint8_t regPos = IRQn%32;

	NVIC.ICER[regNo] |= SET_BIT(regPos);

}

uint32_t NVIC_GetPendingIRQ (IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return 0xFFFFFFFF;
	}

	uint8_t regNo = IRQn/32;
	uint8_t regPos = IRQn%32;
	uint32_t regValue = NVIC.ISPR[regNo];

	if(regValue & MASK_BIT(regPos))
	{
		return IRQn;
	}
	return FALSE;
}

void NVIC_SetPendingIRQ (IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return;
	}

	uint8_t regNo = IRQn/32;
	uint8_t regPos = IRQn%32;

	NVIC.ISPR[regNo] |= SET_BIT(regPos);
}

void NVIC_ClearPendingIRQ (IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return;
	}

	uint8_t regNo = IRQn/32;
	uint8_t regPos = IRQn%32;

	NVIC.ICPR[regNo] |= SET_BIT(regPos);
}

uint32_t NVIC_GetActive (IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return FALSE;
	}

	uint8_t regNo = IRQn/32;
	uint8_t regPos = IRQn%32;
	uint32_t regValue = NVIC.IABR[regNo];

	if(regValue & MASK_BIT(regPos))
	{
		return IRQn;
	}
	return FALSE;
}

void NVIC_SetPriority (IRQn_t IRQn, uint32_t priority)
{
	if((IRQn >= TOTAL_NO_OF_INTERRUPTS)||(priority>255U))
	{
		return;
	}

	uint8_t regNo = IRQn/4;
	uint8_t regPos = 8*(IRQn%4);

	NVIC.IPR[regNo] |= priority<<(regPos+4U);
}

uint32_t NVIC_GetPriority (IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return 0xFFFFFFFF;
	}

	uint8_t regNo = IRQn/4;
	uint8_t regPos = 8*(IRQn%4);
	uint32_t regValue = NVIC.IPR[regNo];

	return (uint32_t)((regValue & MASK_BITS(0xFU,(regPos+4U)))>>(regPos+4U));
}

/** Make sure to SET the USERSETMPEND bit in the SCR.CCR before call */
void NVIC_SoftwareTrig (IRQn_t IRQn)
{
	if(IRQn >= TOTAL_NO_OF_INTERRUPTS)
	{
		return;
	}

	NVIC.STIR.INTID = IRQn;
}














