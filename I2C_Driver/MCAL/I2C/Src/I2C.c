/**********************************************
 * File - I2C.c
 * Author - Michael Jihan
 * Created on 06-Feb-2021
 *
 * Purpose - This file holds the definition of
 * 			 functions that control I2C functionality
 *
 * ********************************************/

/** Points to Remember
 * 1. The interface automatically switches from slave --> master
 *    after it generates a START condition and from master --> slave, if an arbitration loss
 * 	  or a Stop generation occurs.
 * 2. Master initiates the data transfer and generates the Clock Signal.
 * 3. Master mode -  SW has to generate START and STOP condition.
 * 4. Slave Mode - HW can detect its own address.
 * 5. Address is always transmitted in Master mode.
 * 	  1 byte if 7-bit addressing or 2 bytes if 10-bit addressing.
 * 6. DEFAULT MODE - SLAVE (Generate a START condition to switch to master)
 * 7. If START condition is detected then
 * 		a) Address is received via SDA line
 * 		b) Address is compared to Own address in OAR1
 * 		c) Address is compared to Own address in OAR2 if ENDUAL = 1
 * 		d) Address is compared to General call address if ENGC = 1
 * 		e) if header or address don't match wait for next START condition
 * 		f) In 10-bit mode if header matches then ACK and wait for 8-bit slave address.
 * 		g) If address matches then  ACK, ADDR bit is set interrupt generated,
 * 8. a) In 10-bit addressing 1st byte(Header) is 11110xx0 where 'xx' is 2 MSB bits of 2nd Byte
 * 	  b) After receiving address the SLAVE -> Receiver MODE
 * 	  c) SLAVE -> Transmitter MODE if
 * 	  		a) Repeated START condition occurs with header (11110xx1) where xx matches
 * 		    b) Slave clears ADDR bit and sends data from DR register
 * 		    c) SCL is stretched low until b) is finished
 * 	  d) SLAVE -> Receiver MODE
 * 	  		a) data received -> ACK -> Interrupt
 * 	  		b) if DR is not read and next byte is received then BTF is set and
 * 	  			SCL is stretched low until DR is read
 * 	  e) SLAVE -> Closing communication
 * 	  		a) Master generates STOP condition which sets the STOPF bit
 * 	  		b) STOPF bit is cleared by read to SR1 followed by CR1
 * 9. MASTER MODE -
 * 		a) Master mode is selected as soon as the START bit is set
 * 		b) START Bit --> Master Mode --> START bit --> Restart condition
 * 		c)
 * */


/**
 * 1. Create function to check input clk frequency it needs to be at least
 *    2MHz for SM Mode
 *    4MHZ for FM Mode
 * 2. ACK needs to be enabled.
 * 3. Create a function so that  If START condition is detected then
 * 		a) Address is received via SDA line
 * 		b) Address is compared to Own address in OAR1
 * 		c) Address is compared to Own address in OAR2 if ENDUAL = 1
 * 		d) Address is compared to General call address if ENGC = 1
 * 		e) If Header or address dont match then return
 * 		f) ADDR bit is set once address matches OAR1/OAR2 and INTERRUPT is generated
 * 		g) check DUALF to see which address OAR1/OAR2 has been ACK
 * 	4. Poll for TRA bit to see if the slave is in Transmitter or Reciever mode
 * 	5. While Tx Check for BTF before writing data to DR
 * 	6. Clear flags by completing their clear sequence
 * 		eg- if (ADDR == 1)  {READ SR1; READ SR2}
 * 			if (STOPF == 1) {READ SR1; WRITE CR1}
 * 	7. To configure I2C for MASTER mode
 * 		a) program peripheral input clock in I2C_CR2
 * 		b) configure the clock control register.
 * 		c) configure the rise time register
 * 		d) enable PE in CR1
 * 		e) Set the START bit in CR1
 *
 *
 * */

#include <stdint.h>
#include <Reg_Macros.h>

#include "RCC.h"
#include "Dio.h"

#include "I2C.h"

#include "I2C_PbCfg.h"


#define WRITE_MODE   (0x0U)
#define READ_MODE    (0x1U)


static I2C_GlobalParams GlobalParams;
static const I2C_ConfigType* GlobalConfigPtr;

I2C_JobResultType I2C_JobResult[NO_OF_JOBS_CONFIGURED];
I2C_SeqResultType I2C_SeqResult[NO_OF_SEQUENCES_CONFIGURED];

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
static I2C_DataBufferType sInternalBuffer[IB_BUFFERS_AVAILABLE]; /** Reserve IB_BUFFERS_AVAILABLE Bytes for Internal Buffers */
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

