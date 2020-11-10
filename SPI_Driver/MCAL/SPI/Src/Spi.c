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
static uint8_t Spi_GetBaudratePrescaler(Spi_JobConfigType* JobConfig);
static void Spi_ResetModule (Spi_HwType moduleNo);

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
	uint8_t preScaler = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	for(loopItr0 = 0U; loopItr0 < NO_OF_JOBS_CONFIGURED; loopItr0++ )
	{
		moduleNo = Spi_getModuleNo(ConfigPtr->Job[loopItr0]);
		pReg = (SPI_RegTypes *)Spi_BaseAddress[moduleNo];

		/** Turn on the clock for the module */
		Spi_Clk_Enable(moduleNo);

		/** Configure the Baudrate */
		preScaler = Spi_GetBaudratePrescaler(ConfigPtr->Job[loopItr0]);
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x7U,3U),(preScaler)<<(3U));

		/** Select the Data frame format to be 16 bit. Depending on the channels DataFrame SW will handle frame formatting  */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,11U),SET_BIT(11U));

		/** Select Sw/Hw Chip select functionality */
		if((ConfigPtr->Job[loopItr0]->CsFunctionUsed) == EN_CS_SW_HANDLING)
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,9U),SET_BIT(9U));
		}

		/** Configure clock polarity and Clock phase  */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,1U),(ConfigPtr->Job->ShiftClkIdleLevel)<<1U);
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,0U),(ConfigPtr->Job->DataShiftonEdge));

		/** Enable the Peripheral */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),SET_BIT(6U));

		gEn_SpiStatus= SPI_IDLE;
	}

	gu8_SpiInitStatus = MODULE_INITIALIZED;
}

Std_ReturnType Spi_DeInit (void)
{
	if(gEn_SpiStatus == SPI_BUSY)
	{
		return E_NOT_OK;
	}

	uint8_t loopItr = 0U;

	for(loopItr = 0U; loopItr<TOTAL_NO_OF_SPI_HW_UNIT; loopItr++)
	{
		Spi_ResetModule(loopItr);
	}

	gEn_SpiStatus = SPI_UNINIT;

	return E_OK;
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

		case EN_SPI_3:
			RCC_APB1PeripheralClkEnable(EN_SPI3);
			break;

		default:
			break;
	}
}

static uint8_t Spi_GetBaudratePrescaler(Spi_JobConfigType* JobConfig)
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

	if(moduleNo == EN_SPI_1) /** (User has to verify perClk/BaudRate = valid preScaler) */
	{
		peripheralClk = (uint32_t)(RCC_GetAPB2Clk());
		preScaler = (uint8_t)(peripheralClk/requiredClk);
	}
	else
	{
		peripheralClk = (uint32_t)(RCC_GetAPB1Clk());
		preScaler = (uint8_t)(peripheralClk/requiredClk);
	}

	while(preScaler != 2U) /** Get the regValue to be written for the calculated preScaler */
	{
		preScaler = prescaler >> 1U;
		temp++;
	}

	preScaler = temp;

	return preScaler;
}

static void Spi_ResetModule (Spi_HwType moduleNo)
{
	switch(moduleNo)
		{
			case EN_SPI_1:
				RCC_ResetAPB2Peripheral(EN_SPI1);
				break;

			case EN_SPI_2:
				RCC_ResetAPB1Peripheral(EN_SPI2);
				break;

			case EN_SPI_3:
				RCC_ResetAPB1Peripheral(EN_SPI3);
				break;

			default:
				break;
		}
}







