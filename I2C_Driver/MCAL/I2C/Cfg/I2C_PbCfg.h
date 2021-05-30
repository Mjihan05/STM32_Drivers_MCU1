/**********************************************
 * File - I2C_PbCfg.h
 * Author - Michael Jihan
 * Created on 06-Feb-2021
 *
 * Purpose - This file is to hold the configuration
 * 			 structures
 *
 *********************************************/

#ifndef I2C_CFG_I2C_PBCFG_H_
#define I2C_CFG_I2C_PBCFG_H_

#include "I2C_regTypes.h"

#define TOTAL_NO_OF_I2C_HW_UNIT 	(EN_I2C_3 + 1U)

#define NO_OF_HW_CONFIGURED 		(1U)

#define NO_OF_CHANNELS_CONFIGURED	(2U)
#define NO_OF_JOBS_CONFIGURED		(1U)
#define NO_OF_SEQUENCES_CONFIGURED	(1U)

/** LEVEL 0, Simple Synchronous I2C Handler/Driver
 *  LEVEL 1, Basic Asynchronous I2C Handler/Driver
 *  LEVEL 2, Enhanced (Synchronous/Asynchronous) I2C Handler/Driver */
#define I2C_LEVEL_DELIVERED 		(0U)

/** Usage 0: the SPI Handler/Driver manages only Internal Buffers.
 	Usage 1: the SPI Handler/Driver manages only External Buffers.
 	Usage 2: the SPI Handler/Driver manages both buffers types. */
#define I2C_CHANNEL_BUFFERS_ALLOWED (2U)

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
#define IB_BUFFERS_AVAILABLE (128U)	/** (1bytes*x) Reserve 128 Bytes for Internal Buffers */
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */


#define EOL (0xFFU)

typedef uint8_t I2C_ChannelType;
typedef uint8_t I2C_JobType;
typedef uint8_t I2C_SequenceType;
typedef uint8_t I2C_DataBufferType;
typedef uint16_t I2C_NumberOfDataType;

/** Buffer used by the I2C Hw */
typedef enum
{
	EN_INTERNAL_BUFFER,
	EN_EXTERNAL_BUFFER
}I2C_BufferType;

/** I2C Hw units available in STM32 */
typedef enum
{
	EN_I2C_1,
	EN_I2C_2,
	EN_I2C_3
}I2C_HwType;

typedef enum
{
	I2C_START_BIT_GENERATED = 0U,
	I2C_ADDRESS_SENT = 1U,
	I2C_BYTE_TRANSFER_FINISHED = 2U,
	I2C_10_BIT_ADDRESS_SENT = 3U,
	I2C_STOP_CONDITION_DETECTED = 4U,
	I2C_RX_DATA_REGISTER_NOT_EMPTY = 6U,
	I2C_TX_DATA_REGISTER_EMPTY = 7U,
	I2C_BUS_ERROR = 8U,			/** Misplaced Start or Stop condition */
	I2C_ARBITRATION_LOSS_DETECTED = 9U,
	I2C_ACKNOWLEDGE_FAILURE = 10U,
	I2C_OVERRUN_OR_UNDERRUN_DETECTED = 11U,
	I2C_TIMEOUT_DETECTED = 14U,		/** SCL remained LOW for 25 ms (Timeout) */
}I2C_Status1Type;

typedef enum
{
	I2C_MASTER_MODE_SELECTED = 0U,		/** If False then Slave mode is selected */
	I2C_BUS_BUSY	 = 1U,
	I2C_DATA_BYTES_TRANSMITTED = 2U,	/** If False then DATA_BYTES_RECIEVED */
	I2C_GENERAL_CALL_ADDR_DETECTED = 4U,
	I2C_RX_ADDR_MATCHED_OAR2 = 7U,
}I2C_Status2Type;

/** I2C Slave Addressing modes  */
typedef enum
{
	EN_I2C_7_BIT_SLAVE_ADDRESS,
	EN_I2C_10_BIT_SLAVE_ADDRESS
}I2C_SlaveAddressType;

/** I2C Master Communication modes  */
typedef enum
{
	EN_I2C_STANDARD_MODE,	/** min 2MHZ i/p freq needed from PCLK1 */
	EN_I2C_FAST_MODE		/** min 4MHZ i/p freq needed from PCLK1  */
}I2C_MasterModeType;

/** I2C Fast Mode Duty Cycle  */
typedef enum
{
	EN_I2C_FAST_MODE_DUTY_2,		/** tlow/thigh = 2  */
	EN_I2C_FAST_MODE_DUTY_16_DIV_9  /** tlow/thigh = 16/9  */
}I2C_FMDutyCycleType;

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
/** Structure to hold the IB information // Only used for channel with buffer used as Internal */
typedef struct
{
	uint8_t Start;
	uint8_t End;
	Bool InUse;
}Ib_Type;
typedef struct
{
	I2C_ChannelType ChannelId;
	Ib_Type TxBuffer;
	Ib_Type RxBuffer;
}InternalBufferType;
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

typedef struct
{
	I2C_HwType ModuleNo;
	I2C_SlaveAddressType SlaveAddressMode;
	uint16_t SlaveAddress;
	I2C_MasterModeType MasterMode;
	uint32_t ClkSpeed;
	I2C_FMDutyCycleType FastModeDutyCycle;
}I2C_HwConfigType;

/** Structure to hold Channel properties */
typedef struct
{
	I2C_ChannelType ChannelId;
	I2C_BufferType BufferUsed; 					/** Internal or External , STM32 has internal buffers*/
	uint8_t NoOfBuffersUsed;  					/** for IB Channels (at least 1) or it is the maximum of data for EB Channels (a value of 0 makes no sense) */
	I2C_DataBufferType DefaultTransmitValue; 	/** Default Value to be used when Dataptr is NULL, 8-bit value max */
}I2C_ChannelConfigType;

typedef struct
{
	I2C_JobType JobId;
	I2C_HwConfigType* HwConfig;		/** I2C Hw used */
	uint8_t Priority;				/** Priority from 0 lower to 3 highest */
	uint32_t TimebetweenChannel;  	/** Delay between execution of channels in  1/SysClk (us) */
	void (*I2CJobEndNotification)();
	I2C_ChannelType ChannelAssignment[NO_OF_CHANNELS_CONFIGURED];
}I2C_JobConfigType;

typedef struct
{
	I2C_SequenceType SequenceId;
	uint8_t I2CInterruptible;				  /** is the Sequence Interruptible */
	void (*I2CSequenceEndNotification)();
	I2C_JobType Jobs[NO_OF_JOBS_CONFIGURED];  /** Available jobs in sequence */
}I2C_SequenceConfigType;

/** Structure shall contain the initialisation data for the I2C Handler*/
typedef struct
{
/**
 *  MCU dependent properties for I2C HW units
 Definition of Channels
 Definition of Jobs
 Definition of Sequences⌋*/
	const I2C_ChannelConfigType* Channel;
	const I2C_JobConfigType* Job;
	const I2C_SequenceConfigType* Sequence;
}I2C_ConfigType;

#endif /* I2C_CFG_I2C_PBCFG_H_ */
