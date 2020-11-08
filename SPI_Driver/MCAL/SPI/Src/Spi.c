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

Spi_StatusType gEn_SpiStatus = SPI_UNINIT;

uint8_t gu8_SpiInitStatus = MODULE_UNINITIALIZED;

static Spi_HwType Spi_getModuleNo (Spi_JobConfigType* JobConfig);
static void Spi_Clk_Enable(Spi_HwType moduleNo);
static uint8_t Spi_ConfigBaudrate(Spi_JobConfigType* JobConfig)

/********************** Global Functions ******************************/

void Spi_Init (const Spi_ConfigType* ConfigPtr)
{
	if(ConfigPtr == NULL_PTR)
	{
		gu8_SpiInitStatus = MODULE_INIT_FAILED;
		return;
	}

	Spi_HwType moduleNo = 0U;
	uint8_t loopItr0 = 0U;

	for(loopItr0 = 0U; loopItr0 < NO_OF_JOBS_CONFIGURED; loopItr0++ )
	{
		moduleNo = Spi_getModuleNo(ConfigPtr->Job[loopItr0]);

		/** Turn on the clock for the module */
		Spi_Clk_Enable(moduleNo);

		/** Configure the Baudrate */
		Spi_ConfigBaudrate(ConfigPtr->Job[loopItr0]);

	}






}

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

/********************** Local Functions *****************************/

static Spi_HwType Spi_getModuleNo (Spi_JobConfigType* JobConfig)
{
	if(JobConfig == NULL_PTR)
	{
		return 0xFF;
	}

	return (Spi_HwType)(JobConfig->HwUsed);
}

static void Spi_Clk_Enable(Spi_HwType moduleNo)
{
	/** SPI-1 - APB2, SPI-2,3 - APB1,  */
	switch(moduleNo)
	{
		case EN_SPI_1:
			RCC_APB2PeripheralClkEnable(EN_SPI1);
			break;

		case EN_SPI_2:
			RCC_APB1PeripheralClkEnable(EN_SPI2);
			break;

		case EN_SPI_2:
			RCC_APB1PeripheralClkEnable(EN_SPI3);
			break;

		default:
			break;
	}
}

static uint8_t Spi_ConfigBaudrate(Spi_JobConfigType* JobConfig)
{
	if(gu8_SpiInitStatus != MODULE_INITIALIZED)
	{
		return;
	}

	Spi_HwType moduleNo = (Spi_HwType)(JobConfig->HwUsed);
	uint8_t preScaler = 0U;
	uint32_t peripheralClk = 0U;
	uint32_t requiredClk = (uint32_t)(JobConfig->BaudRate);
	uint8_t temp = 0U;

	if(moduleNo == EN_SPI_1)
	{
		peripheralClk = (uint32_t)(RCC_GetAPB2Clk());
		preScaler = (uint8_t)(peripheralClk/requiredClk);
	}
	else
	{
		peripheralClk = (uint32_t)(RCC_GetAPB1Clk());
		preScaler = (uint8_t)(peripheralClk/requiredClk);
	}

	while(preScaler != 2U)
	{
		preScaler = prescaler >> 1U;
		temp++;
	}

	preScaler = temp;

	return preScaler;
}