/********************** Local Function Prototypes *********************/
static void sI2C_SetStatus (I2C_StatusType Status);
static void sI2C_SetSeqResult (I2C_SequenceType Sequence,I2C_SeqResultType Result);
static void sI2C_SetJobResult(I2C_JobType Job,I2C_JobResultType Result);
static I2C_HwType sI2C_getModuleNo (const I2C_JobConfigType* JobConfig);
static uint8_t sI2C_getAPB1Freq (void);
static Bool sI2C_GetSR1Status(I2C_HwType en_moduleNo,I2C_Status1Type flag);
static I2C_StatusType sI2C_GetHwStatus(I2C_HwType en_moduleNo);
static void sI2C_Clk_Enable(I2C_HwType moduleNo);
static Std_ReturnType sI2C_InitHwModule(I2C_HwConfigType* HwConfig);
static Std_ReturnType sI2C_MasterTxData(I2C_HwType ModuleNo,I2C_DataBufferType* DataBufferPtr,
		I2C_NumberOfDataType noOfBytesRemaining,uint16_t SlaveAddress,Bool RepeatedStart);
static Std_ReturnType sI2C_MasterRxData(I2C_HwType ModuleNo,I2C_DataBufferType* DataBufferPtr,
		I2C_NumberOfDataType noOfBytesRemaining,uint16_t SlaveAddress,Bool RepeatedStart);

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
static Std_ReturnType sI2C_AllocateIbMemory(void);
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

#if((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U)))
static void sI2C_StartSyncTransmit(I2C_SequenceType Sequence);
static void sI2C_StartJob(I2C_JobConfigType* JobConfig);
#endif /** ((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U))) */

/********************** Global Functions ******************************/

void I2C_Init (const I2C_ConfigType* ConfigPtr)
{
	if(ConfigPtr == NULL_PTR)
	{
		sI2C_SetStatus (I2C_UNINIT);
		/** TODO - Report DET error here */
		return;
	}

	GlobalConfigPtr = ConfigPtr;

	uint8_t loopItr0 = 0U;
	I2C_JobConfigType* JobConfig = 0U;

	/*volatile  I2C_RegTypes * pReg = 0U;*/

	/** Initialise internal Buffers for channels using IB */
#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
	if((sI2C_AllocateIbMemory()) == E_NOT_OK)
	{
		/** NO MEMORY FOUND */
		/** TODO - Report DET Error */
	}
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

	for(loopItr0 = 0U; loopItr0 < NO_OF_JOBS_CONFIGURED; loopItr0++ )
	{
		JobConfig = (I2C_JobConfigType*)(&ConfigPtr->Job[loopItr0]);

		/** Initialise the Hw module assigned to the job */
		sI2C_InitHwModule(((I2C_HwConfigType*)JobConfig->HwConfig));

		/** Enable Polling Mode for the HW unit */
#if(SPI_LEVEL_DELIVERED == (2U))
		REG_RMW32(&pReg->CR2.R,MASK_BITS(0x7U,5U),(0U)<<5U);
		Spi_SetAsyncMode (SPI_POLLING_MODE);
		GlobalParams.currentHw[moduleNo].currentChannel = EOL;
		GlobalParams.currentHw[moduleNo].dataRemaining = 0U;
#endif
	}

	/** Initialise Job Queue with EOL values */
#if 0
	for(loopItr0 = 0U; loopItr0 < QUEUE_SIZE; loopItr0++ )
	{
		GlobalParams.SpiQueuedJobsBuffer[loopItr0] = EOL;/** TODO - FIx this */
	}
#endif

	/** Initialise the Result for Sequence and jobs  */
	for(loopItr0 = 0U; loopItr0 < NO_OF_JOBS_CONFIGURED; loopItr0++ )
	{
		sI2C_SetJobResult(loopItr0,I2C_JOB_OK);
	}
	for(loopItr0 = 0U; loopItr0 < NO_OF_SEQUENCES_CONFIGURED; loopItr0++ )
	{
		sI2C_SetSeqResult(loopItr0,I2C_SEQ_OK);
		/** Also initialise the pending sequence buffer */
		//GlobalParams.BufferIndex[loopItr0].sequenceId = EOL; /** TODO - FIx this */
	}

	/** Initialise Sequence queue */
	//GlobalParams.NextSequence = EOL;/** TODO - FIx this */

	sI2C_SetStatus(I2C_IDLE);
}

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
Std_ReturnType I2C_WriteIB (I2C_ChannelType Channel,const I2C_DataBufferType* DataBufferPtr)
{
	/** Check for module Init */
	if((I2C_GetStatus()) == I2C_UNINIT)
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
	I2C_ChannelConfigType channelConfig = GlobalConfigPtr->Channel[Channel];
	InternalBufferType* IB_Parameters = GlobalParams.sIB_Config;

	/** Write Data to the IB */
	for(loopItr0 = 0U; loopItr0 < (channelConfig.NoOfBuffersUsed); loopItr0++)
	{
		bufferItr = (uint8_t)(IB_Parameters[Channel].TxBuffer.Start+loopItr0);

		/** If Data Buffer is Null then default transmit data is used */
		if(DataBufferPtr == NULL_PTR)
		{
			sInternalBuffer[bufferItr] = channelConfig.DefaultTransmitValue;
		}
		else
		{
			/** Write the Data to Buffer */
			sInternalBuffer[bufferItr] = (I2C_DataBufferType)(*DataBufferPtr);

			/** Get Next Data */
			DataBufferPtr++;
		}
	}
	GlobalParams.sIB_Config[Channel].TxBuffer.InUse = TRUE;

	return E_OK;

}

