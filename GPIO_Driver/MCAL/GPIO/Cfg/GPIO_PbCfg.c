/**********************************************
 * File - GPIO_PbCfg.c
 * Author - Michael Jihan
 * Created on 25-Jul-2020
 *
 * Purpose - 
 *
 * ********************************************/

#include <stdint.h>
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



Port_ConfigType Port_Config0[NO_OF_PINS_CONFIG] =
{
	{
		PIN_MODE_ALT_FUNC_1,
		PORT_PIN_OUT,
		1U,
		1U,
		1U,
		PIN_OUTPUT_OPEN_DRAIN,
		PIN_HIGH_SPEED,
		PIN_PULL_DOWN,
	},
};












