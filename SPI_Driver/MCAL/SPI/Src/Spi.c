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
#include "Dio.h"
#include "Spi.h"

#define IB_BUFFERS_AVAILABLE (128U)	/** (2bytes*x) Reserve 256 Bytes for Internal Buffers */
#define SEQUENCE_COMPLETED   (NO_OF_SEQUENCES_CONFIGURED)	/** Dummy value needed in sSpi_FillJobQueue */

typedef enum /** Defines a range of specific status for SPI Handler */
{
	SPI_BUFFER_EMPTY = 0U,
	SPI_BUFFER_NOT_EMPTY,
}Spi_BufferStatusType;


//Spi_StatusType gEn_SpiStatus = SPI_UNINIT;

Spi_GlobalParams GlobalParams;

Spi_JobResultType Spi_JobResult[NO_OF_JOBS_CONFIGURED];
Spi_SeqResultType Spi_SeqResult[NO_OF_SEQS_CONFIGURED];

static const Spi_ConfigType* GlobalConfigPtr;
static Spi_DataBufferType sInternalBuffer[IB_BUFFERS_AVAILABLE]; /** Reserve 256 Bytes for Internal Buffers */
static Spi_EbType sExternalBuffer[NO_OF_CHANNELS_CONFIGURED];
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

	/** Initialise internal Buffers for channels using IB */
	if((sSpi_AllocateIbMemory()) == E_NOT_OK)
	{
		/** NO MEMORY FOUND */
		/** TODO - Report DET Error */
	}

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
		/** Select the device as master */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,2U),SET_BIT(2U));

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

#if 0
	/** Allocate memory for the channel */
	if((sSpi_AllocateIbMemory(Channel,channelConfig->NoOfBuffersUsed)) == E_NOT_OK)
	{
		return E_NOT_OK;  /** Memory not available */
	}
#endif

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
			sInternalBuffer[(sIB_Config[Channel].TxBuffer.Start+loopItr0)] = DataBufferPtr;
			DataBufferPtr++;
		}
	}
	sIB_Config[Channel].TxBuffer.InUse = TRUE;

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

	return E_OK;

}

Std_ReturnType Spi_ReadIB (Spi_ChannelType Channel,Spi_DataBufferType* DataBufferPointer)
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
	if(DataBufferPointer == NULL_PTR)
	{
		return E_NOT_OK;
	}

	uint8_t loopItr0 = 0U;
	uint8_t bufferIndex = 0U;
	Spi_ChannelConfigType channelConfig = GlobalConfigPtr->Channel[Channel];

	/** Read Data from IB and copy it to the DataBufferPtr */
	/** If no new data is received then copy old data */
	for(loopItr0 = 0U; loopItr0 < (channelConfig->NoOfBuffersUsed); loopItr0++)
	{
		bufferIndex = sIB_Config[Channel].RxBuffer.Start+loopItr0;

		/** Write the Data from IB to DataBufferPointer */
		DataBufferPointer = sInternalBuffer[bufferIndex];
		DataBufferPointer++;
	}

	sIB_Config[Channel].RxBuffer.InUse = FALSE;
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
	/** Check for module Init */
	if(GlobalParams.gEn_SpiStatus == SPI_UNINIT)
	{
		return;
	}

	sSpi_StartRxForJobs();
	sSpi_StartQueuedSequence();
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
		return (0U);
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

static Std_ReturnType sSpi_AllocateIbMemory(void)
{
	uint8_t channelItr1 = 0U;
	uint8_t bufferItr1 = 0U;
	uint8_t bufferStart = 0U;
	uint8_t bufferEnd = 0U;

	for(channelItr1 = 0U; channelItr1 < NO_OF_CHANNELS_CONFIGURED; channelItr1++)
	{
		if(GlobalConfigPtr->Channel[channelItr1]->BufferUsed == EN_INTERNAL_BUFFER)
		{
			/** Allocate memory for both Tx and Rx Buffers  */
			for(bufferItr1 = 0U; bufferItr1 < 2U; bufferItr1++)
			{
				bufferEnd = (bufferStart + GlobalConfigPtr->Channel[channelItr1]->NoOfBuffersUsed) -1U;

				if((bufferEnd >= IB_BUFFERS_AVAILABLE) || (bufferStart >= IB_BUFFERS_AVAILABLE))
				{
					/** Memory Overflow */
					return E_NOT_OK;
				}

				/** Memory found update the parameters */
				if(bufferItr1 == 0U)
				{
					sIB_Config[channelItr1].RxBuffer.Start = bufferStart;
					sIB_Config[channelItr1].RxBuffer.End = bufferEnd;
					sIB_Config[channelItr1].RxBuffer.InUse = FALSE;
				}
				else /** bufferItr1 == 1U */
				{
					sIB_Config[channelItr1].TxBuffer.Start = bufferStart;
					sIB_Config[channelItr1].TxBuffer.End = bufferEnd;
					sIB_Config[channelItr1].TxBuffer.InUse = FALSE;
				}
				sIB_Config[channelItr1].ChannelId = channelItr1;

				/** Update parameters for next iteration */
				bufferStart = bufferEnd +1U;
			}
		}
	}
	return E_OK;
}

