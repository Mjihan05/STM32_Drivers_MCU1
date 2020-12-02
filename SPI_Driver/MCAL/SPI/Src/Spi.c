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

#define IB_BUFFERS_AVAILABLE (128U)	/** (2bytes*x) Reserve 256 Bytes for Internal Buffers */
#define SEQUENCE_COMPLETED   (NO_OF_SEQUENCES_CONFIGURED)	/** Dummy value needed in sSpi_FillJobQueue */

//Spi_StatusType gEn_SpiStatus = SPI_UNINIT;

Spi_GlobalParams GlobalParams;

Spi_JobResultType Spi_JobResult[NO_OF_JOBS_CONFIGURED];
Spi_SeqResultType Spi_SeqResult[NO_OF_SEQS_CONFIGURED];

static const Spi_ConfigType* GlobalConfigPtr;
static Spi_DataBufferType sInternalBuffer[IB_BUFFERS_AVAILABLE]; /** Reserve 256 Bytes for Internal Buffers */
static InternalBufferType sIB_Config[NO_OF_CHANNELS_CONFIGURED];

static Spi_HwType Spi_getModuleNo (Spi_JobConfigType* JobConfig);
static void Spi_Clk_Enable(Spi_HwType moduleNo);
static uint8_t Spi_GetBaudratePrescaler(Spi_JobConfigType* JobConfig);
static void Spi_ResetModule (Spi_HwType moduleNo);
static Std_ReturnType sSpi_CheckSharedJobs(Spi_SequenceType Sequence);
static Std_ReturnType sSpi_AllocateIbMemory(Spi_ChannelType Channel,uint8_t NoOfBuffersUsed);


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

	GlobalConfigPtr = ConfigPtr;

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

		/** Enable Polling Mode for the HW unit */
#if(SPI_LEVEL_DELIVERED == (2U))
		REG_RMW32(&pReg->CR2.R,MASK_BITS(0x7U,5U),(0U)<<5U);
#endif

		/** Enable the Peripheral */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),SET_BIT(6U));

		GlobalParams.SpiStatus = SPI_IDLE;
	}

	/** Initialise Job Queue with EOL values */
	for(loopItr0 = 0U; loopItr0 < QUEUE_SIZE; loopItr0++ )
	{
		GlobalParams.SpiQueuedJobsBuffer[loopItr0] = EOL;
	}

	/** Initialise the Result for Sequence and jobs  */
	for(loopItr0 = 0U; loopItr0 < NO_OF_JOBS_CONFIGURED; loopItr0++ )
	{
		sSpi_SetJobResult(loopItr0,SPI_JOB_OK);
	}
	for(loopItr0 = 0U; loopItr0 < NO_OF_SEQUENCES_CONFIGURED; loopItr0++ )
	{
		sSpi_SetSeqResult(loopItr0,SPI_SEQ_OK);
		/** Also initialise the pending sequence buffer */
		GlobalParams.BufferIndex[loopItr0].sequenceId = EOL;
	}

	/** Initialise Sequence queue */
	GlobalParams.NextSequence = EOL;
	//gu8_SpiInitStatus = MODULE_INITIALIZED;
}

Std_ReturnType Spi_DeInit (void)
{
	if(GlobalParams.gEn_SpiStatus == SPI_BUSY)
	{
		return E_NOT_OK;
	}

	uint8_t loopItr = 0U;

	for(loopItr = 0U; loopItr<TOTAL_NO_OF_SPI_HW_UNIT; loopItr++)
	{
		Spi_ResetModule(loopItr);
	}

	GlobalParams.gEn_SpiStatus = SPI_UNINIT;

	return E_OK;
}

