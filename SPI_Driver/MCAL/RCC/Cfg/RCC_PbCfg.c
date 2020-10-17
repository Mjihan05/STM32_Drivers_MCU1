/**********************************************
 * File - RCC_PbCfg.c
 * Author - Michael Jihan
 * Created on 06/June/2020
 *
 * Purpose - This files holds the Configuration
 * 			 settings for the RCC modules
 *
 * ********************************************/

#include <stdint.h>
#include "RCC_PbCfg.h"

#ifdef HSE_CLOCK_USED
Hse_Config_Type HseConfig0 =
{
	.HseClockType = HSE_EXTERNAL_CLK,
	.HseOutputFreq = (4000000U),
};
#endif

//#ifdef(HSI_CLOCK_USED)
Hsi_Config_Type HsiConfig0 =
{
	.HsiTrimValue = (17U),
};
//#endif

#ifdef PLL_CLOCK_USED
Pll_Config_Type PllConfig0 =
{
	.PllClkSource = (PLL_SRC_HSI),
	.PllInputFreq =          (16000000U),
	.PllOutputFreq_SYSCLK =  (144000000U),
	.PllOutputFreq_PLL48CK = (48000000U),
#if(PLL_I2S_USED == STD_ON)
	.PllOutputFreq_PLLI2S =  (96000000U),
	.PLLI2S_ClkSource = PLL_I2S_CLK,
#endif
};
#endif

#ifdef RTC_USED
Rtc_Config_Type RtcConfig0 =
{
	.RtcClkSource = (EN_HSE),
};
#endif

Bus_Config_Type BusConfig0 =
{
	.APB1_PreScaler = (EN_AHB_CLK_DIVIDED_BY_4),
	.APB2_PreScaler = (EN_AHB_CLK_DIVIDED_BY_2),
	.AHB_PreScaler = (EN_SYS_CLK_DIVIDED_BY_2)
};

RCC_GlobalConfigType RCC_Config0 =
{
#ifdef HSE_CLOCK_USED
	.HseConfig = &HseConfig0,
#endif
//#ifdef(HSI_CLOCK_USED)
	.HsiConfig = (&HsiConfig0),
//#endif
#ifdef PLL_CLOCK_USED
	.PllConfig = &PllConfig0,
#endif
#ifdef RTC_USED
	.RtcConfig = &RtcConfig0,
#endif
	.BusConfig = &BusConfig0,
};