#if 0
static Std_ReturnType sSpi_xyzIbMemory(Spi_ChannelType Channel,uint8_t NoOfBuffersUsed)
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
#endif

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

static Spi_BufferStatusType sSpi_GetRxBufferStatus(Spi_HwType en_moduleNo)
{
	volatile  SPI_RegTypes * pReg = (SPI_RegTypes *)Spi_BaseAddress[moduleNo];
	uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R);

	if(((REG_READ32(&pReg->SR.R)) & MASK_BIT(0U)) == SET)
	{
		return SPI_BUFFER_NOT_EMPTY;
	}
	return SPI_BUFFER_EMPTY;
}

static Spi_BufferStatusType sSpi_GetTxBufferStatus(Spi_HwType en_moduleNo)
{
	volatile  SPI_RegTypes * pReg = (SPI_RegTypes *)Spi_BaseAddress[moduleNo];
	uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R);

	if(((REG_READ32(&pReg->SR.R)) & MASK_BIT(1U)) == SET)
	{
		return SPI_BUFFER_EMPTY;
	}
	return SPI_BUFFER_NOT_EMPTY;
}

/** TODO - 1. Interruptible sequence is not handled
 * 		   2. if a sequence fails to execute then the function returns whereas the next sequence should be scheduled
 * 		   3. Priority of jobs is not handled between interruptible sequences
 * 		   */
