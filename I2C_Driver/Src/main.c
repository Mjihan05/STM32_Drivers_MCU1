/**
  ******************************************************************************
  * @file    main.c
  * @author  Auto-generated by STM32CubeIDE
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif



#include "Reg_Macros.h"
#include "RCC.h"
#include "Intc.h"
#include "Gpio.h"
#include "Dio.h"
#include "I2C.h"
#include "string.h"

#define CHECK_RETURN_VALUE(x)  while(x)

extern Std_ReturnType sI2C_InitHwModule(I2C_HwConfigType* HwConfig);
extern Std_ReturnType sI2C_MasterTxData(I2C_HwType ModuleNo,I2C_DataBufferType* DataBufferPtr,
		I2C_NumberOfDataType noOfBytesRemaining,uint16_t SlaveAddress,Bool RepeatedStart);
extern Std_ReturnType sI2C_MasterRxData(I2C_HwType ModuleNo,I2C_DataBufferType* DataBufferPtr,
		I2C_NumberOfDataType noOfBytesRemaining,uint16_t SlaveAddress,Bool RepeatedStart);
extern const I2C_HwConfigType I2C_HwConfig0[NO_OF_HW_CONFIGURED];

/** Pin Definition I2C 1
 * PB6 - I2C1_SCL
 * PB7 - I2C1_SDA
 * */

uint8_t myName[] = "Michael Jihan";
uint8_t myRxBuffer[30];

void delay()
{
	for(volatile uint32_t j =0; j<2; j++)
	{
		for(volatile uint32_t i =0; i<500000;)
		{
			  i++;
		}
	}

}

int main(void)
{
	Std_ReturnType returnValue = E_NOT_OK;

	RCC_Init(&RCC_Config0);
	Port_Init (&Port_Config0);

	while(1)
	{
		if(Dio_ReadChannel(GPIO_A_PIN_0) == 0x1U)
		{
			returnValue = sI2C_InitHwModule((I2C_HwConfigType*)I2C_HwConfig0);
			CHECK_RETURN_VALUE(returnValue);

			uint8_t commandValue = 0x51;
			returnValue = sI2C_MasterTxData(I2C_HwConfig0[0U].ModuleNo,(I2C_DataBufferType*)(&commandValue),1U,0x68,STD_ON);
			CHECK_RETURN_VALUE(returnValue);

			uint8_t u8_length = 0U;
			returnValue = sI2C_MasterRxData(I2C_HwConfig0[0U].ModuleNo,(I2C_DataBufferType*)(&u8_length),1U,0x68,STD_ON);
			CHECK_RETURN_VALUE(returnValue);

			commandValue = 0x52;
			returnValue = sI2C_MasterTxData(I2C_HwConfig0[0U].ModuleNo,(I2C_DataBufferType*)(&commandValue),1U,0x68,STD_ON);
			CHECK_RETURN_VALUE(returnValue);

			returnValue = sI2C_MasterRxData(I2C_HwConfig0[0U].ModuleNo,(I2C_DataBufferType*)(myRxBuffer),u8_length,0x68,STD_OFF);
			CHECK_RETURN_VALUE(returnValue);

			delay();
		}
	}

	for(;;);
}


















