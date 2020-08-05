/**********************************************
 * File - GPIO_Pbcfg.h
 * Author - Michael Jihan
 * Created on 25-Jul-2020
 *
 * Purpose - 
 *
 * ********************************************/

#ifndef GPIO_CFG_GPIO_PBCFG_H_
#define GPIO_CFG_GPIO_PBCFG_H_

#include "GPIO_regTypes.h"

#define GPIO_PORT_LOCK				(STD_ON)	 /** Locks all the pins of the port after INIT until next reset */
#define PORT_SET_PIN_DIRECTION_API	(STD_ON) 	 /** Port_SetPinDirection() will be available during runtime */
#define PORT_SET_PIN_MODE_API		(STD_ON) 	 /** Port_SetPinMode() will be available during runtime */

#define NO_OF_PINS_CONFIG   (1U)
#define TOTAL_NO_OF_PORTS	(9U)
#define TOTAL_NO_OF_PINS	(TOTAL_NO_OF_PORTS*16U)

#define PIN_UNUSED	(0xFFU)

typedef enum  /* Autosar 4.4.0 -  SWS_Port_00229 */
{
	GPIO_A_PIN_0 =0U,
	GPIO_A_PIN_1,
	GPIO_A_PIN_2,
	GPIO_A_PIN_3,
	GPIO_A_PIN_4,
	GPIO_A_PIN_5,
	GPIO_A_PIN_6,
	GPIO_A_PIN_7,
	GPIO_A_PIN_8,
	GPIO_A_PIN_9,
	GPIO_A_PIN_10,
	GPIO_A_PIN_11,
	GPIO_A_PIN_12,
	GPIO_A_PIN_13,
	GPIO_A_PIN_14,
	GPIO_A_PIN_15,

	GPIO_B_PIN_0,
	GPIO_B_PIN_1,
	GPIO_B_PIN_2,
	GPIO_B_PIN_3,
	GPIO_B_PIN_4,
	GPIO_B_PIN_5,
	GPIO_B_PIN_6,
	GPIO_B_PIN_7,
	GPIO_B_PIN_8,
	GPIO_B_PIN_9,
	GPIO_B_PIN_10,
	GPIO_B_PIN_11,
	GPIO_B_PIN_12,
	GPIO_B_PIN_13,
	GPIO_B_PIN_14,
	GPIO_B_PIN_15,

	GPIO_C_PIN_0,
	GPIO_C_PIN_1,
	GPIO_C_PIN_2,
	GPIO_C_PIN_3,
	GPIO_C_PIN_4,
	GPIO_C_PIN_5,
	GPIO_C_PIN_6,
	GPIO_C_PIN_7,
	GPIO_C_PIN_8,
	GPIO_C_PIN_9,
	GPIO_C_PIN_10,
	GPIO_C_PIN_11,
	GPIO_C_PIN_12,
	GPIO_C_PIN_13,
	GPIO_C_PIN_14,
	GPIO_C_PIN_15,

	GPIO_D_PIN_0,
	GPIO_D_PIN_1,
	GPIO_D_PIN_2,
	GPIO_D_PIN_3,
	GPIO_D_PIN_4,
	GPIO_D_PIN_5,
	GPIO_D_PIN_6,
	GPIO_D_PIN_7,
	GPIO_D_PIN_8,
	GPIO_D_PIN_9,
	GPIO_D_PIN_10,
	GPIO_D_PIN_11,
	GPIO_D_PIN_12,
	GPIO_D_PIN_13,
	GPIO_D_PIN_14,
	GPIO_D_PIN_15,

	GPIO_E_PIN_0,
	GPIO_E_PIN_1,
	GPIO_E_PIN_2,
	GPIO_E_PIN_3,
	GPIO_E_PIN_4,
	GPIO_E_PIN_5,
	GPIO_E_PIN_6,
	GPIO_E_PIN_7,
	GPIO_E_PIN_8,
	GPIO_E_PIN_9,
	GPIO_E_PIN_10,
	GPIO_E_PIN_11,
	GPIO_E_PIN_12,
	GPIO_E_PIN_13,
	GPIO_E_PIN_14,
	GPIO_E_PIN_15,

	GPIO_F_PIN_0,
	GPIO_F_PIN_1,
	GPIO_F_PIN_2,
	GPIO_F_PIN_3,
	GPIO_F_PIN_4,
	GPIO_F_PIN_5,
	GPIO_F_PIN_6,
	GPIO_F_PIN_7,
	GPIO_F_PIN_8,
	GPIO_F_PIN_9,
	GPIO_F_PIN_10,
	GPIO_F_PIN_11,
	GPIO_F_PIN_12,
	GPIO_F_PIN_13,
	GPIO_F_PIN_14,
	GPIO_F_PIN_15,

	GPIO_G_PIN_0,
	GPIO_G_PIN_1,
	GPIO_G_PIN_2,
	GPIO_G_PIN_3,
	GPIO_G_PIN_4,
	GPIO_G_PIN_5,
	GPIO_G_PIN_6,
	GPIO_G_PIN_7,
	GPIO_G_PIN_8,
	GPIO_G_PIN_9,
	GPIO_G_PIN_10,
	GPIO_G_PIN_11,
	GPIO_G_PIN_12,
	GPIO_G_PIN_13,
	GPIO_G_PIN_14,
	GPIO_G_PIN_15,

	GPIO_H_PIN_0,
	GPIO_H_PIN_1,
	GPIO_H_PIN_2,
	GPIO_H_PIN_3,
	GPIO_H_PIN_4,
	GPIO_H_PIN_5,
	GPIO_H_PIN_6,
	GPIO_H_PIN_7,
	GPIO_H_PIN_8,
	GPIO_H_PIN_9,
	GPIO_H_PIN_10,
	GPIO_H_PIN_11,
	GPIO_H_PIN_12,
	GPIO_H_PIN_13,
	GPIO_H_PIN_14,
	GPIO_H_PIN_15,

	GPIO_I_PIN_0,
	GPIO_I_PIN_1,
	GPIO_I_PIN_2,
	GPIO_I_PIN_3,
	GPIO_I_PIN_4,
	GPIO_I_PIN_5,
	GPIO_I_PIN_6,
	GPIO_I_PIN_7,
	GPIO_I_PIN_8,
	GPIO_I_PIN_9,
	GPIO_I_PIN_10,
	GPIO_I_PIN_11,
	GPIO_I_PIN_12,
	GPIO_I_PIN_13,
	GPIO_I_PIN_14,
	GPIO_I_PIN_15,

	GPIO_J_PIN_0,
	GPIO_J_PIN_1,
	GPIO_J_PIN_2,
	GPIO_J_PIN_3,
	GPIO_J_PIN_4,
	GPIO_J_PIN_5,
	GPIO_J_PIN_6,
	GPIO_J_PIN_7,
	GPIO_J_PIN_8,
	GPIO_J_PIN_9,
	GPIO_J_PIN_10,
	GPIO_J_PIN_11,
	GPIO_J_PIN_12,
	GPIO_J_PIN_13,
	GPIO_J_PIN_14,
	GPIO_J_PIN_15,

	GPIO_K_PIN_0,
	GPIO_K_PIN_1,
	GPIO_K_PIN_2,
	GPIO_K_PIN_3,
	GPIO_K_PIN_4,
	GPIO_K_PIN_5,
	GPIO_K_PIN_6,
	GPIO_K_PIN_7,
	GPIO_K_PIN_8,
	GPIO_K_PIN_9,
	GPIO_K_PIN_10,
	GPIO_K_PIN_11,
	GPIO_K_PIN_12,
	GPIO_K_PIN_13,
	GPIO_K_PIN_14,
	GPIO_K_PIN_15,
}Port_PinType;