static void sSpi_StartQueuedSequence(void)
{
	Spi_JobConfigType jobConfig;
	Spi_SequenceType SequenceId = GlobalParams.NextSequence;
	uint8_t loopItr0 = 0U;
	uint8_t loopItr1 = 0U;
	uint8_t sequenceIndex = 0U;
	Spi_HwType en_moduleNo = 0U;

	/** No Pending Sequence */
	if(SequenceId == EOL)
	{
		return;
	}

	/** Get the Index values of the job buffer for the next pending sequence */
	for(loopItr1 = 0U; loopItr1 < NO_OF_SEQUENCES_CONFIGURED; loopItr1++ )
	{
		if(GlobalParams.BufferIndex[loopItr1].sequenceId == SequenceId)
		{
			sequenceIndex = loopItr1;
			break;
		}
	}
	/**
	 * do
	 * {
	 * complete all jobs in the sequence then
	 * sequenceIndex++;
	 * if(sequenceIndex == NO_OF_SEQUENCES_CONFIGURED)
	 *	{
	 *		sequenceIndex = 0U;
	 *	}
	 * }(while(sequenceIndex!= loopItr1))
	 *
	 *	 * */

	do /** while((sequenceIndex!= loopItr1)); */
	{

	/** Execute all queued jobs in the sequence */
	/** != instead of < because of loop over, a small value could also be endBufferIndex */
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
			GlobalParams.NextSequence = GlobalParams.BufferIndex[sequenceIndex].sequenceId;
			sSpi_UpdateSequenceBuffer(sequenceIndex,SPI_SEQ_FAILED);
			return;
		}
		else /** Start the Job in sequence */
		{
			sSpi_StartJob(jobConfig);
			/** Update the start index so that this job is not executed again */
			GlobalParams.BufferIndex[sequenceIndex].startBufferIndex = loopItr0;
		}
	}
	/** Now that all the jobs in the sequence have been executed  */
	/** Update sequence status */
	GlobalParams.BufferIndex[sequenceIndex].sequenceId = SEQUENCE_COMPLETED;
	sSpi_SetSeqResult(SequenceId,SPI_SEQ_OK);
	/** Call Notification function */
	GlobalConfigPtr->Sequence[SequenceId]->SpiSequenceEndNotification();

	/** Schedule Next sequence */
	while((GlobalParams.BufferIndex[sequenceIndex].sequenceId == EOL) ||
					(GlobalParams.BufferIndex[sequenceIndex].sequenceId == SEQUENCE_COMPLETED))
	{
		sequenceIndex++;
		if((sequenceIndex) == NO_OF_SEQUENCES_CONFIGURED)
		{
			sequenceIndex = 0U;
		}

		if((sequenceIndex == loopItr1))
		{
			/** No more pending sequences */
			GlobalParams.NextSequence = EOL;
			GlobalParams.gEn_SpiStatus = SPI_IDLE;
			break;
		}
	}

	if(sequenceIndex != loopItr1)
	{
		/** Store the sequence id  */
		GlobalParams.NextSequence = GlobalParams.BufferIndex[sequenceIndex].sequenceId;
	}

	}while((sequenceIndex!= loopItr1));

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
	uint8_t loopbreaker = 0U; /** TODO - Optimize this */

	Spi_SequenceType seqId =  GlobalParams.BufferIndex[sequenceIndex].sequenceId;

	/** Fail the sequence if no of retries is exceeded */
	/** Delete the sequence jobs from buffer  */
	/** Update the next pending sequence index */
	if((GlobalParams.BufferIndex[sequenceIndex].noOfRetries++) >= NO_OF_SEQ_RETRIES)
	{
		/** Update job and sequence results */
		Spi_SequenceConfigType SequenceConfig = GlobalConfigPtr->Sequence[seqId];
		Spi_JobType * jobPtr = &SequenceConfig->Jobs[0U];

		while(*jobPtr != EOL)
		{
			sSpi_SetJobResult(*jobPtr,SPI_JOB_FAILED);
		}

		sSpi_SetSeqResult(seqId,SPI_SEQ_FAILED);

		/** Delete the sequence */
		GlobalParams.BufferIndex[sequenceIndex].sequenceId = SEQUENCE_COMPLETED;
		loopbreaker = sequenceIndex;

		while((GlobalParams.BufferIndex[sequenceIndex].sequenceId == EOL) ||
				(GlobalParams.BufferIndex[sequenceIndex].sequenceId == SEQUENCE_COMPLETED))
		{
			sequenceIndex++;
			if((sequenceIndex) == NO_OF_SEQUENCES_CONFIGURED)
			{
				sequenceIndex = 0U;
			}

			if(loopbreaker == sequenceIndex)
			{
				break;  /** TODO - Optimise this */
			}
		}

		if(loopbreaker == sequenceIndex)
		{
			/** No More pending sequence */
			GlobalParams.NextSequence = EOL;
		}
		else
		{
			GlobalParams.NextSequence = GlobalParams.BufferIndex[sequenceIndex].sequenceId;
		}
	}
}

