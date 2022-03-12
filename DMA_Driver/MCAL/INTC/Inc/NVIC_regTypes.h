/**********************************************
 * File - NVIC_regTypes.h
 * Author - Michael Jihan
 * Created on 22-Aug-2020
 *
 * Purpose - This file holds the register structures
 * 				and base addresses of NVIC Module
 *
 * ********************************************/

#ifndef INTC_INC_NVIC_REGTYPES_H_
#define INTC_INC_NVIC_REGTYPES_H_

#define NO_OF_INTERRUPTS_IN_STM32F407  (82U)
#define NO_OF_REGS			 		   ((NO_OF_INTERRUPTS_IN_STM32F407/32U)+1U)
#define NO_OF_PRIORITY_REGS			   ((NO_OF_INTERRUPTS_IN_STM32F407/4U))
#define RESERVED_SPACE				   (104+(NO_OF_REGS*4))
#define RESERVED_SPACE_PRIO			   (232+(NO_OF_REGS*4))

#define NVIC_BASE_ADDRESS      (0xE000E100U)
#define NVIC_ISER_BASE_ADDRESS (NVIC_BASE_ADDRESS + 0x100 )
#define NVIC_ICER_BASE_ADDRESS (NVIC_BASE_ADDRESS + 0x180 )
#define NVIC_ISPR_BASE_ADDRESS (NVIC_BASE_ADDRESS + 0x200 )
#define NVIC_ICPR_BASE_ADDRESS (NVIC_BASE_ADDRESS + 0x280 )
#define NVIC_IABR_BASE_ADDRESS (NVIC_BASE_ADDRESS + 0x300 )
#define NVIC_IPR_BASE_ADDRESS  (NVIC_BASE_ADDRESS + 0x400 )

#define NVIC_STIR_BASE_ADDRESS (0xE000EF00U)

typedef uint32_t NVIC_ISER_Type;
typedef uint32_t NVIC_ICER_Type;
typedef uint32_t NVIC_ISPR_Type;
typedef uint32_t NVIC_ICPR_Type;
typedef uint32_t NVIC_IABR_Type;
typedef uint32_t NVIC_IPR_Type;

typedef struct
{
	uint32_t INTID:9;
	uint32_t reserved1:23;
}NVIC_STIR_Type;

#define NVIC_ISER		(*(volatile NVIC_ISER_Type*)(NVIC_ISER_BASE_ADDRESS))
#define NVIC_ICER		(*(volatile NVIC_ICER_Type*)(NVIC_ICER_BASE_ADDRESS))
#define NVIC_ISPR		(*(volatile NVIC_ISPR_Type*)(NVIC_ISPR_BASE_ADDRESS))
#define NVIC_ICPR		(*(volatile NVIC_ICPR_Type*)(NVIC_ICPR_BASE_ADDRESS))
#define NVIC_IABR		(*(volatile NVIC_IABR_Type*)(NVIC_IABR_BASE_ADDRESS))
#define NVIC_IPR		(*(volatile NVIC_IPR_Type*)(NVIC_IPR_BASE_ADDRESS))

#define NVIC_STIR		(*(volatile NVIC_STIR_Type*)(NVIC_STIR_BASE_ADDRESS))

#if 1

typedef struct
{
	NVIC_ISER_Type ISER[NO_OF_REGS];	/** Interrupt set-enable register  */
	uint8_t reserved3[RESERVED_SPACE];
	NVIC_ICER_Type ICER[NO_OF_REGS]; /** Interrupt clear-enable register */
	uint8_t reserved4[RESERVED_SPACE];
	NVIC_ISPR_Type ISPR[NO_OF_REGS]; /** Interrupt set-pending register */
	uint8_t reserved5[RESERVED_SPACE];
	NVIC_ICPR_Type ICPR[NO_OF_REGS]; /** Interrupt clear-pending register */
	uint8_t reserved6[RESERVED_SPACE];
	NVIC_IABR_Type IABR[NO_OF_REGS]; /** Interrupt active bit register */
	uint8_t reserved7[RESERVED_SPACE_PRIO];
	NVIC_IPR_Type  IPR[NO_OF_PRIORITY_REGS]; /** Interrupt active bit register */
	uint8_t reserved8[2736];
	NVIC_STIR_Type STIR;	/** Software trigger interrupt register */
}NVIC_RegTypes;

#define NVIC 			(*(volatile NVIC_RegTypes*)(NVIC_BASE_ADDRESS))
#define NVIC_STIR		(*(volatile NVIC_STIR_Type*)(NVIC_STIR_BASE_ADDRESS))
#endif

#endif /* INTC_INC_NVIC_REGTYPES_H_ */
