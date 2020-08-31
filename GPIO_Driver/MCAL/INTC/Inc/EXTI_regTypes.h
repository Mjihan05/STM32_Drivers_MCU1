/**********************************************
 * File - EXTI_regTypes.h
 * Author - Michael Jihan
 * Created on 30-Aug-2020
 *
 * Purpose - 
 *
 * ********************************************/

#ifndef INTC_INC_EXTI_REGTYPES_H_
#define INTC_INC_EXTI_REGTYPES_H_

#define EXTI_BASE_ADDRESS 		(0x40013C00U)

typedef union EXTI_IMR_Tag
{
	struct
	{
		uint32_t MR:23;
		uint32_t reserved1:9;
	}B;
	uint32_t R;
}EXTI_IMR_Type;

typedef union EXTI_EMR_Tag
{
	struct
	{
		uint32_t MR:23;
		uint32_t reserved2:9;
	}B;
	uint32_t R;
}EXTI_EMR_Type;

typedef union EXTI_RTSR_Tag
{
	struct
	{
		uint32_t TR:23;
		uint32_t reserved3:9;
	}B;
	uint32_t R;
}EXTI_RTSR_Type;

typedef union EXTI_FTSR_Tag
{
	struct
	{
		uint32_t TR:23;
		uint32_t reserved4:9;
	}B;
	uint32_t R;
}EXTI_FTSR_Type;

typedef union EXTI_SWIER_Tag
{
	struct
	{
		uint32_t SWIER:23;
		uint32_t reserved5:9;
	}B;
	uint32_t R;
}EXTI_SWIER_Type;

typedef union EXTI_PR_Tag
{
	struct
	{
		uint32_t PR:23;
		uint32_t reserved6:9;
	}B;
	uint32_t R;
}EXTI_PR_Type;

typedef struct
{
	EXTI_IMR_Type IMR;			/** Interrupt mask register  */
	EXTI_EMR_Type EMR; 			/** Event mask register */
	EXTI_RTSR_Type RTSR; 		/** Rising trigger selection register  */
	EXTI_FTSR_Type FTSR;		/** Falling trigger selection register */
	EXTI_SWIER_Type SWIER;		/** Software interrupt event register */
	EXTI_PR_Type PR;			/** Pending register */
}EXTI_RegTypes;

#define EXTI 			(*(volatile EXTI_RegTypes*)(EXTI_BASE_ADDRESS))

#endif /* INTC_INC_EXTI_REGTYPES_H_ */
