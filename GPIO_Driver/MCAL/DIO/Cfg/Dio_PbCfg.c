/**********************************************
 * File - Dio_PbCfg.c
 * Author - Michael Jihan
 * Created on 09-Aug-2020
 *
 * Purpose - To hold the Configuration structures
 * 				for DIO Module
 *
 * ********************************************/

#include <stdint.h>
#include "Reg_Macros.h"
#include "Dio_PbCfg.h"


Dio_ChannelType Dio_ChannelUsed[DIO_PINS_CONFIGURED] =
{
	DIO_PIN_A0,
	DIO_PIN_A1,
	DIO_PIN_B2,
	DIO_PIN_B3,
	DIO_PIN_B4,
	DIO_PIN_B6,
	DIO_PIN_B7,
	DIO_PIN_B8,
};

Dio_PortType Dio_PortUsed[DIO_PORT_CONFIGURED] =
{
	(0U),
	(1U)
};

Dio_ChannelGroupType Dio_ChannelGroup[DIO_GROUP_CONFIGURED] =
{
	{
		.mask = 0x0C,
		.offset = 2U,
		.port = 0U
	},
	{
		.mask = 0x1C,
		.offset = 2U,
		.port = 1U
	},
};