Std_ReturnType I2C_ReadIB (I2C_ChannelType Channel,I2C_DataBufferType* DataBufferPointer)
{
	/** Check for module Init */
	if((I2C_GetStatus()) == I2C_UNINIT)
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
	I2C_ChannelConfigType channelConfig = GlobalConfigPtr->Channel[Channel];

	/** Read Data from IB and copy it to the DataBufferPtr */
	/** If no new data is received then copy old data */
	for(loopItr0 = 0U; loopItr0 < (channelConfig.NoOfBuffersUsed); loopItr0++)
	{
		bufferIndex = GlobalParams.sIB_Config[Channel].RxBuffer.Start+loopItr0;

		/** Write the Data from IB to DataBufferPointer */
		(*DataBufferPointer) = sInternalBuffer[bufferIndex];
		DataBufferPointer++;
	}

	GlobalParams.sIB_Config[Channel].RxBuffer.InUse = FALSE;
	return E_OK;
}

#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
Std_ReturnType I2C_SetupEB (I2C_ChannelType Channel,const I2C_DataBufferType* SrcDataBufferPtr,
							I2C_DataBufferType* DesDataBufferPtr,I2C_NumberOfDataType Length)
{
	/** Check for module Init */
	if((I2C_GetStatus()) == I2C_UNINIT)
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
		GlobalParams.sExternalBuffer[Channel].TxBuffer = NULL_PTR;
	}
	else /** (SrcDataBufferPtr != NULL_PTR) */
	{
		GlobalParams.sExternalBuffer[Channel].TxBuffer = (I2C_DataBufferType*)(SrcDataBufferPtr);
	}

	if(DesDataBufferPtr == NULL_PTR)
	{
		GlobalParams.sExternalBuffer[Channel].RxBuffer = NULL_PTR;
	}
	else /** (SrcDataBufferPtr != NULL_PTR) */
	{
		GlobalParams.sExternalBuffer[Channel].RxBuffer = DesDataBufferPtr;
	}

	/** Configure the Length of the buffer*/
	GlobalParams.sExternalBuffer[Channel].length = Length;

	return E_OK;
}
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

#if((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U)))
Std_ReturnType I2C_SyncTransmit (I2C_SequenceType Sequence)
{
	/** Check for module Init */
	if((I2C_GetStatus()) == I2C_UNINIT)
	{
		return E_NOT_OK;
	}

	/** Parameter Checking */
	if(Sequence >= NO_OF_SEQUENCES_CONFIGURED)
	{
		return E_NOT_OK;
	}

	/** if the sequence is already in pending state then return */
	if(I2C_SeqResult[Sequence] == I2C_SEQ_PENDING)
	{
		return E_NOT_OK;
	}

	I2C_SequenceConfigType SequenceConfig = GlobalConfigPtr->Sequence[Sequence];
	I2C_StatusType moduleStatus = I2C_GetStatus();

	/** Put the Sequence in pending and start transmission of the sequence */
	sI2C_SetStatus(I2C_BUSY);
	sI2C_SetSeqResult(Sequence,I2C_SEQ_PENDING);

	I2C_JobType * jobPtr = &SequenceConfig.Jobs[0U];

	while(*jobPtr != EOL)
	{
		sI2C_SetJobResult(*jobPtr,I2C_JOB_QUEUED);
		jobPtr++;
	}

	sI2C_StartSyncTransmit(Sequence);

	sI2C_SetStatus(moduleStatus);

	return E_OK;
}
#endif  /** ((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U))) */

