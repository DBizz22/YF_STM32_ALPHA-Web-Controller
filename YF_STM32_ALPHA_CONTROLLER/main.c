// #include "ST7735.h"
// #include "fonts.h"
#include "my_adc.h"
#include "my_clock.h"
#include "my_gpio.h" // Device header
#include "my_systick.h"
#include "my_timer.h"
#include "my_usart.h"
#include "stm32f10x.h"

// #include "COD.h"
#include "ch422.h"
#include "client_control.c"
// #include "fifa.h"
#include "fonts.h"
#include "st7735.h"
#include "ws2812.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// #include "images.h"

#define KEY1 PC14
#define KEY2 PC15
#define DS PC6
#define STCP PC7
#define SHCP PC8
#define BUZ PA5

bool temp_1_2 = false;
// uint16_t leds=0;
// uint8_t adc_num = 0;
// uint32_t data[2];
float temp[2];
int tempD;

const uint8_t ADC_CH[] = {ADC_CH0, ADC_CH1, ADC_CH4, ADC_CH16};
uint16_t result[4];

/**************PWM config************************/

/****************************************************/

/**********USART params***************/

/*************************************/

void shiftREG(uint16_t);
// bool delay(uint16_t milisec);
void bar(void);
void ring(void);

/*******************stopwatch/timer************** */
// TIME watch, stopwatch;
void watch_func(void);
void stopwatch_func(void);
void temp_func(void);

/**************SPI display*****************/
// extern const uint16_t fifa[];

//***********Client Control***********/
bool execute_control;
uint16_t uart_timer_counter;
STATUS controls = {.led1 = PB8, .led2 = PB9, .display = {.BLK = PD2, .CS = PA4, .DC = PC12, .RST = PA13, .spi = SPI_1}};

int main(void)
{
    SystemInit();
    setCLK(S72MHZ);
    char output_pins[] = {DS, STCP, SHCP}; // SCL,SDA};
    systickCONFIG(72);
    startTICK();
    falINT(KEY1);
    falINT(KEY2);

    for (int i = 0; i < 3; i++)
        outMODE(output_pins[i]);

    //******************LEDS***********************/
    outMODE(controls.led1);
    outMODE(controls.led2);
    pinON(controls.led1);
    pinON(controls.led2);

    //******************ADC***********************/
    initADC(ADC_1, 4, ADC_CH, result);
    adcINT(ADC_1);
    startADC(ADC_1);

    //******************4 LED display*****************/
    initLEDS();

    //*********USART config and timer***************/
    uartINT_RX(USART3_CH, 115200);
    timerINT_50MS(T1, 50);

    //********Timers for watch and stopwatch***********/
    timerINT_50MS(T2, 50); //@param for watch
    stopTIMER(T2);
    timerINT_50MS(T4, 50); //@param for stopwatch
    stopTIMER(T4);

    //*********st7735*************/
    ST7735_Init(72, &controls.display);
    ST7735_FillScreen(&controls.display, ST7735_COLOR_BLACK);
    ST7735_WriteString(&controls.display, 0, 0, "My name is divine, i love eating rice. Chelsea is my team. Who am i?",
                       Font_7x10, ST7735_COLOR_YELLOW, ST7735_COLOR_BLACK);

    // FIXME: Modify main loop for different functionalities
    controls.buzzer = false;
    while (1)
    {
        watch_func();
        stopwatch_func();
        temp_func();
        if (execute_control)
        {
            processRequest(&controls);
            execute_control = false;
            uart_timer_counter = 0;
            stopTIMER(T1);
        }
        // process();
        if (controls.buzzer)
            ring();

        bar();
    }
    return 0;
}

void shiftREG(uint16_t leds)
{
    pinOFF(STCP);
    for (int i = 0; i < 16; i++)
    {
        pinOFF(SHCP);

        if ((leds << i) & 0x8000)
            pinON(DS);
        else
            pinOFF(DS);

        pinON(SHCP);
    }
    pinON(STCP);
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    controls.watch.msecs += 50;
}

void TIM4_IRQHandler(void)
{
    TIM4->SR &= ~TIM_SR_UIF;
    controls.stopwatch.msecs += 50;
}

