/**********************************************
 * File - SPI_regTypes.h
 * Author - Michael Jihan
 * Created on Oct 17, 2020
 *
 * Purpose - Holds the register definitions
 *
 * ********************************************/

#ifndef SPI_INC_SPI_REGTYPES_H_
#define SPI_INC_SPI_REGTYPES_H_

#define SPI_1_BASE_ADDRESS      (0x40013000U)
#define SPI_2_BASE_ADDRESS      (0x40003800U)
#define SPI_3_BASE_ADDRESS      (0x40003C00U)
#define SPI_4_BASE_ADDRESS      (0x40013400U) /** Not supported in F407*/
#define SPI_5_BASE_ADDRESS      (0x40015000U) /** Not supported in F407*/
#define SPI_6_BASE_ADDRESS      (0x40015400U) /** Not supported in F407*/

extern uint32_t Spi_BaseAddress[];

typedef union SPI_CR1_Tag
{
	struct {
		uint16_t CPHA:1;
		uint16_t CPOL:1;
		uint16_t MSTR:1;
		uint16_t BR:3;
		uint16_t SPE:1;
		uint16_t LSBFIRST:1;
		uint16_t SSI:1;
		uint16_t SSM:1;
		uint16_t RXONLY:1;
		uint16_t DFF:1;
		uint16_t CRCNEXT:1;
		uint16_t CRCEN:1;
		uint16_t BIDIOE:1;
		uint16_t BIDIMODE:1;
	}B;
	uint16_t R;
}SPI_CR1_Type;

typedef union SPI_CR2_Tag
{
	struct {
		uint16_t RXDMAEN:1;
		uint16_t TXDMAEN:1;
		uint16_t SSOE:1;
		uint16_t reserved1:1;
		uint16_t FRF:1;
		uint16_t ERRIE:1;
		uint16_t RXNEIE:1;
		uint16_t TXEIE:1;
		uint16_t reserved2:8;
	}B;
	uint16_t R;
}SPI_CR2_Type;

typedef union SPI_SR_Tag
{
	struct {
		uint16_t RXNE:1;
		uint16_t TXE:1;
		uint16_t CHSIDE:1;
		uint16_t UDR:1;
		uint16_t CRCERR:1;
		uint16_t MODF:1;
		uint16_t OVR:1;
		uint16_t BSY:1;
		uint16_t FRE:1;
		uint16_t reserved1:7;
	}B;
	uint16_t R;
}SPI_SR_Type;

typedef union SPI_DR_Tag
{
	struct {
		uint16_t DR:16;
	}B;
	uint16_t R;
}SPI_DR_Type;

typedef union SPI_CRCPR_Tag
{
	struct {
		uint16_t CRCPOLY:16;
	}B;
	uint16_t R;
}SPI_CRCPR_Type;

typedef union SPI_RXCRCR_Tag
{
	struct {
		uint16_t RXCRC:16;
	}B;
	uint16_t R;
}SPI_RXCRCR_Type;

typedef union SPI_TXCRCR_Tag
{
	struct {
		uint16_t TXCRC:16;
	}B;
	uint16_t R;
}SPI_TXCRCR_Type;

typedef struct
{
	SPI_CR1_Type CR1;			/** SPI control register 1 */
	uint16_t reserved1;
	SPI_CR2_Type CR2;			/** SPI control register 2 */
	uint16_t reserved2;
	SPI_SR_Type SR;				/** SPI status register */
	uint16_t reserved3;
	SPI_DR_Type DR;				/** SPI data register */
	uint16_t reserved4;
	SPI_CRCPR_Type CRCPR;		/** SPI CRC polynomial register */
	uint16_t reserved5;
	SPI_RXCRCR_Type RXCRCR;		/** SPI RX CRC register */
	uint16_t reserved6;
	SPI_TXCRCR_Type TXCRCR; 	/** SPI TX CRC register */
}SPI_RegTypes;

#define SPI_1 			(*(volatile SPI_RegTypes*)(SPI_1_BASE_ADDRESS))
#define SPI_2 			(*(volatile SPI_RegTypes*)(SPI_2_BASE_ADDRESS))
#define SPI_3 			(*(volatile SPI_RegTypes*)(SPI_3_BASE_ADDRESS))
#define SPI_4 			(*(volatile SPI_RegTypes*)(SPI_4_BASE_ADDRESS))
#define SPI_5 			(*(volatile SPI_RegTypes*)(SPI_5_BASE_ADDRESS))
#define SPI_6 			(*(volatile SPI_RegTypes*)(SPI_6_BASE_ADDRESS))

#endif /* SPI_INC_SPI_REGTYPES_H_ */
