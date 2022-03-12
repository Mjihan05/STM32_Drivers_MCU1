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

#include "SPI_PbCfg.h"

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
Spi_SeqResultType Spi_SeqResult[NO_OF_SEQUENCES_CONFIGURED];

static const Spi_ConfigType* GlobalConfigPtr;
static Spi_DataBufferType sInternalBuffer[IB_BUFFERS_AVAILABLE]; /** Reserve 256 Bytes for Internal Buffers */
static Spi_EbType sExternalBuffer[NO_OF_CHANNELS_CONFIGURED];
static InternalBufferType sIB_Config[NO_OF_CHANNELS_CONFIGURED];

/********************** Local Functions Prototypes *****************************/

static Spi_HwType sSpi_getModuleNo (const Spi_JobConfigType* JobConfig);
static void sSpi_Clk_Enable(Spi_HwType moduleNo);
static uint8_t sSpi_GetBaudratePrescaler(Spi_JobConfigType* JobConfig);
static void sSpi_ResetModule (Spi_HwType moduleNo);
#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
static Std_ReturnType sSpi_CheckSharedJobs(Spi_SequenceType Sequence);
static void sSpi_StartQueuedSequence(void);
static void sSpi_FillJobQueue(Spi_SequenceConfigType SequenceConfig);
#if 0
static void sSpi_StartRxForJobs(void);
#endif
static void sSpi_UpdateSequenceBuffer(uint8_t sequenceIndex,Spi_SeqResultType Result);
#endif /** #if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */
static Std_ReturnType sSpi_AllocateIbMemory(void);
static Spi_StatusType sSpi_GetHwStatus(Spi_HwType en_moduleNo);
static Spi_BufferStatusType sSpi_GetRxBufferStatus(Spi_HwType en_moduleNo);
static Spi_BufferStatusType sSpi_GetTxBufferStatus(Spi_HwType en_moduleNo);
static void sSpi_StartJob(Spi_JobConfigType* JobConfig);
static void sSpi_StartHw(Spi_JobType JobId);
static void sSpi_CopyRxDataToChannelBuffer(Spi_ChannelConfigType* channelConfig,uint16_t u16_Data,uint8_t bufferNo);
#if 0
static void sSpi_CopyDataToChannelBuffer(Spi_JobType jobId);
#endif
#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (0U)))
static void sSpi_StartSyncTransmit(Spi_SequenceType Sequence);
#endif /** #if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (0U))) */
static void sSpi_CancelSequence(Spi_SequenceType Sequence);
#if(SPI_LEVEL_DELIVERED == (2U))
static void sSpi_ConfigInterrupt(Spi_JobType jobitr, uint8_t Enable);
static Spi_AsyncModeType sSpi_GetAsyncMode (void);
static void sSpi_ConfigAsyncMode (Spi_JobType JobId);
#if 0
static void sSpi_ScheduleJob(Spi_JobType JobId);
#endif
static void sSpi_LoadFifoBuffer(Spi_HwType en_moduleNo);
static void sSpi_EndJob(Spi_JobType JobId);
static void sSpi_ScheduleNextSequence(void);
void sISR_Spi_Global(Spi_HwType en_moduleNo);
#endif /** (SPI_LEVEL_DELIVERED == (2U))*/
static void sSpi_SetSpiStatus (Spi_StatusType Status);
static void sSpi_SetSeqResult (Spi_SequenceType Sequence,Spi_SeqResultType Result);
static void sSpi_SetJobResult(Spi_JobType Job,Spi_JobResultType Result);

/********************* END of Local Functions Prototypes ***********************/


/********************** Global Functions ******************************/

void Spi_Init (const Spi_ConfigType* ConfigPtr)
{
	if(ConfigPtr == NULL_PTR)
	{
		sSpi_SetSpiStatus (SPI_UNINIT);
		/** TODO - Report DET error here */
		return;
	}

	Spi_HwType moduleNo = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t preScaler = 0U;
	Spi_JobConfigType* JobConfig = 0U;

	GlobalConfigPtr = ConfigPtr;


	volatile  SPI_RegTypes * pReg = 0U;

#if ((SPI_CHANNEL_BUFFERS_ALLOWED == 0U) || (SPI_CHANNEL_BUFFERS_ALLOWED == 2U))
	/** Initialise internal Buffers for channels using IB */
	if((sSpi_AllocateIbMemory()) == E_NOT_OK)
	{
		/** NO MEMORY FOUND */
		/** TODO - Report DET Error */
	}
#endif /** #if ((SPI_CHANNEL_BUFFERS_ALLOWED == 0U) || (SPI_CHANNEL_BUFFERS_ALLOWED == 2U)) */

	for(loopItr0 = 0U; loopItr0 < NO_OF_JOBS_CONFIGURED; loopItr0++ )
	{
		JobConfig = (Spi_JobConfigType*)(&ConfigPtr->Job[loopItr0]);
		moduleNo = sSpi_getModuleNo(JobConfig);
		pReg = (SPI_RegTypes *)Spi_BaseAddress[moduleNo];

		/** Turn on the clock for the module */
		sSpi_Clk_Enable(moduleNo);

		/** Configure the Baudrate */
		preScaler = sSpi_GetBaudratePrescaler(JobConfig);
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x7U,3U),(preScaler)<<(3U));

		/** Select the Data frame format to be 16 bit. Depending on the channels DataFrame SW will handle frame formatting  */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,11U),SET_BIT(11U));

		/** Select Sw/Hw Chip select functionality */
		if((JobConfig->CsFunctionUsed) == EN_CS_SW_HANDLING)
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,9U),SET_BIT(9U));
			/** Set the SSI BIT */
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),SET_BIT(8U));
			/** Set the SSOE Bit  */
			REG_RMW32(&pReg->CR2.R,MASK_BITS(0x1U,2U),SET_BIT(2U));
		}
		else if((JobConfig->CsFunctionUsed) == EN_CS_HW_HANDLING)
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,9U),CLEAR_BIT(9U));
			/** Set the SSOE Bit  */
			REG_RMW32(&pReg->CR2.R,MASK_BITS(0x1U,2U),SET_BIT(2U));
		}
		else /** ((JobConfig->CsFunctionUsed) == EN_CS_NOT_USED)  */
		{
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,9U),SET_BIT(9U));
			/** Set the SSI BIT */
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),SET_BIT(8U));
			/** Clear the SSOE Bit  */
			REG_RMW32(&pReg->CR2.R,MASK_BITS(0x1U,2U),CLEAR_BIT(2U));
		}

		/** Configure clock polarity and Clock phase  */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,1U),(ConfigPtr->Job->ShiftClkIdleLevel)<<1U);
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,0U),(ConfigPtr->Job->DataShiftonEdge));

		/** Enable Polling Mode for the HW unit */
#if(SPI_LEVEL_DELIVERED == (2U))
		REG_RMW32(&pReg->CR2.R,MASK_BITS(0x7U,5U),(0U)<<5U);
		Spi_SetAsyncMode (SPI_POLLING_MODE);
		GlobalParams.currentHw[moduleNo].currentChannel = EOL;
		GlobalParams.currentHw[moduleNo].dataRemaining = 0U;
