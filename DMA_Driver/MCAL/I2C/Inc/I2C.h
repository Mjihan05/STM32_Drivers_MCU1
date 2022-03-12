/**********************************************
 * File - I2C.h
 * Author - Michael Jihan
 * Created on 13-Apr-2021
 *
 * Purpose - 
 *
 * ********************************************/

#ifndef I2C_INC_I2C_H_
#define I2C_INC_I2C_H_

#include "I2C_regTypes.h"

#include "I2C_PbCfg.h"

typedef enum /** Defines a range of specific status for I2C Handler */
{
	I2C_UNINIT = 0U,
	I2C_IDLE,
	I2C_BUSY,
}I2C_StatusType;

typedef enum
{
	I2C_INIT_ERROR = 0U,
}I2C_GlobalErrorType;

typedef enum /** Defines a range of specific Jobs status for I2C Handler/ */
{
	I2C_JOB_OK = 0U,
	I2C_JOB_PENDING,
	I2C_JOB_FAILED,
	I2C_JOB_QUEUED,
}I2C_JobResultType;

typedef enum
{
	I2C_SEQ_OK = 0U,
	I2C_SEQ_PENDING,
	I2C_SEQ_FAILED,
	I2C_SEQ_CANCELED
}I2C_SeqResultType;

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
typedef struct {
	const I2C_DataBufferType * TxBuffer;    /** Pointer to source buffer */
	I2C_DataBufferType * RxBuffer;   /** Pointer to destination buffer */
	I2C_NumberOfDataType length; // Number of elements of I2C_DataBufferType in destination buffer
	Bool active; /** Set if the buffer is configured */
} I2C_EbType;
#endif /**  ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

typedef struct
{
	I2C_StatusType I2CStatus;
	I2C_GlobalErrorType LastError;
#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
	InternalBufferType sIB_Config[NO_OF_CHANNELS_CONFIGURED];
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */
#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
	I2C_EbType sExternalBuffer[NO_OF_CHANNELS_CONFIGURED];
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */
}I2C_GlobalParams;

/** I2c Configuration structure */
extern I2C_ConfigType I2C_Config0;

/** Global Functions available */
void I2C_Init (const I2C_ConfigType* ConfigPtr);
I2C_StatusType I2C_GetStatus (void);
I2C_JobResultType I2C_GetJobResult (I2C_JobType Job);
I2C_SeqResultType I2C_GetSequenceResult (I2C_SequenceType Sequence);

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
Std_ReturnType I2C_WriteIB (I2C_ChannelType Channel,const I2C_DataBufferType* DataBufferPtr);
Std_ReturnType I2C_ReadIB (I2C_ChannelType Channel,I2C_DataBufferType* DataBufferPointer);
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
Std_ReturnType I2C_SetupEB (I2C_ChannelType Channel,const I2C_DataBufferType* SrcDataBufferPtr,
							I2C_DataBufferType* DesDataBufferPtr,I2C_NumberOfDataType Length);
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 1U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */

#if((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U)))
Std_ReturnType I2C_SyncTransmit (I2C_SequenceType Sequence);
#endif /** ((I2C_LEVEL_DELIVERED == (2U)) || (I2C_LEVEL_DELIVERED == (0U))) */

#endif /* I2C_INC_I2C_H_ */
