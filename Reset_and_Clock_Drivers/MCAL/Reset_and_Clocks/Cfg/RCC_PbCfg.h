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

#ifdef(PLL_CLOCK_USED)
#define PLL_I2S_USED STD_OFF
#endif


#define HSE_OSCILLATOR   (0x0U)
#define HSE_EXTERNAL_CLK (0x1U)

#define LSE_OSCILLATOR   (0x0U)
#define LSE_EXTERNAL_CLK (0x1U)

#define PLL_I2S_CLK      (0x0U)
#define PLL_I2S_EXTERNAL_CLK (0x1U)

typedef enum
{
	EN_AHB_CLK_NOT_DIVIDED =0U,
	EN_AHB_CLK_DIVIDED_BY_2 =4U,
	EN_AHB_CLK_DIVIDED_BY_4,
	EN_AHB_CLK_DIVIDED_BY_8,
	EN_AHB_CLK_DIVIDED_BY_16,
	EN_SYS_CLK_NOT_DIVIDED = 0U,
	EN_SYS_CLK_DIVIDED_BY_2 = 8U,
	EN_SYS_CLK_DIVIDED_BY_4,
	EN_SYS_CLK_DIVIDED_BY_8,
	EN_SYS_CLK_DIVIDED_BY_16,
	EN_SYS_CLK_DIVIDED_BY_64,
	EN_SYS_CLK_DIVIDED_BY_128,
	EN_SYS_CLK_DIVIDED_BY_256,
	EN_SYS_CLK_DIVIDED_BY_512,
} Bus_PreScalers ;

#ifdef(HSE_CLOCK_USED)
typedef struct
{
	uint8_t HseClockType;
	uint32_t HseOutputFreq;
}Hse_Config_Type;
#endif

//#ifdef(HSI_CLOCK_USED)
typedef struct
{
	uint8_t HsiTrimValue;
}Hsi_Config_Type;
//#endif

#ifdef(PLL_CLOCK_USED)
typedef struct
{
	uint8_t PllClkSource;
	uint32_t PllInputFreq;
	uint32_t PllOutputFreq_SYSCLK;
	uint32_t PllOutputFreq_PLL48CK;
#if(PLL_I2S_USED == STD_ON)
	uint32_t PllOutputFreq_PLLI2S;
	uint8_t PLLI2S_ClkSource;
#endif
}Pll_Config_Type;
#endif

#ifdef(RTC_USED)
typedef struct
{
//#ifdef(HSE_CLOCK_USED)
	uint32_t RtcClkSource;
//#endif
}Rtc_Config_Type;
#endif

typedef struct
{
	uint32_t APB1_PreScaler;
	uint32_t APB2_PreScaler;
	uint32_t AHB_PreScaler;
}Bus_Config_Type;

typedef struct
{
#ifdef(HSE_CLOCK_USED)
	Hse_Config_Type HseConfig;
#endif
//#ifdef(HSI_CLOCK_USED)
	Hsi_Config_Type HsiConfig;
//#endif
#ifdef(PLL_CLOCK_USED)
	Pll_Config_Type PllConfig;
#endif
#ifdef(RTC_USED)
	Rtc_Config_Type RtcConfig;
#endif
	Bus_Config_Type BusConfig;
}RCC_GlobalConfigType;


