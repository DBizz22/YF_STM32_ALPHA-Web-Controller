#include "my_i2c.h"

void initLEDS(void)
{
	i2cCONFIG(I2C2_CH);
	i2c2WRITE(0x48,0x05);
	i2c2WRITE(0x70,0xff);
	i2c2WRITE(0x72,0xff);
	i2c2WRITE(0x74,0xff);
	i2c2WRITE(0x76,0xff);
}

void led_display(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4)
{
	i2c2WRITE(0x70,d1);
	i2c2WRITE(0x72,d2);
	i2c2WRITE(0x74,d3);
	i2c2WRITE(0x76,d4);	
}
