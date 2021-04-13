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

typedef struct
{
	I2C_StatusType I2CStatus;
	I2C_GlobalErrorType LastError;

}I2C_GlobalParams;

#endif /* I2C_INC_I2C_H_ */
