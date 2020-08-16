/**********************************************
 * File - Dio_PbCfg.h
 * Author - Michael Jihan
 * Created on 09-Aug-2020
 *
 * Purpose - 
 *
 * ********************************************/

#ifndef DIO_CFG_DIO_PBCFG_H_
#define DIO_CFG_DIO_PBCFG_H_

/** Configured Pins  */
#define DIO_PINS_CONFIGURED  (8U)
#define DIO_PORT_CONFIGURED  (2U)
#define DIO_GROUP_CONFIGURED (2U)
#define DIO_PIN_A0  (0U)
#define DIO_PIN_A1  (1U)
#define DIO_PIN_B2  (18U)
#define DIO_PIN_B3  (19U)
#define DIO_PIN_B4  (20U)
#define DIO_PIN_B6  (22U)
#define DIO_PIN_B7  (23U)
#define DIO_PIN_B8  (24U)



#define DIO_PIN_UNUSED  	(0xFFFFU)
#define TOTAL_NO_OF_PORTS	(9U)  					/** NOTE - Change in GPIO_Pbcfg.h also */
#define TOTAL_NO_OF_PINS	(TOTAL_NO_OF_PORTS*16U) /** NOTE - Change in GPIO_Pbcfg.h also */

typedef uint8_t Dio_ChannelType; /** Numeric ID of a DIO channel. */
typedef uint8_t Dio_PortType;	 /** Numeric ID of a DIO port. */
typedef uint16_t Dio_PortLevelType;

typedef enum
{
	STD_LOW,
	STD_HIGH,
}Dio_LevelType;

typedef struct
{
	uint16_t mask;   /** This element mask which defines the positions of the channel group. */
	uint8_t offset;	 /** This element shall be the position of the Channel Group on the port,counted from the LSB. */
	uint8_t port; 	 /** This shall be the port on which the Channel group is defined. */
}Dio_ChannelGroupType;

/**
typedef struct
{
	Dio_ChannelType Dio_Channel;
	Dio_PortType portNo;
	uint8_t pinNo;
};
*/



extern Dio_ChannelType Dio_ChannelUsed[DIO_PINS_CONFIGURED];
extern Dio_ChannelGroupType Dio_ChannelGroup[DIO_GROUP_CONFIGURED];





#endif /* DIO_CFG_DIO_PBCFG_H_ */
