/**********************************************
 * File - RCC_regTypes.h
 * Author - Michael Jihan
 * Created on 31/May/2020
 *
 * Purpose - This files holds the register
 * 			 base addresses and offsets of all
 * 			 registers in RCC
 *
 * ********************************************/

#ifndef RCC_REG_TYPES_H

#define RCC_REG_TYPES_H

#define RCC_BASE_ADDRESS        (0x40023800UL)
#define RCC_CR_OFFSET           (0x00UL)
#define RCC_PLLCFGR_OFFSET      (0x04UL)
#define RCC_CFGR_OFFSET         (0x08UL)
#define RCC_CIR_OFFSET          (0x0CUL)
#define RCC_AHB1RSTR_OFFSET     (0x10UL)
#define RCC_AHB2RSTR_OFFSET     (0x14UL)
#define RCC_AHB3RSTR_OFFSET     (0x18UL)
#define RCC_APB1RSTR_OFFSET     (0x20UL)
#define RCC_APB2RSTR_OFFSET     (0x24UL)
#define RCC_AHB1ENR_OFFSET   	(0x30UL)
#define RCC_AHB2ENR_OFFSET   	(0x34UL)
#define RCC_AHB3ENR_OFFSET   	(0x38UL)
#define RCC_APB1ENR_OFFSET   	(0x40UL)
#define RCC_APB2ENR_OFFSET   	(0x44UL)
#define RCC_AHB1LPENR_OFFSET   	(0x50UL)
#define RCC_AHB2LPENR_OFFSET   	(0x54UL)
#define RCC_AHB3LPENR_OFFSET   	(0x58UL)
#define RCC_APB1LPENR_OFFSET   	(0x60UL)
#define RCC_APB2LPENR_OFFSET   	(0x64UL)
#define RCC_BDCR_OFFSET         (0x70UL)
#define RCC_CSR_OFFSET          (0x74UL)
#define RCC_SSCGR_OFFSET        (0x80UL)
#define RCC_PLLI2SCFGR_OFFSET	(0x84UL)

#define RCC_CR 			(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_CR_OFFSET)
#define RCC_PLLCFGR    	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_PLLCFGR_OFFSET)
#define RCC_CFGR 	   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_CFGR_OFFSET)
#define RCC_CIR 	   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_CIR_OFFSET)
#define RCC_AHB1RSTR   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB1RSTR_OFFSET)
#define RCC_AHB2RSTR   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB2RSTR_OFFSET)
#define RCC_AHB3RSTR   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB3RSTR_OFFSET)
#define RCC_APB1RSTR   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_APB1RSTR_OFFSET)
#define RCC_APB2RSTR   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_APB2RSTR_OFFSET)
#define RCC_AHB1ENR    	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB1ENR_OFFSET)
#define RCC_AHB2ENR    	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB2ENR_OFFSET)
#define RCC_AHB3ENR    	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB3ENR_OFFSET)
#define RCC_APB1ENR	   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_APB1ENR_OFFSET)
#define RCC_APB2ENR	   	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET)
#define RCC_AHB1LPENR  	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB1LPENR_OFFSET)
#define RCC_AHB2LPENR  	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB2LPENR_OFFSET)
#define RCC_AHB3LPENR  	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_AHB3LPENR_OFFSET)
#define RCC_APB1LPENR  	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_APB1LPENR_OFFSET)
#define RCC_APB2LPENR  	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_APB2LPENR_OFFSET)
#define RCC_BDCR       	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_BDCR_OFFSET)
#define RCC_CSR        	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_CSR_OFFSET)
#define RCC_SSCGR      	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_SSCGR_OFFSET)
#define RCC_PLLI2SCFGR 	(volatile uint32_t*)(RCC_BASE_ADDRESS + RCC_PLLI2SCFGR_OFFSET)

#endif /* #ifndef RCC_REG_TYPES_H */
