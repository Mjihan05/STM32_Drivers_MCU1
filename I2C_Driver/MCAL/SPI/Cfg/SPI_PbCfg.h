/**********************************************
 * File - SPI_PbCfg.h
 * Author - Michael Jihan
 * Created on Oct 17, 2020
 *
 * Purpose - This file is to hold the configuration
 * 			 structures
 *
 * ********************************************/

#ifndef SPI_CFG_SPI_PBCFG_H_
#define SPI_CFG_SPI_PBCFG_H_

#include "SPI_regTypes.h"
#include "Dio_PbCfg.h"

#define TOTAL_NO_OF_SPI_HW_UNIT 	(EN_SPI_3 + 1U)

/** LEVEL 0, Simple Synchronous SPI Handler/Driver
 *  LEVEL 1, Basic Asynchronous SPI Handler/Driver
 *  LEVEL 2, Enhanced (Synchronous/Asynchronous) SPI Handler/Driver */
#define SPI_LEVEL_DELIVERED 		(2U)

#define NO_OF_CHANNELS_CONFIGURED	(3U)
#define NO_OF_JOBS_CONFIGURED		(3U)
#define NO_OF_SEQUENCES_CONFIGURED	(4U)

#define NO_OF_SEQ_RETRIES 			(5U)

/** Usage 0: the SPI Handler/Driver manages only Internal Buffers.
 	Usage 1: the SPI Handler/Driver manages only External Buffers.
 	Usage 2: the SPI Handler/Driver manages both buffers types. */
#define SPI_CHANNEL_BUFFERS_ALLOWED (2U)


#define EOL (0xFFU)

typedef uint8_t Spi_DataBufferType;
typedef uint16_t Spi_NumberOfDataType;
typedef uint8_t Spi_ChannelType;
typedef uint16_t Spi_JobType;
typedef uint8_t Spi_SequenceType;
typedef uint8_t Spi_HWUnitType;

/** Buffer used by the SPI Hw */
typedef enum
{
	EN_INTERNAL_BUFFER,
	EN_EXTERNAL_BUFFER
}Spi_BufferType;

/** Data Frame Format Types */
typedef enum
{
	EN_8_BIT_DATA_FRAME,
	EN_16_BIT_DATA_FRAME
}Spi_DataFrameType;

/** Type of Transmission */
typedef enum
{
	EN_MSB_FIRST,
	EN_LSB_FIRST
}Spi_DataShiftType;

typedef enum
{
	EN_CS_NOT_USED,
	EN_CS_SW_HANDLING,
	EN_CS_HW_HANDLING
}Spi_CsFunctionality;

typedef enum
{
	EN_SHIFT_ON_RISING_EDGE,
	EN_SHIFT_ON_FALLING_EDGE,
}Spi_ShiftEdgeType;

/** Spi Hw units available in STM32 */
typedef enum
{
	EN_SPI_1,
	EN_SPI_2,
	EN_SPI_3
}Spi_HwType;

/** Structure to hold the IB information // Only used for channel with buffer used as Internal */
typedef struct
{
	uint8_t Start;
	uint8_t End;
	Bool InUse;
}Ib_Type;
typedef struct
{
	Spi_ChannelType ChannelId;
	Ib_Type TxBuffer;
	Ib_Type RxBuffer;
}InternalBufferType;

/** Structure to hold Channel properties */
typedef struct
{
	Spi_ChannelType ChannelId;
	Spi_BufferType BufferUsed; /** Internal or External , STM32 has internal buffers*/
	Spi_DataFrameType DataFrame; /** Contains the width of the transmitted data, has to be under 16bits */
	uint8_t NoOfBuffersUsed;  /** for IB Channels (at least 1) or it is the maximum of data for EB Channels (a value of 0 makes no sense) */
	Spi_DataShiftType TransferStart; /** MSB first or LSB first transmission */
	Spi_DataBufferType DefaultTransmitValue;  /** Default Value to be used when Dataptr is NULL, 16bit value max */
}Spi_ChannelConfigType;

typedef struct
{
	Spi_JobType JobId;
	Spi_HwType HwUsed;		/** SPI Hw used */
	Spi_CsFunctionality CsFunctionUsed;	/** Chip select functionality notUsed/SW/Hw */
	Dio_ChannelType CsPinUsed;		/** if used on which pin*/
	uint8_t CsPinPolarity;  /** And the selection polarity */
	uint32_t BaudRate;		/** Baud rate in MHz (User has to verify perClk/BaudRate = valid Prescaler) */
	uint32_t TimeClkandCs;	/** Time between CLk and CS (TODO - Check if this is needed) */
	uint8_t ShiftClkIdleLevel; /** CPOL (clock polarity) bit controls the steady state value of the clock when no data is being transferred */
	Spi_ShiftEdgeType DataShiftonEdge;	/**  CPHA (clock phase) bit Controls on which edge Tx takes place */
	uint8_t Priority;			/** Priority from 0 lower to 3 highest */
	uint32_t TimebetweenChannel;  /** Delay between execution of channels in  1/SysClk (us) */
	void (*SpiJobEndNotification)();
	Spi_ChannelType ChannelAssignment[NO_OF_CHANNELS_CONFIGURED];
}Spi_JobConfigType;

typedef struct
{
	Spi_SequenceType SequenceId;
	uint8_t SpiInterruptible;	 /** is the Sequence Interruptible */
	void (*SpiSequenceEndNotification)();
	Spi_JobType Jobs[NO_OF_JOBS_CONFIGURED];  /** Available jobs in sequence */
}Spi_SequenceConfigType;

/** Structure shall contain the initialization data for the SPI Handler*/
typedef struct
{
/**
 *  MCU dependent properties for SPI HW units
 Definition of Channels
 Definition of Jobs
 Definition of Sequences⌋*/
	const Spi_ChannelConfigType* Channel;
	const Spi_JobConfigType* Job;
	const Spi_SequenceConfigType* Sequence;
}Spi_ConfigType;


#endif /* SPI_CFG_SPI_PBCFG_H_ */
