/**********************************************
 * File - Dio.h
 * Author - Michael Jihan
 * Created on 08-Aug-2020
 *
 * Purpose - This files holds the prototype
 * 			 Global functions that will Read/Modify
 * 			 the pins configured as GPIO in PORT
 *
 * ********************************************/

#ifndef DIO_INC_DIO_H_
#define DIO_INC_DIO_H_

#include "Dio_PbCfg.h"

typedef enum
{
	DIO_E_PARAM_INVALID_CHANNEL_ID = 0x0A,
	DIO_E_PARAM_INVALID_PORT_ID = 0x14,
	DIO_E_PARAM_INVALID_GROUP = 0x1F,
	DIO_E_PARAM_POINTER = 0x20,
}DIO_Error_Codes;


/** Function Prototypes */
Dio_LevelType Dio_ReadChannel (Dio_ChannelType ChannelId);
void Dio_WriteChannel (Dio_ChannelType ChannelId,Dio_LevelType Level);
Dio_PortLevelType Dio_ReadPort (Dio_PortType PortId);				 /** NOTE - Call Dio_ConfigPortMasks() first only once */
void Dio_WritePort (Dio_PortType PortId,Dio_PortLevelType Level);    /** NOTE - Call Dio_ConfigPortMasks() first only once */
Dio_PortLevelType Dio_ReadChannelGroup (const Dio_ChannelGroupType* ChannelGroupIdPtr);
void Dio_WriteChannelGroup (const Dio_ChannelGroupType* ChannelGroupIdPtr,Dio_PortLevelType Level);
Dio_LevelType Dio_FlipChannel (Dio_ChannelType ChannelId);
void Dio_MaskedWritePort (Dio_PortType PortId,Dio_PortLevelType Level,Dio_PortLevelType Mask);

void Dio_ConfigPortMasks(void);

#endif /* DIO_INC_DIO_H_ */