I2C_StatusType I2C_GetStatus (void)
{
	/**
	 *  API service I2C_GetStatus shall return I2C_UNINIT when the
		I2C Handler/Driver is not initialized or not usable
	 *	API service I2C_GetStatus shall return I2C_IDLE when The
		I2C Handler/Driver is not currently transmitting any Job
	 *	API service I2C_GetStatus shall return I2C_BUSY when The
		I2C Handler/Driver is performing a I2C Job transmit
	 */
	return (I2C_StatusType)(GlobalParams.I2CStatus);
}

I2C_JobResultType I2C_GetJobResult (I2C_JobType Job)
{
	if(Job >= NO_OF_JOBS_CONFIGURED)
	{
		return (I2C_JobResultType)(I2C_JOB_FAILED);
	}

	return (I2C_JobResultType)(I2C_JobResult[Job]);
}

I2C_SeqResultType I2C_GetSequenceResult (I2C_SequenceType Sequence)
{
	if(Sequence >= NO_OF_SEQUENCES_CONFIGURED)
	{
		return (I2C_SeqResultType)(I2C_SEQ_FAILED);
	}

	return (I2C_SeqResultType)(I2C_SeqResult[Sequence]);
}

/********************** Local Functions *****************************/

static void sI2C_SetStatus (I2C_StatusType Status)
{
	GlobalParams.I2CStatus = Status;
}

static void sI2C_SetSeqResult (I2C_SequenceType Sequence,I2C_SeqResultType Result)
{
	I2C_SeqResult[Sequence] = Result;
}

static void sI2C_SetJobResult(I2C_JobType Job,I2C_JobResultType Result)
{
	I2C_JobResult[Job] = Result;
}


static I2C_HwType sI2C_getModuleNo (const I2C_JobConfigType* JobConfig)
{
	if(JobConfig == NULL_PTR)
	{
		return 0xFF;
	}

	return (I2C_HwType)(JobConfig->HwConfig->ModuleNo);
}

/** Initialise internal Buffers for channels using IB */
#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
static Std_ReturnType sI2C_AllocateIbMemory(void)
{
	uint8_t channelItr1 = 0U;
	uint8_t bufferItr1 = 0U;
	uint8_t bufferStart = 0U;
	uint8_t bufferEnd = 0U;
	I2C_ChannelConfigType* channelConfig = 0U;
	InternalBufferType* IB_Parameters = GlobalParams.sIB_Config;

	for(channelItr1 = 0U; channelItr1 < NO_OF_CHANNELS_CONFIGURED; channelItr1++)
	{
		channelConfig = (I2C_ChannelConfigType*)(&GlobalConfigPtr->Channel[channelItr1]);
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
					IB_Parameters[channelItr1].RxBuffer.Start = bufferStart;
					IB_Parameters[channelItr1].RxBuffer.End = bufferEnd;
					IB_Parameters[channelItr1].RxBuffer.InUse = FALSE;
				}
				else /** bufferItr1 == 1U */
				{
					IB_Parameters[channelItr1].TxBuffer.Start = bufferStart;
					IB_Parameters[channelItr1].TxBuffer.End = bufferEnd;
					IB_Parameters[channelItr1].TxBuffer.InUse = FALSE;
				}
				IB_Parameters[channelItr1].ChannelId = channelItr1;

				/** Update parameters for next iteration */
				bufferStart = bufferEnd +1U;
			}
		}
	}
	return E_OK;
}
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

static uint8_t sI2C_getAPB1Freq (void)
{
	uint32_t freqInMHz = 0U;
	freqInMHz = RCC_GetAPB1Clk();

	return (uint8_t)(freqInMHz/1000000);
}

static Bool sI2C_GetSR1Status(I2C_HwType en_moduleNo,I2C_Status1Type flag)
{
	volatile  I2C_RegTypes * pReg = (I2C_RegTypes *)I2C_BaseAddress[en_moduleNo];

	if((REG_READ16(&pReg->SR1.R)) & MASK_BIT(flag))
	{
		return TRUE;
	}
	return FALSE;
}

static Bool sI2C_GetSR2Status(I2C_HwType en_moduleNo,I2C_Status2Type flag)
{
	volatile  I2C_RegTypes * pReg = (I2C_RegTypes *)I2C_BaseAddress[en_moduleNo];

	if((REG_READ16(&pReg->SR2.R)) & MASK_BIT(flag))
	{
		return TRUE;
	}
	return FALSE;
}

