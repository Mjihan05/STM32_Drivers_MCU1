/**********************************************
 * File - Intc.h
 * Author - Michael Jihan
 * Created on 22-Aug-2020
 *
 * Purpose - This files holds the Global functions
 * 			 and data structures that configures
 * 			 NVIC and EXTI
 *
 * ********************************************/

#ifndef INTC_INC_INTC_H_
#define INTC_INC_INTC_H_

#define TOTAL_NO_OF_INTERRUPTS		(82U)
#define TOTAL_NO_OF_EXTERNAL_INT	(23U)

typedef uint8_t IRQn_t;



typedef enum
{
	EXTI_PA0,
	EXTI_PB0,
	EXTI_PC0,
	EXTI_PD0,
	EXTI_PE0,
	EXTI_PF0,
	EXTI_PG0,
	EXTI_PH0,
	EXTI_PI0,

	EXTI_PA1,
	EXTI_PB1,
	EXTI_PC1,
	EXTI_PD1,
	EXTI_PE1,
	EXTI_PF1,
	EXTI_PG1,
	EXTI_PH1,
	EXTI_PI1,

	EXTI_PA2,
	EXTI_PB2,
	EXTI_PC2,
	EXTI_PD2,
	EXTI_PE2,
	EXTI_PF2,
	EXTI_PG2,
	EXTI_PH2,
	EXTI_PI2,

	EXTI_PA3,
	EXTI_PB3,
	EXTI_PC3,
	EXTI_PD3,
	EXTI_PE3,
	EXTI_PF3,
	EXTI_PG3,
	EXTI_PH3,
	EXTI_PI3,

	EXTI_PA4,
	EXTI_PB4,
	EXTI_PC4,
	EXTI_PD4,
	EXTI_PE4,
	EXTI_PF4,
	EXTI_PG4,
	EXTI_PH4,
	EXTI_PI4,

	EXTI_PA5,
	EXTI_PB5,
	EXTI_PC5,
	EXTI_PD5,
	EXTI_PE5,
	EXTI_PF5,
	EXTI_PG5,
	EXTI_PH5,
	EXTI_PI5,

	EXTI_PA6,
	EXTI_PB6,
	EXTI_PC6,
	EXTI_PD6,
	EXTI_PE6,
	EXTI_PF6,
	EXTI_PG6,
	EXTI_PH6,
	EXTI_PI6,

	EXTI_PA7,
	EXTI_PB7,
	EXTI_PC7,
	EXTI_PD7,
	EXTI_PE7,
	EXTI_PF7,
	EXTI_PG7,
	EXTI_PH7,
	EXTI_PI7,

	EXTI_PA8,
	EXTI_PB8,
	EXTI_PC8,
	EXTI_PD8,
	EXTI_PE8,
	EXTI_PF8,
	EXTI_PG8,
	EXTI_PH8,
	EXTI_PI8,

	EXTI_PA9,
	EXTI_PB9,
	EXTI_PC9,
	EXTI_PD9,
	EXTI_PE9,
	EXTI_PF9,
	EXTI_PG9,
	EXTI_PH9,
	EXTI_PI9,

	EXTI_PA10,
	EXTI_PB10,
	EXTI_PC10,
	EXTI_PD10,
	EXTI_PE10,
	EXTI_PF10,
	EXTI_PG10,
	EXTI_PH10,
	EXTI_PI10,

	EXTI_PA11,
	EXTI_PB11,
	EXTI_PC11,
	EXTI_PD11,
	EXTI_PE11,
	EXTI_PF11,
	EXTI_PG11,
	EXTI_PH11,
	EXTI_PI11,

	EXTI_PA12,
	EXTI_PB12,
	EXTI_PC12,
	EXTI_PD12,
	EXTI_PE12,
	EXTI_PF12,
	EXTI_PG12,
	EXTI_PH12,

	EXTI_PA13=117U,
	EXTI_PB13,
	EXTI_PC13,
	EXTI_PD13,
	EXTI_PE13,
	EXTI_PF13,
	EXTI_PG13,
	EXTI_PH13,

	EXTI_PA14=126U,
	EXTI_PB14,
	EXTI_PC14,
	EXTI_PD14,
	EXTI_PE14,
	EXTI_PF14,
	EXTI_PG14,
	EXTI_PH14,

	EXTI_PA15=135U,
	EXTI_PB15,
	EXTI_PC15,
	EXTI_PD15,
	EXTI_PE15,
	EXTI_PF15,
	EXTI_PG15,
	EXTI_PH15,

	EXTI_PVD_OUT=144U,
	EXTI_RTC_ALARM=153U,
	EXTI_USB_OTG_FS_WKUP=162U,
	EXTI_ETHERNET_WKUP=171U,
	EXTI_USB_OTG_HS_WKUP=180U,
	EXTI_RTC_TAMPER_TSTAMP=189U,
	EXTI_RTC_WKUP=198U,
}EXTIn_t;

typedef enum
{
	EXTI_RISING_EDGE_TRIG = 0U,
	EXTI_FALLING_EDGE_TRIG,
	EXTI_BOTH_EDGE_TRIG,
}TriggerEdge_t;

void NVIC_EnableIRQ(IRQn_t IRQn);
void NVIC_DisableIRQ(IRQn_t IRQn);
uint32_t NVIC_GetPendingIRQ (IRQn_t IRQn);
void NVIC_SetPendingIRQ (IRQn_t IRQn);
void NVIC_ClearPendingIRQ (IRQn_t IRQn);
uint32_t NVIC_GetActive (IRQn_t IRQn);
void NVIC_SetPriority (IRQn_t IRQn, uint32_t priority);
uint32_t NVIC_GetPriority (IRQn_t IRQn);

/** Make sure to SET the USERSETMPEND bit in the SCR.CCR before call */
void NVIC_SoftwareTrig (IRQn_t IRQn);


void EXTI_ConfigIRQ(EXTIn_t IRQn,TriggerEdge_t trigEdge,uint32_t priority);
void EXTI_ConfigEvent(EXTIn_t IRQn,TriggerEdge_t trigEdge);
uint8_t EXTI_GetPendingIRQ(EXTIn_t IRQn);
void EXTI_ClearPendingIRQ(EXTIn_t IRQn);    /** This function needs to be called in the handler or the INT keeps executing */
void EXTI_ChangeTrigEdge(EXTIn_t IRQn,TriggerEdge_t trigEdge);

void EXTI_SoftwareTrig(EXTIn_t IRQn);



#endif /* INTC_INC_INTC_H_ */
