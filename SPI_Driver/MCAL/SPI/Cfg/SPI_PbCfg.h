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

/** Spi Hw units available in STM32 */
typedef enum
{
	EN_SPI_1,
	EN_SPI_2,
	EN_SPI_3,
	EN_SPI_4,
	EN_SPI_5,
	EN_SPI_6
}Spi_HwType;

/** Structure to hold Channel properties */
typedef struct
{
	Spi_ChannelType ChannelId;
	Spi_BufferType BufferUsed; /** Internal or External , STM32 has internal buffers*/
	Spi_DataFrameType DataFrame; /** Contains the width of the transmitted data, has to be under 16bits */
	uint8_t NoOfBuffersUsed;  /** for IB Channels (at least 1) or it is the maximum of data for EB Channels (a value of 0 makes no sense) */
	Spi_DataShiftType TransferStart; /** MSB first or LSB first transmission */
	uint16_t DefaultTransmitValue;  /** Default Value to be used when Dataptr is NULL, 16bit value max */
}Spi_ChannelConfigType;

typedef struct
{
	Spi_JobType JobId;
	Spi_HwType HwUsed;		/** SPI Hw used */
	uint8_t CsFunctionUsed;	/** Chip select functionality notUsed/SW/Hw */
	uint16_t CsPinUsed;		/** if used on which pin*/
	uint8_t CsPinPolarity;  /** And the selection polarity */
	uint32_t BaudRate;		/** Baud rate in MHz */
	uint32_t TimeClkandCs;	/** Time between CLk and CS (TODO - Check if this is needed) */
	uint8_t ShiftClkIdleLevel; /** CPOL (clock polarity) bit controls the steady state value of the clock when no data is being transferred */
	uint8_t DataShiftonEdge;	/**  CPHA (clock phase) bit Controls on which edge Tx takes place */
	uint8_t Priority;			/** Priority */
	void (*SpiJobEndNotification)();
}Spi_JobConfigType;

typedef struct
{
	Spi_SequenceType SequenceId;
	Spi_JobType Jobs[Max_Jobs];  /** Available jobs in sequence */
	uint8_t SpiInterruptible;	 /** is the Sequence Interruptible */
	void (*SpiSequenceEndNotification)();
}Spi_SequenceConfigType;

/** Structure shall contain the initialization data for the SPI Handler*/
typedef struct
{
/**
 *  MCU dependent properties for SPI HW units
 Definition of Channels
 Definition of Jobs
 Definition of Sequences⌋*/
	Spi_ChannelConfigType* Channel;
	Spi_JobConfigType* Job;
	Spi_SequenceConfigType* Sequence;
}Spi_ConfigType;


#endif /* SPI_CFG_SPI_PBCFG_H_ */