#if(SPI_CHANNEL_BUFFERS_ALLOWED != (1U))
Std_ReturnType Spi_WriteIB (Spi_ChannelType Channel,const Spi_DataBufferType* DataBufferPtr)
{
	/** Check for module Init */
	if(GlobalParams.gEn_SpiStatus == SPI_UNINIT)
	{
		return E_NOT_OK;
	}

	/** Parameter Checking */
	if((Channel >= NO_OF_CHANNELS_CONFIGURED) || (GlobalConfigPtr->Channel[Channel]->BufferUsed |= EN_INTERNAL_BUFFER))
	{
		return E_NOT_OK;
	}

	uint8_t loopItr0 = 0U;
	Spi_ChannelConfigType channelConfig = GlobalConfigPtr->Channel[Channel];

	/** Allocate memory for the channel */
	if((sSpi_AllocateIbMemory(Channel,channelConfig->NoOfBuffersUsed)) == E_NOT_OK)
	{
		return E_NOT_OK;  /** Memory not available */
	}

	/** Write Data to the IB */
	for(loopItr0 = 0U; loopItr0 < (channelConfig->NoOfBuffersUsed); loopItr0++)
	{
		/** If Data Buffer is Null then default transmit data is used */
		if(DataBufferPtr == NULL_PTR)
		{
			sInternalBuffer[(sIB_Config[Channel].BufferStart+loopItr0)] = GlobalConfigPtr->Channel[Channel]->DefaultTransmitValue;
		}
		else
		{
			/** Reformat the data as per dataFrame */
			if(GlobalConfigPtr->Channel[Channel]->DataFrame < 9U)
			{
				DataBufferPtr = DataBufferPtr & 0x00FF;
			}
			else if(GlobalConfigPtr->Channel[Channel]->DataFrame < 17U)
			{
				DataBufferPtr = DataBufferPtr & 0xFFFF;
			}
			else
			{
				DataBufferPtr = GlobalConfigPtr->Channel[Channel]->DefaultTransmitValue;
			}

			/** Write the Data to Buffer */
			sInternalBuffer[(sIB_Config[Channel].BufferStart+loopItr0)] = DataBufferPtr;
			DataBufferPtr++;
		}
	}
	sIB_Config[Channel].BufferInUse = TRUE;

	return E_OK;

}
#endif /** (SPI_CHANNEL_BUFFERS_ALLOWED != (1U)) */

