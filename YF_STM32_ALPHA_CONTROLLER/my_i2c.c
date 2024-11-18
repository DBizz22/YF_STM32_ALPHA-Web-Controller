#include "my_i2c.h"
#include "my_systick.h"
#include "stm32f10x.h"
#include <stdint.h>

I2C_TypeDef* getI2C(uint8_t ch)
{
    if(ch==I2C1_CH)
        return I2C1;
    else if(ch==I2C2_CH)
        return I2C2;
}

void i2cCONFIG(uint8_t ch)
{
    if(ch==I2C1_CH)
    {
        outOAF(I2C1_SCL);
        outOAF(I2C1_SDA);
        RCC->APB1ENR|=(0x01<<21);
    }
    else
    {
        outOAF(I2C2_SCL);
        outOAF(I2C2_SDA); 
        RCC->APB1ENR|=(0x01<<22);   
    }
    
    I2C_TypeDef* i2c=getI2C(ch);
    i2c->CR1|=(0x01<<15);      //reset i2c
    i2c->CR1&=~(0x01<<15);
    /*********get pclk1 clock************/
    i2c->CR2=8;                      //using 8Mhz
    i2c->CCR&=~(0x01<<15);        //   
	//i2c->CCR=(0x2D);								
    i2c->CCR|=0x28;                 //setup for 100khz scl clock
    i2c->TRISE|=0x09;
	i2c->CR1|=(0x01<<10);           //enable ACK
    i2c->CR1|=(0x01);               //enable peripheral
}

void i2cSTART(I2C_TypeDef* i2c)
{
    i2c->CR1|=(0x01<<8);            //enable start gen
    while( (i2c->SR1&(0x01))==0);       //wait to detect start gen was done
}

void i2cADDR(I2C_TypeDef* i2c,uint8_t addr)
{
    volatile int temp;
    i2c->DR=addr;                       //get addr
    while((i2c->SR1&(0x02))==0x0);      //wait for address transmission

    /*while(i2c->SR1&(0x02))
    {
        temp = i2c->SR1;
        temp=i2c->SR2;
        if((i2c->SR1&(0x02))==0x00)
            break;
    }*/
    temp=i2c->SR1;
    temp=i2c->SR2;
}

void i2cDATA(I2C_TypeDef* i2c,uint8_t byte)
{
    //volatile int temp;
    while( (i2c->SR1&(0x01<<7))==0);    //wait for data register to be empty
    i2c->DR=byte;                           //get data
    //while( (i2c->SR1&(0x01<<7))==0);    //wait for data register to be empty

    while( (i2c->SR1&(0x01<<2))==0 );   //wait for data transfer
    //temp = i2c->SR1;
    //i2c->CR1|=(0x01<<9);            //enable stop gen
}

void i2cSTOP(I2C_TypeDef* i2c)
{
    volatile int temp;
    temp=i2c->SR1;
    temp=i2c->SR2;
    i2c->CR1|=(0x01<<9);
}

void i2c1WRITE(uint8_t addr,uint8_t data)
{
    i2cSTART(I2C1);
    i2cADDR(I2C1,addr|0x00);            //LSB for read/write func
    i2cDATA(I2C1,data);
    i2cSTOP(I2C1);
}

void i2c2WRITE(uint8_t addr,uint8_t data)
{
    i2cSTART(I2C2);
    i2cADDR(I2C2,addr|0x00);            //LSB for read/write func
    i2cDATA(I2C2,data);
    i2cSTOP(I2C2);
}