#endif
		/** Select the device as master */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,2U),SET_BIT(2U));

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
	if((Spi_GetStatus()) == SPI_BUSY)
	{
		return E_NOT_OK;
	}

	Spi_JobConfigType* JobConfig = 0U;

	uint8_t loopItr = 0U;
	Spi_HwType moduleNo = 0U;

	for(loopItr = 0U; loopItr<NO_OF_JOBS_CONFIGURED; loopItr++)
	{
		JobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[loopItr]);
		moduleNo = sSpi_getModuleNo(JobConfig);

		while(sSpi_GetHwStatus((Spi_HwType)(moduleNo)) == SPI_BUSY);
		if(sSpi_GetRxBufferStatus((Spi_HwType)(moduleNo)) == SPI_BUFFER_NOT_EMPTY)
		{
			return E_NOT_OK;
		}
		if(sSpi_GetTxBufferStatus((Spi_HwType)(moduleNo)) == SPI_BUFFER_NOT_EMPTY)
		{
			return E_NOT_OK;
		}

		/** TODO - Timeout handling */
		sSpi_ResetModule(loopItr);
	}

	sSpi_SetSpiStatus(SPI_UNINIT);

	return E_OK;
}

#if(SPI_CHANNEL_BUFFERS_ALLOWED != (1U))
Std_ReturnType Spi_WriteIB (Spi_ChannelType Channel,const Spi_DataBufferType* DataBufferPtr)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
	{
		return E_NOT_OK;
	}

	/** Parameter Checking */
	if((Channel >= NO_OF_CHANNELS_CONFIGURED) || (GlobalConfigPtr->Channel[Channel].BufferUsed != EN_INTERNAL_BUFFER))
	{
		return E_NOT_OK;
	}

	uint8_t loopItr0 = 0U;
	uint8_t bufferItr = 0U;
	Spi_ChannelConfigType channelConfig = GlobalConfigPtr->Channel[Channel];

#if 0
	/** Allocate memory for the channel */
	if((sSpi_AllocateIbMemory(Channel,channelConfig->NoOfBuffersUsed)) == E_NOT_OK)
	{
		return E_NOT_OK;  /** Memory not available */
	}
#endif

	/** Write Data to the IB */
	for(loopItr0 = 0U; loopItr0 < (channelConfig.NoOfBuffersUsed); loopItr0++)
	{
		bufferItr = (uint8_t)(sIB_Config[Channel].TxBuffer.Start+loopItr0);

		/** If Data Buffer is Null then default transmit data is used */
		if(DataBufferPtr == NULL_PTR)
		{
			sInternalBuffer[bufferItr] = channelConfig.DefaultTransmitValue;
		}
		else
		{
			/** Write the Data to Buffer */
			sInternalBuffer[bufferItr] = (Spi_DataBufferType)(*DataBufferPtr);
#if 0
			/** Reformat the data as per dataFrame */
			if(channelConfig.DataFrame == EN_8_BIT_DATA_FRAME)
			{
				/** Write the Data to Buffer */
				sInternalBuffer[bufferItr] = ((*DataBufferPtr) & 0x00FF);

//				/** Get Next Data */
//				loopItr0++;
//				if(loopItr0 < (channelConfig.NoOfBuffersUsed))
//				{
//					DataBufferPtr++;
//					/** Write the Data to Buffer */
//					sInternalBuffer[bufferItr] |= (uint8_t)((*DataBufferPtr) & 0x00FF);
//				}
			}
			else if(channelConfig.DataFrame == EN_16_BIT_DATA_FRAME)
			{
				/** Write the Data to Buffer */
				sInternalBuffer[bufferItr] = (*DataBufferPtr) & 0xFFFF;
			}
			else
			{
				/** Write the Data to Buffer */
				sInternalBuffer[bufferItr] = channelConfig.DefaultTransmitValue;
			}
#endif

			/** Get Next Data */
			DataBufferPtr++;
		}
	}
	sIB_Config[Channel].TxBuffer.InUse = TRUE;

	return E_OK;

}
#endif /** (SPI_CHANNEL_BUFFERS_ALLOWED != (1U)) */

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
Std_ReturnType Spi_AsyncTransmit (Spi_SequenceType Sequence)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
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
	sSpi_SetSpiStatus(SPI_BUSY);
	sSpi_SetSeqResult(Sequence,SPI_SEQ_PENDING);

	/** Fill the next sequence index */
	if(GlobalParams.NextSequence == EOL)
	{
		GlobalParams.NextSequence = Sequence;
	}

	Spi_JobType * jobPtr = &SequenceConfig.Jobs[0U];

	while(*jobPtr != EOL)
	{
		sSpi_SetJobResult(*jobPtr,SPI_JOB_QUEUED);
		jobPtr++;
	}

	/** Fill the Queue with jobs as per Priority */
	sSpi_FillJobQueue(SequenceConfig);

	/** Configure Interrupt if used */
#if(SPI_LEVEL_DELIVERED == (2U))
	if((sSpi_GetAsyncMode() == SPI_INTERRUPT_MODE) && (GlobalParams.NextSequence == Sequence))
	{
		if(SequenceConfig.Jobs[0U] != EOL)
		{
			Spi_JobType jobId = GlobalConfigPtr->Job[SequenceConfig.Jobs[0U]].JobId;
			/** Update the Job status */
			sSpi_SetJobResult(jobId,SPI_JOB_PENDING);
			/** Configure the Global parameters */
			sSpi_ConfigAsyncMode(jobId);
			/** Config Hw of first job for Interrupt  */
			sSpi_ConfigInterrupt(SequenceConfig.Jobs[0U],STD_ON);

			/** Schedule First Job */
			sSpi_StartHw(SequenceConfig.Jobs[0U]);
			//sSpi_ScheduleJob(SequenceConfig.Jobs[0U]);
		}
	}
#endif /** (SPI_LEVEL_DELIVERED == (2U))*/

	return E_OK;

}
#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */

Std_ReturnType Spi_ReadIB (Spi_ChannelType Channel,Spi_DataBufferType* DataBufferPointer)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
	{
		return E_NOT_OK;
	}

	/** Parameter Checking */
	if((Channel >= NO_OF_CHANNELS_CONFIGURED) || (GlobalConfigPtr->Channel[Channel].BufferUsed != EN_INTERNAL_BUFFER))
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
	for(loopItr0 = 0U; loopItr0 < (channelConfig.NoOfBuffersUsed); loopItr0++)
	{
		bufferIndex = sIB_Config[Channel].RxBuffer.Start+loopItr0;

		/** Write the Data from IB to DataBufferPointer */
		(*DataBufferPointer) = sInternalBuffer[bufferIndex];
		DataBufferPointer++;
	}

	sIB_Config[Channel].RxBuffer.InUse = FALSE;
	return E_OK;
}

