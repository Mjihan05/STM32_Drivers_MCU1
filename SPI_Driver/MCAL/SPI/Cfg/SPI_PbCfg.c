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

extern Std_ReturnType Spi_DeInit (void);

void dummyNotif(void)
{
	;
}

uint32_t Spi_BaseAddress[TOTAL_NO_OF_SPI_HW_UNIT] =
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
			.SpiSequenceEndNotification = &dummyNotif,
			.Jobs = {/** Jobs need to be filled in the order of priority  */
				0U,EOL,
			},
		},
		{
			.SequenceId = 1U,
			.SpiInterruptible = TRUE,
			.SpiSequenceEndNotification = &dummyNotif,
			.Jobs = {/** Jobs need to be filled in the order of priority  */
				2U,EOL,
			},
		},
		{
			.SequenceId = 2U,
			.SpiInterruptible = TRUE,
			.SpiSequenceEndNotification = &dummyNotif,
			.Jobs = {/** Jobs need to be filled in the order of priority  */
				2U,0U,EOL,
			},
		},
		{
			.SequenceId = 3U,
			.SpiInterruptible = TRUE,
			.SpiSequenceEndNotification = &dummyNotif,
			.Jobs = {/** Jobs need to be filled in the order of priority  */
					0U,2U,EOL,
			},
		},
};

const Spi_JobConfigType Spi_JobConfig0[NO_OF_JOBS_CONFIGURED] =
{
		{
			.JobId = 0U,
			.HwUsed = EN_SPI_1,
			.CsFunctionUsed = EN_CS_HW_HANDLING,
			.CsPinUsed = DIO_PIN_A4,
			.CsPinPolarity = STD_LOW,
			.BaudRate = 15625U,
			.DataShiftonEdge = EN_SHIFT_ON_RISING_EDGE,
			.ShiftClkIdleLevel = STD_LOW,
			.TimebetweenChannel = 1000U,
			.SpiJobEndNotification = (void*)&dummyNotif,
			.ChannelAssignment = {
					0U,1U,EOL,
			},
		},
	    {
			.JobId = 1U,
			.HwUsed = EN_SPI_2,
			.CsFunctionUsed = EN_CS_SW_HANDLING,
			.CsPinUsed = DIO_PIN_B3,
			.CsPinPolarity = STD_LOW,
			.BaudRate = 980000U,
			.DataShiftonEdge = EN_SHIFT_ON_FALLING_EDGE,
			.ShiftClkIdleLevel = STD_HIGH,
			.TimebetweenChannel = 1000U,
			.SpiJobEndNotification = (void*)&dummyNotif,
			.ChannelAssignment = {
					2U,1U,EOL,
			},
	    },
		{
			.JobId = 2U,
			.HwUsed = EN_SPI_3,
			.CsFunctionUsed = EN_CS_NOT_USED,
			.BaudRate = 980000U,
			.DataShiftonEdge = EN_SHIFT_ON_RISING_EDGE,
			.ShiftClkIdleLevel = STD_LOW,
			.TimebetweenChannel = 1000U,
			.SpiJobEndNotification = (void*)&dummyNotif,
			.ChannelAssignment = {
					0U,2U,EOL,
			},
		},
};

const Spi_ChannelConfigType Spi_ChannelConfig0[NO_OF_CHANNELS_CONFIGURED] =
{
		{
				.ChannelId = 0U,
				.BufferUsed = EN_EXTERNAL_BUFFER,
				.NoOfBuffersUsed = 4U,
				.DataFrame = EN_8_BIT_DATA_FRAME,
				.TransferStart = EN_MSB_FIRST,
				.DefaultTransmitValue = 0xFF,
		},
		{
				.ChannelId = 1U,
				.BufferUsed = EN_EXTERNAL_BUFFER,
				.NoOfBuffersUsed = 10U,
				.DataFrame = EN_8_BIT_DATA_FRAME,
				.TransferStart = EN_MSB_FIRST,
				.DefaultTransmitValue = 0xFF,
		},
		{
				.ChannelId = 2U,
				.BufferUsed = EN_INTERNAL_BUFFER,
				.NoOfBuffersUsed = 2U,
				.DataFrame = EN_8_BIT_DATA_FRAME,
				.TransferStart = EN_MSB_FIRST,
				.DefaultTransmitValue = 0xD1,
		}
};

const Spi_ConfigType Spi_Config0 =
{
		.Channel = &Spi_ChannelConfig0[0U],
		.Job = &Spi_JobConfig0[0U],
		.Sequence = &Spi_SequenceConfig0[0U]
};


