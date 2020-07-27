/*
 * GPIO_regTypes.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Michael Jihan
 */

#ifndef GPIO_INC_GPIO_REGTYPES_H_
#define GPIO_INC_GPIO_REGTYPES_H_

#define GPIO_A_BASE_ADDRESS      (0x40020000U)
#define GPIO_B_BASE_ADDRESS      (0x40020400U)
#define GPIO_C_BASE_ADDRESS      (0x40020800U)
#define GPIO_D_BASE_ADDRESS      (0x40020C00U)
#define GPIO_E_BASE_ADDRESS      (0x40021000U)
#define GPIO_F_BASE_ADDRESS      (0x40021400U)
#define GPIO_G_BASE_ADDRESS      (0x40021800U)
#define GPIO_H_BASE_ADDRESS      (0x40021C00U)
#define GPIO_I_BASE_ADDRESS      (0x40022000U)
//#define GPIO_J_BASE_ADDRESS      (0x40022400U)
//#define GPIO_K_BASE_ADDRESS      (0x40022800U)

typedef union GPIO_Mode_Tag
{
	struct {
		uint32_t ModeR0:2;
		uint32_t ModeR1:2;
		uint32_t ModeR2:2;
		uint32_t ModeR3:2;
		uint32_t ModeR4:2;
		uint32_t ModeR5:2;
		uint32_t ModeR6:2;
		uint32_t ModeR7:2;
		uint32_t ModeR8:2;
		uint32_t ModeR9:2;
		uint32_t ModeR10:2;
		uint32_t ModeR11:2;
		uint32_t ModeR12:2;
		uint32_t ModeR13:2;
		uint32_t ModeR14:2;
		uint32_t ModeR15:2;
	}B;
	uint32_t R;
}GPIO_Mode_Type;

typedef union GPIO_Output_Tag
{
	struct {
		uint16_t OT0:1;
		uint16_t OT1:1;
		uint16_t OT2:1;
		uint16_t OT3:1;
		uint16_t OT4:1;
		uint16_t OT5:1;
		uint16_t OT6:1;
		uint16_t OT7:1;
		uint16_t OT8:1;
		uint16_t OT9:1;
		uint16_t OT10:1;
		uint16_t OT11:1;
		uint16_t OT12:1;
		uint16_t OT13:1;
		uint16_t OT14:1;
		uint16_t OT15:1;
		uint16_t Reserved0;
	}B;
	uint32_t R;
}GPIO_Output_Type;

typedef union GPIO_OPSPEED_Tag
{
	struct {
		uint32_t OSPEEDR0:2;
		uint32_t OSPEEDR1:2;
		uint32_t OSPEEDR2:2;
		uint32_t OSPEEDR3:2;
		uint32_t OSPEEDR4:2;
		uint32_t OSPEEDR5:2;
		uint32_t OSPEEDR6:2;
		uint32_t OSPEEDR7:2;
		uint32_t OSPEEDR8:2;
		uint32_t OSPEEDR9:2;
		uint32_t OSPEEDR10:2;
		uint32_t OSPEEDR11:2;
		uint32_t OSPEEDR12:2;
		uint32_t OSPEEDR13:2;
		uint32_t OSPEEDR14:2;
		uint32_t OSPEEDR15:2;
	}B;
	uint32_t R;
}GPIO_OPSPEED_Type;

typedef union GPIO_PullUp_Down_Tag
{
	struct {
		uint32_t PUPDR0:2;
		uint32_t PUPDR1:2;
		uint32_t PUPDR2:2;
		uint32_t PUPDR3:2;
		uint32_t PUPDR4:2;
		uint32_t PUPDR5:2;
		uint32_t PUPDR6:2;
		uint32_t PUPDR7:2;
		uint32_t PUPDR8:2;
		uint32_t PUPDR9:2;
		uint32_t PUPDR10:2;
		uint32_t PUPDR11:2;
		uint32_t PUPDR12:2;
		uint32_t PUPDR13:2;
		uint32_t PUPDR14:2;
		uint32_t PUPDR15:2;
	}B;
	uint32_t R;
}GPIO_PullUp_Down_Type;

typedef union GPIO_Input_Data_Tag
{
	struct {
		uint16_t IDR0:1;
		uint16_t IDR1:1;
		uint16_t IDR2:1;
		uint16_t IDR3:1;
		uint16_t IDR4:1;
		uint16_t IDR5:1;
		uint16_t IDR6:1;
		uint16_t IDR7:1;
		uint16_t IDR8:1;
		uint16_t IDR9:1;
		uint16_t IDR10:1;
		uint16_t IDR11:1;
		uint16_t IDR12:1;
		uint16_t IDR13:1;
		uint16_t IDR14:1;
		uint16_t IDR15:1;
		uint16_t Reserved0;
	}B;
	uint32_t R;
}GPIO_Input_Data_Type;

typedef union GPIO_Output_Data_Tag
{
	struct {
		uint16_t ODR0:1;
		uint16_t ODR1:1;
		uint16_t ODR2:1;
		uint16_t ODR3:1;
		uint16_t ODR4:1;
		uint16_t ODR5:1;
		uint16_t ODR6:1;
		uint16_t ODR7:1;
		uint16_t ODR8:1;
		uint16_t ODR9:1;
		uint16_t ODR10:1;
		uint16_t ODR11:1;
		uint16_t ODR12:1;
		uint16_t ODR13:1;
		uint16_t ODR14:1;
		uint16_t ODR15:1;
		uint16_t Reserved0;
	}B;
	uint32_t R;
}GPIO_Output_Data_Type;

