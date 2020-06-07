/**********************************************
 * File - RCC.c
 * Author - Michael Jihan
 * Created on 04/June/2020
 *
 * Purpose - This files holds the Global functions
 * 			 that will configure and control
 * 			 Reset and clock settings
 *
 * ********************************************/
#include <stdint.h>
#include "RCC.h"

Pll_PreScalerType PllPreScalerValues;

void RCC_ClockInit(void)
{

}

Clk_Status_Type RCC_GetClockReadyStatus(Clk_Types En_clockType)
{
	switch(En_clockType)
	{
		case EN_PLL_I2S:
			return (Clk_Status_Type)(((REG_READ32(RCC_CR))&(0x08000000))>>27U);
			break;

		case EN_PLL:
			return (Clk_Status_Type)(((REG_READ32(RCC_CR))&(0x02000000))>>25U);
			break;

		case EN_HSE:
			return (Clk_Status_Type)(((REG_READ32(RCC_CR))&(0x00020000))>>17U);
			break;

		case EN_HSI:
			return (Clk_Status_Type)(((REG_READ32(RCC_CR))&(0x00000002))>>1U);
			break;

		default:
			return EN_INVALID_CLK;
	}
}

void RCC_HseConfigure(Hse_Config_Type* config)
{
	/** Disable HSE before configuration */
	REG_RMW32(RCC_CR,0x00010000,CLEAR<<16U);

	/** Select the HSE to be used */
	REG_RMW32(RCC_CR,SET<<18U,config->HseClockType);

	/** Check if Clock Security System is needed */
#if(HSE_CLK_SECURITY == STD_ON)
	REG_RMW32(RCC_CR,SET<<19U,SET);
#endif

	/** Enable HSE */
	REG_RMW32(RCC_CR,SET<<16U,SET);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_HSE)==EN_CLK_NOT_READY);
}

void RCC_HsiConfigure(Hsi_Config_Type* config)
{
	/** Disable HSI before configuration */
	REG_RMW32(RCC_CR,0x00000001,CLEAR);

	/** Configure the Trim Value */
	RCC_HsiSetTrimValue(config);

	/** Enable HSI */
	REG_RMW32(RCC_CR,SET<<0U,SET);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_HSI)==EN_CLK_NOT_READY);
}

uint8_t RCC_HsiGetCalibValue(void)
{
	return (uint8_t)((((REG_READ32(RCC_CR))&(0x0000FF00))>>8U));
}

void RCC_HsiSetTrimValue(Hsi_Config_Type* config)
{
	/** TODO - If trim value can be adjusted during runtime then write to register
	 *         else modify the HSI config Structure */
	REG_RMW32(RCC_CR,0x000000F8,(config->HsiTrimValue)<<3U);
}

void RCC_PllConfigure(Pll_Config_Type* config)
{

	/** Disable PLL before configuration */
	REG_RMW32(RCC_CR,0x01000000,CLEAR<<24U);

	/** Set PLL Source */
	REG_RMW32(RCC_PLLCFGR,0x00400000,SET<<22U);

	RCC_CalculatePllPrescaler(config,&PllPreScalerValues);

	/** Write Prescaler values to Register*/
	REG_RMW32(RCC_PLLCFGR,0x0F000000,PllPreScalerValues.PreScaler_Q<<24U);
	REG_RMW32(RCC_PLLCFGR,0x00030000,PllPreScalerValues.PreScaler_P<<16U);

	REG_RMW32(RCC_PLLCFGR,0x0000003F,PllPreScalerValues.PreScaler_M<<0U);
	REG_RMW32(RCC_PLLCFGR,0x00007FC0,PllPreScalerValues.PreScaler_N<<6U);

	/** Enable PLL */
	REG_RMW32(RCC_CR,0x01000000,SET<<24U);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_PLL)==EN_CLK_NOT_READY);
}

