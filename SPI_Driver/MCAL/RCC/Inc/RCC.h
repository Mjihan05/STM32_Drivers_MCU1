/**********************************************
 * File - RCC.h
 * Author - Michael Jihan
 * Created on 04/June/2020
 *
 * Purpose - This files holds the Global functions
 * 			 that will configure and control
 * 			 Reset and clock settings
 *
 * ********************************************/
#ifndef RCC_H
#define RCC_H

#include "RCC_PbCfg.h"

typedef enum
{
	EN_PLL_I2S =0U,
	EN_PLL,
	EN_HSE,
	EN_HSI,
	EN_LSE,
	EN_LSI,
	EN_SYS_CLK,
} Clk_Types ;

typedef enum
{
	EN_CLK_SECURITY_INT =0U,
	EN_PLL_I2S_READY,
	EN_PLL_READY,
	EN_HSE_READY,
	EN_HSI_READY,
	EN_LSE_READY,
	EN_LSI_READY,
} Rcc_Int_Type ;

typedef enum
{
	EN_OTGHSULPI = 30U,
	EN_OTGHS = 29U,
	EN_ETHMACPTP = 28U,
	EN_ETHMACRX = 27U,
	EN_ETHMACTX = 26U,
	EN_ETHMAC = 25U,
	EN_DMA2 = 22U,
	EN_DMA1 = 21U,
	EN_CCMDATARAM = 20U,
	EN_BKPSRAM = 18U,
	EN_CRC = 12U,
	EN_GPIOI = 8U,
	EN_GPIOH = 7U,
	EN_GPIOG = 6U,
	EN_GPIOF = 5U,
	EN_GPIOE = 4U,
	EN_GPIOD = 3U,
	EN_GPIOC = 2U,
	EN_GPIOB = 1U,
	EN_GPIOA = 0U,
} Rcc_AHB1_Peripherals ;

typedef enum
{
	EN_OTGFS = 7U,
	EN_RNG = 6U,
	EN_HASH = 5U,
	EN_CRYP = 4U,
	EN_DCMI = 0U,
} Rcc_AHB2_Peripherals ;

typedef enum
{
	EN_FSMC = 0U,
} Rcc_AHB3_Peripherals ;

typedef enum
{
	EN_DAC = 29U,
	EN_PWR = 28U,
	EN_CAN2 = 26U,
	EN_CAN1 = 25U,
	EN_I2C3 = 23U,
	EN_I2C2 = 22U,
	EN_I2C1 = 21U,
	EN_UART5 = 20U,
	EN_UART4 = 19U,
	EN_UART3 = 18U,
	EN_UART2 = 17U,
	EN_SPI3 = 15U,
	EN_SPI2 = 14U,
	EN_WWDG = 11U,
	EN_TIM14 = 8U,
	EN_TIM13 = 7U,
	EN_TIM12 = 6U,
	EN_TIM7 = 5U,
	EN_TIM6 = 4U,
	EN_TIM5 = 3U,
	EN_TIM4 = 2U,
	EN_TIM3 = 1U,
	EN_TIM2 = 0U,
} Rcc_APB1_Peripherals ;

typedef enum
{
	EN_TIM11 = 18U,
	EN_TIM10 = 17U,
	EN_TIM9 = 16U,
	EN_SYSCFG = 14U,
	EN_SPI1 = 12U,
	EN_SDIO = 11U,
	EN_ADC = 8U,
	EN_USART6 = 5U,
	EN_USART1 = 4U,
	EN_TIM8 = 1U,
	EN_TIM1 = 0U,
} Rcc_APB2_Peripherals ;

typedef enum
{
	EN_CLK_NOT_READY =0U,
	EN_CLK_READY,
	EN_INVALID_CLK,
} Clk_Status_Type ;

typedef enum
{
	EN_BURNOUT_RESET =25U,
	EN_PIN_RESET,
	EN_POR_PDR_RESET,
	EN_SW_RESET,
	EN_IWDG_RESET,
	EN_WWDG_RESET,
	EN_LPWR_RESET
} Reset_Status_Type ;


typedef struct
{
	uint8_t PreScaler_M;
	uint8_t PreScaler_N;
	uint8_t PreScaler_P;
	uint8_t PreScaler_Q;
	uint8_t PreScaler_I2SN;
	uint8_t PreScaler_R;
}Pll_PreScalerType;



extern Pll_PreScalerType PllPreScalerValues;

