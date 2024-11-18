#include "my_adc.h"
#include "my_dma.h"
#include "my_gpio.h"
#include "stm32f10x.h"
#include <stdint.h>

ADC_TypeDef *getADC(uint8_t adc)
{
    if (adc == ADC_1)
        return ADC1;
    else if (adc == ADC_2)
        return ADC2;
    else if (adc == ADC_3)
        return ADC3;
}

void initADC(uint8_t ADC, uint8_t num, const uint8_t portpins[], uint16_t result[])
{
    RCC->CFGR &= ~(0x03 << 14);
    RCC->CFGR |= (0x02 << 14);           // set ADC prescaler to 6
    RCC->APB2ENR |= (0x01 << (ADC + 9)); // Enable ADC clock
    ADC_TypeDef *adc = getADC(ADC);
    adc->CR1 |= 0x100; // Enable scan mode

    adc->CR2 = 0;
    /*****Sequence order**********/
    adc->SQR1 |= ((num - 1) << 20);
    uint8_t shift, pin, ch;           // Set number of conversions
    for (uint8_t i = 0; i < num; i++) // Set Sequence order
    {
        shift = 5 * (i % 6);
        /**********Internal Temp Config*************/
        if (portpins[i] == ADC_CH16)
        {
            // adc->SMPR1|=(0x06<<18);         //Set ADC Ch16 sampling rate to 7us using ADC clock of 12Mhz
            if (i < 6)
                adc->SQR3 |= (16 << shift);
            else if (i < 12)
                adc->SQR2 |= (16 << shift);
            else if (i < 16)
                adc->SQR1 |= (16 << shift);
            adc->CR2 |= (0x01 << 23); // Enable temp sensor and Vrefint
            continue;
        }
        /**********Internal Temp Config*************/

        pin = portpins[i] % 16;
        if (portpins[i] < PA8)
            ch = pin;
        else if (portpins[i] < PB2)
            ch = pin + 8;
        else if (portpins[i] < PC6)
            ch = pin + 10;

        if (i < 6)
            adc->SQR3 |= (ch << shift);
        else if (i < 12)
            adc->SQR2 |= (ch << shift);
        else if (i < 16)
            adc->SQR1 |= (ch << shift);

        /************Config portpins******************/
        inANA(portpins[i]);
    }

    if (ADC == ADC_1)
    {
        /*******ADC set DMA*************/
        adc->CR2 |= (0x01 << 8); // Enable DMA access mode
        initDMA(DMA1_CH1, num, (uint32_t)&adc->DR, (uint32_t)result);
    }
}

void adcINT(uint8_t ADC)
{
    /***********Set ADC interrupt************/
    ADC_TypeDef *adc = getADC(ADC);
    __disable_irq();
    adc->CR1 |= (0x01 << 5); // Enable End of conversion interrupt

    /***********Set ADC watchdog******************/
    adc->HTR = (0x0300); // Set ADC watchdog High threshold
    adc->LTR = (0x0010); // Set ADC watchdog low threshold
    adc->CR1 &= ~(0x01f);
    adc->CR1 |= (0x01);       // Set watchdog to monitor ADC channel 1
    adc->CR1 |= (0x01 << 9);  // Set Analog watchdog to monitor a single channel
    adc->CR1 |= (0x01 << 23); // Enable Analog watchdog on regular channels
    adc->CR1 |= (0x01 << 6);  // Enable Analog watchdog interrupt

    NVIC_EnableIRQ(ADC1_2_IRQn);
    __enable_irq();
}

void startADC(uint8_t ADC)
{
    ADC_TypeDef *adc = getADC(ADC);
    adc->CR2 |= 0x001; // Enable A/D converter
    delayMS(10);
    adc->CR2 |= 0x001;        // Enable A/D converter again
    adc->CR2 |= 0x002;        // Enable Continuous conversion
    startDMA(DMA1_CH1, true); // Start DMA circular mode
}

void stopADC(uint8_t ADC)
{
    ADC_TypeDef *adc = getADC(ADC);
    adc->CR2 &= ~(0x01); // Disable A/D converter
    stopDMA(DMA1_CH1);   // Stop DMA
}
