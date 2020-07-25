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

#define TOTAL_NO_OF_PORTS	(11U)
#define TOTAL_NO_OF_PINS	(TOTAL_NO_OF_PORTS*16U)

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

typedef enum  /* Autosar 4.4.0 -  SWS_Port_00230 - Possible directions of a port pin */
{
	PORT_PIN_IN,
	PORT_PIN_OUT,
}Port_PinDirectionType;

typedef enum  /* Autosar 4.4.0 -  SWS_Port_00230 - Different port pin modes */
{
	/** TODO */
}Port_PinModeType;

typedef struct  /**Autosar 4.4.0 SWS_Port_00072*/
{
	uint8_t PinMode;		/** To control the Pin Mode  @Refer - Port_PinModeType*/
	uint8_t PinDirection;	/** To set the direction of the pin @Refer- Port_PinDirectionType*/
	uint8_t PinInitValue;	/** Pin will be initialised with this value during Init */
	uint8_t PinDirectionChangeable;	/** Can the direction be changed during runtime */
	uint8_t PinModeChangeable;	/** Can the mode be changed during runtime	*/
	/** Controller Specific Parameters  */
	uint8_t PinSlewRate;
}Port_ConfigType;

#endif /* GPIO_CFG_GPIO_PBCFG_H_ */
