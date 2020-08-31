/**********************************************
 * File - ExtI.c
 * Author - Michael Jihan
 * Created on 30-Aug-2020
 *
 * Purpose - This files contains the definition of
 * 			 functions and data structures that
 * 			 configures EXT_I.
 *
 * ********************************************/

#include <Reg_Macros.h>

#include "SYSCFG_regTypes.h"
#include "EXTI_regTypes.h"
#include "Intc.h"

void EXTI_ConfigIRQ(EXTIn_t IRQn,TriggerEdge_t trigEdge,uint32_t priority)
{
	if((IRQn>EXTI_RTC_WKUP)||(trigEdge>2U)||(priority>255U))
	{
		return;
	}

	uint8_t regPos = (IRQn/9U);

	/** Configure EXTICR for the port */
	if(IRQn<EXTI_PVD_OUT)
	{
		SYSCFG.EXTICR[IRQn/36U].R |= (IRQn%9U)<<(4*regPos);
	}

	/** Configure the Mask for the EXTI Interrupt */
	EXTI.IMR.R |= SET<<regPos;

	/** Select the Trigger source */
	switch(trigEdge)
	{
	case EXTI_RISING_EDGE_TRIG:
		EXTI.RTSR.R |= SET<<regPos;
		break;
	case EXTI_FALLING_EDGE_TRIG:
		EXTI.FTSR.R |= SET<<regPos;
		break;
	case EXTI_BOTH_EDGE_TRIG:
		EXTI.RTSR.R |= SET<<regPos;
		EXTI.FTSR.R |= SET<<regPos;
		break;
	}

	/** Config NVIC to enable Interrupt */
	switch(regPos)
	{
	case 0U: /** EXTI0 */
	case 1U: /** EXTI1 */
	case 2U: /** EXTI2 */
	case 3U: /** EXTI3 */
	case 4U: /** EXTI4 */
		NVIC_SetPriority (regPos+6U,priority);
		NVIC_EnableIRQ(regPos+6U);
		break;

	case 5U: /** EXTI5 */
	case 6U: /** EXTI6 */
	case 7U: /** EXTI7 */
	case 8U: /** EXTI8 */
	case 9U: /** EXTI9 */
		NVIC_SetPriority (23U,priority);
		NVIC_EnableIRQ(23U);
		break;

	case 10U: /** EXTI10 */
	case 11U: /** EXTI11 */
	case 12U: /** EXTI12 */
	case 13U: /** EXTI13 */
	case 14U: /** EXTI14 */
	case 15U: /** EXTI15 */
		NVIC_SetPriority (40U,priority);
		NVIC_EnableIRQ(40U);
		break;

	case 16U: /** EXTI_PVD_OUT */
		NVIC_SetPriority (1U,priority);
		NVIC_EnableIRQ(1U);
		break;

	case 17U: /** EXTI_RTC_ALARM */
		NVIC_SetPriority (41U,priority);
		NVIC_EnableIRQ(41U);
		break;

	case 18U: /** EXTI_USB_OTG_FS_WKUP */
		NVIC_SetPriority (42U,priority);
		NVIC_EnableIRQ(42U);
		break;

	case 19U: /** EXTI_ETHERNET_WKUP */
		NVIC_SetPriority (62U,priority);
		NVIC_EnableIRQ(62U);
		break;

	case 20U: /** EXTI_USB_OTG_HS_WKUP */
		NVIC_SetPriority (76U,priority);
		NVIC_EnableIRQ(76U);
		break;

	case 21U: /** EXTI_RTC_TAMPER_TSTAMP */
		NVIC_SetPriority (2U,priority);
		NVIC_EnableIRQ(2U);
		break;

	case 22U: /** EXTI_RTC_WKUP */
		NVIC_SetPriority (3U,priority);
		NVIC_EnableIRQ(3U);
		break;
	}
}


void EXTI_ConfigEvent(EXTIn_t IRQn,TriggerEdge_t trigEdge)
{
	if((IRQn>EXTI_RTC_WKUP)||(trigEdge>2U))
	{
		return;
	}

	uint8_t regPos = (IRQn/9U);

	/** Configure the Mask for the EXTI Event */
	EXTI.EMR.R |= SET<<regPos;

	/** Select the Trigger source */
	switch(trigEdge)
	{
	case EXTI_RISING_EDGE_TRIG:
		EXTI.RTSR.R |= SET<<regPos;
		break;
	case EXTI_FALLING_EDGE_TRIG:
		EXTI.FTSR.R |= SET<<regPos;
		break;
	case EXTI_BOTH_EDGE_TRIG:
		EXTI.RTSR.R |= SET<<regPos;
		EXTI.FTSR.R |= SET<<regPos;
		break;
	}
}

uint8_t EXTI_GetPendingIRQ(EXTIn_t IRQn)
{
	if((IRQn>EXTI_RTC_WKUP))
	{
		return 0xFF;
	}

	uint8_t regPos = (IRQn/9U);
	uint32_t regValue = EXTI.PR.R;

	if(regValue & MASK_BIT(regPos))
	{
		return IRQn;
	}
	return FALSE;
}

void EXTI_ClearPendingIRQ(EXTIn_t IRQn)
{
	if((IRQn>EXTI_RTC_WKUP))
	{
		return;
	}

	uint8_t regPos = (IRQn/9U);

	EXTI.PR.R |= SET_BIT(regPos);
}

void EXTI_SoftwareTrig(EXTIn_t IRQn)
{
	if((IRQn>EXTI_RTC_WKUP))
	{
		return;
	}

	uint8_t regPos = (IRQn/9U);

	EXTI.SWIER.R |= SET_BIT(regPos);
}

void EXTI_ChangeTrigEdge(EXTIn_t IRQn,TriggerEdge_t trigEdge)
{
	if((IRQn>EXTI_RTC_WKUP)||(trigEdge>2U))
	{
		return;
	}

	uint8_t regPos = (IRQn/9U);

	switch(trigEdge)
	{
	case EXTI_RISING_EDGE_TRIG:
		EXTI.RTSR.R |= SET<<regPos;
		break;
	case EXTI_FALLING_EDGE_TRIG:
		EXTI.FTSR.R |= SET<<regPos;
		break;
	case EXTI_BOTH_EDGE_TRIG:
		EXTI.RTSR.R |= SET<<regPos;
		EXTI.FTSR.R |= SET<<regPos;
		break;
	}
}