Std_ReturnType Spi_SetupEB (Spi_ChannelType Channel,const Spi_DataBufferType* SrcDataBufferPtr,
							Spi_DataBufferType* DesDataBufferPtr,Spi_NumberOfDataType Length)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
	{
		return E_NOT_OK;
	}

	/** Parameter Checking */
	if(Channel >= NO_OF_CHANNELS_CONFIGURED)
	{
		return E_NOT_OK;
	}

	/** Configure the buffers */
	if(SrcDataBufferPtr == NULL_PTR)
	{
		sExternalBuffer[Channel].TxBuffer = NULL_PTR;
	}
	else /** (SrcDataBufferPtr != NULL_PTR) */
	{
		sExternalBuffer[Channel].TxBuffer = (Spi_DataBufferType*)(SrcDataBufferPtr);
	}

	if(DesDataBufferPtr == NULL_PTR)
	{
		sExternalBuffer[Channel].RxBuffer = NULL_PTR;
	}
	else /** (SrcDataBufferPtr != NULL_PTR) */
	{
		sExternalBuffer[Channel].RxBuffer = DesDataBufferPtr;
	}

	/** Configure the Length of the buffer*/
	sExternalBuffer[Channel].length = Length;

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
	return (Spi_StatusType)(GlobalParams.SpiStatus);
}

Spi_JobResultType Spi_GetJobResult (Spi_JobType Job)
{
	if(Job >= NO_OF_JOBS_CONFIGURED)
	{
		return (Spi_JobResultType)(SPI_JOB_FAILED);
	}

	return (Spi_JobResultType)(Spi_JobResult[Job]);
}

Spi_SeqResultType Spi_GetSequenceResult (Spi_SequenceType Sequence)
{
	if(Sequence >= NO_OF_SEQUENCES_CONFIGURED)
	{
		return (Spi_SeqResultType)(SPI_SEQ_FAILED);
	}

	return (Spi_SeqResultType)(Spi_SeqResult[Sequence]);
}

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (0U)))
Std_ReturnType Spi_SyncTransmit (Spi_SequenceType Sequence)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
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

	Spi_SequenceConfigType SequenceConfig = GlobalConfigPtr->Sequence[Sequence];
	Spi_StatusType moduleStatus = Spi_GetStatus();

	/** Put the Sequence in pending and start transmission of the sequence */
	sSpi_SetSpiStatus(SPI_BUSY);
	sSpi_SetSeqResult(Sequence,SPI_SEQ_PENDING);

	Spi_JobType * jobPtr = &SequenceConfig.Jobs[0U];

	while(*jobPtr != EOL)
	{
		sSpi_SetJobResult(*jobPtr,SPI_JOB_QUEUED);
		jobPtr++;
	}

	sSpi_StartSyncTransmit(Sequence);

	sSpi_SetSpiStatus(moduleStatus);

	return E_OK;
}
#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (0U))) */

Spi_StatusType Spi_GetHWUnitStatus (Spi_HWUnitType HWUnit)
{
	if(HWUnit >= TOTAL_NO_OF_SPI_HW_UNIT)
	{
		return SPI_UNINIT;
	}

	return (Spi_StatusType)(sSpi_GetHwStatus(HWUnit));
}

void Spi_Cancel (Spi_SequenceType Sequence)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
	{
		return ;
	}

	/** Parameter Checking */
	if(Sequence >= NO_OF_SEQUENCES_CONFIGURED)
	{
		return ;
	}

	sSpi_CancelSequence(Sequence);
}

#if(SPI_LEVEL_DELIVERED == (2U))
Std_ReturnType Spi_SetAsyncMode (Spi_AsyncModeType Mode)
{
	/** Check for module Init */
	if(((Spi_GetStatus()) == SPI_UNINIT) || ((Spi_GetStatus()) == SPI_BUSY))
	{
		return E_NOT_OK;
	}

	GlobalParams.Spi_AsyncMode = Mode;

	return E_OK;
}
#endif /** (SPI_LEVEL_DELIVERED == (2U))*/

void Spi_MainFunction_Handling (void)
{
	/** Check for module Init */
	if((Spi_GetStatus()) == SPI_UNINIT)
	{
		return;
	}

#if(SPI_LEVEL_DELIVERED == (2U))
	if(sSpi_GetAsyncMode() == SPI_POLLING_MODE)
#endif /** (SPI_LEVEL_DELIVERED == (2U))*/

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
	{
		/** Start Tx for Queued Jobs */
		sSpi_StartQueuedSequence();
	}

#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */

}

#if(SPI_LEVEL_DELIVERED == (2U))
void ISR_Spi_1_Global(void)
{
	sISR_Spi_Global(EN_SPI_1);
}

void ISR_Spi_2_Global(void)
{
	sISR_Spi_Global(EN_SPI_2);
}

void ISR_Spi_3_Global(void)
{
	sISR_Spi_Global(EN_SPI_3);
}
#endif /** (SPI_LEVEL_DELIVERED == (2U))*/

/********************** Local Functions *****************************/

static Spi_HwType sSpi_getModuleNo (const Spi_JobConfigType* JobConfig)
{
	if(JobConfig == NULL_PTR)
	{
		return 0xFF;
	}

	return (Spi_HwType)(JobConfig->HwUsed);
}

static void sSpi_Clk_Enable(Spi_HwType moduleNo)
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

static uint8_t sSpi_GetBaudratePrescaler(Spi_JobConfigType* JobConfig)
{
	Spi_HwType moduleNo = (Spi_HwType)(JobConfig->HwUsed);
	uint32_t preScaler = 0U;
	uint32_t peripheralClk = 0U;
	uint32_t requiredClk = (uint32_t)(JobConfig->BaudRate);
	uint8_t temp = 0U;

	if(moduleNo == EN_SPI_1) /** (User has to verify perClk/BaudRate = valid preScaler) */
	{
		peripheralClk = (uint32_t)(RCC_GetAPB2Clk());
		preScaler = (uint32_t)(peripheralClk/requiredClk);
	}
	else
	{
		peripheralClk = (uint32_t)(RCC_GetAPB1Clk());
		preScaler = (uint8_t)(peripheralClk/requiredClk);
	}

	while(preScaler != 2U) /** Get the regValue to be written for the calculated preScaler */
	{
		preScaler = preScaler >> 1U;
		temp++;
		if(preScaler<2U)
		{
			temp = 0 ;
			preScaler = 2U;
		}
	}

	preScaler = temp;

	return preScaler;
}

