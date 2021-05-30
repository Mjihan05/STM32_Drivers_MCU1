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

uint32_t I2C_BaseAddress[TOTAL_NO_OF_I2C_HW_UNIT] =
{
	I2C_1_BASE_ADDRESS,
	I2C_2_BASE_ADDRESS,
	I2C_3_BASE_ADDRESS
};

const I2C_HwConfigType I2C_HwConfig0[NO_OF_HW_CONFIGURED] =
{
	{
		.ModuleNo = EN_I2C_1,
		.SlaveAddressMode = EN_I2C_7_BIT_SLAVE_ADDRESS,
		.OwnAddress = (uint16_t)0x0A0A,
		.MasterMode = EN_I2C_STANDARD_MODE,
		.ClkSpeed = 100000U, /* 100KHz */
		.FastModeDutyCycle = EN_I2C_FAST_MODE_DUTY_2,
	},
};

const I2C_SequenceConfigType I2C_SequenceConfig0[NO_OF_SEQUENCES_CONFIGURED] =
{
		{
			.SequenceId = 0U,
			.I2CInterruptible = TRUE,
			.I2CSequenceEndNotification = &dummyNotif,
			.Jobs = {/** Jobs need to be filled in the order of priority  */
				0U,/*1U,*/EOL,
			},
		},
};

const I2C_JobConfigType I2C_JobConfig0[NO_OF_JOBS_CONFIGURED] =
{
		{
			.JobId = 0U,
			.Priority = 0U,
			.TimebetweenChannel = 1000U,
			.I2CJobEndNotification = (void*)&dummyNotif,
			.HwConfig = &I2C_HwConfig0[0U],
			.ChannelAssignment = {
					0U,1U,EOL,
			},
		},
};

const I2C_ChannelConfigType I2C_ChannelConfig0[NO_OF_CHANNELS_CONFIGURED] =
{
		{
				.ChannelId = 0U,
				.BufferUsed = EN_INTERNAL_BUFFER,
				.NoOfBuffersUsed = 1U,
				.DefaultTransmitValue = 0xFF,
		},
		{
				.ChannelId = 1U,
				.BufferUsed = EN_EXTERNAL_BUFFER,
				.NoOfBuffersUsed = 10U,
				.DefaultTransmitValue = 0xFF,
		},
};

const I2C_ConfigType I2C_Config0 =
{
		.Channel = &I2C_ChannelConfig0[0U],
		.Job = &I2C_JobConfig0[0U],
		.Sequence = &I2C_SequenceConfig0[0U]
};








