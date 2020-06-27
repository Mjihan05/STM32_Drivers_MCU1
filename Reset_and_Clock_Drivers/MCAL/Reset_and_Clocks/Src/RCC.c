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
#include <Reg_Macros.h>
#include "RCC_regTypes.h"

#include "RCC.h"

Pll_PreScalerType PllPreScalerValues;

void RCC_Init(RCC_GlobalConfigType* Config )
{
#ifdef HSE_CLOCK_USED
	RCC_HseConfigure(Config->HseConfig);
#else
	RCC_HsiConfigure(Config->HsiConfig);
#endif

#ifdef PLL_CLOCK_USED
	RCC_PllConfigure(Config->PllConfig);
#if(PLL_I2S_USED == STD_ON)
	RCC_PllI2SConfigure(Config->PllConfig);
#endif
#endif

#ifdef RTC_CLOCK_USED
/*NOT TESTED*/
	void RCC_RtcConfig(Config->RtcConfig);
#endif

	/** Bus Config */
	RCC_BusConfig(Config->BusConfig);

	/** Set System Clock Source */
	RCC_SetClockSource(EN_SYS_CLK, SYS_CLK_SOURCE);
}

Clk_Status_Type RCC_GetClockReadyStatus(Clk_Types En_clockType)
{
	switch(En_clockType)
	{
		case EN_PLL_I2S:
			return (Clk_Status_Type)( ( (REG_READ32(RCC_CR))&(0x08000000))>>27U );
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

		case EN_LSE:
			return (Clk_Status_Type)(((REG_READ32(RCC_BDCR))&(0x00000002))>>1U);
			break;

		case EN_LSI:
			return (Clk_Status_Type)(((REG_READ32(RCC_CSR))&(0x00000002))>>1U);
			break;

		default:
			return EN_INVALID_CLK;
	}
}

#ifdef HSE_CLOCK_USED
void RCC_HseConfigure(Hse_Config_Type* config)
{
	/** Disable HSE before configuration */
	REG_RMW32(RCC_CR,0x00010000,CLEAR<<16U);

	/** Select the HSE to be used */
	RCC_SetClockSource(EN_HSE, (config->HseClockType));
	//REG_RMW32(RCC_CR,MASK_BIT(18U),(config->HseClockType)<<18U);

	/** Check if Clock Security System is needed */
#if(HSE_CLK_SECURITY == STD_ON)
	REG_RMW32(RCC_CR,MASK_BIT(19U),SET_BIT(19U));
#endif

	/** Enable HSE */
	//REG_RMW32(RCC_CR,MASK_BIT(16U),SET_BIT(16U));
	RCC_EnableClk(EN_HSE);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_HSE)==EN_CLK_NOT_READY);
}
#endif /*#ifdef HSE_CLOCK_USED*/

void RCC_HsiSetTrimValue(Hsi_Config_Type* config)
{
	/** TODO - If trim value can be adjusted during runtime then write to register
	 *         else modify the HSI config Structure */
	REG_RMW32(RCC_CR,0x000000F8,(config->HsiTrimValue)<<3U);
}

void RCC_HsiConfigure(Hsi_Config_Type* config)
{
	/** Disable HSI before configuration */
	REG_RMW32(RCC_CR,0x00000001,CLEAR);

	/** Configure the Trim Value */
	RCC_HsiSetTrimValue(config);

	/** Enable HSI */
	//REG_RMW32(RCC_CR,SET<<0U,SET);
	RCC_EnableClk(EN_HSI);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_HSI)==EN_CLK_NOT_READY);
}

uint8_t RCC_HsiGetCalibValue(void)
{
	return (uint8_t)((((REG_READ32(RCC_CR))&(0x0000FF00))>>8U));
}

#ifdef PLL_CLOCK_USED
void RCC_CalculatePllPrescaler(Pll_Config_Type* config,Pll_PreScalerType* returnVal)
{
	uint32_t u32_PllIpFreq = 0U;
	uint32_t u32_PllOpFreq = 0U;
	uint8_t  u8_PrescalerM = 0U,u8_PrescalerN = 2U;
	uint8_t  u8_PrescalerP = 0U,u8_PrescalerQ = 0U;

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
			while((u8_PrescalerM<=63U)&&((u32_PlltempFreq/u8_PrescalerM)>u32_PllOpFreq))
			{
				//u32_PlltempFreq = (uint32_t)(u32_PlltempFreq/u8_PrescalerM);
				u8_PrescalerM++;
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
			returnVal->PreScaler_M = u8_PrescalerM;
			returnVal->PreScaler_N = u8_PrescalerN;
			returnVal->PreScaler_P = u8_PrescalerP;
			returnVal->PreScaler_Q = u8_PrescalerQ;
			//u8_OperationCompleteFlag = FALSE;
			break;
		}
		}
		if(u8_OperationCompleteFlag == TRUE)
		{
			break;
		}
	}
}

