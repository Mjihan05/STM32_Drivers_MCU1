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

	if(NVIC.ISPR[regNo] & MASK_BIT(regPos))
	{
		return TRUE;
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

	if(NVIC.IABR[regNo] & MASK_BIT(regPos))
	{
		return TRUE;
	}
	return FALSE;
}


















