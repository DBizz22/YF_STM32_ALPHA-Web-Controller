#include "ws2812.h"
#include "my_systick.h"
#include "my_timer.h"
#include "my_usart.h"
#include "stm32f10x.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const uint8_t color[][3] = {BLACK,  WHITE, RED,       LIME,   BLUE,   YELLOW, CYAN,   MAGENTA,  SILVER, GRAY,
                            MAROON, OLIVE, GREEN,     PURPLE, TEAL,   NAVY,   GOLD,   ORANGE,   CHOCO,  FOREST,
                            INDIGO, BROWN, FIREBRICK, TEAL,   ORCHID, BEIGE,  SIENNA, LAVENDER, DIM};

const char exec[][10] = {"black",  "white",  "red",    "lime",     "blue",   "yellow", "cyan",      "magenta",
                         "silver", "gray",   "maroon", "olive",    "green",  "purple", "teal",      "navy",
                         "gold",   "orange", "choco",  "forest",   "indigo", "brown",  "firebrick", "teal",
                         "orchid", "beige",  "sienna", "lavender", "dim"};

static const int SIZE = 100;
static uint8_t bindex = 0;
static char buffer[SIZE];
static uint16_t data[88];
static uint32_t counter;

void process(void)
{
    counter = bindex = 0;
    uint8_t t = 1;

    char cmd[12];
    char timer[4];
    int i = 0, j = 0, l = 0;
    while (buffer[i])
    {
        if (isalpha(buffer[i]))
        {
            cmd[j] = buffer[i];
            j++;
            // continue;
        }
        else if (isdigit(buffer[i]))
        {
            timer[l] = buffer[i];
            l++;
        }
        if ((isspace(buffer[i]) && l > 0) || buffer[i + 1] == '\0')
        {
            timer[l] = cmd[j] = '\0';
            l = 0;
            t = atoi(timer);
            int k = 0;
            while (cmd[k]) // make the cmd into all lowercase
            {
                cmd[k] = tolower(cmd[k]);
                k++;
            }
            for (k = 0; k < NO_COLORS; k++)
            {
                if (!strcmp(cmd, exec[k]))
                {
                    light_1(color[k][0], color[k][1], color[k][2]);
                    light_2(color[k][0], color[k][1], color[k][2]);
                    ws2812TX();
                    delayMS(t * 1000);
                    break;
                }
            }
            uartSTR_TX(USART3_CH, cmd);
            if (k == NO_COLORS)
            {
                uartSTR_TX(USART3_CH, " Light command not available\n");
            }
            else
            {
                uartSTR_TX(USART3_CH, " Light command executed\n");
            }
            memset(cmd, 0, 12 * sizeof(char));
            j = 0;
        }
        i++;
    }
}

void light(uint8_t led, uint8_t green, uint8_t red, uint8_t blue)
{
    uint32_t pixel = 0;
    pixel = (green << 16);
    pixel |= (red << 8);
    pixel |= (blue);

    uint8_t j = led - 1;
    j *= 24;
    for (int i = 0; i < 24; i++)
    {
        if ((pixel >> (23 - i)) & (0x01))
        {
            data[j + i] = 50;
        }
        else
        {
            data[j + i] = 25;
        }
    }
}

void light_1(uint8_t green, uint8_t red, uint8_t blue)
{
    light(1, green, red, blue);
}

void light_2(uint8_t green, uint8_t red, uint8_t blue)
{
    light(2, green, red, blue);
}

void ws2812TX(void)
{

    timer_ch_PWMH(TIM3_CH1, 0, 90, true);
    init2DMA(DMA1_CH6, 88, (uint32_t)data, (uint32_t)&TIM3->DMAR);
    startDMA(DMA1_CH6, false);
    startTIMER(T3);

    delayMS(1);
    stopDMA(DMA1_CH6);
    stopTIMER(T3);
}
