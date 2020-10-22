/**********************************************
 * File - Spi.c
 * Author - Michael Jihan
 * Created on Oct 17, 2020
 *
 * Purpose - This file holds the definition of
 * 			 functions that configure SPI settings
 *
 * ********************************************/

#include <stdint.h>
#include <Reg_Macros.h>

#include "RCC.h"
#include "Gpio.h"
#include "Spi.h"

Spi_StatusType Spi_GetStatus (void)
{
	/**
	 *  API service Spi_GetStatus shall return SPI_UNINIT when the
		SPI Handler/Driver is not initialized or not usable
	 *	API service Spi_GetStatus shall return SPI_IDLE when The
		SPI Handler/Driver is not currently transmitting any Job
	 *	API service Spi_GetStatus shall return SPI_BUSY when The
		SPI Handler/Driver is performing a SPI Job transmit
	 */
}

Spi_JobResultType Spi_GetJobResult (Spi_JobType Job)
{
	/**
	 * he function Spi_GetJobResult shall return SPI_JOB_OK
when the last transmission of the Job has been finished successfully*/
}

Spi_SeqResultType Spi_GetSequenceResult (Spi_SequenceType Sequence)
{
	/**
	 *  Spi_GetSequenceResult function shall return SPI_SEQ_OK
when the last transmission of the Sequence has been finished successfully
	 *  Spi_GetSequenceResult function shall return SPI_SEQ_PENDING
 when the SPI Handler/Driver is performing a SPI Sequence.
	 *  Spi_GetSequenceResult function shall return
SPI_SEQ_FAILED when the last transmission of the Sequence has failed
	 * */
}