static void sSpi_ResetModule (Spi_HwType moduleNo)
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

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
/** Function returns non zero value if shared Jobs exist else it returns 0 */
static Std_ReturnType sSpi_CheckSharedJobs(Spi_SequenceType Sequence)
{
	uint32_t mask1 = 0U;
	uint32_t mask2 = 0U;
	uint8_t sequenceItr = 0U;

	Spi_SequenceConfigType SequenceConfig = GlobalConfigPtr->Sequence[Sequence];
	Spi_JobType *jobPtr = &SequenceConfig.Jobs[0];

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
			jobPtr = &SequenceConfig.Jobs[0];

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
#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */

static Std_ReturnType sSpi_AllocateIbMemory(void)
{
	uint8_t channelItr1 = 0U;
	uint8_t bufferItr1 = 0U;
	uint8_t bufferStart = 0U;
	uint8_t bufferEnd = 0U;
	Spi_ChannelConfigType* channelConfig = 0U;

	for(channelItr1 = 0U; channelItr1 < NO_OF_CHANNELS_CONFIGURED; channelItr1++)
	{
		channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[channelItr1]);
		if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
		{
			/** Allocate memory for both Tx and Rx Buffers  */
			for(bufferItr1 = 0U; bufferItr1 < 2U; bufferItr1++)
			{
				bufferEnd = (bufferStart + channelConfig->NoOfBuffersUsed) -1U;

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
	volatile  SPI_RegTypes * pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];
	/* uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R); */

	if(((REG_READ32(&pReg->SR.R)) & MASK_BIT(7U)) == SET)
	{
		return SPI_BUSY;
	}
	return SPI_IDLE;
}

static Spi_BufferStatusType sSpi_GetRxBufferStatus(Spi_HwType en_moduleNo)
{
	volatile  SPI_RegTypes * pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];
	/*uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R);*/

	if(((REG_READ32(&pReg->SR.R)) & MASK_BIT(0U)) == SET)
	{
		return SPI_BUFFER_NOT_EMPTY;
	}
	return SPI_BUFFER_EMPTY;
}

static Spi_BufferStatusType sSpi_GetTxBufferStatus(Spi_HwType en_moduleNo)
{
	volatile  SPI_RegTypes * pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];
	/*uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R);*/

	if((((REG_READ32(&pReg->SR.R)) & MASK_BIT(1U))>>(1U)) == SET)
	{
		return SPI_BUFFER_EMPTY;
	}
	return SPI_BUFFER_NOT_EMPTY;
}

#if (0)
	static void sSpi_FlushBuffer(Spi_JobConfigType* jobConfig)
	{
		uint8_t jobId = jobConfig->JobId;
		Spi_HwType en_moduleNo = 0U;

		volatile  SPI_RegTypes * pReg = 0U;

		/** Get SPI Hw ID */
		en_moduleNo = sSpi_getModuleNo(jobConfig);
		pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

		REG_WRITE32(&pReg->DR.R,(uint16_t)(0x0U));
		REG_READ32(&pReg->DR.R);
		REG_READ32(&pReg->SR.R);
	}
#endif

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
/** TODO - 1. Interruptible sequence is not handled
 * 		   2. if a sequence fails to execute then the function returns whereas the next sequence should be scheduled
 * 		   3. Priority of jobs is not handled between interruptible sequences
 * 		   */
static void sSpi_StartQueuedSequence(void)
{
	Spi_JobConfigType* jobConfig = 0U;
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

		if(loopItr0 == (GlobalParams.BufferIndex[sequenceIndex].endBufferIndex))
		{
			break;
		}

		jobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[(GlobalParams.SpiQueuedJobsBuffer[loopItr0])]);
		en_moduleNo = sSpi_getModuleNo(jobConfig);

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
	SequenceId = GlobalParams.BufferIndex[sequenceIndex].sequenceId;
	GlobalParams.BufferIndex[sequenceIndex].sequenceId = SEQUENCE_COMPLETED;
	sSpi_SetSeqResult(SequenceId,SPI_SEQ_OK);
	/** Call Notification function */
	GlobalConfigPtr->Sequence[SequenceId].SpiSequenceEndNotification();

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
			sSpi_SetSpiStatus(SPI_IDLE);
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
#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
/** Jobs are filled in as per priority since the channels are put in the cfg as per priority */
/** Circular queue is implemented */
static void sSpi_FillJobQueue(Spi_SequenceConfigType SequenceConfig)
{
	static uint8_t u8_startIndex = 0U;
	Spi_JobType * jobPtr = &SequenceConfig.Jobs[0U];
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

	GlobalParams.BufferIndex[u8_GlobalPrioIndex].sequenceId = SequenceConfig.SequenceId;
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
		Spi_JobType * jobPtr = &SequenceConfig.Jobs[0U];
		Spi_JobConfigType JobConfig = GlobalConfigPtr->Job[*jobPtr];

		while(*jobPtr != EOL)
		{
			sSpi_SetJobResult(*jobPtr,SPI_JOB_FAILED);
			/** Call the notification function */
			JobConfig.SpiJobEndNotification();
		}

		sSpi_SetSeqResult(seqId,Result);
		/** Call the notification function */
		SequenceConfig.SpiSequenceEndNotification();

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
#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */

static void sSpi_StartJob(Spi_JobConfigType* JobConfig)
{
	uint8_t jobId = JobConfig->JobId;
	uint8_t channelId = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t loopItr1 = 0U;
	uint32_t loopItr2 = 0U;
	uint16_t u16_data = 0U;
	volatile uint16_t u16_statusFlag = 0U;
	Spi_HwType en_moduleNo = 0U;
	Spi_ChannelConfigType* channelConfig = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	/** Update the Job status */
	sSpi_SetJobResult(jobId,SPI_JOB_PENDING);

	/** Get SPI Hw ID */
	en_moduleNo = sSpi_getModuleNo(JobConfig);
	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

	/** Start Channels execution */
	while(JobConfig->ChannelAssignment[loopItr0] != EOL)
	{
		channelId = JobConfig->ChannelAssignment[loopItr0];
		channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[channelId]);

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
			//REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(CLEAR_BIT(8U)));
			Dio_WriteChannel(JobConfig->CsPinUsed,STD_LOW);
		}

		/** Select the Frame Format */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,11U),(channelConfig->DataFrame)<<11U);

		/** Enable the Peripheral */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),SET_BIT(6U));

		if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
		{
			for(loopItr1 = 0U; loopItr1<(channelConfig->NoOfBuffersUsed); loopItr1++)
			{
				/** Check if Tx buffer is empty and write data*/
				while( (sSpi_GetTxBufferStatus(en_moduleNo)) != SPI_BUFFER_EMPTY );
				/** TODO - Implement timeout */

				if(sIB_Config[channelId].TxBuffer.InUse == FALSE)
				{
					REG_WRITE32(&pReg->DR.R,(channelConfig->DefaultTransmitValue)&0xFFFF);
				}
				else /** (sIB_Config[channelId].BufferInUse == TRUE) */
				{
					if(channelConfig->DataFrame == EN_16_BIT_DATA_FRAME)
					{
						u16_data = ((Spi_DataBufferType)(sInternalBuffer[sIB_Config[channelId].TxBuffer.Start+loopItr1]))<<8U;
						loopItr1++;
						if(loopItr1<(channelConfig->NoOfBuffersUsed))
						{
							u16_data |= ((Spi_DataBufferType)(sInternalBuffer[sIB_Config[channelId].TxBuffer.Start+loopItr1]));
						}
						REG_WRITE32(&pReg->DR.R,(uint16_t)(u16_data));
					}
					else
					{
						REG_WRITE32(&pReg->DR.R,(sInternalBuffer[sIB_Config[channelId].TxBuffer.Start+loopItr1]));
					}
				}
				u16_statusFlag = (REG_READ32(&pReg->SR.R));
				u16_data = REG_READ32(&pReg->DR.R);

				/** Get Rx buffer status and copy data into buffer  */
				if(((u16_statusFlag) & MASK_BIT(0U)) == SET)
				{
					//u16_data = REG_READ32(&pReg->DR.R);
					sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,loopItr1-1U);
				}

				/** Give delay between the channels */
				for(loopItr2 = 0 ; loopItr2 < (JobConfig->TimebetweenChannel); loopItr2++);

				/** Read the last data (for some reason this works dont ask why) */
				if(loopItr1 == ((channelConfig->NoOfBuffersUsed)-1U))
				{
					u16_data = REG_READ32(&pReg->DR.R);
					sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,loopItr1);
				}