void EXTI15_10_IRQHandler(void)
{
    delayMS(500);
    if (EXTI->PR & EXTI_PR_PR14)
    {
        EXTI->PR |= EXTI_IMR_MR14;
        if (pinR(KEY1) == false)
            while (pinR(KEY1) == false)
                ;
        if (controls.mode == 0 && controls.watch.enabled)
        {
            controls.watch.enabled = false;
            stopTIMER(T2);
        }
        else if (controls.mode == 0 && !controls.watch.enabled)
        {
            controls.watch.enabled = true;
            startTIMER(T2);
        }
        else if (controls.mode == 1 && controls.stopwatch.enabled)
        {
            controls.stopwatch.enabled = false;
            stopTIMER(T4);
        }
        else if (controls.mode == 2 && !controls.stopwatch.enabled)
        {
            controls.stopwatch.enabled = true;
            startTIMER(T4);
        }
        else
        {
            temp_1_2 = !temp_1_2;
        }
    }
    else if (EXTI->PR & EXTI_PR_PR15)
    {
        EXTI->PR |= EXTI_IMR_MR15;
        uint8_t count = 0;
        while (pinR(KEY2) == false)
        {
            count++;
            delayMS(100);
        }
        if (count > 10)
        {
            controls.mode++;
            if (controls.mode >= 2)
                controls.mode = 0;
        }
        else
        {
            controls.reset = true;
        }
    }
}

void ADC1_2_IRQHandler(void)
{
    /*adc_num++;
    if(adc_num==4)
    {
    for (int i=0; i<4; i++)
            {
                    while( (DMA1->ISR&0x02)==0 );
                    DMA1->IFCR|=(0x02);
            }
            adc_num=0;
    }*/
    if ((ADC1->SR & 0x01) == 1) // Analog Watchdog event
    {
        controls.buzzer = true;
        ADC1->SR &= ~(0x01);
    }
}

void USART3_IRQHandler(void)
{
    if (request_index == 0)
    {
        memset(request_msg, 0, sizeof(request_msg));
        startTIMER(T1);
    }
    request_msg[request_index] = USART3->DR;
    if (request_msg[request_index] == '/')
    {
        execute_control = true;
    }
    request_index++;
    if (request_index == BUFFER_SIZE)
        execute_control = true;
}

void TIM1_UP_IRQHandler(void)
{
    TIM1->SR &= ~(0x01);
    uart_timer_counter++;
    if (uart_timer_counter == 100)
    {
        uart_timer_counter = 0;
        execute_control = true;
        stopTIMER(T1);
    }
}

void bar(void)
{
    uint16_t num = 0;
    int leds = result[0] / 252;
    if (leds)
    {
        for (int i = 1; i <= leds; i++)
        {
            num |= (0x01 << (i - 1));
        }
    }
    shiftREG(num);
}

void ring(void)
{
    outMODE(BUZ);
    for (int i = 0; i < 50; i++)
    {
        pinTOGGLE(BUZ);
        delayMS(1);
    }
}

void temp_func(void)
{
    if (controls.mode == 2)
    {
        temp[0] = (V25 - VSENSE * result[3]) + 25;
        temp[1] = ((float)result[2] / (float)4095) * 500;
        if (temp_1_2)
        {
            tempD = temp[0] * 10;
            led_display(LED_L, led_nums[tempD / 100], led_nums[(((tempD / 10)) % 10)] | 0x80, led_nums[tempD % 10]);
        }
        else
        {
            tempD = temp[1] * 10;
            led_display(LED_H, led_nums[tempD / 100], led_nums[(((tempD / 10)) % 10)] | 0x80, led_nums[tempD % 10]);
        }
        delayMS(500);
    }
}

void watch_func(void)
{
    update_time_up(&controls.watch);
    if (controls.mode == 0)
    {
        if (controls.reset)
        {
            reset_time(&controls.watch);
            controls.reset = false;
        }
        led_display(led_nums[controls.watch.mins / 10], led_nums[controls.watch.mins % 10] | 0x80,
                    led_nums[controls.watch.secs / 10], led_nums[controls.watch.secs % 10]);
    }
    pinTOGGLE(BUZ);
}

void stopwatch_func(void)
{
    update_time_down(&controls.stopwatch);
    if (controls.mode == 1)
    {
        if (controls.reset)
        {
            reset_time(&controls.stopwatch);
            controls.reset = false;
        }
        led_display(led_nums[controls.stopwatch.mins / 10], led_nums[controls.stopwatch.mins % 10] | 0x80,
                    led_nums[controls.stopwatch.secs / 10], led_nums[controls.stopwatch.secs % 10]);
    }
    pinTOGGLE(BUZ);
    if (!controls.stopwatch.hrs && !controls.stopwatch.mins && !controls.stopwatch.secs)
        ring();
}
