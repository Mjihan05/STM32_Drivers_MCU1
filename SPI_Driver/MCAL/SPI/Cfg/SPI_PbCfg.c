/**********************************************
 * File - SPI_PbCfg.c
 * Author - Michael Jihan
 * Created on 17-Oct-2020
 *
 * Purpose - This file is to hold the configuration
 * 			 structures
 *
 * ********************************************/

#include <stdint.h>
#include "Reg_Macros.h"
#include "SPI_PbCfg.h"

uint32_t Spi_BaseAddress[] =
{
	SPI_1_BASE_ADDRESS,
	SPI_2_BASE_ADDRESS,
	SPI_3_BASE_ADDRESS
};

const Spi_SequenceConfigType Spi_SequenceConfig0[NO_OF_SEQUENCES_CONFIGURED] =
{
		{
			.SequenceId = 0U,
			.SpiInterruptible = TRUE,
			.SpiSequenceEndNotification = NULL_PTR,
			.Jobs = {/** Jobs need to be filled in the order of priority  */
				0U,1U,EOL,
			},
		},
};

const Spi_JobConfigType Spi_JobConfig0[NO_OF_JOBS_CONFIGURED] =
{
		{
			.JobId = 0U,
			.HwUsed = EN_SPI_1,
			.CsFunctionUsed = EN_CS_NOT_USED,
			.BaudRate = 980000U,
			.DataShiftonEdge = EN_SHIFT_ON_RISING_EDGE,
			.ShiftClkIdleLevel = STD_LOW,
			.SpiJobEndNotification = NULL_PTR,
			.ChannelAssignment = {
					0U,1U,EOL,
			},
		},
};

const Spi_ChannelConfigType Spi_ChannelConfig0[NO_OF_CHANNELS_CONFIGURED] =
{
		{
				.ChannelId = 0U,
				.BufferUsed = EN_INTERNAL_BUFFER,
				.NoOfBuffersUsed = 0U,
				.DataFrame = 8U,
				.TransferStart = EN_MSB_FIRST,
				.DefaultTransmitValue = 5U,
		}
};

Spi_ConfigType Spi_Config0 =
{
		.Channel = &Spi_ChannelConfig0,
		.Job = &Spi_JobConfig0,
		.Sequence = &Spi_SequenceConfig0
};