void RCC_PllConfigure(Pll_Config_Type* config)
{

	/** Disable PLL before configuration */
	REG_RMW32(RCC_CR,0x01000000,CLEAR<<24U);

	/** Set PLL Source */
	//REG_RMW32(RCC_PLLCFGR,0x00400000,(config->PllClkSource)<<22U);
	RCC_SetClockSource(EN_PLL, (config->PllClkSource));

	RCC_CalculatePllPrescaler(config,&PllPreScalerValues);

	/** Write Prescaler values to Register*/
	REG_RMW32(RCC_PLLCFGR,0x0F000000,PllPreScalerValues.PreScaler_Q<<24U);
	REG_RMW32(RCC_PLLCFGR,0x00030000,PllPreScalerValues.PreScaler_P<<16U);

	REG_RMW32(RCC_PLLCFGR,0x0000003F,PllPreScalerValues.PreScaler_M<<0U);
	REG_RMW32(RCC_PLLCFGR,0x00007FC0,PllPreScalerValues.PreScaler_N<<6U);

	/** Enable PLL */
	//REG_RMW32(RCC_CR,0x01000000,SET<<24U);
	RCC_EnableClk(EN_PLL);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_PLL)==EN_CLK_NOT_READY);
}
#endif /*#ifdef PLL_CLOCK_USED */

#if 0
void RCC_LsiEnable(void)
{
	/** Turn on LSI */
	REG_WRITE32(RCC_CSR,0x1U);
	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_LSI)==EN_CLK_NOT_READY);
}
#endif


#if(PLL_I2S_USED == STD_ON)

void RCC_CalculatePllI2SPrescaler(Pll_Config_Type* config,Pll_PreScalerType* returnVal)
{
	uint32_t u32_PllIpFreq = 0U;
	uint32_t u32_PllOpFreq = 0U;
	uint8_t  u8_PrescalerI2sN = 50U;
	uint8_t  u8_PrescalerR = 2U;

	uint8_t u8_OperationCompleteFlag = FALSE;

	/** Get source for PLL clk */
	if((((REG_READ32(RCC_PLLCFGR))&(0x00400000))>>22U)==(0U))
	{/** HSI Selected */
		u32_PllIpFreq = (uint32_t)((16000000U));
		u32_PllOpFreq = config->PllOutputFreq_PLLI2S;
	}
	else
	{/** HSE Selected */
		u32_PllIpFreq = ((config->PllInputFreq)/returnVal->PreScaler_M);
		u32_PllOpFreq = config->PllOutputFreq_PLLI2S;
	}

	for(u8_PrescalerI2sN = 50U; u8_PrescalerI2sN<=432; u8_PrescalerI2sN++)
	{
		u8_PrescalerR = 2U;
		while( ((u32_PllIpFreq*u8_PrescalerI2sN)/(u8_PrescalerR*(returnVal->PreScaler_M))) > u32_PllOpFreq )
		{
			u8_PrescalerR++;
		}

		if( ((u32_PllIpFreq*(u8_PrescalerI2sN))/(u8_PrescalerR*(returnVal->PreScaler_M))) == u32_PllOpFreq )
		{
			if(u8_PrescalerR<8U)
			{
				u8_OperationCompleteFlag = TRUE;
			}
		}

		if(u8_OperationCompleteFlag == TRUE)
		{
			returnVal->PreScaler_I2SN = u8_PrescalerI2sN;
			returnVal->PreScaler_R = u8_PrescalerR;
			break;
		}
	}

	if(u8_OperationCompleteFlag == FALSE)
	{
		/** Error No valid prescaler found */
	}

}