Std_ReturnType Spi_AsyncTransmit (Spi_SequenceType Sequence)
{
	/** Check for module Init */
	if(GlobalParams.gEn_SpiStatus == SPI_UNINIT)
	{
		return E_NOT_OK;
	}

	/** Parameter Checking */
	if(Sequence >= NO_OF_SEQUENCES_CONFIGURED)
	{
		return E_NOT_OK;
	}

	/** if the sequence is already in pending state then return */
	if(Spi_SeqResult[Sequence] == SPI_SEQ_PENDING)
	{
		return E_NOT_OK;
	}

	/** Check if the sequence shares any jobs with sequences already in pending */
	if((sSpi_CheckSharedJobs(Sequence)) == E_NOT_OK)
	{
		return E_NOT_OK;
	}

	Spi_SequenceConfigType SequenceConfig = GlobalConfigPtr->Sequence[Sequence];

	/** Put the Sequence in pending and start transmission of the sequence */
	GlobalParams.gEn_SpiStatus = SPI_BUSY;
	sSpi_SetSeqResult(Sequence,SPI_SEQ_PENDING);

	/** Fill the next sequence index */
	if(GlobalParams.NextSequence == EOL)
	{
		GlobalParams.NextSequence = Sequence;
	}

	Spi_JobType * jobPtr = &SequenceConfig->Jobs[0U];

	while(*jobPtr != EOL)
	{
		sSpi_SetJobResult(*jobPtr,SPI_JOB_QUEUED);
	}

	/** Fill the Queue with jobs as per Priority */
	sSpi_FillJobQueue(SequenceConfig);


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
	 * The function Spi_GetJobResult shall return SPI_JOB_OK
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

void Spi_MainFunction_Handling (void)
{
	sSpi_StartQueuedJobs();
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

/** Function returns non zero value if shared Jobs exist else it returns 0 */
static Std_ReturnType sSpi_CheckSharedJobs(Spi_SequenceType Sequence)
{
	uint32_t mask1 = 0U;
	uint32_t mask2 = 0U;
	uint8_t sequenceItr = 0U;

	Spi_SequenceConfigType SequenceConfig = GlobalConfigPtr->Sequence[Sequence];
	Spi_JobType *jobPtr = &SequenceConfig->Jobs[0];

	while(*jobPtr != EOL)
	{
		mask1 |= SET_BIT((*jobPtr));
		jobPtr++;
	}

	for(sequenceItr = 0U; sequenceItr < NO_OF_SEQUENCES_CONFIGURED; sequenceItr++)
	{
		if ((Spi_GetSequenceResult(sequenceItr)!=SPI_SEQ_PENDING)||(Sequence == sequenceItr))
		{
			continue;
		}
		else
		{
			SequenceConfig = GlobalConfigPtr->Sequence[sequenceItr];
			jobPtr = &SequenceConfig->Jobs[0];

			while(*jobPtr != EOL)
			{
				mask2 |= SET_BIT((*jobPtr));
				jobPtr++;
			}

			if(mask1&mask2) /*non zero value if shared Jobs exist else 0*/
			{
				return E_NOT_OK;
			}
		}
	}

	return E_OK;
}

static Std_ReturnType sSpi_AllocateIbMemory(Spi_ChannelType Channel,uint8_t NoOfBuffersUsed)
{
	uint8_t channelItr1 = 0U;
	uint8_t channelItr2 = 0U;
	uint8_t bufferStart = 0U;
	uint8_t bufferEnd = 0U;

	if(sIB_Config[Channel].BufferInUse == TRUE)
	{
		sIB_Config[Channel].BufferInUse = FALSE;
		return E_OK;
	}
	else	/** sIB_Config[Channel].BufferInUse == FALSE */
	{
		for(channelItr1 = 0U; channelItr1 < NO_OF_CHANNELS_CONFIGURED; channelItr1++)
		{
			if(channelItr1 == Channel)
			{
				continue;
			}
			else if (sIB_Config[channelItr1].BufferInUse == TRUE)
			{
				bufferStart = sIB_Config[channelItr1].BufferEnd + 1U;
				bufferEnd = bufferStart + NoOfBuffersUsed - 1U;
				if(bufferEnd >= IB_BUFFERS_AVAILABLE )
				{
					bufferEnd = bufferEnd % IB_BUFFERS_AVAILABLE;
				}
				for(channelItr2 = 0U; channelItr2 < NO_OF_CHANNELS_CONFIGURED; channelItr2++)
				{
					if(sIB_Config[channelItr2].BufferInUse == TRUE)
					{
						if(((bufferStart >= sIB_Config[channelItr2].BufferStart)&&(bufferStart <= sIB_Config[channelItr2].BufferEnd)) ||
						   ((bufferEnd >= sIB_Config[channelItr2].BufferStart)&&(bufferEnd <= sIB_Config[channelItr2].BufferEnd)))
						{
							break;
						}
						else /** Found Space */
						{
							sIB_Config[Channel].ChannelId = Channel;
							sIB_Config[Channel].BufferStart = bufferStart;
							sIB_Config[Channel].BufferEnd = bufferEnd;
							return E_OK;
						}
						break;
					}
				}
			}
		}
	}
	return E_NOT_OK;
}

static Spi_StatusType sSpi_GetHwStatus(Spi_HwType en_moduleNo)
{
	volatile  SPI_RegTypes * pReg = (SPI_RegTypes *)Spi_BaseAddress[moduleNo];
	uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R);

	if(((REG_READ32(&pReg->SR.R)) & MASK_BIT(7U)) == SET)
	{
		return SPI_BUSY;
	}
	return SPI_IDLE;
}

static void sSpi_StartQueuedJobs(void)
{
	Spi_JobConfigType jobConfig;
	Spi_SequenceType SequenceId = GlobalParams.NextSequence;
	uint8_t loopItr0 = 0U;
	uint8_t sequenceIndex = 0U;
	Spi_HwType en_moduleNo = 0U;

	/** Get the Index values of the job buffer for the next pending sequence */
	for(loopItr0 = 0U; loopItr0 < NO_OF_SEQUENCES_CONFIGURED; loopItr0++ )
	{
		if(GlobalParams.BufferIndex[loopItr0].sequenceId == SequenceId)
		{
			sequenceIndex = loopItr0;
			break;
		}
	}

	/** != instead of < because of loop over a small value could also be endBufferIndex */
	for(loopItr0 = GlobalParams.BufferIndex[sequenceIndex].startBufferIndex;
			(loopItr0 != GlobalParams.BufferIndex[sequenceIndex].endBufferIndex); loopItr0++ )
	{
		if(loopItr0 == QUEUE_SIZE)
		{
			loopItr0 = 0U;
		}

		jobConfig = GlobalConfigPtr->Job[(GlobalParams.SpiQueuedJobsBuffer[loopItr0])];
		en_moduleNo = Spi_getModuleNo(jobConfig);

		/** Check if the module is busy */
		if(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY)
		{
			/** Fail the sequence if no of retries is exceeded */
			/** Delete the sequence jobs from buffer  */
			/** Update the next pending sequence index */
			sSpi_UpdateSequenceBuffer(sequenceIndex,SPI_SEQ_FAILED);
			return;
		}

	}
}

/** Jobs are filled in as per priority since the channels are put in the cfg as per priority */
/** Circular queue is implemented */
static void sSpi_FillJobQueue(Spi_SequenceConfigType SequenceConfig)
{
	static uint8_t u8_startIndex = 0U;
	Spi_JobConfigType jobConfig;
	Spi_JobType * jobPtr = &SequenceConfig->Jobs[0U];
	static uint8_t u8_GlobalPrioIndex = 0U;

	/** This is to make sure the first called sequence is executed first */
	while((GlobalParams.BufferIndex[u8_GlobalPrioIndex].sequenceId != EOL) &&
			(GlobalParams.BufferIndex[u8_GlobalPrioIndex].sequenceId != SEQUENCE_COMPLETED))
	{
		u8_GlobalPrioIndex++;
		if(u8_GlobalPrioIndex == NO_OF_SEQUENCES_CONFIGURED)
		{
			u8_GlobalPrioIndex = 0U;
		}
	}

	GlobalParams.BufferIndex[u8_GlobalPrioIndex].sequenceId = &SequenceConfig->SequenceId;
	GlobalParams.BufferIndex[u8_GlobalPrioIndex].noOfRetries = 0U;
	GlobalParams.BufferIndex[u8_GlobalPrioIndex].startBufferIndex = u8_startIndex;
	//GlobalParams.startBufferIndex = u8_startIndex;

	while(*jobPtr != EOL)
	{
		//jobConfig = GlobalConfigPtr->Job[*jobPtr];
		GlobalParams.SpiQueuedJobsBuffer[u8_startIndex] = (*jobPtr);
		jobPtr++;
		u8_startIndex++;
		if(u8_startIndex == QUEUE_SIZE)
		{
			u8_startIndex = 0U;
		}
	}

	/** Not (u8_startIndex-1) because loopover of the buffer needs to be handled  */
	GlobalParams.BufferIndex[u8_GlobalPrioIndex].endBufferIndex = u8_startIndex;
}

static void sSpi_UpdateSequenceBuffer(uint8_t sequenceIndex,Spi_SeqResultType Result)
{
	/** Fail the sequence if no of retries is exceeded */
	/** Delete the sequence jobs from buffer  */
	/** Update the next pending sequence index */
	if((GlobalParams.BufferIndex[sequenceIndex].noOfRetries++) > NO_OF_SEQ_RETRIES)
	{
		sSpi_SetSeqResult(SequenceId,SPI_SEQ_FAILED);
		GlobalParams.BufferIndex[sequenceIndex].sequenceId = SEQUENCE_COMPLETED;

		if((sequenceIndex+1U)>=NO_OF_SEQUENCES_CONFIGURED)
		{
			sequenceIndex = 0U;
		}
		else
		{
			sequenceIndex++;
		}

		if((GlobalParams.BufferIndex[sequenceIndex].sequenceId != EOL) &&
				(GlobalParams.BufferIndex[sequenceIndex].sequenceId != SEQUENCE_COMPLETED))
		{
			GlobalParams.NextSequence = sequenceIndex;
		}
		else
		{
			/** No More pending sequence */
			GlobalParams.NextSequence = EOL;
		}
	}
}

static void sSpi_SetSeqResult (Spi_SequenceType Sequence,Spi_SeqResultType Result)
{
	Spi_SeqResult[Sequence] = Result;
}

static void sSpi_SetJobResult(Spi_JobType Job,Spi_JobResultType Result)
{
	Spi_JobResult[Job] = Result;
}