static I2C_StatusType sI2C_GetHwStatus(I2C_HwType en_moduleNo)
{
	//volatile  I2C_RegTypes * pReg = (I2C_RegTypes *)I2C_BaseAddress[en_moduleNo];
	/* uint8_t u8_statusFlag = REG_READ32(&pReg->SR.R); */

	if((sI2C_GetSR2Status(en_moduleNo,I2C_BUS_BUSY)) == TRUE)
	{
		return I2C_BUSY;
	}
	return I2C_IDLE;
}

static void sI2C_Clk_Enable(I2C_HwType moduleNo)
{
	/** I2C-1,2,3 - APB1  */
	switch(moduleNo)
	{
	case EN_I2C_1:
		RCC_APB1PeripheralClkEnable(EN_I2C1);
		break;

	case EN_I2C_2:
		RCC_APB1PeripheralClkEnable(EN_I2C2);
		break;

	case EN_I2C_3:
		RCC_APB1PeripheralClkEnable(EN_I2C3);
		break;

	default:
		break;
	}
}

static Std_ReturnType sI2C_InitHwModule(I2C_HwConfigType* HwConfig)
{
	uint8_t apb1Freq = 0U;
	uint16_t ccrValue = 0U;

	volatile  I2C_RegTypes * pReg = 0U;
	pReg = (I2C_RegTypes *)I2C_BaseAddress[HwConfig->ModuleNo];

	/** Enable Clock for the Module */
	sI2C_Clk_Enable(HwConfig->ModuleNo);

	/** Enable ACK mode  */
	//REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,10U),SET_BIT(10U));/** TODO - Check why not working */

	/** Configure the Freq of APB1 in CR2 Register */
	apb1Freq = sI2C_getAPB1Freq();
	REG_RMW32(&pReg->CR2.R,MASK_BITS(0x3FU,0U),(apb1Freq & 0x3FU));

	/** Configure the Slave Address */
	/** Select 7bit or 10bit addressing */
	if((HwConfig->SlaveAddressMode) == EN_I2C_7_BIT_SLAVE_ADDRESS)
	{
		REG_RMW32(&pReg->OAR1.R,MASK_BITS(0x1U,15U),(CLEAR_BIT(15U)));
		REG_RMW32(&pReg->OAR1.R,MASK_BITS(0x7FU,1U),((HwConfig->OwnAddress) & 0x7F)<<1U);
	}
	else	/*((HwConfig->SlaveAddressMode) == EN_I2C_10_BIT_SLAVE_ADDRESS)*/
	{
		REG_RMW32(&pReg->OAR1.R,MASK_BITS(0x1U,15U),(SET_BIT(15U)));
		REG_RMW32(&pReg->OAR1.R,MASK_BITS(0x3FFU,0U),((HwConfig->OwnAddress) & 0x3FFU));
	}
	REG_RMW32(&pReg->OAR1.R,MASK_BITS(0x1U,14U),(SET_BIT(14U)));

	/** Configure the Clock for I2C module */
	/** Select the mode of operation */
	if(HwConfig->MasterMode == EN_I2C_STANDARD_MODE)
	{
		/** Check if APB1 freq is min 2MHZ to support standard mode */
		if(apb1Freq >= 2U )
		{
			REG_RMW32(&pReg->CCR.R,MASK_BITS(0x1U,15U),(CLEAR_BIT(15U)));
			ccrValue = (apb1Freq*1000000) / (2*HwConfig->ClkSpeed);
		}
		else
		{
			/** SET AN ERROR and RETURN */
			GlobalParams.LastError = I2C_INIT_ERROR;
			return (Std_ReturnType)E_NOT_OK;
		}
	}
	else /** (HwConfig->MasterMode == EN_I2C_FAST_MODE) */
	{
		/** Check if APB1 freq is min 4MHZ to support standard mode */
		if(apb1Freq >= 4U )
		{
			REG_RMW32(&pReg->CCR.R,MASK_BITS(0x1U,15U),(SET_BIT(15U)));
			if(HwConfig->FastModeDutyCycle == EN_I2C_FAST_MODE_DUTY_2)
			{
				REG_RMW32(&pReg->CCR.R,MASK_BITS(0x1U,14U),(CLEAR_BIT(14U)));
				ccrValue = (apb1Freq*1000000) / (3*HwConfig->ClkSpeed);
			}
			else	/** (HwConfig->FastModeDutyCycle == EN_I2C_FAST_MODE_DUTY_16_DIV_9) */
			{
				REG_RMW32(&pReg->CCR.R,MASK_BITS(0x1U,14U),(SET_BIT(14U)));
				ccrValue = (apb1Freq*1000000) / (25*HwConfig->ClkSpeed);
			}
		}
		else
		{
			/** SET AN ERROR and RETURN */
			GlobalParams.LastError = I2C_INIT_ERROR;
			return (Std_ReturnType)E_NOT_OK;
		}
	}
	/** Minimum ccr value =1U  if */
	if((ccrValue < 0x01) && ((HwConfig->MasterMode == EN_I2C_FAST_MODE)) &&
			((HwConfig->FastModeDutyCycle == EN_I2C_FAST_MODE_DUTY_16_DIV_9)))
	{
		/** SET AN ERROR and RETURN */
		GlobalParams.LastError = I2C_INIT_ERROR;
		return (Std_ReturnType)E_NOT_OK;
	}
	/** Minimum value =4U */
	else if (ccrValue < 0x04)
	{
		/** SET AN ERROR and RETURN */
		GlobalParams.LastError = I2C_INIT_ERROR;
		return (Std_ReturnType)E_NOT_OK;
	}
	else
	{
		REG_RMW32(&pReg->CCR.R,MASK_BITS(0xFFFU,0U),(ccrValue&0xFFFU));
	}

	/** Configure the Max TRISE Value */
	if(HwConfig->MasterMode == EN_I2C_STANDARD_MODE)
	{
		/** Max value is (Trise/Tpck1) +1 Trise = 1000ns = /1000000 which is already done in sI2C_getAPB1Freq() */
		REG_RMW32(&pReg->TRISE.R,MASK_BITS(0x3FU,0U),((apb1Freq + 1U)&0x3FU));
	}
	else /** (HwConfig->MasterMode == EN_I2C_FAST_MODE) */
	{
		/** Max value is (Trise/Tpck1) +1 Trise = 300ns = 300/1000000000 = 3/10 as /1000000 is already done in sI2C_getAPB1Freq() */
		REG_RMW32(&pReg->TRISE.R,MASK_BITS(0x3FU,0U),((((apb1Freq*3)/10) + 1U)&0x3FU));
	}


	return (Std_ReturnType)E_OK;

}

