/**********************************************
 * File - I2C_PbCfg.h
 * Author - Michael Jihan
 * Created on 06-Feb-2021
 *
 * Purpose - This file is to hold the configuration
 * 			 structures
 *
 *********************************************/

#ifndef I2C_CFG_I2C_PBCFG_H_
#define I2C_CFG_I2C_PBCFG_H_

#include "I2C_regTypes.h"

#define TOTAL_NO_OF_I2C_HW_UNIT 	(EN_I2C_3 + 1U)

#define NO_OF_HW_CONFIGURED 		(1U)

typedef uint8_t I2C_DataBufferType;
typedef uint16_t I2C_NumberOfDataType;

/** I2C Hw units available in STM32 */
typedef enum
{
	EN_I2C_1,
	EN_I2C_2,
	EN_I2C_3
}I2C_HwType;

typedef enum
{
	I2C_START_BIT_GENERATED = 0U,
	I2C_ADDRESS_SENT = 1U,
	I2C_BYTE_TRANSFER_FINISHED = 2U,
	I2C_10_BIT_ADDRESS_SENT = 3U,
	I2C_STOP_CONDITION_DETECTED = 4U,
	I2C_RX_DATA_REGISTER_NOT_EMPTY = 6U,
	I2C_TX_DATA_REGISTER_EMPTY = 7U,
	I2C_BUS_ERROR = 8U,			/** Misplaced Start or Stop condition */
	I2C_ARBITRATION_LOSS_DETECTED = 9U,
	I2C_ACKNOWLEDGE_FAILURE = 10U,
	I2C_OVERRUN_OR_UNDERRUN_DETECTED = 11U,
	I2C_TIMEOUT_DETECTED = 14U,		/** SCL remained LOW for 25 ms (Timeout) */
}I2C_Status1Type;

typedef enum
{
	I2C_MASTER_MODE_SELECTED = 0U,		/** If False then Slave mode is selected */
	I2C_BUS_BUSY	 = 1U,
	I2C_DATA_BYTES_TRANSMITTED = 2U,	/** If False then DATA_BYTES_RECIEVED */
	I2C_GENERAL_CALL_ADDR_DETECTED = 4U,
	I2C_RX_ADDR_MATCHED_OAR2 = 7U,
}I2C_Status2Type;

/** I2C Slave Addressing modes  */
typedef enum
{
	EN_I2C_7_BIT_SLAVE_ADDRESS,
	EN_I2C_10_BIT_SLAVE_ADDRESS
}I2C_SlaveAddressType;

/** I2C Master Communication modes  */
typedef enum
{
	EN_I2C_STANDARD_MODE,	/** min 2MHZ i/p freq needed from PCLK1 */
	EN_I2C_FAST_MODE		/** min 4MHZ i/p freq needed from PCLK1  */
}I2C_MasterModeType;

/** I2C Fast Mode Duty Cycle  */
typedef enum
{
	EN_I2C_FAST_MODE_DUTY_2,		/** tlow/thigh = 2  */
	EN_I2C_FAST_MODE_DUTY_16_DIV_9  /** tlow/thigh = 16/9  */
}I2C_FMDutyCycleType;

typedef struct
{
	I2C_HwType ModuleNo;
	I2C_SlaveAddressType SlaveAddressMode;
	uint16_t SlaveAddress;
	I2C_MasterModeType MasterMode;
	uint32_t ClkSpeed;
	I2C_FMDutyCycleType FastModeDutyCycle;
}I2C_HwConfigType;

#endif /* I2C_CFG_I2C_PBCFG_H_ */
