#ifndef MY_ADC_H_
#define MY_ADC_H_
#include "stm32f10x.h"
#include "my_gpio.h"
#include <stdint.h>
#include "my_systick.h"
#include "my_dma.h"

//ADC
#define ADC_1 0
#define ADC_2 1
#define ADC_3 2

//ADC channels
#define ADC_CH0 PA0
#define ADC_CH1 PA1
#define ADC_CH2 PA2
#define ADC_CH3 PA3
#define ADC_CH4 PA4
#define ADC_CH5 PA5
#define ADC_CH6 PA6
#define ADC_CH7 PA7
#define ADC_CH8 PB0
#define ADC_CH9 PB1
#define ADC_CH10 PC0
#define ADC_CH11 PC1
#define ADC_CH12 PC2
#define ADC_CH13 PC3
#define ADC_CH14 PC4
#define ADC_CH15 PC5
#define ADC_CH16 0xFF

//Internal Temp. constants
#define V25 1.43
#define VSENSE 3.3/4095
#define AVG_SLOPE .0043

//ADC functions
void initADC(uint8_t ADC,uint8_t num,const uint8_t portpins[],uint16_t result[]);
void adcINT(uint8_t ADC);
void startADC(uint8_t ADC);
void stopADC(uint8_t ADC);

#endif
