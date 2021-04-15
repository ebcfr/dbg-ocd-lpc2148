#include "i2c.h"

#define LM75_ADDRESS		(0x48)

#define LM75_REG_TEMP		(0x00)
#define LM75_REG_CONFIG		(0x01)
#define LM75_REG_THYST		(0x02)
#define LM75_REG_TOS		(0x03)
#define LM75_REG_NONE		(0x04)

static uint8_t last_reg = LM75_REG_NONE;

int  lm75_read_temp(int *temp)
{
	uint8_t buffer [2];
	int st;

	if (last_reg!=LM75_REG_TEMP) {
		buffer [0] = last_reg = LM75_REG_TEMP;
		st = i2c_write_read(LM75_ADDRESS, buffer, 1, 2);
	} else {
		st = i2c_read(LM75_ADDRESS, buffer, 2);
	}
	
	if (st==I2C_OK) {
		//  Sign extend negative numbers
		*temp = ((buffer [0] << 8) | buffer [1]) >> 7;

		if (buffer [0] & 0x80) *temp |= 0xfffffe00;
	} else {
		*temp=0;
	}
	return st;
}

int  lm75_read_thyst(int *thyst)
{
	uint8_t buffer [2];
	int st;

	if (last_reg!=LM75_REG_THYST) {
		buffer [0] = last_reg = LM75_REG_THYST;
		st = i2c_write_read(LM75_ADDRESS, buffer, 1, 2);
	} else {
		st = i2c_read(LM75_ADDRESS, buffer, 2);
	}
	
	if (st==I2C_OK) {
		//  Sign extend negative numbers
		*thyst = ((buffer [0] << 8) | buffer [1]) >> 7;

		if (buffer [0] & 0x80) *thyst |= 0xfffffe00;
	} else {
		*thyst=0;
	}
	return st;
}

int  lm75_read_tos(int *tos)
{
	uint8_t buffer [2];
	int st;

	if (last_reg!=LM75_REG_TOS) {
		buffer [0] = last_reg = LM75_REG_TOS;
		st = i2c_write_read(LM75_ADDRESS, buffer, 1, 2);
	} else {
		st = i2c_read(LM75_ADDRESS, buffer, 2);
	}
	
	if (st==I2C_OK) {
		//  Sign extend negative numbers
		*tos = ((buffer [0] << 8) | buffer [1]) >> 7;

		if (buffer [0] & 0x80) *tos |= 0xfffffe00;
	} else {
		*tos=0;
	}
	return st;
}

int  lm75_write_thyst(int thyst)
{
	uint8_t buffer [3];
	
	buffer [0] = last_reg = LM75_REG_THYST;
	buffer [1] = thyst >> 1;
	buffer [2] = thyst << 7;
	
	return i2c_write(LM75_ADDRESS, buffer, 3);
}

int  lm75_write_tos(int tos)
{
	uint8_t buffer [3];
	
	buffer [0] = last_reg = LM75_REG_TOS;
	buffer [1] = tos >> 1;
	buffer [2] = tos << 7;
	
	return i2c_write(LM75_ADDRESS, buffer, 3);
}

