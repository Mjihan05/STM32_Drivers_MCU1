/**********************************************
 * File - DMA_regTypes.h
 * Author - Michael Jihan
 * Created on Mar 12, 2022
 *
 * Purpose - Holds the register definitions
 *
 * ********************************************/

#ifndef MCAL_DMA_INC_DMA_REGTYPES_H_
#define MCAL_DMA_INC_DMA_REGTYPES_H_

#define DMA_1_BASE_ADDRESS      (0x40026000U)
#define DMA_2_BASE_ADDRESS      (0x40026400U)

typedef union DMA_LISR_Tag
{
	struct {
		uint32_t FEIF0:1;
		uint32_t reserved1:1;
		uint32_t DMEIF0:1;
		uint32_t TEIF0:1;
		uint32_t HTIF0:1;
		uint32_t TCIF0:1;
		uint32_t FEIF1:1;
		uint32_t reserved2:1;
		uint32_t DMEIF1:1;
		uint32_t TEIF1:1;
		uint32_t HTIF1:1;
		uint32_t TCIF1:1;
		uint32_t reserved3:4;
		uint32_t FEIF2:1;
		uint32_t reserved4:1;
		uint32_t DMEIF2:1;
		uint32_t TEIF2:1;
		uint32_t HTIF2:1;
		uint32_t TCIF2:1;
		uint32_t FEIF3:1;
		uint32_t reserved5:1;
		uint32_t DMEIF3:1;
		uint32_t TEIF3:1;
		uint32_t HTIF3:1;
		uint32_t TCIF3:1;
		uint32_t reserved6:4;
	}B;
	uint32_t R;
}DMA_LISR_Type;

typedef union DMA_LISR_Tag
{
	struct {
		uint32_t FEIF4:1;
		uint32_t reserved1:1;
		uint32_t DMEIF4:1;
		uint32_t TEIF4:1;
		uint32_t HTIF4:1;
		uint32_t TCIF4:1;
		uint32_t FEIF5:1;
		uint32_t reserved2:1;
		uint32_t DMEIF5:1;
		uint32_t TEIF5:1;
		uint32_t HTIF5:1;
		uint32_t TCIF5:1;
		uint32_t reserved3:4;
		uint32_t FEIF5:1;
		uint32_t reserved4:1;
		uint32_t DMEIF6:1;
		uint32_t TEIF6:1;
		uint32_t HTIF6:1;
		uint32_t TCIF6:1;
		uint32_t FEIF6:1;
		uint32_t reserved5:1;
		uint32_t DMEIF7:1;
		uint32_t TEIF7:1;
		uint32_t HTIF7:1;
		uint32_t TCIF7:1;
		uint32_t reserved6:4;
	}B;
	uint32_t R;
}DMA_HISR_Type;


#endif /* MCAL_DMA_INC_DMA_REGTYPES_H_ */