#if 0
				if(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_NOT_EMPTY)
				{
					sSpi_CopyRxDataToChannelBuffer(en_moduleNo,(Spi_ChannelConfigType*)&channelConfig);
				}
#endif

			}
		}

		else if(channelConfig->BufferUsed == EN_EXTERNAL_BUFFER)
		{
			for(loopItr1 = 0U; loopItr1<(sExternalBuffer[channelId].length); loopItr1++)
			{
				/** Check if Tx buffer is empty and write data*/
				while( (sSpi_GetTxBufferStatus(en_moduleNo)) == SPI_BUFFER_NOT_EMPTY );

				if(sExternalBuffer[channelId].TxBuffer == NULL_PTR)
				{
					REG_WRITE32(&pReg->DR.R,(channelConfig->DefaultTransmitValue)&0xFFFF);
				}
				else /** (sExternalBuffer[channelId].TxBuffer != NULL_PTR) */
				{
					if(channelConfig->DataFrame == EN_16_BIT_DATA_FRAME)
					{
						//REG_WRITE32(&pReg->DR.R,(uint16_t)*((uint16_t*)(sExternalBuffer[channelId].TxBuffer)-loopItr1));
						u16_data = ((Spi_DataBufferType)(sExternalBuffer[channelId].TxBuffer[loopItr1]))<<8U;
						loopItr1++;
						if(loopItr1<(sExternalBuffer[channelId].length))
						{
							u16_data |= ((Spi_DataBufferType)(sExternalBuffer[channelId].TxBuffer[loopItr1]));
						}
						REG_WRITE32(&pReg->DR.R,(uint16_t)(u16_data));
					}
					else
					{
						REG_WRITE32(&pReg->DR.R,(Spi_DataBufferType)(sExternalBuffer[channelId].TxBuffer[loopItr1]));
					}
				}

				u16_statusFlag = (REG_READ32(&pReg->SR.R));
				u16_data = REG_READ32(&pReg->DR.R);

				/** Get Rx buffer status and copy data into buffer  */
				if(((u16_statusFlag) & MASK_BIT(0U)) == SET)
				{
					//u16_data = REG_READ32(&pReg->DR.R);
					sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,loopItr1-1U);
				}

				/** Give delay between the channels */
				for(loopItr2 = 0 ; loopItr2 < (JobConfig->TimebetweenChannel); loopItr2++);

				/** Read the last data (for some reason this works dont ask why) */
				if(loopItr1 == ((sExternalBuffer[channelId].length)-1U))
				{
					u16_data = REG_READ32(&pReg->DR.R);
					sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,loopItr1);
				}
			}
		}

		/** Next Channel */
		loopItr0++;
	}

	/** Check if Tx buffer is empty and shut down*/
	while( (sSpi_GetTxBufferStatus(en_moduleNo)) != SPI_BUFFER_EMPTY );
	/** Check if the module is busy */
	while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

	/** Make the NSS Pin High to indicate to slave of end transfer */
	if(JobConfig->CsFunctionUsed == EN_CS_SW_HANDLING)
	{
		//REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(SET_BIT(8U)));
		Dio_WriteChannel(JobConfig->CsPinUsed,STD_HIGH);
	}

	/** Disable the Peripheral */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),CLEAR_BIT(6U));

	sSpi_SetJobResult(JobConfig->JobId,SPI_JOB_OK);

	/** Call the notification function */
	JobConfig->SpiJobEndNotification();
}

#if 0
#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U)))
static void sSpi_StartRxForJobs(void)
{
	Spi_JobType jobId = 0U;

	for(jobId = 0U; jobId < NO_OF_JOBS_CONFIGURED; jobId++)
	{
		sSpi_CopyDataToChannelBuffer(jobId);
	}
}
#endif  /** ((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (1U))) */
#endif

static void sSpi_CopyRxDataToChannelBuffer(Spi_ChannelConfigType* channelConfig,uint16_t u16_Data,uint8_t bufferNo)
{
	Spi_ChannelType channelId = channelConfig->ChannelId;
	Spi_DataBufferType * destBufferPtr = 0U;

	if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
	{
		destBufferPtr = (Spi_DataBufferType *)(&sInternalBuffer[sIB_Config[channelId].RxBuffer.Start+bufferNo]);
		sIB_Config[channelId].RxBuffer.InUse = TRUE;
	}
	else /** channelConfig->BufferUsed == EN_EXTERNAL_BUFFER */
	{
		destBufferPtr = sExternalBuffer[channelId].RxBuffer+bufferNo;
		sExternalBuffer[channelId].active = TRUE;
	}

	/** Copy data to Buffer */
	if(destBufferPtr != NULL_PTR)
	{
		if(channelConfig->DataFrame == EN_8_BIT_DATA_FRAME)
		{
			*destBufferPtr = (u16_Data & 0x00FF);
		}
		else /** (channelConfig->DataFrame == EN_16_BIT_DATA_FRAME) */
		{
			*destBufferPtr = ((u16_Data & 0xFF00)>>8U);
			destBufferPtr++;
			*destBufferPtr = (u16_Data & 0x00FF);
		}
	}
}

#if 0
static void sSpi_CopyDataToChannelBuffer(Spi_JobType jobId)
{
	Spi_HwType en_moduleNo = 0U;
	Spi_JobConfigType* jobConfig = 0U;
	Spi_ChannelConfigType* channelConfig = 0U;
	Spi_ChannelType channelId = 0U;
	uint8_t channelItr = 0U;
	Spi_DataBufferType * destBufferPtr = 0U;
	uint8_t loopItr0 = 0U;
	uint16_t noOfBuffers = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	jobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[jobId]);
	en_moduleNo = sSpi_getModuleNo(jobConfig);

	while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

