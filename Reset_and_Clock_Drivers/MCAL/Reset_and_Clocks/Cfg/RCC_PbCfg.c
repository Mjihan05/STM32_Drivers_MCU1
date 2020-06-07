/**********************************************
 * File - RCC_PbCfg.c
 * Author - Michael Jihan
 * Created on 06/June/2020
 *
 * Purpose - This files holds the Configuration
 * 			 settings for the RCC modules
 *
 * ********************************************/

#include "RCC_PbCfg.h"

#ifdef(HSE_CLOCK_USED)
Hse_Config_Type HseConfig0 =
{
	.HseClockType = HSE_OSCILLATOR,
};
#endif

#ifdef(HSI_CLOCK_USED)
Hsi_Config_Type HsiConfig0 =
{
	.HsiTrimValue = (0U),
};
#endif

#ifdef(PLL_CLOCK_USED)
Pll_Config_Type PllConfig0 =
{
	.PllClkSource = (EN_HSI),
	.PllInputFreq =          (16000000U),
	.PllOutputFreq_SYSCLK =  (168000000U),
	.PllOutputFreq_PLL48CK = (48000000U),
	.PllOutputFreq_PLLI2S =  (96000000U),
};
#endif

RCC_GlobalConfigType RCC_Config0 =
{
#ifdef(HSE_CLOCK_USED)
	.HseConfig = &HseConfig0,
#endif
#ifdef(HSI_CLOCK_USED)
	.HsiConfig = &HsiConfig0,
#endif
#ifdef(PLL_CLOCK_USED)
	.PllConfig = &PllConfig0,
#endif
};
