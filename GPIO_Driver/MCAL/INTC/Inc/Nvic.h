/**********************************************
 * File - Nvic.h
 * Author - Michael Jihan
 * Created on 22-Aug-2020
 *
 * Purpose - This files holds the Global functions
 * 			 and data structures that configures
 * 			 NVIC
 *
 * ********************************************/

#ifndef INTC_INC_NVIC_H_
#define INTC_INC_NVIC_H_

#define TOTAL_NO_OF_INTERRUPTS		(82U)

typedef uint8_t IRQn_t;

void NVIC_EnableIRQ(IRQn_t IRQn);
void NVIC_DisableIRQ(IRQn_t IRQn);
uint32_t NVIC_GetPendingIRQ (IRQn_t IRQn);
void NVIC_SetPendingIRQ (IRQn_t IRQn);
void NVIC_ClearPendingIRQ (IRQn_t IRQn);
uint32_t NVIC_GetActive (IRQn_t IRQn);

#endif /* INTC_INC_NVIC_H_ */
