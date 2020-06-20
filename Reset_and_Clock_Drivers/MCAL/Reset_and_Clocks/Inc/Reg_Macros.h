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

#define TRUE (0x1U)
#define FALSE (0x0U)

#define SET TRUE
#define CLEAR FALSE

#define STD_ON  TRUE
#define STD_OFF FALSE

#define SET_BIT(x)  (SET<<x)
#define CLEAR_BIT(x) ~(SET<<x)
#define MASK_BIT(x) (SET<<x)

#define REG_READ32(regAddress) (uint32_t)(*regAddress)
#define REG_WRITE32(regAddress,value) ((uint32_t)(*regAddress = value) )
#define REG_RMW32(regAddress,mask,value) REG_WRITE32(regAddress,( ( ((uint32_t)(*regAddress)) &(~mask) )|(value&mask)))

typedef uint8_t Bool;

#endif /* #ifndef REG_MACROS_H */
