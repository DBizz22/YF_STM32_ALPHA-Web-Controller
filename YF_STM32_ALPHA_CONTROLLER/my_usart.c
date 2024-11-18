#include "my_usart.h"
#include "my_gpio.h"
#include "stm32f10x.h"

USART_TypeDef *getUSART(uint8_t ch)
{
    if (ch == USART1_CH)
        return USART1;
    else if (ch == USART2_CH)
        return USART2;
    else if (ch == USART3_CH)
        return USART3;
#ifdef USART4_CH
    else if (ch == USART4_CH)
        return USART4;
#endif
#ifdef USART5_CH
    else if (ch == USART5_CH)
        return USART5;
#endif
}

void usartCONFIG(uint8_t ch, uint32_t baud)
{
    // Enable Af, USART channel and remap portpin
    RCC->APB2ENR |= (0x01);
    if (ch == USART1_CH)
    {
        RCC->APB2ENR |= (0x01 << 14);
        outAF(USART1_TX);  // usart1_tx PA9
        inMODE(USART1_RX); // usart1_rx PA10
    }
    else if (ch == USART2_CH)
    {
        RCC->APB1ENR |= (0x01 << 17);
        outAF(USART2_TX);  // usart2_tx PA2
        inMODE(USART2_RX); // usart2_rx PA3
    }
    else if (ch == USART3_CH)
    {
        RCC->APB1ENR |= (0x01 << 18);
        outAF(USART3_TX);  // usart3_tx PB10
        inMODE(USART3_RX); // usart3_rx PB11
    }
#ifdef USART4
    else if (ch == USART4_CH)
    {
        RCC->APB1ENR |= (0x01 << 19);
    }
#endif
#ifdef USART5
    else if (ch == USART5_CH)
    {
        RCC->APB1ENR |= (0x01 << 20);
    }
#endif

    /**********Remapping*******************/
    AFIO->MAPR &= ~(0x03 << 4); // reset
#if REMAP_3 == 1
    AFIO->MAPR |= (0x01 << 4); // USART3 partial remap
#elif REMAP_3 == 2
    AFIO->MAPR |= (0x10 << 4); // USART3 full remap
#endif

    USART_TypeDef *usart = getUSART(ch);
    usart->CR1 = 0x0;
    usart->CR1 |= (0x01 << 13); // Enable USART
    usart->CR1 |= (0x01 << 3);  // Enable transmission
    usart->CR1 |= (0x01 << 2);  // Enable reception

    double pclk; // pclk for baud rate gen
    if (ch == USART1_CH)
    {
        extern uint16_t PCLK2;
        pclk = PCLK2 * 1000000;
    }
    else
    {
        extern uint16_t PCLK1;
        pclk = PCLK1 * 1000000;
    }

    double div = pclk / (16 * baud);
    uint16_t div_man = div;
    double full_div_frac = (div - div_man) * 16;
    uint16_t div_frac = full_div_frac;
    uint8_t round = (full_div_frac - div_frac) * 100;
    if (round > 44)
        div_frac++;
    if (div_frac >= 16)
    {
        div_man++;
        div_frac = 0;
    }
    usart->BRR = (div_man << 4);
    usart->BRR |= div_frac;
    usart->CR2 = 0;
    usart->CR3 = 0;

    /**********DMA Config***************/
    uint8_t dma_tx, dma_rx;
    // DMA transmission
    if (ch == USART1_CH)
    {
        dma_rx = DMA1_CH3;
        dma_tx = DMA1_CH2;
    }
    else if (ch == USART2_CH)
    {
        dma_rx = DMA1_CH5;
        dma_tx = DMA1_CH4;
    }
    else if (ch == USART3_CH)
    {
        dma_rx = DMA1_CH6;
        dma_tx = DMA1_CH7;
    }
}

void uartINT_RX(uint8_t ch, uint32_t baud)
{
    USART_TypeDef *usart = getUSART(ch);
    usartCONFIG(ch, baud);
    __disable_irq();
    usart->CR1 |= (0x01 << 5); // Enable Recieve interrupt
    switch (ch)
    {
    case USART1_CH:
        NVIC_EnableIRQ(USART1_IRQn);
        break;
    case USART2_CH:
        NVIC_EnableIRQ(USART2_IRQn);
        break;
    case USART3_CH:
        NVIC_EnableIRQ(USART3_IRQn);
    }
    __enable_irq();
}

void sendCHAR(USART_TypeDef *usart, const char chr)
{
    while ((usart->SR & (0x01 << 7)) == 0x00)
        ; // wait for tansmit data register to be empty
    usart->DR = chr;
    while ((usart->SR & (0x01 << 6)) == 0x00)
        ;                      // wait to complete transmission
    usart->SR &= ~(0x01 << 6); // Clear TC bit
}

char uartCHAR_RX(uint8_t ch)
{
    USART_TypeDef *usart = getUSART(ch);
    char temp;
    while ((usart->SR & (0x01 << 5)) == 0x00)
        ; // wait to recieve data
    temp = usart->DR;
    return temp;
}

void uartSTR_TX(uint8_t ch, char *str)
{
    USART_TypeDef *usart = getUSART(ch);
    while (*str)
    {
        sendCHAR(usart, *str);
        str++;
    }
}
