/**********************************************
 * File - RCC_PbCfg.h
 * Author - Michael Jihan
 * Created on 06/June/2020
 *
 * Purpose - This files holds the Configuration
 * 			 settings for the RCC modules
 *
 * ********************************************/


#ifdef(HSE_CLOCK_USED)
	#define HSE_CLK_SECURITY STD_OFF
#endif






#define HSE_OSCILLATOR   (0x0U)
#define HSE_EXTERNAL_CLK (0x1U)

#ifdef(HSE_CLOCK_USED)
typedef struct
{
	uint8_t HseClockType;
}Hse_Config_Type;
#endif

#ifdef(HSI_CLOCK_USED)
typedef struct
{
	uint8_t HsiTrimValue;
}Hsi_Config_Type;
#endif

#ifdef(PLL_CLOCK_USED)
typedef struct
{
	uint8_t PllClkSource;
	uint32_t PllInputFreq;
	uint32_t PllOutputFreq_SYSCLK;
	uint32_t PllOutputFreq_PLL48CK;
	uint32_t PllOutputFreq_PLLI2S;
}Pll_Config_Type;
#endif

typedef struct
{
#ifdef(HSE_CLOCK_USED)
	Hse_Config_Type HseConfig;
#endif
#ifdef(HSI_CLOCK_USED)
	Hsi_Config_Type HsiConfig;
#endif
#ifdef(PLL_CLOCK_USED)
	Pll_Config_Type PllConfig;
#endif
}RCC_GlobalConfigType;


