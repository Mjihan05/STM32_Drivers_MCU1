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
extern Port_ConfigType* GlobalConfigPtr;


/** Function Prototypes AUTOSAR  */
void Port_Init (const Port_ConfigType* ConfigPtr); /** Initialises the Port Driver module. */
#if(PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection (Port_PinType Pin,Port_PinDirectionType Direction);
#endif
void Port_RefreshPortDirection (void);
#if(PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode (Port_PinType Pin,Port_PinModeType Mode);
#endif

/** Function Prototypes Non-AUTOSAR */
#if(GPIO_PORT_LOCK == STD_ON)
void GPIO_Port_Lock(uint8_t moduleNo, uint16_t lockMask);
void GPIO_Port_Unlock(uint8_t moduleNo);
#endif
void GPIO_Port_Enable(uint8_t portNo);
void GPIO_Port_Disable(uint8_t portNo);
void GPIO_Reset_Port(uint8_t portNo);

#endif /* GPIO_INC_GPIO_H_ */
