#ifndef MY_USART_H_
#define MY_USART_H_
#include "my_gpio.h"
#include "my_clock.h"
#include "my_dma.h"
//#define REMAP_3 1

//usart channels
#define USART1_CH 0
#define USART2_CH 1
#define USART3_CH 2
#ifdef USART4
#define USART4_CH 3
#endif
#ifdef USART5
#define USART5_CH 4
#endif

//usart portpins
#ifndef REMAP_1_2
//usart1 No remap
#define USART1_TX PA9
#define USART1_RX PA10
//usart2 No remap
#define USART2_TX PA2
#define USART2_RX PA3
#endif

#ifdef REMAP_1_2
//usart1 remap
#define USART1_TX PB6
#define USART1_RX PB7
//usart2 remap
#define USART2_TX PD5
#define USART2_RX PD6
#endif

#if REMAP_3==1
//usart3 partial remap
#define USART3_TX PC10
#define USART3_RX PC11
#elif REMAP_3==2
//usart3 full remap
#define USART3_TX PD8
#define USART3_RX PD9
#else
//usart3 no remap
#define USART3_TX PB10
#define USART3_RX PB11
#endif



//functions
void usartCONFIG(uint8_t ch,uint32_t baud);
void uartSTR_TX(uint8_t ch,const char* str);
char uartCHAR_RX(uint8_t ch);
void uartINT_RX(uint8_t ch,uint32_t baud);

#endif
