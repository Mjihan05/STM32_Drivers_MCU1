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

typedef struct
{
/**
 * Buffer usage with EB/IB Channel
 * Transmit data width (1 up to 32 bits)
 * Number of data buffers for IB Channels (at least 1) or it is the maximum of data for EB Channels (a value of 0 makes no sense)
 * Transfer start LSB or MSB
 * Default transmit value*/
}Spi_LChannelType;

typedef struct
{
	/**
	 * Assigned SPI HW Unit
	 * Assigned Chip Select pin (it is possible to assign no pin)
	 * Chip select functionality on/off
	 * Chip select pin polarity high or low
	 * Baud rate
	 * Timing between clock and chip select
	 *  Shift clock idle low or idle high
	 *  Data shift with leading or trailing edge
	 *  Priority (4 levels are available from 0, the lower to 3, the higher)
	 *  Job finish end notification function
	 *  MCU dependent properties for the Job (only if needed)
	 *  Fixed link of Channels (at least one)*/
}Spi_LJobType;

typedef struct
{
	/**
	 * Collection of Jobs (at least one)
	 * Interruptible or not interruptible after each Job
	 * Sequence finish end notification function */
}Spi_LSequenceType;

/** Structure shall contain the initialization data for the SPI Handler*/
typedef struct
{
/**
 *  MCU dependent properties for SPI HW units
 Definition of Channels
 Definition of Jobs
 Definition of Sequences⌋*/
	Spi_LChannelType* Channel;
	Spi_LJobType* Job;
	Spi_LSequenceType* Sequence;
}Spi_ConfigType;


#endif /* SPI_CFG_SPI_PBCFG_H_ */
