/**********************************************
 * File - I2C_PbCfg.c
 * Author - Michael Jihan
 * Created on 06-Feb-2021
 *
 * Purpose - This file is to hold the configuration
 * 			 structures
 *
 * ********************************************/

#include "Reg_Macros.h"
#include "I2C_PbCfg.h"

const I2C_HwConfigType I2C_HwConfig0[NO_OF_HW_CONFIGURED] =
{
	{
		.ModuleNo = EN_I2C_1,
		.SlaveAddressMode = EN_I2C_7_BIT_SLAVE_ADDRESS,
		.SlaveAddress = (uint16_t)0x0A0A,
		.MasterMode = EN_I2C_STANDARD_MODE,
		.ClkSpeed = 100000U, /* 100KHz */
		.FastModeDutyCycle = EN_I2C_FAST_MODE_DUTY_2,
	},
};
