/**********************************************
 * File - I2C_regTypes.h
 * Author - Michael Jihan
 * Created on 30-Jan-2021
 *
 * Purpose - Holds the register definitions
 *
 * ********************************************/

#ifndef I2C_INC_I2C_REGTYPES_H_
#define I2C_INC_I2C_REGTYPES_H_

#define I2C_1_BASE_ADDRESS      (0x40005400U)
#define I2C_2_BASE_ADDRESS      (0x40005800U)
#define I2C_3_BASE_ADDRESS      (0x40005C00U)

extern uint32_t I2c_BaseAddress[];

typedef union I2C_CR1_Tag
{
	struct {
		uint16_t PE:1;
		uint16_t SMBUS:1;
		uint16_t reserved1:1;
		uint16_t SMBTYPE:1;
		uint16_t ENARP:1;
		uint16_t ENPEC:1;
		uint16_t ENGC:1;
		uint16_t NOSTRETCH:1;
		uint16_t START:1;
		uint16_t STOP:1;
		uint16_t ACK:1;
		uint16_t POS:1;
		uint16_t PEC:1;
		uint16_t ALERT:1;
		uint16_t reserved2:1;
		uint16_t SWRST:1;
	}B;
	uint16_t R;
}I2C_CR1_Type;

typedef union I2C_CR2_Tag
{
	struct {
		uint16_t FREQ:6;
		uint16_t reserved1:2;
		uint16_t ITERREN:1;
		uint16_t ITEVTEN:1;
		uint16_t ITBUFEN:1;
		uint16_t DMAEN:1;
		uint16_t LAST:1;
		uint16_t reserved2:3;
	}B;
	uint16_t R;
}I2C_CR2_Type;

typedef union I2C_CR2_Tag
{
	struct {
		uint16_t FREQ:6;
		uint16_t reserved1:2;
		uint16_t ITERREN:1;
		uint16_t ITEVTEN:1;
		uint16_t ITBUFEN:1;
		uint16_t DMAEN:1;
		uint16_t LAST:1;
		uint16_t reserved2:3;
	}B;
	uint16_t R;
}I2C_CR2_Type;

typedef union I2C_OAR1_Tag
{
	struct {
		uint16_t ADD0:1;
		uint16_t ADD:9;
		uint16_t reserved1:4;
		uint16_t HIGHBIT:1;
		uint16_t ADDMODE:1;
	}B;
	uint16_t R;
}I2C_OAR1_Type;

typedef union I2C_OAR2_Tag
{
	struct {
		uint16_t ENDUAL:1;
		uint16_t ADD2:7;
		uint16_t reserved1:8;
	}B;
	uint16_t R;
}I2C_OAR2_Type;

typedef union I2C_DR_Tag
{
	struct {
		uint16_t DR:8;
		uint16_t reserved1:8;
	}B;
	uint16_t R;
}I2C_DR_Type;

typedef union I2C_SR1_Tag
{
	struct {
		uint16_t SB:1;
		uint16_t ADDR:1;
		uint16_t BTF:1;
		uint16_t ADD10:1;
		uint16_t STOPF:1;
		uint16_t reserved1:1;
		uint16_t RXNE:1;
		uint16_t TXE:1;
		uint16_t BERR:1;
		uint16_t ARLO:1;
		uint16_t AF:1;
		uint16_t OVR:1;
		uint16_t PECERR:1;
		uint16_t reserved2:1;
		uint16_t TIMEOUT:1;
		uint16_t SMBALERT:1;
	}B;
	uint16_t R;
}I2C_SR1_Type;

typedef union I2C_SR2_Tag
{
	struct {
		uint16_t MSL:1;
		uint16_t BUSY:1;
		uint16_t TRA:1;
		uint16_t reserved1:1;
		uint16_t GENCALL:1;
		uint16_t SMBDEFAULT:1;
		uint16_t SMBHOST:1;
		uint16_t DUALF:1;
		uint16_t PEC:8;
	}B;
	uint16_t R;
}I2C_SR2_Type;

typedef union I2C_CCR_Tag
{
	struct {
		uint16_t CCR:12;
		uint16_t reserved1:2;
		uint16_t DUTY:1;
		uint16_t FM_SM:1;
	}B;
	uint16_t R;
}I2C_CCR_Type;

typedef union I2C_TRISE_Tag
{
	struct {
		uint16_t TRISE:6;
		uint16_t reserved1:10;
	}B;
	uint16_t R;
}I2C_TRISE_Type;

typedef union I2C_FLTR_Tag
{
	struct {
		uint16_t DNF:4;
		uint16_t ANOFF:1;
		uint16_t reserved1:11;
	}B;
	uint16_t R;
}I2C_FLTR_Type;

typedef struct
{
	I2C_CR1_Type CR1;			/** I2C control register 1 */
	uint16_t reserved1;
	I2C_CR2_Type CR2;			/** I2C control register 2 */
	uint16_t reserved2;
	I2C_OAR1_Type OAR1;			/** I2C Own address register 1  */
	uint16_t reserved3;
	I2C_OAR2_Type OAR2;			/** I2C Own address register 2  */
	uint16_t reserved4;
	I2C_DR_Type DR;				/** I2C data register */
	uint16_t reserved5;
	I2C_SR1_Type SR1;			/** I2C status register 1 */
	uint16_t reserved6;
	I2C_SR2_Type SR2;			/** I2C status register 2 */
	uint16_t reserved7;
	I2C_CCR_Type CCR;			/** I2C Clock control register */
	uint16_t reserved8;
	I2C_TRISE_Type TRISE;		/** I2C TRISE register */
	uint16_t reserved9;
	I2C_FLTR_Type FLTR;			/** I2C FLTR register */
}I2C_RegTypes;

#define I2C_1 			(*(volatile I2C_RegTypes*)(I2C_1_BASE_ADDRESS))
#define I2C_2 			(*(volatile I2C_RegTypes*)(I2C_2_BASE_ADDRESS))
#define I2C_3 			(*(volatile I2C_RegTypes*)(I2C_3_BASE_ADDRESS))

#endif /* I2C_INC_I2C_REGTYPES_H_ */
