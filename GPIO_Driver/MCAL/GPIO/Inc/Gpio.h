/**********************************************
 * File - Gpio.h
 * Author - Michael Jihan
 * Created on 25-Jul-2020
 *
 * Purpose - This files holds the Global functions
 * 			 that will configure and control
 * 			 Gpio settings
 *
 * ********************************************/

#ifndef GPIO_INC_GPIO_H_
#define GPIO_INC_GPIO_H_

#include "GPIO_regTypes.h"
#include "GPIO_PbCfg.h"

extern InitStatusType gu8_PortInitStatus;
extern Port_ConfigType Port_Config0;


/** Function Prototypes */
void Port_Init (const Port_ConfigType* ConfigPtr); /** Initialises the Port Driver module. */

#endif /* GPIO_INC_GPIO_H_ */