typedef union GPIO_Bit_Set_Reset_Tag
{
	struct {
		uint16_t BS0:1;
		uint16_t BS1:1;
		uint16_t BS2:1;
		uint16_t BS3:1;
		uint16_t BS4:1;
		uint16_t BS5:1;
		uint16_t BS6:1;
		uint16_t BS7:1;
		uint16_t BS8:1;
		uint16_t BS9:1;
		uint16_t BS10:1;
		uint16_t BS11:1;
		uint16_t BS12:1;
		uint16_t BS13:1;
		uint16_t BS14:1;
		uint16_t BS15:1;

		uint16_t BR0:1;
		uint16_t BR1:1;
		uint16_t BR2:1;
		uint16_t BR3:1;
		uint16_t BR4:1;
		uint16_t BR5:1;
		uint16_t BR6:1;
		uint16_t BR7:1;
		uint16_t BR8:1;
		uint16_t BR9:1;
		uint16_t BR10:1;
		uint16_t BR11:1;
		uint16_t BR12:1;
		uint16_t BR13:1;
		uint16_t BR14:1;
		uint16_t BR15:1;
	}B;
	uint32_t R;
}GPIO_Bit_Set_Reset_Type;

typedef union GPIO_Lock_Register_Tag
{
	 struct {
		uint16_t LCK0:1;
		uint16_t LCK1:1;
		uint16_t LCK2:1;
		uint16_t LCK3:1;
		uint16_t LCK4:1;
		uint16_t LCK5:1;
		uint16_t LCK6:1;
		uint16_t LCK7:1;
		uint16_t LCK8:1;
		uint16_t LCK9:1;
		uint16_t LCK10:1;
		uint16_t LCK11:1;
		uint16_t LCK12:1;
		uint16_t LCK13:1;
		uint16_t LCK14:1;
		uint16_t LCK15:1;
		uint16_t LCKK:1;
		uint16_t Reserved0:15;
	}B;
	uint32_t R;
}GPIO_Lock_Register_Type;

typedef union GPIO_Alt_Func_Low_Tag
{
	struct {
		uint32_t AFRL0:4;
		uint32_t AFRL1:4;
		uint32_t AFRL2:4;
		uint32_t AFRL3:4;
		uint32_t AFRL4:4;
		uint32_t AFRL5:4;
		uint32_t AFRL6:4;
		uint32_t AFRL7:4;
	}B;
	uint32_t R;
}GPIO_Alt_Func_Low_Type;

typedef union GPIO_Alt_Func_High_Tag
{
	struct {
		uint32_t AFRL8:4;
		uint32_t AFRL9:4;
		uint32_t AFRL10:4;
		uint32_t AFRL11:4;
		uint32_t AFRL12:4;
		uint32_t AFRL13:4;
		uint32_t AFRL14:4;
		uint32_t AFRL15:4;
	}B;
	uint32_t R;
}GPIO_Alt_Func_High_Type;

typedef struct
{
	GPIO_Mode_Type MODER;			/** GPIO port mode register */
	GPIO_Output_Type OTYPER;		/** GPIO port output type register */
	GPIO_OPSPEED_Type OSPEEDR;		/** GPIO port output speed register */
	GPIO_PullUp_Down_Type PUPDR;	/** GPIO port pull-up/pull-down register */
	GPIO_Input_Data_Type IDR;		/** GPIO port input data register */
	GPIO_Output_Data_Type ODR;		/** GPIO port output data register */
	GPIO_Bit_Set_Reset_Type BSRR;	/** GPIO port bit set/reset register */
	GPIO_Lock_Register_Type LCKR;	/** GPIO port configuration lock register */
	GPIO_Alt_Func_Low_Type AFRL;	/**	GPIO alternate function low register */
	GPIO_Alt_Func_High_Type AFRH;	/**	GPIO alternate function low register */
}GPIO_RegTypes;

#define GPIO_A 			(*(volatile GPIO_RegTypes*)(GPIO_A_BASE_ADDRESS))
#define GPIO_B 			(*(volatile GPIO_RegTypes*)(GPIO_B_BASE_ADDRESS))
#define GPIO_C 			(*(volatile GPIO_RegTypes*)(GPIO_C_BASE_ADDRESS))
#define GPIO_D 			(*(volatile GPIO_RegTypes*)(GPIO_D_BASE_ADDRESS))
#define GPIO_E 			(*(volatile GPIO_RegTypes*)(GPIO_E_BASE_ADDRESS))
#define GPIO_F 			(*(volatile GPIO_RegTypes*)(GPIO_F_BASE_ADDRESS))
#define GPIO_G 			(*(volatile GPIO_RegTypes*)(GPIO_G_BASE_ADDRESS))
#define GPIO_H 			(*(volatile GPIO_RegTypes*)(GPIO_H_BASE_ADDRESS))
#define GPIO_I 			(*(volatile GPIO_RegTypes*)(GPIO_I_BASE_ADDRESS))
//#define GPIO_J 			(*(volatile GPIO_RegTypes*)(GPIO_J_BASE_ADDRESS))
//#define GPIO_K 			(*(volatile GPIO_RegTypes*)(GPIO_K_BASE_ADDRESS))

#endif /* GPIO_INC_GPIO_REGTYPES_H_ */
