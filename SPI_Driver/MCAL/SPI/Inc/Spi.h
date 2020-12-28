/**********************************************
 * File - Spi.h
 * Author - Michael Jihan
 * Created on Oct 17, 2020
 *
 * Purpose - This file holds the global prototype
 * 			 of functions that configure SPI
 * 			 settings
 *
 * ********************************************/

#ifndef SPI_INC_SPI_H_
#define SPI_INC_SPI_H_

#include "SPI_regTypes.h"
#include "SPI_PbCfg.h"


#define QUEUE_SIZE (NO_OF_JOBS_CONFIGURED)

typedef enum /** Defines a range of specific status for SPI Handler */
{
	SPI_UNINIT = 0U,
	SPI_IDLE,
	SPI_BUSY,
}Spi_StatusType;

#if(SPI_LEVEL_DELIVERED == (2U))
typedef enum /** Specifies the asynchronous mechanism mode for SPI busses handled asynchronously in LEVEL 2 */
{
	SPI_POLLING_MODE = 0U,
	SPI_INTERRUPT_MODE,
}Spi_AsyncModeType;
#endif

typedef enum /** Defines a range of specific Jobs status for SPI Handler/ */
{
	SPI_JOB_OK = 0U,
	SPI_JOB_PENDING,
	SPI_JOB_FAILED,
	SPI_JOB_QUEUED,
}Spi_JobResultType;

typedef enum
{
	SPI_SEQ_OK = 0U,
	SPI_SEQ_PENDING,
	SPI_SEQ_FAILED,
	SPI_SEQ_CANCELED
}Spi_SeqResultType;

typedef uint16_t Spi_DataBufferType;
typedef uint16_t Spi_NumberOfDataType;
typedef uint8_t Spi_ChannelType;
typedef uint16_t Spi_JobType;
typedef uint8_t Spi_SequenceType;
typedef uint8_t Spi_HWUnitType;

typedef struct {
	Spi_DataBufferType * TxBuffer;    /** Pointer to source buffer */
	Spi_DataBufferType * RxBuffer;   /** Pointer to destination buffer */
	Spi_NumberOfDataType length; // Number of elements of Spi_DataBufferType in destination buffer
	Bool active; /** Set if the buffer is configured */
} Spi_EbType;

typedef struct
{
	Spi_SequenceType sequenceId;
	uint8_t startBufferIndex;
	uint8_t endBufferIndex;
	uint8_t noOfRetries;
}Spi_BufferIndex;

/* TODO - check sExternalBuffer implementation, this also can be implemented with ptr */
typedef struct
{
	Spi_StatusType SpiStatus;
	Spi_JobType SpiQueuedJobsBuffer[QUEUE_SIZE];
	Spi_BufferIndex BufferIndex[NO_OF_SEQUENCES_CONFIGURED]; /** Holds the index of the queued jobs of a sequence */
	Spi_SequenceType NextSequence;
}Spi_GlobalParams;

extern Spi_ConfigType Spi_Config0;

void Spi_Init (const Spi_ConfigType* ConfigPtr);
Std_ReturnType Spi_DeInit (void);
Std_ReturnType Spi_WriteIB (Spi_ChannelType Channel,const Spi_DataBufferType* DataBufferPtr);
Std_ReturnType Spi_AsyncTransmit (Spi_SequenceType Sequence);
Std_ReturnType Spi_ReadIB (Spi_ChannelType Channel,Spi_DataBufferType* DataBufferPointer);
Std_ReturnType Spi_SetupEB (Spi_ChannelType Channel,const Spi_DataBufferType* SrcDataBufferPtr,
							Spi_DataBufferType* DesDataBufferPtr,Spi_NumberOfDataType Length);
Spi_StatusType Spi_GetStatus (void);
Spi_JobResultType Spi_GetJobResult (Spi_JobType Job);
Spi_SeqResultType Spi_GetSequenceResult (Spi_SequenceType Sequence);
Std_ReturnType Spi_SyncTransmit (Spi_SequenceType Sequence);
Spi_StatusType Spi_GetHWUnitStatus (Spi_HWUnitType HWUnit);
void Spi_Cancel (Spi_SequenceType Sequence);
void Spi_MainFunction_Handling (void);

#endif /* SPI_INC_SPI_H_ */
