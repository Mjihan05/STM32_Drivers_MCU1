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

typedef union DMA_LIFCR_Tag
{
	struct {
		uint32_t CFEIF0:1;
		uint32_t reserved1:1;
		uint32_t CDMEIF0:1;
		uint32_t CTEIF0:1;
		uint32_t CHTIF0:1;
		uint32_t CTCIF0:1;
		uint32_t CFEIF1:1;
		uint32_t reserved2:1;
		uint32_t CDMEIF1:1;
		uint32_t CTEIF1:1;
		uint32_t CHTIF1:1;
		uint32_t CTCIF1:1;
		uint32_t reserved3:4;

		uint32_t CFEIF2:1;
		uint32_t reserved4:1;
		uint32_t CDMEIF2:1;
		uint32_t CTEIF2:1;
		uint32_t CHTIF2:1;
		uint32_t CTCIF2:1;
		uint32_t CFEIF3:1;
		uint32_t reserved5:1;
		uint32_t CDMEIF3:1;
		uint32_t CTEIF3:1;
		uint32_t CHTIF3:1;
		uint32_t CTCIF3:1;
		uint32_t reserved6:4;
	}B;
	uint32_t R;
}DMA_LIFCR_Type;

typedef union DMA_HIFCR_Tag
{
	struct {
		uint32_t CFEIF4:1;
		uint32_t reserved1:1;
		uint32_t CDMEIF4:1;
		uint32_t CTEIF4:1;
		uint32_t CHTIF4:1;
		uint32_t CTCIF4:1;
		uint32_t CFEIF5:1;
		uint32_t reserved2:1;
		uint32_t CDMEIF5:1;
		uint32_t CTEIF5:1;
		uint32_t CHTIF5:1;
		uint32_t CTCIF5:1;
		uint32_t reserved3:4;

		uint32_t CFEIF6:1;
		uint32_t reserved4:1;
		uint32_t CDMEIF6:1;
		uint32_t CTEIF6:1;
		uint32_t CHTIF6:1;
		uint32_t CTCIF6:1;
		uint32_t CFEIF7:1;
		uint32_t reserved5:1;
		uint32_t CDMEIF7:1;
		uint32_t CTEIF7:1;
		uint32_t CHTIF7:1;
		uint32_t CTCIF7:1;
		uint32_t reserved6:4;
	}B;
	uint32_t R;
}DMA_HIFCR_Type;

typedef union DMA_SxCR_Tag
{
	struct {
		uint32_t EN:1;
		uint32_t DMEIE:1;
		uint32_t TEIE:1;
		uint32_t HTIE:1;
		uint32_t TCIE:1;
		uint32_t PFCTRL:1;
		uint32_t DIR:2;
		uint32_t CIRC:1;
		uint32_t PINC:1;
		uint32_t MINC:1;
		uint32_t PSIZE:2;
		uint32_t MSIZE:2;
		uint32_t PINCOS:1;

		uint32_t PL:2;
		uint32_t DBM:1;
		uint32_t CT:1;
		uint32_t reserved1:1;
		uint32_t PBURST:2;
		uint32_t MBURST:2;
		uint32_t CHSEL:3;
		uint32_t reserved2:4;
	}B;
	uint32_t R;
}DMA_SxCR_Type;

typedef union DMA_SxNDTR_Tag
{
	struct {
		uint32_t NDT:16;
		uint32_t reserved1:16;
	}B;
	uint32_t R;
}DMA_SxNDTR_Type;

typedef union DMA_SxPAR_Tag
{
	uint32_t R;
}DMA_SxPAR_Type;

typedef union DMA_SxM0AR_Tag
{
	uint32_t R;
}DMA_SxM0AR_Type;

typedef union DMA_SxM1AR_Tag
{
	uint32_t R;
}DMA_SxM1AR_Type;

typedef union DMA_SxFCR_Tag
{
	struct {
		uint32_t FTH:2;
		uint32_t DMDIS:1;
		uint32_t FS:3;
		uint32_t reserved1:1;
		uint32_t FEIE:1;
		uint32_t reserved2:24;
	}B;
	uint32_t R;
}DMA_SxFCR_Type;


#endif /* MCAL_DMA_INC_DMA_REGTYPES_H_ */