static Std_ReturnType sI2C_MasterTxData(I2C_HwType ModuleNo,I2C_DataBufferType* DataBufferPtr,
									I2C_NumberOfDataType noOfBytesRemaining,uint16_t SlaveAddress,Bool RepeatedStart)
{
	volatile  I2C_RegTypes * pReg = 0U;
	pReg = (I2C_RegTypes *)I2C_BaseAddress[ModuleNo];

	/** Enable the Peripheral */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,0U),(SET_BIT(0U)));

	/** Enable ACK mode  */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,10U),SET_BIT(10U));

	/** Generate a START condition  */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(SET_BIT(8U)));
	/** Check if Start bit generation is completed */
	while(sI2C_GetSR1Status(ModuleNo,I2C_START_BIT_GENERATED) != TRUE);
	/** TODO - Implement timeout */

	/** Send the Address of the Slave */
	/** Clear the R/W bit to indicate Write mode */
	SlaveAddress = (SlaveAddress << 1U) | WRITE_MODE;
	REG_WRITE32(&pReg->DR.R,(uint8_t)(SlaveAddress));
	/** Check if address Tx is completed */
	while((sI2C_GetSR1Status(ModuleNo,I2C_ADDRESS_SENT)) != TRUE);
	REG_READ16(&pReg->SR1.R);	/** Dummy read to clear ADDR in SR1 */
	REG_READ16(&pReg->SR2.R);	/** Dummy read to clear ADDR in SR1 */

	/** Tx 1 byte of data until buffer is empty */
	while(noOfBytesRemaining > 0U)
	{
		while(sI2C_GetSR1Status(ModuleNo,I2C_TX_DATA_REGISTER_EMPTY) != TRUE);
		/** TODO - Implement timeout */
		REG_WRITE32(&pReg->DR.R,(uint8_t)(*DataBufferPtr));
		DataBufferPtr++;
		noOfBytesRemaining--;
	}

	/** Wait for TXE and BTF bit to SET which indicates completion */
	while(sI2C_GetSR1Status(ModuleNo,I2C_TX_DATA_REGISTER_EMPTY) != TRUE);
	while(sI2C_GetSR1Status(ModuleNo,I2C_BYTE_TRANSFER_FINISHED) != TRUE);

	/** Generate a STOP condition or Repeated START */
	if(RepeatedStart == STD_OFF)
	{
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,9U),(SET_BIT(9U)));
		/** Check if STOP bit generation is completed NO NEED FOR THIS */
		//while(sI2C_GetSR1Status(ModuleNo,I2C_STOP_CONDITION_DETECTED) != TRUE);

		/** Disable the Peripheral */
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,0U),(CLEAR_BIT(0U)));
	}

	return (Std_ReturnType)E_OK;

}