//	/** Skip iteration if no messages in Rx buffer */
//	if(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_EMPTY)
//	{
//		return;
//	}

	/** Copy data from Hw Buffer to Channel Buffer  */
	while(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_NOT_EMPTY)
	{
		channelId = jobConfig->ChannelAssignment[channelItr];

		/** If channel exists copy it to buffer */
		if(channelId != EOL)
		{
			channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[channelId]);

			/** Get Buffer Type */
			if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
			{
				destBufferPtr = &sInternalBuffer[sIB_Config[channelId].RxBuffer.Start];
				noOfBuffers = channelConfig->NoOfBuffersUsed;
				sIB_Config[channelId].RxBuffer.InUse = TRUE;
			}
			else /** channelConfig->BufferUsed == EN_EXTERNAL_BUFFER */
			{
				destBufferPtr = sExternalBuffer[channelId].RxBuffer;
				noOfBuffers = sExternalBuffer[channelId].length;
				sExternalBuffer[channelId].active = TRUE;
			}

			/** Copy data to Buffer */
			if(destBufferPtr != NULL_PTR)
			{
				for(loopItr0 = 0U; (loopItr0 != noOfBuffers); loopItr0++)
				{
					if(sSpi_GetRxBufferStatus(en_moduleNo) == SPI_BUFFER_NOT_EMPTY)
					{
						(*destBufferPtr) = REG_READ32(&pReg->DR.R);
					}
					else
					{
						(*destBufferPtr) = 0U;
					}
					destBufferPtr++;
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
#endif

#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (0U)))
static void sSpi_StartSyncTransmit(Spi_SequenceType Sequence)
{
	Spi_SequenceConfigType* sequenceConfig = (Spi_SequenceConfigType*)(&GlobalConfigPtr->Sequence[Sequence]);
	Spi_JobType* jobPtr = (Spi_JobType*)(&sequenceConfig->Jobs[0U]);
	Spi_JobConfigType* jobConfig = 0U;
	Spi_StatusType en_moduleNo = 0U;

	while((*jobPtr) != EOL)
	{
		jobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[*jobPtr]);
		en_moduleNo = sSpi_getModuleNo(jobConfig);

		/** Wait for the Hw to get free */
		while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

		/** Start the job */
		sSpi_StartJob(jobConfig);

		/** TODO - Check if Rx needs to be done here itself since this is Sync Transmit */
		//sSpi_CopyDataToChannelBuffer(jobConfig->JobId);

		jobPtr++;
	}

	/** Now that all the jobs have been executed call the notification function */
	sSpi_SetSeqResult(sequenceConfig->SequenceId,SPI_SEQ_OK);
	/** Call Notification function */
	sequenceConfig->SpiSequenceEndNotification();
}
#endif /*#if((SPI_LEVEL_DELIVERED == (2U)) || (SPI_LEVEL_DELIVERED == (0U)))*/

static void sSpi_CancelSequence(Spi_SequenceType Sequence)
{
	uint8_t loopItr1 = 0U;
	uint8_t loopItr2 = 0U;
	uint8_t sequenceIndex = 0U;
	Spi_JobConfigType* jobConfig = 0U;

	/** Get the Index values of the sequence */
	for(loopItr1 = 0U; loopItr1 < NO_OF_SEQUENCES_CONFIGURED; loopItr1++ )
	{
		if(GlobalParams.BufferIndex[loopItr1].sequenceId == Sequence)
		{
			sequenceIndex = loopItr1;
			break;
		}
	}

	/** Mark the sequence complete so that the execution does not happen */
	GlobalParams.BufferIndex[sequenceIndex].sequenceId = SEQUENCE_COMPLETED;

	/** Update Job and Sequence results */
	for(loopItr2 = GlobalParams.BufferIndex[sequenceIndex].startBufferIndex;
				(loopItr2 != GlobalParams.BufferIndex[sequenceIndex].endBufferIndex); loopItr2++ )
	{
		if(loopItr2 == QUEUE_SIZE)
		{
			loopItr2 = 0U;
		}

		if(loopItr2 == (GlobalParams.BufferIndex[sequenceIndex].endBufferIndex))
		{
			break;
		}

		jobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[(GlobalParams.SpiQueuedJobsBuffer[loopItr2])]);
		sSpi_SetJobResult(jobConfig->JobId,SPI_JOB_OK);
	}
	sSpi_SetSeqResult(Sequence,SPI_SEQ_CANCELED);

	/** Call Notification function */
	GlobalConfigPtr->Sequence[Sequence].SpiSequenceEndNotification();


	/** If the cancelled sequence is the next pending sequence then update the Next Sequence */
	if(GlobalParams.NextSequence == Sequence)
	{
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
				sSpi_SetSpiStatus(SPI_IDLE);
				break;
			}
		}

		if(sequenceIndex != loopItr1)
		{
			GlobalParams.NextSequence = GlobalParams.BufferIndex[sequenceIndex].sequenceId;
		}

	}
}

#if(SPI_LEVEL_DELIVERED == (2U))
static void sSpi_ConfigInterrupt(Spi_JobType jobitr, uint8_t Enable)
{
	Spi_JobConfigType* jobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[jobitr]);
	Spi_HwType en_moduleNo = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	en_moduleNo = sSpi_getModuleNo(jobConfig);

	while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

	/** Configure Tx Interrupt */
	REG_RMW32(&pReg->CR2.R,MASK_BITS(0x1U,7U),(/*SET_BIT(6U)|*/Enable<<7U));

	return;
}
#endif  /** (SPI_LEVEL_DELIVERED == (2U)*/

#if(SPI_LEVEL_DELIVERED == (2U))
static Spi_AsyncModeType sSpi_GetAsyncMode (void)
{
	return (Spi_AsyncModeType)(GlobalParams.Spi_AsyncMode);
}

static void sSpi_ConfigAsyncMode (Spi_JobType JobId)
{
	Spi_JobConfigType* JobConfig = 0U;
	Spi_ChannelConfigType* channelConfig = 0U;

//	/** Get the Index values of the sequence */
//	for(loopItr1 = 0U; loopItr1 < NO_OF_SEQUENCES_CONFIGURED; loopItr1++ )
//	{
//		if(GlobalParams.BufferIndex[loopItr1].sequenceId == Sequence)
//		{
//			sequenceIndex = loopItr1;
//			break;
//		}
//	}
//
//	JobId = GlobalParams.BufferIndex[sequenceIndex].startBufferIndex;

	JobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[JobId]);

	if(JobConfig->ChannelAssignment[0U] == EOL)
	{
		return; /** Report Error */
	}

	GlobalParams.currentHw[JobConfig->HwUsed].jobId = JobId;
	GlobalParams.currentHw[JobConfig->HwUsed].currentChannel = JobConfig->ChannelAssignment[0U];
	channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[JobConfig->ChannelAssignment[0U]]);

	if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
	{
		GlobalParams.currentHw[JobConfig->HwUsed].dataRemaining = channelConfig->NoOfBuffersUsed;
	}
	else /** (channelConfig->BufferUsed == EN_EXTERNAL_BUFFER) */
	{
		GlobalParams.currentHw[JobConfig->HwUsed].dataRemaining = sExternalBuffer[channelConfig->ChannelId].length;
	}

}

