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


typedef enum
{
	EN_PLL_I2S =0U,
	EN_PLL,
	EN_HSE,
	EN_HSI,
} Clk_Types ;

typedef enum
{
	EN_CLK_NOT_READY =0U,
	EN_CLK_READY,
	EN_INVALID_CLK,
} Clk_Status_Type ;


typedef struct
{
	uint8_t PreScaler_M;
	uint8_t PreScaler_N;
	uint8_t PreScaler_P;
	uint8_t PreScaler_Q;
	uint8_t PreScaler_R;
}Pll_PreScalerType;



extern Pll_PreScalerType PllPreScalerValues;