/**
 * 1. To Receive data we first send the address with the read bit enabled.
 * 2. we wait for the byte to be recieved read it and ACK it.
 * 3. if there is only 1 byte dont ACK as we need to NACK to end communication.
 * 4. if more than 1 byte then disable ACK when length is 2.
 * */
static Std_ReturnType sI2C_MasterRxData(I2C_HwType ModuleNo,I2C_DataBufferType* DataBufferPtr,
		I2C_NumberOfDataType noOfBytesRemaining,uint16_t SlaveAddress,Bool RepeatedStart)
{
	volatile  I2C_RegTypes * pReg = 0U;
	pReg = (I2C_RegTypes *)I2C_BaseAddress[ModuleNo];

	/** Enable the Peripheral */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,0U),(SET_BIT(0U)));

	/** Enable ACK mode  */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,10U),SET_BIT(10U));

	/** Generate a START condition  */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,8U),(SET_BIT(8U)));
	/** Check if Start bit generation is completed */
	while(sI2C_GetSR1Status(ModuleNo,I2C_START_BIT_GENERATED) != TRUE);

	/** Send the Address of the Slave */
	/** Set the R/W bit to indicate Read mode */
	SlaveAddress = (SlaveAddress << 1U) | READ_MODE;
	REG_WRITE32(&pReg->DR.R,(uint8_t)(SlaveAddress));
	/** Check if address Tx is completed */
	while((sI2C_GetSR1Status(ModuleNo,I2C_ADDRESS_SENT)) != TRUE);
	REG_READ16(&pReg->SR1.R);	/** Dummy read to clear ADDR in SR1 */
	REG_READ16(&pReg->SR2.R);	/** Dummy read to clear ADDR in SR1 */

	/** Rx 1 byte of data until buffer is empty */
	while(noOfBytesRemaining > 0U)
	{
		if(noOfBytesRemaining < 2U)
		{
			/** Disable ACKing */
			REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,10U),CLEAR_BIT(10U));
		}
		while(sI2C_GetSR1Status(ModuleNo,I2C_RX_DATA_REGISTER_NOT_EMPTY) != TRUE);
		(*DataBufferPtr) = (uint8_t)(REG_READ16(&pReg->DR.R));
		DataBufferPtr++;
		noOfBytesRemaining--;
	}

	/** Wait for RXNE and BTF bit to SET which indicates completion */
	while(sI2C_GetSR1Status(ModuleNo,I2C_RX_DATA_REGISTER_NOT_EMPTY) == TRUE);
	//while(sI2C_GetSR1Status(ModuleNo,I2C_BYTE_TRANSFER_FINISHED) != TRUE); Since for last byte NACK is received --> The BTF bit is not set after a NACK reception

	/** Generate a STOP condition or Repeated START */
	if(RepeatedStart == STD_OFF)
	{
		REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,9U),(SET_BIT(9U)));
	}

	/** Enable ACK mode  */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,10U),SET_BIT(10U));

	/** Disable the Peripheral */
	REG_RMW32(&pReg->CR1.R,MASK_BITS(0x1U,0U),(CLEAR_BIT(0U)));

	return (Std_ReturnType)E_OK;
}

#if((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U)))
static void sI2C_StartSyncTransmit(I2C_SequenceType Sequence)
{
	I2C_SequenceConfigType* sequenceConfig = (I2C_SequenceConfigType*)(&GlobalConfigPtr->Sequence[Sequence]);
	I2C_JobType* jobPtr = (I2C_JobType*)(&sequenceConfig->Jobs[0U]);
	I2C_JobConfigType* jobConfig = 0U;
	I2C_StatusType en_moduleNo = 0U;

	while((*jobPtr) != EOL)
	{
		jobConfig = (I2C_JobConfigType*)(&GlobalConfigPtr->Job[*jobPtr]);
		en_moduleNo = sI2C_getModuleNo(jobConfig);

		/** Wait for the Hw to get free */
		while(sI2C_GetHwStatus(en_moduleNo) == I2C_BUSY);

		/** Start the job */
		sI2C_StartJob(jobConfig);

		jobPtr++;
	}

	/** Now that all the jobs have been executed call the notification function */
	sI2C_SetSeqResult(sequenceConfig->SequenceId,I2C_SEQ_OK);
	/** Call Notification function */
	sequenceConfig->I2CSequenceEndNotification();
}

