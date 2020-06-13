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
#include "RCC_regTypes.h"

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

void RCC_EnableClk(Clk_Types clk)
{
	switch(clk)
	{
		case EN_PLL_I2S:
			REG_RMW32(RCC_CR,0x04000000,SET_BIT(26U));
			break;

		case EN_PLL:
			REG_RMW32(RCC_CR,0x01000000,SET_BIT(24U));
			break;

		case EN_HSE:
			REG_RMW32(RCC_CR,0x00010000,SET_BIT(16U));
			break;

		case EN_HSI:
			REG_RMW32(RCC_CR,0x00000001,SET_BIT(0U));
			break;

		default:
			break;
	}
}

void RCC_DisableClk(Clk_Types clk)
{
	switch(clk)
	{
		case EN_PLL_I2S:
			REG_RMW32(RCC_CR,0x04000000,CLEAR_BIT(26U));
			break;

		case EN_PLL:
			REG_RMW32(RCC_CR,0x01000000,CLEAR_BIT(24U));
			break;

		case EN_HSE:
			REG_RMW32(RCC_CR,0x00010000,CLEAR_BIT(16U));
			break;

		case EN_HSI:
			REG_RMW32(RCC_CR,0x00000001,CLEAR_BIT(0U));
			break;

		default:
			break;
	}
}

void RCC_EnableSpreadSpectrumClkGen()
{

}

void RCC_Mco1Config(Clk_Types clk, uint8_t preScaler)
{
	if( (preScaler<(2U)) || (preScaler>(5U)) )
		{
			return;
		}

		switch(clk)
		{
			case EN_HSI:
				REG_RMW32(RCC_CFGR,0x07600000,(((0U)<<21U)|((preScaler)<<24U)) );
				break;

			case EN_LSE:
				RCC_DisableClk(EN_LSE);
				REG_RMW32(RCC_CFGR,0x07600000,(((1U)<<21U)|((preScaler)<<24U)) );
				break;

			case EN_HSE:
				RCC_DisableClk(EN_HSE);
				REG_RMW32(RCC_CFGR,0x07600000,(((2U)<<21U)|((preScaler)<<24U)));
				break;

			case EN_PLL:
				RCC_DisableClk(EN_PLL);
				REG_RMW32(RCC_CFGR,0x07600000,(((3U)<<21U)|((preScaler)<<24U)));
				break;

			default:
				return;
		}
}

void RCC_Mco2Config(Clk_Types clk, uint8_t preScaler)
{
	if( (preScaler<(2U)) || (preScaler>(5U)) )
	{
		return;
	}

	switch(clk)
	{
		case EN_SYS_CLK:
			REG_RMW32(RCC_CFGR,0xF8000000,(((0U)<<30U)|((preScaler)<<27U)) );
			break;

		case EN_PLL_I2S:
			RCC_DisableClk(EN_PLL_I2S);
			REG_RMW32(RCC_CFGR,0xF8000000,(((1U)<<30U)|((preScaler)<<27U)) );
			break;

		case EN_HSE:
			RCC_DisableClk(EN_HSE);
			REG_RMW32(RCC_CFGR,0xF8000000,(((2U)<<30U)|((preScaler)<<27U)));
			break;

		case EN_PLL:
			RCC_DisableClk(EN_PLL);
			REG_RMW32(RCC_CFGR,0xF8000000,(((3U)<<30U)|((preScaler)<<27U)));
			break;

		default:
			return;
	}
}

void RCC_SetClockSource(Clk_Types clk, uint8_t source)
{
	switch(clk)
		{
			case EN_PLL_I2S:
				REG_RMW32(RCC_CFGR,0x00800000, (source<<23U) );
				break;

			case EN_SYS_CLK:
				REG_RMW32(RCC_CFGR,0x00000003, (source<<0U) );
				break;

			default:
				return;
		}
}

void RCC_RtcConfig(Rtc_Config_Type* config)
{
	/** if HSE is being used then 1MHz clock needs to be supplied */
#ifdef(HSE_CLOCK_USED)
	if((config->RtcClkSource)== EN_HSE)
	{
		uint32_t HseOutputClkFreq = GlobalConfig->HseConfig->HseOutputFreq;
		if(HseOutputClkFreq%(1000000U)== 0U)
		{
			uint8_t RtcPreScaler = HseOutputClkFreq/(1000000U);
			REG_RMW32(RCC_CFGR,0x001F0000, (RtcPreScaler<<16U) );
		}
		else
		{
			/** Error*/
		}
	}
	else
#endif
	{

	}
}

Clk_Types RCC_GetSystemClkSource()
{
	uint8_t source = (uint8_t)((REG_READ32(RCC_CFGR)&0x0000000C)>>2U);
	return (Clk_Types)(3U-source);
}

void RCC_BusConfig(Bus_Config_Type* config)
{
	/** Configure AHB, APB1, APB2 */
	REG_RMW32(RCC_CFGR,0x000000F0, ((config->AHB_PreScaler)<<4U) );
	REG_RMW32(RCC_CFGR,0x00001C00, ((config->APB1_PreScaler)<<10U) );
	REG_RMW32(RCC_CFGR,0x0000E000, ((config->APB1_PreScaler)<<13U) );
}

