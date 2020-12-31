/**********************************************
 * File - Reg_Macros.h
 * Author - Michael Jihan
 * Created on 04/June/2020
 *
 * Purpose - This files holds Macros that
 * 			 interacts with the register
 *
 * ********************************************/


#ifndef REG_MACROS_H
#define REG_MACROS_H

#include <stdint.h>

#define GLOBAL
#define NULL_PTR ((void*)0)

#define TRUE (0x1U)
#define FALSE (0x0U)

#define SET TRUE
#define CLEAR FALSE

#define STD_ON  TRUE
#define STD_OFF FALSE

//#define STD_HIGH STD_ON
//#define STD_LOW STD_OFF

#define SET_BIT(x)  (SET<<x)
#define CLEAR_BIT(x) ((~(SET<<x))&MASK_BIT(x))
#define MASK_BIT(x) (SET<<x)
#define MASK_BITS(bitMask,positionOfBits) ((0x00000000|bitMask)<<positionOfBits)

#define REG_READ32(regAddress) (uint32_t)(*regAddress)
#define REG_WRITE32(regAddress,value) ((uint32_t)(*regAddress = value) )
#define REG_RMW32(regAddress,mask,value) REG_WRITE32(regAddress,( ( ((uint32_t)(*regAddress)) &(~mask) )|(value&mask)))

typedef uint8_t Bool;
typedef enum
{
	E_OK,
	E_NOT_OK
}Std_ReturnType;

typedef enum  /* Init Status */
{
	MODULE_UNINITIALIZED,
	MODULE_INITIALIZED,
	MODULE_INIT_FAILED
}InitStatusType;

#endif /* #ifndef REG_MACROS_H */