static void sI2C_StartJob(I2C_JobConfigType* JobConfig)
{
	uint8_t jobId = JobConfig->JobId;
	uint8_t channelId = 0U;
	uint8_t loopItr0 = 0U;
	uint8_t loopItr1 = 0U;
	volatile uint32_t loopItr2 = 0U;
	I2C_HwType en_moduleNo = 0U;
	I2C_ChannelConfigType* channelConfig = 0U;

	/** Update the Job status */
	sI2C_SetJobResult(jobId,I2C_JOB_PENDING);

	/** Get I2C Hw ID */
	en_moduleNo = sI2C_getModuleNo(JobConfig);

	/** Start Channels execution */
	while(JobConfig->ChannelAssignment[loopItr0] != EOL)
	{
		channelId = JobConfig->ChannelAssignment[loopItr0];
		channelConfig = (I2C_ChannelConfigType*)(&GlobalConfigPtr->Channel[channelId]);

		/** Configure the LSBFIRST bit*/
		/** Check if the module is busy */
		while(sI2C_GetHwStatus(en_moduleNo) == I2C_BUSY);

		if(channelConfig->BufferUsed == EN_INTERNAL_BUFFER)
		{
			for(loopItr1 = 0U; loopItr1<(channelConfig->NoOfBuffersUsed); loopItr1++)
			{
				/** Check if Tx buffer is empty and write data*/
				//while( (sI2C_GetSR1Status(en_moduleNo,I2C_TX_DATA_REGISTER_EMPTY)) != TRUE );
				/** TODO - Implement timeout */

				if(GlobalParams.sIB_Config[channelId].TxBuffer.InUse == FALSE)
				{
					/** Transmit the Default Value */
					if((loopItr1+1U) != (channelConfig->NoOfBuffersUsed))
					{
						sI2C_MasterTxData(en_moduleNo,&channelConfig->DefaultTransmitValue,
																(I2C_NumberOfDataType)1U,channelConfig->SlaveAddress,TRUE);
					}
					else /** No repeated Start */
					{
						sI2C_MasterTxData(en_moduleNo,&channelConfig->DefaultTransmitValue,
																(I2C_NumberOfDataType)1U,channelConfig->SlaveAddress,FALSE);
					}
				}
				else /** (sIB_Config[channelId].BufferInUse == TRUE) */
				{
					uint8_t IB_index = GlobalParams.sIB_Config[channelId].TxBuffer.Start+loopItr1;

					if((loopItr1+1U) != (channelConfig->NoOfBuffersUsed))
					{
						sI2C_MasterTxData(en_moduleNo,&sInternalBuffer[IB_index],(I2C_NumberOfDataType)1U,
													channelConfig->SlaveAddress,TRUE);
					}
					else  /** No repeated Start */
					{
						sI2C_MasterTxData(en_moduleNo,&sInternalBuffer[IB_index],(I2C_NumberOfDataType)1U,
																			channelConfig->SlaveAddress,FALSE);
					}
				}
			}
		}

		else if(channelConfig->BufferUsed == EN_EXTERNAL_BUFFER)
		{
			if(GlobalParams.sExternalBuffer[channelId].TxBuffer == NULL_PTR)
			{
				for(loopItr1 = 0U; loopItr1<(GlobalParams.sExternalBuffer[channelId].length); loopItr1++)
				{

					if((loopItr1+1U) != (channelConfig->NoOfBuffersUsed))
					{
						sI2C_MasterTxData(en_moduleNo,&channelConfig->DefaultTransmitValue,
								(I2C_NumberOfDataType)1U,channelConfig->SlaveAddress,TRUE);
					}
					else  /** No repeated Start */
					{
						sI2C_MasterTxData(en_moduleNo,&channelConfig->DefaultTransmitValue,
								(I2C_NumberOfDataType)1U,channelConfig->SlaveAddress,FALSE);
					}
				}
			}
			else /** (sExternalBuffer[channelId].TxBuffer != NULL_PTR) */
			{

				sI2C_MasterTxData(en_moduleNo,(I2C_DataBufferType*)&GlobalParams.sExternalBuffer[channelId].TxBuffer[0U],
									          (I2C_NumberOfDataType)(GlobalParams.sExternalBuffer[channelId].length),
											   channelConfig->SlaveAddress,FALSE);

			}
		}



		/** Give delay between the channels */
		for(loopItr2 = 0 ; loopItr2 < (JobConfig->TimebetweenChannel); loopItr2++);

		/** Next Channel */
		loopItr0++;
	}

	sI2C_SetJobResult(JobConfig->JobId,I2C_JOB_OK);

	/** Call the notification function */
	JobConfig->I2CJobEndNotification();
}
#endif /*#if((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U)))*/

















