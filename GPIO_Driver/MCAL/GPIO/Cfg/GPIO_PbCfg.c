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












