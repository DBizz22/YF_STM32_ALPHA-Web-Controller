#include "board_control.h"
#include "my_clock.h"
#include "my_esp01.h"
#include "my_timer.h"
#include "my_usart.h"
#include "server.h"
#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include <stdbool.h>
#include <string.h>

/*
        * Highlight
        ! Alert
        ? Query
        TODO task
        @param parameter
        // removed code
*/
uint16_t index1, index2, index3;

int main()
{
    SystemInit();
    setCLK(S72MHZ);

    timerINT_50MS(T1, 50);
    init_ESP01(USART1_CH, USART2_CH, T2);
    init_board_control();
    init_server();
    load_webpages();

    while (1)
    {
        board_control_panel();
    }
    return 0;
}

void USART1_IRQHandler(void)
{
    if (index1 == 0)
        memset(pc_msg, 0, sizeof(pc_msg));
    pc_msg[index1] = USART1->DR;
    if (pc_msg[index1] == '\n')
    {
        pc_msg_rdy = true;
        index1 = 0;
    }
    else
        index1++;
}

void USART2_IRQHandler(void)
{
    if (index2 == 0)
    {
        memset(esp01_msg, 0, sizeof(esp01_msg));
        startTIMER(T1);
    }
    esp01_msg[index2] = USART2->DR;
    index2++;
}

void USART3_IRQHandler(void)
{
    if (index3 == 0)
    {
        memset(board_response, 0, sizeof(board_response));
        startTIMER(T3);
    }
    board_response[index3] = USART3->DR;
    index3++;
}
void TIM1_UP_IRQHandler(void)
{
    TIM1->SR &= (~TIM_SR_UIF);
    esp01_msg_delay--;
    if (esp01_msg_delay == 0)
    {
        stopTIMER(T1);
        TIM1->SR &= (~TIM_SR_UIF);
        esp01_msg_rdy = true;
        esp01_msg_delay = ESP01_TIMEOUT; //*Default timeout
        index2 = 0;
    }
}

void TIM3_IRQHandler(void)
{
    TIM3->SR &= (~TIM_SR_UIF);
    board_msg_delay--;
    if (board_msg_delay == 0)
    {
        stopTIMER(T3);
        TIM3->SR &= (~TIM_SR_UIF);
        board_response[index3] = '\0';
        board_msg_rdy = true;
        board_msg_delay = BOARD_MSG_TIMEOUT; //*Default timeout
        index3 = 0;
    }
}
