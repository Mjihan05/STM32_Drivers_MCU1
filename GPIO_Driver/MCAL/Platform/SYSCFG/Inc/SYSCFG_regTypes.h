/**********************************************
 * File - SYSCFG_regTypes.h
 * Author - Michael Jihan
 * Created on 30-Aug-2020
 *
 * Purpose - 
 *
 * ********************************************/

#ifndef PLATFORM_SYSCFG_INC_SYSCFG_REGTYPES_H_
#define PLATFORM_SYSCFG_INC_SYSCFG_REGTYPES_H_

#define SYSCFG_BASE_ADDRESS 		(0x40013800U)

typedef struct
{
	uint32_t MEM_MODE:2;
	uint32_t reserved1:30;
}SYSCFG_MEMRMP_Type;

typedef struct
{
	uint32_t reserved2:8;
	uint32_t MII_RMII_SEL:1;
	uint32_t reserved3:23;
}SYSCFG_PMC_Type;

typedef union SYSCFG_EXTICR_Tag
{
	struct
	{
		uint32_t EXTI_A:4;
		uint32_t EXTI_B:4;
		uint32_t EXTI_C:4;
		uint32_t EXTI_D:4;
		uint32_t reserved4:16;
	}B;
	uint32_t R;
}SYSCFG_EXTICR_Type;

#if 0
typedef struct
{
	uint32_t EXTI_A:4;
	uint32_t EXTI_B:4;
	uint32_t EXTI_C:4;
	uint32_t EXTI_D:4;
	uint32_t reserved4:16;
}SYSCFG_EXTICR_Type;
#endif

typedef struct
{
	uint32_t reserved5:23;
	uint32_t READY:1;
	uint32_t reserved6:7;
	uint32_t CMP_PD:1;
}SYSCFG_CMPCR_Type;

typedef struct
{
	SYSCFG_MEMRMP_Type MEMRMP;			/** SYSCFG memory re-map register   */
	SYSCFG_PMC_Type PMC; 				/** SYSCFG peripheral mode configuration register */
	SYSCFG_EXTICR_Type EXTICR[4U]; 		/** SYSCFG external interrupt configuration register  */
	uint8_t reserved7[8U];
	SYSCFG_CMPCR_Type CMPCR;			 /** Compensation cell control register */
}SYSCFG_RegTypes;

#define SYSCFG 			(*(volatile SYSCFG_RegTypes*)(SYSCFG_BASE_ADDRESS))

#endif /* PLATFORM_SYSCFG_INC_SYSCFG_REGTYPES_H_ */
