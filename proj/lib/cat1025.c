#include "i2c.h"

#define CAT1025_ADDRESS		(0x50)

int cat1025_read(uint32_t address, uint8_t *data, uint32_t nb)
{
	data[0] = address;
	return i2c_write_read(CAT1025_ADDRESS, data, 1, nb);
}

int cat1025_write(uint32_t address, uint8_t *data, uint32_t nb)
{
	int i;
	uint8_t buffer[17];
	
	if (nb>16) nb=16;
	buffer[0] = address;
	for (i=1;i<nb+1;i++) buffer[i]=data[i-1];
	
	return i2c_write(CAT1025_ADDRESS, buffer, nb+1);
}
