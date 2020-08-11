/**********************************************
 * File - GPIO_PbCfg.c
 * Author - Michael Jihan
 * Created on 25-Jul-2020
 *
 * Purpose - 
 *
 * ********************************************/

#include <stdint.h>
#include "Reg_Macros.h"
#include "GPIO_PbCfg.h"


uint32_t Gpio_BaseAddress[] = {
#ifdef GPIO_A_BASE_ADDRESS
	   GPIO_A_BASE_ADDRESS,
#endif
#ifdef GPIO_B_BASE_ADDRESS
	   GPIO_B_BASE_ADDRESS,
#endif
#ifdef GPIO_C_BASE_ADDRESS
	   GPIO_C_BASE_ADDRESS,
#endif
#ifdef GPIO_D_BASE_ADDRESS
	   GPIO_D_BASE_ADDRESS,
#endif
#ifdef GPIO_E_BASE_ADDRESS
	   GPIO_E_BASE_ADDRESS,
#endif
#ifdef GPIO_F_BASE_ADDRESS
	   GPIO_F_BASE_ADDRESS,
#endif
#ifdef GPIO_G_BASE_ADDRESS
	   GPIO_G_BASE_ADDRESS,
#endif
#ifdef GPIO_H_BASE_ADDRESS
	   GPIO_H_BASE_ADDRESS,
#endif
#ifdef GPIO_I_BASE_ADDRESS
	   GPIO_I_BASE_ADDRESS,
#endif
#ifdef GPIO_J_BASE_ADDRESS
	   GPIO_J_BASE_ADDRESS,
#endif
#ifdef GPIO_K_BASE_ADDRESS
	   GPIO_K_BASE_ADDRESS,
#endif
							};


Port_ConfigType Port_Config0[TOTAL_NO_OF_PINS] =
{
	{  /** GPIO_A_PIN_0 */
		PIN_MODE_GPIO,
		PORT_PIN_OUT,
		PORT_PIN_HIGH,
		FALSE,
		FALSE,
		PIN_OUTPUT_OPEN_DRAIN,
		PIN_HIGH_SPEED,
		PIN_PULL_DOWN
	},

	{  /** GPIO_A_PIN_1 */
		PIN_MODE_GPIO,
		PORT_PIN_IN,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_A_PIN_2 */
		PIN_MODE_ALT_FUNC_3,
		PORT_PIN_OUT,
		PORT_PIN_HIGH,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_A_PIN_3 */
		PIN_MODE_ANALOG,
		PORT_PIN_OUT,
		PORT_PIN_HIGH,
		TRUE,
		FALSE,
		PIN_OUTPUT_OPEN_DRAIN,
		PIN_HIGH_SPEED,
		PIN_PULL_DOWN
	},

	{  /** GPIO_A_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_A_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_2 */
		PIN_MODE_GPIO,
		PORT_PIN_IN,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_B_PIN_3 */
		PIN_MODE_GPIO,
		PORT_PIN_IN,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_B_PIN_4 */
		PIN_MODE_GPIO,
		PORT_PIN_IN,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_B_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_6 */
		PIN_MODE_GPIO,
		PORT_PIN_OUT,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_B_PIN_7 */
		PIN_MODE_GPIO,
		PORT_PIN_IN,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP,
	},

	{  /** GPIO_B_PIN_8 */
		PIN_MODE_GPIO,
		PORT_PIN_OUT,
		PORT_PIN_LOW,
		TRUE,
		TRUE,
		PIN_OUTPUT_PUSH_PULL,
		PIN_VERY_HIGH_SPEED,
		PIN_PULL_UP
	},

	{  /** GPIO_B_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_B_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_C_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_D_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_E_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_F_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_G_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_H_PIN_15 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_0 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_1 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_2 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_3 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_4 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_5 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_6 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_7 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_8 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_9 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_10 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_11 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_12 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_13 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_14 */
		PIN_UNUSED,
	},

	{  /** GPIO_I_PIN_15 */
		PIN_UNUSED,
	},

};