void RCC_PllI2SConfigure(Pll_Config_Type* config)
{
	/** Disable PLLI2S */
	REG_RMW32(RCC_CR,MASK_BIT(26U),CLEAR_BIT(26U));

	/** Select source for PLLI2S*/
	RCC_SetClockSource(EN_PLL_I2S,config->PLLI2S_ClkSource);

	if((config->PLLI2S_ClkSource)==PLL_I2S_CLK)
	{
		RCC_CalculatePllI2SPrescaler(config,&PllPreScalerValues);

		/** Write values to the register */
		REG_RMW32(RCC_PLLI2SCFGR,0x00007FC0, (PllPreScalerValues.PreScaler_I2SN)<<6U );
		REG_RMW32(RCC_PLLI2SCFGR,0x70000000, (PllPreScalerValues.PreScaler_R)<<28U );
	}

	/** Enable PLLI2S */
	//REG_RMW32(RCC_CR,MASK_BIT(26U),SET_BIT(26U));
	RCC_EnableClk(EN_PLL_I2S);

	/** Wait until the clk is ready */
	while(RCC_GetClockReadyStatus(EN_PLL_I2S)==EN_CLK_NOT_READY);
}
#endif /*#if(PLL_I2S_USED == STD_ON)*/

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

		case EN_LSI:
			REG_RMW32(RCC_CSR,MASK_BIT(1U),SET_BIT(1U));
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

			case EN_PLL:
				REG_RMW32(RCC_PLLCFGR,0x00400000,(source)<<22U);
				break;

			case EN_SYS_CLK:
				REG_RMW32(RCC_CFGR,0x00000003, (source) );
				break;

			case EN_HSE:
				REG_RMW32(RCC_CR,MASK_BIT(18U),(source)<<18U);
				break;

			default:
				return;
		}
}

#ifdef RTC_CLOCK_USED
/*NOT TESTED*/
void RCC_RtcConfig(Rtc_Config_Type* config)
{
	if( ((config->RtcClkSource)== LSE_OSCILLATOR) || ((config->RtcClkSource)== LSE_EXTERNAL_CLK))
	{
		REG_RMW32(RCC_BDCR,0x00000300, ((0x1U)<<8U) );
		if(((config->RtcClkSource)== LSE_EXTERNAL_CLK))
		{
			REG_RMW32(RCC_BDCR,0x00000004, SET_BIT(2) );
		}
		/** Turn on LSE */
		REG_RMW32(RCC_BDCR,0x00000001, ((0x1U)<<0U) );
		/** Wait until the clk is ready */
		while(RCC_GetClockReadyStatus(EN_LSE)==EN_CLK_NOT_READY);
	}
	/** if HSE is being used then 1MHz clock needs to be supplied and LSE needs to run in bypass mode */
#ifdef(HSE_CLOCK_USED)
	else if((config->RtcClkSource)== EN_HSE)
	{
		uint32_t HseOutputClkFreq = GlobalConfig->HseConfig->HseOutputFreq;
		if(HseOutputClkFreq%(1000000U)== 0U)
		{
			uint8_t RtcPreScaler = HseOutputClkFreq/(1000000U);
			REG_RMW32(RCC_CFGR,0x001F0000, (RtcPreScaler<<16U) );
			REG_RMW32(RCC_BDCR,0x00000300, ((0x3U)<<8U) );
		}
		else
		{
			/** Error*/
		}
	}
#endif
	else if((config->RtcClkSource)== EN_LSI)
	{
		REG_RMW32(RCC_BDCR,0x00000300, SET_BIT(15) );
	}
	else
	{
		/** No clock */
	}

	/** Turn on RTC */
	REG_RMW32(RCC_BDCR,0x00008000, ((0x3U)<<8U) );
}
#endif /*#ifdef RTC_CLOCK_USED*/

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

void RCC_ResetBackupDomain(void)
{
	REG_RMW32(RCC_BDCR,SET_BIT(16U) ,SET_BIT(16U) );
}

Reset_Status_Type RCC_GetResetStatus(void)
{
	uint8_t u8_returnValue=0U;
	uint32_t u32_flag = (uint32_t)(REG_READ32(RCC_CSR)&0xFE000000);
	while(u32_flag!=0U)
	{
		u8_returnValue++;
		u32_flag = u32_flag>>1U;
	}
	REG_RMW32(RCC_CSR,MASK_BIT(24U) ,SET_BIT(24U) );
	return (Reset_Status_Type)(u8_returnValue);
}