static void sSpi_StartJob(Spi_JobConfigType* JobConfig)
{
	uint8_t jobId = JobConfig->JobId;
	uint8_t channelId = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t loopItr1 = 0U;
	Spi_HwType en_moduleNo = 0U;
	Spi_ChannelConfigType channelConfig = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	/** Update the Job status */
	sSpi_SetJobResult(jobId,SPI_JOB_PENDING);

	/** Get SPI Hw ID */
	en_moduleNo = Spi_getModuleNo(JobConfig);
	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

	/** Start Channels execution */
	while(JobConfig->ChannelAssignment[loopItr0] != EOL)
	{
		channelId = JobConfig->ChannelAssignment[loopItr0];
		channelConfig = GlobalConfigPtr->Channel[channelId];

		/** Configure the LSBFIRST bit*/
		/** Check if the module is busy */
		while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

		if(channelConfig->TransferStart == EN_LSB_FIRST)
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,7U),(SET_BIT(7U)));
		}
		else
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,7U),(CLEAR_BIT(7U)));
		}

		/** Make the NSS Pin low to indicate to slave of start transfer */
		if(JobConfig->CsFunctionUsed == EN_CS_SW_HANDLING)
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(CLEAR_BIT(8U)));
			Dio_WriteChannel(JobConfig->CsPinUsed,STD_LOW);
		}

		if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
		{
			for(loopItr1 = 0U; loopItr1<(channelConfig->NoOfBuffersUsed); loopItr1++)
			{
				/** Check if Tx buffer is empty and write data*/
				while( (sSpi_GetTxBufferStatus(en_moduleNo)) != SPI_BUFFER_EMPTY );

				if(sIB_Config[channelId].BufferInUse == FALSE)
				{
					REG_WRITE32(&pReg->DR.R,(channelConfig->DefaultTransmitValue)&0xFFFF);
				}
				else /** (sIB_Config[channelId].BufferInUse == TRUE) */
				{
					REG_WRITE32(&pReg->DR.R,(sInternalBuffer[sIB_Config[channelId].BufferStart+loopItr1]));
				}
			}
		}

		else if(channelConfig->BufferUsed == EN_EXTERNAL_BUFFER)
		{
			for(loopItr1 = 0U; loopItr1<(sExternalBuffer[channelId].length); loopItr1++)
			{
				/** Check if Tx buffer is empty and write data*/
				while( (REG_READ32((&pReg->SR.B.TXE))) != SET );

				if(sExternalBuffer[channelId].active == FALSE)
				{
					REG_WRITE32(&pReg->DR.R,(channelConfig->DefaultTransmitValue)&0xFFFF);
				}
				else /** (sExternalBuffer[channelId].active == TRUE) */
				{
					REG_WRITE32(&pReg->DR.R,(sExternalBuffer[channelId].TxBuffer[loopItr1]));
				}
			}
		}

		/** Make the NSS Pin High to indicate to slave of end transfer */
		if(JobConfig->CsFunctionUsed == EN_CS_SW_HANDLING)
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(SET_BIT(8U)));
			Dio_WriteChannel(JobConfig->CsPinUsed,STD_HIGH);
		}
	}

	sSpi_SetJobResult(JobConfig->JobId,SPI_JOB_OK);

	/** Call the notification function */
	JobConfig->SpiJobEndNotification();
}

static void sSpi_StartRxForJobs(void)
{
	Spi_HwType en_moduleNo = 0U;
	Spi_JobType jobId = 0U;
	Spi_JobConfigType jobConfig = 0U;
	Spi_ChannelConfigType channelConfig = 0U;
	Spi_ChannelType channelId = 0U;
	uint8_t channelItr = 0U;
	Spi_DataBufferType * destBufferPtr = 0U;
	uint8_t loopItr0 = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	for(jobId = 0U; jobId < NO_OF_JOBS_CONFIGURED; jobId++)
	{
		jobConfig = GlobalConfigPtr->Job[jobId];
		en_moduleNo = jobConfig->HwUsed;
		channelItr = 0U;

		while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

		pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

		/** Skip iteration if no messages in Rx buffer */
		if(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_EMPTY)
		{
			continue;
		}

		/** Copy data from Hw Buffer to Channel Buffer  */
		while(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_NOT_EMPTY)
		{
			channelId = jobConfig->ChannelAssignment[channelItr];

			/** If channel exists copy it to buffer */
			if(channelId != EOL)
			{
				channelConfig = GlobalConfigPtr->Channel[channelId];

				/** Get Buffer Type */
				if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
				{
					destBufferPtr = sInternalBuffer[sIB_Config[channelId].RxBuffer.Start];
					sIB_Config[channelId].RxBuffer.InUse = TRUE;
				}
				else /** channelConfig->BufferUsed == EN_EXTERNAL_BUFFER */
				{
					destBufferPtr = sExternalBuffer[channelId].RxBuffer;
					sExternalBuffer[channelId].active = TRUE;
				}

				/** Copy data to Buffer */
				for(loopItr0 = 0U; loopItr0 = channelConfig->NoOfBuffersUsed; loopItr0++)
				{
					if(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_NOT_EMPTY)
					{
						(*destBufferPtr) = REG_READ32(&pReg->DR.R);
					}
					else
					{
						(*destBufferPtr) = 0U;
					}
				}
				channelItr++;
			}
			else /** Channel buffers are full but data is still coming */
			{
				/** TODO - Figure out what to do, currently just reading the register to clear the data */
				while(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_NOT_EMPTY)
				{
					REG_READ32(&pReg->DR.R);
				}
			}
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