static void sSpi_StartHw(Spi_JobType JobId)
{
	Spi_JobConfigType* JobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[JobId]);
		Spi_ChannelConfigType* channelConfig = 0U;
		Spi_HwType en_moduleNo = 0U;
		uint8_t channelItr = 0U;

		volatile  SPI_RegTypes * pReg = 0U;

		/** Get SPI Hw ID */
		en_moduleNo = sSpi_getModuleNo(JobConfig);
		pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

		if(JobConfig->ChannelAssignment[channelItr] == EOL)
		{
			return;
			/** TODO - Report ERROR */
		}

		channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[JobConfig->ChannelAssignment[channelItr]]);

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
			//REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(CLEAR_BIT(8U)));
			Dio_WriteChannel(JobConfig->CsPinUsed,STD_LOW);
		}

		/** Select the Frame Format */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,11U),(channelConfig->DataFrame)<<11U);

		/** Enable the Peripheral */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),SET_BIT(6U));
}

#if 0
static void sSpi_ScheduleJob(Spi_JobType JobId)
{
	Spi_JobConfigType* JobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[JobId]);
	Spi_ChannelConfigType* channelConfig = 0U;
	Spi_HwType en_moduleNo = 0U;
	uint8_t channelItr = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	/** Get SPI Hw ID */
	en_moduleNo = sSpi_getModuleNo(JobConfig);
	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

	if(JobConfig->ChannelAssignment[channelItr] == EOL)
	{
		return;
		/** TODO - Report ERROR */
	}

	channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[JobConfig->ChannelAssignment[channelItr]]);

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
		//REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(CLEAR_BIT(8U)));
		Dio_WriteChannel(JobConfig->CsPinUsed,STD_LOW);
	}

	/** Select the Frame Format */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,11U),(channelConfig->DataFrame)<<11U);

	/** Enable the Peripheral */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),SET_BIT(6U));

	sSpi_LoadFifoBuffer(en_moduleNo);

	if(GlobalParams.currentHw[en_moduleNo].dataRemaining == 0U)
	{
		channelItr++;
		GlobalParams.currentHw[en_moduleNo].currentChannel = JobConfig->ChannelAssignment[channelItr];
	}

	if(GlobalParams.currentHw[en_moduleNo].currentChannel == EOL)
	{
		sSpi_EndJob(JobId);
	}
}
#endif

static void sSpi_LoadFifoBuffer(Spi_HwType en_moduleNo)
{
	Spi_ChannelConfigType* channelConfig = 0U;
	Spi_ChannelType channelItr = 0U;
	Spi_ChannelType channelId = 0U;
	Spi_NumberOfDataType noOfBytesRemaining = 0U;
	volatile uint16_t u16_statusFlag = 0U;
	uint16_t u16_data = 0U;
	Spi_NumberOfDataType bufferItr = 0U;
	uint32_t loopItr1 = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	/** Get SPI Hw ID */
	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

	channelItr = GlobalParams.currentHw[en_moduleNo].currentChannel;
	noOfBytesRemaining = GlobalParams.currentHw[en_moduleNo].dataRemaining;

	/** Write Channel Data to Buffer */
	if((channelItr != EOL) && (noOfBytesRemaining > 0U))
	{
		channelConfig = (Spi_ChannelConfigType*)(&GlobalConfigPtr->Channel[channelItr]);
		channelId = channelConfig->ChannelId;

		/** Check if Tx buffer is empty and write data*/
		while( (sSpi_GetTxBufferStatus(en_moduleNo)) != SPI_BUFFER_EMPTY );

		if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
		{
			bufferItr = (channelConfig->NoOfBuffersUsed) - noOfBytesRemaining;

			if(sIB_Config[channelId].TxBuffer.InUse == FALSE)
			{
				REG_WRITE32(&pReg->DR.R,(channelConfig->DefaultTransmitValue)&0xFFFF);
			}
			else /** (sIB_Config[channelId].BufferInUse == TRUE) */
			{
				if(channelConfig->DataFrame == EN_16_BIT_DATA_FRAME)
				{
					u16_data = ((Spi_DataBufferType)(sInternalBuffer[sIB_Config[channelId].TxBuffer.Start+bufferItr]))<<8U;
					bufferItr++;
					if(bufferItr<(channelConfig->NoOfBuffersUsed))
					{
						u16_data |= ((Spi_DataBufferType)(sInternalBuffer[sIB_Config[channelId].TxBuffer.Start+bufferItr]));
					}
					REG_WRITE32(&pReg->DR.R,(uint16_t)(u16_data));
				}
				else
				{
					REG_WRITE32(&pReg->DR.R,(sInternalBuffer[sIB_Config[channelId].TxBuffer.Start+bufferItr]));
				}
			}
		}

		else if(channelConfig->BufferUsed == EN_EXTERNAL_BUFFER)
		{
			bufferItr = (sExternalBuffer[channelId].length) - noOfBytesRemaining;

			if(sExternalBuffer[channelId].TxBuffer == NULL_PTR)
			{
				REG_WRITE32(&pReg->DR.R,(channelConfig->DefaultTransmitValue)&0xFFFF);
			}
			else /** (sExternalBuffer[channelId].TxBuffer != NULL_PTR) */
			{
				if(channelConfig->DataFrame == EN_16_BIT_DATA_FRAME)
				{
					//REG_WRITE32(&pReg->DR.R,(uint16_t)*((uint16_t*)(sExternalBuffer[channelId].TxBuffer)-loopItr1));
					u16_data = ((Spi_DataBufferType)(sExternalBuffer[channelId].TxBuffer[bufferItr]))<<8U;
					bufferItr++;
					if(bufferItr<(sExternalBuffer[channelId].length))
					{
						u16_data |= ((Spi_DataBufferType)(sExternalBuffer[channelId].TxBuffer[bufferItr]));
					}
					REG_WRITE32(&pReg->DR.R,(uint16_t)(u16_data));
				}
				else
				{
					REG_WRITE32(&pReg->DR.R,(Spi_DataBufferType)(sExternalBuffer[channelId].TxBuffer[bufferItr]));
				}
			}
		}

		u16_statusFlag = (REG_READ32(&pReg->SR.R));
		u16_data = REG_READ32(&pReg->DR.R);

		/** Get Rx buffer status and copy data into buffer  */
		if(((u16_statusFlag) & MASK_BIT(0U)) == SET)
		{
			//u16_data = REG_READ32(&pReg->DR.R);
			sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,bufferItr-1U);
		}

		/** Give delay between the channels */
		for(loopItr1 = 0 ; loopItr1 < (uint32_t)(GlobalConfigPtr->Job[GlobalParams.currentHw[en_moduleNo].jobId].TimebetweenChannel); loopItr1++);

		if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
		{
			/** Read the last data (for some reason this works dont ask why) */
			if(bufferItr == ((channelConfig->NoOfBuffersUsed)-1U))
			{
				u16_data = REG_READ32(&pReg->DR.R);
				sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,bufferItr);
			}
		}
		else if(channelConfig->BufferUsed == EN_EXTERNAL_BUFFER)
		{
			/** Read the last data (for some reason this works dont ask why) */
			if(bufferItr == ((sExternalBuffer[channelId].length)-1U))
			{
				u16_data = REG_READ32(&pReg->DR.R);
				sSpi_CopyRxDataToChannelBuffer((Spi_ChannelConfigType*)channelConfig,u16_data,bufferItr);
			}
		}

		bufferItr++;
		noOfBytesRemaining = noOfBytesRemaining - 1U;

		GlobalParams.currentHw[en_moduleNo].dataRemaining = noOfBytesRemaining;
	}
}