typedef enum
{
	PORT_PIN_LOW,
	PORT_PIN_HIGH,
}Port_PinInitValue;

typedef enum  /* Autosar 4.4.0 -  SWS_Port_00230 - Possible directions of a port pin */
{
	PORT_PIN_IN,
	PORT_PIN_OUT,
}Port_PinDirectionType;

typedef enum  /* Autosar 4.4.0 -  SWS_Port_00230 - Different port pin modes */
{
	PIN_MODE_GPIO =1U,
	PIN_MODE_ANALOG =3U,
	PIN_MODE_ALT_FUNC_0 =4U,
	PIN_MODE_ALT_FUNC_1,
	PIN_MODE_ALT_FUNC_2,
	PIN_MODE_ALT_FUNC_3,
	PIN_MODE_ALT_FUNC_4,
	PIN_MODE_ALT_FUNC_5,
	PIN_MODE_ALT_FUNC_6,
	PIN_MODE_ALT_FUNC_7,
	PIN_MODE_ALT_FUNC_8,
	PIN_MODE_ALT_FUNC_9,
	PIN_MODE_ALT_FUNC_10,
	PIN_MODE_ALT_FUNC_11,
	PIN_MODE_ALT_FUNC_12,
	PIN_MODE_ALT_FUNC_13,
	PIN_MODE_ALT_FUNC_14,
	PIN_MODE_ALT_FUNC_15
}Port_PinModeType;

typedef enum  /* Different port Output Type modes */
{
	PIN_OUTPUT_PUSH_PULL,
	PIN_OUTPUT_OPEN_DRAIN
}Port_PinOutputType;

typedef enum  /* Different port Output Speeds */
{
	PIN_LOW_SPEED,
	PIN_MEDIUM_SPEED,
	PIN_HIGH_SPEED,
	PIN_VERY_HIGH_SPEED
}Port_PinOutputSpeedType;

typedef enum  /* Different port Output Settings */
{
	PIN_NO_PULL_UP_PULL_DOWN,
	PIN_PULL_UP,
	PIN_PULL_DOWN,
}Port_PinPullUpDownType;

typedef struct  /**Autosar 4.4.0 SWS_Port_00072*/
{
	Port_PinModeType PinMode;		/** To control the Pin Mode  @Refer - Port_PinModeType*/
	Port_PinDirectionType PinDirection;	/** To set the direction of the pin @Refer- Port_PinDirectionType*/
	uint8_t PinInitValue;	/** Pin will be initialised with this value during Init */
	uint8_t PinDirectionChangeable;	/** Can the direction be changed during runtime */
	uint8_t PinModeChangeable;	/** Can the mode be changed during runtime	*/
	/** Controller Specific Parameters  */
	Port_PinOutputType PinOutputMode;	/** OpnenDrain or Push-pull  */
	Port_PinOutputSpeedType PinSlewRate; /** Controls the Output speed of the pin @Refer - Port_PinOutputSpeedType*/
	Port_PinPullUpDownType PinPupDown;	/** Enables Pull Up and Pull Down mechanism */
}Port_ConfigType;

#endif /* GPIO_CFG_GPIO_PBCFG_H_ */