void RCC_EnableIntFlag(Rcc_Int_Type IntFlag)
{
	switch(IntFlag)
	{
	case EN_CLK_SECURITY_INT:
		REG_RMW32(RCC_CIR,0x00000080 ,SET_BIT(7U) );
		break;

	case EN_PLL_I2S_READY:
		REG_RMW32(RCC_CIR,0x00002000 ,SET_BIT(13U) );
		break;

	case EN_PLL_READY:
		REG_RMW32(RCC_CIR,0x00001000 ,SET_BIT(12U) );
		break;

	case EN_HSE_READY:
		REG_RMW32(RCC_CIR,0x00000800 ,SET_BIT(11U) );
		break;

	case EN_HSI_READY:
		REG_RMW32(RCC_CIR,0x00000400 ,SET_BIT(10U) );
		break;

	case EN_LSE_READY:
		REG_RMW32(RCC_CIR,0x00000200 ,SET_BIT(9U) );
		break;

	case EN_LSI_READY:
		REG_RMW32(RCC_CIR,0x00000100 ,SET_BIT(8U) );
		break;

	default:
		break;

	}
}

Rcc_Int_Type RCC_ReadIntFlag(void)
{
	uint8_t u8_returnValue=0U;
	uint8_t u8_flag = (uint8_t)(REG_READ32(RCC_CIR)&0x0000003F);
	while(u8_flag!=0U)
	{
		u8_returnValue++;
		u8_flag = u8_flag>>1U;
	}
	return (Rcc_Int_Type)(7U-u8_returnValue);
}

void RCC_ClearIntFlag(Rcc_Int_Type IntFlag)
{
	switch(IntFlag)
	{
	case EN_CLK_SECURITY_INT:
		REG_WRITE32(RCC_CIR, SET_BIT(23U) );
		break;

	case EN_PLL_I2S_READY:
		REG_WRITE32(RCC_CIR, SET_BIT(21U) );
		break;

	case EN_PLL_READY:
		REG_WRITE32(RCC_CIR, SET_BIT(20U) );
		break;

	case EN_HSE_READY:
		REG_WRITE32(RCC_CIR, SET_BIT(19U) );
		break;

	case EN_HSI_READY:
		REG_WRITE32(RCC_CIR, SET_BIT(18U) );
		break;

	case EN_LSE_READY:
		REG_WRITE32(RCC_CIR, SET_BIT(17U) );
		break;

	case EN_LSI_READY:
		REG_WRITE32(RCC_CIR, SET_BIT(16U) );
		break;

	default:
		break;

	}
}

void RCC_AHB1PeripheralClkEnable(Rcc_AHB1_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB1ENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_AHB2PeripheralClkEnable(Rcc_AHB2_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB2ENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_AHB3PeripheralClkEnable(Rcc_AHB3_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB3ENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_APB1PeripheralClkEnable(Rcc_APB1_Peripherals peripheral)
{
	REG_RMW32(RCC_APB1ENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_APB2PeripheralClkEnable(Rcc_APB2_Peripherals peripheral)
{
	REG_RMW32(RCC_APB2ENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_AHB1EnablePerSleepMode(Rcc_AHB1_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB1LPENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_AHB2EnablePerSleepMode(Rcc_AHB2_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB2LPENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_AHB3EnablePerSleepMode(Rcc_AHB3_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB3LPENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_APB1EnablePerSleepMode(Rcc_APB1_Peripherals peripheral)
{
	REG_RMW32(RCC_APB1LPENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_APB2EnablePerSleepMode(Rcc_APB2_Peripherals peripheral)
{
	REG_RMW32(RCC_APB2LPENR,SET_BIT(peripheral) ,SET_BIT(peripheral) );
}

void RCC_AHB1PeripheralClkDisable(Rcc_AHB1_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB1ENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_AHB2PeripheralClkDisable(Rcc_AHB2_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB2ENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_AHB3PeripheralClkDisable(Rcc_AHB3_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB3ENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_APB1PeripheralClkDisable(Rcc_APB1_Peripherals peripheral)
{
	REG_RMW32(RCC_APB1ENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_APB2PeripheralClkDisable(Rcc_APB2_Peripherals peripheral)
{
	REG_RMW32(RCC_APB2ENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_AHB1DisablePerSleepMode(Rcc_AHB1_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB1LPENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_AHB2DisablePerSleepMode(Rcc_AHB2_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB2LPENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_AHB3DisablePerSleepMode(Rcc_AHB3_Peripherals peripheral)
{
	REG_RMW32(RCC_AHB3LPENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_APB1DisablePerSleepMode(Rcc_APB1_Peripherals peripheral)
{
	REG_RMW32(RCC_APB1LPENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_APB2DisablePerSleepMode(Rcc_APB2_Peripherals peripheral)
{
	REG_RMW32(RCC_APB2LPENR,SET_BIT(peripheral) ,CLEAR_BIT(peripheral) );
}

void RCC_ResetAHB1Peripheral(Rcc_AHB1_Peripherals peripheral)
{
	REG_WRITE32(RCC_AHB1RSTR, SET_BIT(peripheral) );
}

void RCC_ResetAHB2Peripheral(Rcc_AHB2_Peripherals peripheral)
{
	REG_WRITE32(RCC_AHB2RSTR, SET_BIT(peripheral) );
}

void RCC_ResetAHB3Peripheral(Rcc_AHB3_Peripherals peripheral)
{
	REG_WRITE32(RCC_AHB3RSTR, SET_BIT(peripheral) );
}

void RCC_ResetAPB1Peripheral(Rcc_APB1_Peripherals peripheral)
{
	REG_WRITE32(RCC_APB1RSTR, SET_BIT(peripheral) );
}

void RCC_ResetAPB2Peripheral(Rcc_APB2_Peripherals peripheral)
{
	REG_WRITE32(RCC_APB2RSTR, SET_BIT(peripheral) );
}

void RCC_ResetBackupDomain()
{

}

void RCC_GetResetStatus()
{

}