/** Function Prototypes */
void RCC_Init(RCC_GlobalConfigType* Config );
Clk_Status_Type RCC_GetClockReadyStatus(Clk_Types En_clockType);
void RCC_EnableClk(Clk_Types clk);
void RCC_DisableClk(Clk_Types clk);
void RCC_SetClockSource(Clk_Types clk, uint8_t source);
Clk_Types RCC_GetSystemClkSource();
void RCC_Mco1Config(Clk_Types clk, uint8_t preScaler);
void RCC_Mco2Config(Clk_Types clk, uint8_t preScaler);
void RCC_BusConfig(Bus_Config_Type* config);
void RCC_ResetBackupDomain(void);
Reset_Status_Type RCC_GetResetStatus(void);

uint32_t RCC_GetSysClk(void);   /** Returns the value of Sys Clk in MHz */
uint32_t RCC_GetAHB1Clk(void);  /** Returns the value of AHB1 Clk in MHz */
uint32_t RCC_GetAPB1Clk(void); 	/** Returns the value of APB1 Clk in MHz */
uint32_t RCC_GetAPB2Clk(void);  /** Returns the value of APB2 Clk in MHz */

void RCC_EnableIntFlag(Rcc_Int_Type IntFlag);
Rcc_Int_Type RCC_ReadIntFlag(void);
void RCC_ClearIntFlag(Rcc_Int_Type IntFlag);

#ifdef HSE_CLOCK_USED
void RCC_HseConfigure(Hse_Config_Type* config);
#endif /*#ifdef HSE_CLOCK_USED*/

void RCC_HsiConfigure(Hsi_Config_Type* config);
void RCC_HsiSetTrimValue(Hsi_Config_Type* config);
uint8_t RCC_HsiGetCalibValue(void);

#ifdef PLL_CLOCK_USED
void RCC_PllConfigure(Pll_Config_Type* config);
void RCC_PllI2SConfigure(Pll_Config_Type* config);
#endif

void RCC_AHB1PeripheralClkEnable(Rcc_AHB1_Peripherals peripheral);
void RCC_AHB2PeripheralClkEnable(Rcc_AHB2_Peripherals peripheral);
void RCC_AHB3PeripheralClkEnable(Rcc_AHB3_Peripherals peripheral);
void RCC_APB1PeripheralClkEnable(Rcc_APB1_Peripherals peripheral);
void RCC_APB2PeripheralClkEnable(Rcc_APB2_Peripherals peripheral);

void RCC_AHB1EnablePerSleepMode(Rcc_AHB1_Peripherals peripheral);
void RCC_AHB2EnablePerSleepMode(Rcc_AHB2_Peripherals peripheral);
void RCC_AHB3EnablePerSleepMode(Rcc_AHB3_Peripherals peripheral);
void RCC_APB1EnablePerSleepMode(Rcc_APB1_Peripherals peripheral);
void RCC_APB2EnablePerSleepMode(Rcc_APB2_Peripherals peripheral);

void RCC_AHB1PeripheralClkDisable(Rcc_AHB1_Peripherals peripheral);
void RCC_AHB2PeripheralClkDisable(Rcc_AHB2_Peripherals peripheral);
void RCC_AHB3PeripheralClkDisable(Rcc_AHB3_Peripherals peripheral);
void RCC_APB1PeripheralClkDisable(Rcc_APB1_Peripherals peripheral);
void RCC_APB2PeripheralClkDisable(Rcc_APB2_Peripherals peripheral);

void RCC_AHB1DisablePerSleepMode(Rcc_AHB1_Peripherals peripheral);
void RCC_AHB2DisablePerSleepMode(Rcc_AHB2_Peripherals peripheral);
void RCC_AHB3DisablePerSleepMode(Rcc_AHB3_Peripherals peripheral);
void RCC_APB1DisablePerSleepMode(Rcc_APB1_Peripherals peripheral);
void RCC_APB2DisablePerSleepMode(Rcc_APB2_Peripherals peripheral);

void RCC_ResetAHB1Peripheral(Rcc_AHB1_Peripherals peripheral);
void RCC_ResetAHB2Peripheral(Rcc_AHB2_Peripherals peripheral);
void RCC_ResetAHB3Peripheral(Rcc_AHB3_Peripherals peripheral);
void RCC_ResetAPB1Peripheral(Rcc_APB1_Peripherals peripheral);
void RCC_ResetAPB2Peripheral(Rcc_APB2_Peripherals peripheral);


#endif /*#ifndef RCC_H*/

