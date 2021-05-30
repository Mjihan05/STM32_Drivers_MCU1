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

typedef struct
{
	I2C_StatusType I2CStatus;
	I2C_GlobalErrorType LastError;
#if ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U))
	InternalBufferType sIB_Config[NO_OF_CHANNELS_CONFIGURED];
#endif /** ((I2C_CHANNEL_BUFFERS_ALLOWED == 0U) || (I2C_CHANNEL_BUFFERS_ALLOWED == 2U)) */
}I2C_GlobalParams;

#endif /* I2C_INC_I2C_H_ */