static void sSpi_EndJob(Spi_JobType JobId)
{
	Spi_JobConfigType* JobConfig = (Spi_JobConfigType*)(&GlobalConfigPtr->Job[JobId]);
	Spi_HwType en_moduleNo = 0U;
	uint8_t loopItr1 = 0U;
	uint8_t sequenceIndex = 0U;

	volatile  SPI_RegTypes * pReg = 0U;

	/** Get SPI Hw ID */
	en_moduleNo = sSpi_getModuleNo(JobConfig);
	pReg = (SPI_RegTypes *)Spi_BaseAddress[en_moduleNo];

	/** Check if Tx buffer is empty and shut down*/
	while( (sSpi_GetTxBufferStatus(en_moduleNo)) != SPI_BUFFER_EMPTY );
	/** Check if the module is busy */
	while(sSpi_GetHwStatus(en_moduleNo) == SPI_BUSY);

	/** Make the NSS Pin High to indicate to slave of end transfer */
	if(JobConfig->CsFunctionUsed == EN_CS_SW_HANDLING)
	{
		//REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(SET_BIT(8U)));
		Dio_WriteChannel(JobConfig->CsPinUsed,STD_HIGH);
	}

	/** Disable the Peripheral */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,6U),CLEAR_BIT(6U));

	sSpi_SetJobResult(JobConfig->JobId,SPI_JOB_OK);

	/** Call the notification function */
	JobConfig->SpiJobEndNotification();

	/** Get the Index values of the job buffer for the next pending sequence */
	for(loopItr1 = 0U; loopItr1 < NO_OF_SEQUENCES_CONFIGURED; loopItr1++ )
	{
		if(GlobalParams.BufferIndex[loopItr1].sequenceId == GlobalParams.NextSequence)
		{
			sequenceIndex = loopItr1;
			break;
		}
	}

	GlobalParams.BufferIndex[sequenceIndex].startBufferIndex++;
	if(GlobalParams.BufferIndex[sequenceIndex].startBufferIndex == (QUEUE_SIZE))
	{
		GlobalParams.BufferIndex[sequenceIndex].startBufferIndex = 0U;
	}

//	/** Schedule next Job */
//	if(GlobalParams.BufferIndex[GlobalParams.NextSequence].startBufferIndex != GlobalParams.BufferIndex[GlobalParams.NextSequence].endBufferIndex)
//	{
//		JobId = GlobalParams.BufferIndex[GlobalParams.NextSequence].startBufferIndex;
//		/** Configure Global Parameters for next job */
//		sSpi_ConfigAsyncMode(JobId);
//		/** Config Hw for Interrupt  */
//		sSpi_ConfigInterrupt(JobId,STD_ON);
//		sSpi_SetJobResult(JobId,SPI_JOB_PENDING);
//		sSpi_ScheduleJob(JobId);
//	}
//	else
	if(GlobalParams.BufferIndex[sequenceIndex].startBufferIndex == (GlobalParams.BufferIndex[sequenceIndex].endBufferIndex))
	{
		sSpi_ScheduleNextSequence();
	}
	else
	{
		/** Schedule next Job */
		JobId = GlobalParams.SpiQueuedJobsBuffer[GlobalParams.BufferIndex[GlobalParams.NextSequence].startBufferIndex];
		/** Configure Global Parameters for next job */
		sSpi_ConfigAsyncMode(JobId);
		/** Config Hw for Interrupt  */
		sSpi_ConfigInterrupt(JobId,STD_ON);
		sSpi_SetJobResult(JobId,SPI_JOB_PENDING);
		sSpi_StartHw(JobId);
	}
}

static void sSpi_ScheduleNextSequence(void)
{
	Spi_SequenceType SequenceId = GlobalParams.NextSequence;
	uint8_t sequenceIndex = 0U;
	uint8_t loopItr1 = 0U;

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

	/** Now that all the jobs in the sequence have been executed  */
	/** Update sequence status */
	GlobalParams.BufferIndex[sequenceIndex].sequenceId = SEQUENCE_COMPLETED;
	sSpi_SetSeqResult(SequenceId,SPI_SEQ_OK);
	/** Call Notification function */
	GlobalConfigPtr->Sequence[SequenceId].SpiSequenceEndNotification();

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
			sSpi_SetSpiStatus(SPI_IDLE);
			break;
		}
	}

	if(sequenceIndex != loopItr1)
	{
		/** Store the sequence id  */
		GlobalParams.NextSequence = GlobalParams.BufferIndex[sequenceIndex].sequenceId;
	}
}
#endif /** (SPI_LEVEL_DELIVERED == (2U))*/

static void sSpi_SetSpiStatus (Spi_StatusType Status)
{
	GlobalParams.SpiStatus = Status;
}

static void sSpi_SetSeqResult (Spi_SequenceType Sequence,Spi_SeqResultType Result)
{
	Spi_SeqResult[Sequence] = Result;
}

static void sSpi_SetJobResult(Spi_JobType Job,Spi_JobResultType Result)
{
	Spi_JobResult[Job] = Result;
}

#if(SPI_LEVEL_DELIVERED == (2U))
void sISR_Spi_Global(Spi_HwType en_moduleNo)
{
	Spi_JobType JobId = GlobalParams.currentHw[en_moduleNo].jobId;
	Spi_ChannelType* channelItr = (Spi_ChannelType*)(&GlobalConfigPtr->Job[JobId].ChannelAssignment[0U]);

	if(GlobalParams.currentHw[en_moduleNo].dataRemaining == 0U)
	{
		while((*channelItr) != GlobalParams.currentHw[en_moduleNo].currentChannel)
		{
			channelItr++;
		}
		channelItr++;

		if((*channelItr) == EOL)
		{
			GlobalParams.currentHw[en_moduleNo].currentChannel = EOL;
		}
		else
		{
			GlobalParams.currentHw[en_moduleNo].currentChannel = (*channelItr);

			if((GlobalConfigPtr->Channel[(*channelItr)].BufferUsed) == EN_INTERNAL_BUFFER)
			{
				GlobalParams.currentHw[en_moduleNo].dataRemaining = GlobalConfigPtr->Channel[(*channelItr)].NoOfBuffersUsed;
			}
			else /** (channelConfig->BufferUsed == EN_EXTERNAL_BUFFER) */
			{
				GlobalParams.currentHw[en_moduleNo].dataRemaining = sExternalBuffer[GlobalConfigPtr->Channel[(*channelItr)].ChannelId].length;
			}
		}
	}

	sSpi_LoadFifoBuffer(en_moduleNo);

	if(GlobalParams.currentHw[en_moduleNo].currentChannel == EOL)
	{
		sSpi_ConfigInterrupt(JobId,STD_OFF);
		sSpi_EndJob(JobId);
	}
}
#endif /*(SPI_LEVEL_DELIVERED == (2U))*/