void RCC_CalculatePllPrescaler(Pll_Config_Type* config,Pll_PreScalerType* returnVal)
{
	uint32_t u32_PllIpFreq = 0U;
	uint32_t u32_PllOpFreq = 0U;
	uint8_t  u8_PrescalerM,u8_PrescalerN = 2U;
	uint8_t  u8_PrescalerP,u8_PrescalerQ = 0U;

	uint8_t u8_OperationCompleteFlag = FALSE;

	/** Get source for PLL clk */
	if((((REG_READ32(RCC_PLLCFGR))&(0x00400000))>>22U)==(0U))
	{/** HSI Selected */
		u32_PllIpFreq = 16000000U;
		u32_PllOpFreq = config->PllOutputFreq_SYSCLK;
	}
	else
	{/** HSE Selected */
		u32_PllIpFreq = config->PllInputFreq;
		u32_PllOpFreq = config->PllOutputFreq_SYSCLK;
	}

	/** Find the best Prescalers */
	for(uint8_t loopItr=0U; loopItr<4U; loopItr++)
	{
		uint32_t u32_PlltempFreq = 0U;
		u8_PrescalerP += 2U;
		u32_PllOpFreq = (uint32_t)(u32_PllOpFreq*u8_PrescalerP);
		u8_PrescalerN = (uint8_t)(u32_PllOpFreq/u32_PllIpFreq);

		/*while((u8_PrescalerN<50U)  &&(u32_PllOpFreq!= (uint32_t)(u32_PllIpFreq*(u8_PrescalerN/u8_PrescalerM))) )
		{
			u8_PrescalerN = u8_PrescalerN*u8_PrescalerM++;
		}*/

		for(u8_PrescalerN = 50U; u8_PrescalerN<=432; u8_PrescalerN++)
		{
			u32_PlltempFreq = (uint32_t)(u32_PllIpFreq*u8_PrescalerN);
			u8_PrescalerM = 2U;
			while((u8_PrescalerM<=63U)&&(u32_PlltempFreq>u32_PllOpFreq))
			{
				u32_PlltempFreq = (uint32_t)(u32_PlltempFreq/u8_PrescalerM);
				u8_PrescalerM++;
			}
		}

		if(u32_PllOpFreq == (uint32_t)(u32_PllIpFreq*(u8_PrescalerN/u8_PrescalerM)))
		{
			/** PreScaler Found*/
			u8_OperationCompleteFlag = TRUE;

			if((u8_PrescalerP>8U)&&((u8_PrescalerP%2)!=0U))
			{
				u8_OperationCompleteFlag = FALSE;
			}

			u8_PrescalerQ = (uint8_t)(u32_PllOpFreq/(config->PllOutputFreq_PLL48CK));

			if( ((u8_PrescalerQ<2U)&&((u8_PrescalerQ>15U))) && ((u32_PllOpFreq%(config->PllOutputFreq_PLL48CK))!=0U) )
			{
				u8_OperationCompleteFlag = FALSE;
			}
		}

		if(u8_OperationCompleteFlag == TRUE)
		{
			returnVal.PreScaler_M = u8_PrescalerM;
			returnVal.PreScaler_N = u8_PrescalerN;
			returnVal.PreScaler_P = u8_PrescalerP;
			returnVal.PreScaler_Q = u8_PrescalerQ;
			u8_OperationCompleteFlag = FALSE;
			break;
		}
	}
}

void RCC_PllI2SConfigure(void)
{
	/** Enable PLLI2S */
	REG_WRITE32(RCC_CR,SET<<26U);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_PLL_I2S)==EN_CLK_NOT_READY);
}

void RCC_EnableSpreadSpectrumClkGen()
{

}

void RCC_McoConfig(enum clk, uint8_t preScaler)
{

}

void RCC_SetClockSource(enum clk, uint8_t source)
{

}

void RCC_RtcConfig()
{

}

void RCC_SystemClkConfig()
{

}

void RCC_BusConfig()
{

}

void RCC_ClearClkRdyIntFlag()
{

}

void RCC_PeripheralClkEnable()
{

}

void RCC_PeripheralClkEnableLowPower()
{

}

void RCC_ResetPeripheral(enum peripheral)
{

}

void RCC_ResetBackupDomain()
{

}

void RCC_GetResetStatus()
{

}









