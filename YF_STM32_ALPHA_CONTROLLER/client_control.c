#include "my_timer.h"
#include "my_usart.h"
#include "st7735.h"
#include "stm32f10x.h"
#include "time.c"
#include "ws2812.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 500
static uint8_t request_index;
static char request_msg[BUFFER_SIZE];
static char response_msg[BUFFER_SIZE];

static const char REQUESTS[10][15] = {"mode-btn",    "start-btn", "reset-btn",  "toggle1",    "toggle2",
                                      "display-btn", "clear-btn", "light-btn1", "light-btn2", "buzz-btn"};

typedef struct status
{
    uint8_t mode;
    TIME watch;
    TIME stopwatch;
    uint8_t led1;
    uint8_t led2;
    ST7735_DISPLAY display;
    uint8_t light1[3];
    uint8_t light2[3];
    uint8_t buzzer;
    bool reset;
} STATUS;

static void format_txt(const char *input)
{
    char *ptr = input;

    while (*ptr)
    {
        if ((*ptr) == '%' && (*(ptr + 1)) == '2')
        {
            if ((*(ptr + 2)) == '0')
            {
                *(ptr) = ' ';
                strcpy(ptr + 1, ptr + 3);
                ptr += 3;
            }
            else
            {
                strcpy(ptr, ptr + 2);
                ptr += 2;
            }
            continue;
        }

        ptr++;
    }
    ptr = strrchr(input, '/');
    *ptr = '\0';
}
static char *search_after(const char *msg, const char *find)
{
    char *loc = strstr(msg, find);
    if (loc == NULL)
        return NULL;
    loc += strlen(find);
    return loc;
}

static bool get_text_before(const char *msg, char *dest, const char *begin)
{
    char *loc = strstr(msg, begin);
    if (loc == NULL)
        return false;

    strncpy(dest, request_msg, loc - request_msg);
    return true;
}

static bool get_text(const char *msg, char *dest, const char *begin, const char *end)
{
    char *loc = search_after(msg, begin);
    if (loc == NULL)
        return false;

    char *loc2 = strstr(msg, end);
    if (loc2 == NULL)
        return false;

    strncpy(dest, loc, loc2 - loc);
    if (dest == NULL)
        return false;
    dest[loc2 - loc] = '\0';

    return true;
}

static void reset_spi(STATUS *status)
{
    switch (status->display.spi)
    {
    case SPI_1:
        RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
        RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
        break;
    case SPI_2:
        RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
        RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
        break;
    }
}

static void mode_func(const char *state, STATUS *status)
{
    if (*state == '0')
    {
        status->mode = 0;
        sprintf(response_msg, "mode-btn=0m%ds%d", status->watch.mins, status->watch.secs);
    }
    else if (*state == '1')
    {
        status->mode = 1;
        sprintf(response_msg, "mode-btn=1m%ds%d", status->stopwatch.mins, status->stopwatch.secs);
    }
}

static void start_func(const char *state, STATUS *status)
{
    char min[3];
    char sec[3];
    char *endptr;
    if (*state == '0')
    {
        if (status->mode == 0)
        {
            status->watch.enabled = false;
            stopTIMER(T2);
            sprintf(min, "%d", status->watch.mins);
            sprintf(sec, "%d", status->watch.secs);
        }
        else if (status->mode == 1)
        {
            status->stopwatch.enabled = false;
            stopTIMER(T4);
            sprintf(min, "%d", status->stopwatch.mins);
            sprintf(sec, "%d", status->stopwatch.secs);
        }

        sprintf(response_msg, "start-btn=0m%ss%s", min, sec);
    }
    else
    {
        if (!get_text(state, min, "min:", "sec:"))
            return;

        if (!get_text(state, sec, "sec:", "/"))
            return;

        int8_t min_val, sec_val;

        min_val = (int8_t)strtol(min, &endptr, 10);
        if (*endptr != '\0')
            return;
        sec_val = (int8_t)strtol(sec, &endptr, 10);
        if (*endptr != '\0')
            return;

        if (status->mode == 0)
        {
            set_time(&status->watch, 0, min_val, sec_val);
            status->watch.enabled = true;
            startTIMER(T2);
        }
        else if (status->mode == 1)
        {
            set_time(&status->stopwatch, 0, min_val, sec_val);
            status->stopwatch.enabled = true;
            startTIMER(T4);
        }
        strcpy(response_msg, "start-btn=1");
    }
}

static void reset_func(const char *state, STATUS *status)
{
    status->reset = true;
    strcpy(response_msg, "reset-btn=0");
}

static void toggle1_func(const char *state, STATUS *status)
{
    if (*state == '0')
    {
        pinON(status->led1);
        strcpy(response_msg, "toggle1=0");
    }
    else if (*state == '1')
    {
        pinOFF(status->led1);
        strcpy(response_msg, "toggle1=1");
    }
}

static void toggle2_func(const char *state, STATUS *status)
{
    if (*state == '0')
    {
        pinON(status->led2);
        strcpy(response_msg, "toggle2=0");
    }
    else if (*state == '1')
    {
        pinOFF(status->led2);
        strcpy(response_msg, "toggle2=1");
    }
}

static void display_func(const char *state, STATUS *status)
{
    ST7735_Init(72, &status->display);
    ST7735_FillScreen(&status->display, ST7735_COLOR_BLACK);
    format_txt(state);
    ST7735_WriteString(&status->display, 0, 0, state, Font_7x10, ST7735_COLOR_YELLOW, ST7735_COLOR_BLACK);
    strcpy(response_msg, "display-btn=0");
}

static void clear_func(const char *state, STATUS *status)
{
    ST7735_Init(72, &status->display);
    ST7735_FillScreen(&status->display, ST7735_COLOR_YELLOW);
    strcpy(response_msg, "clear-btn=0");
}

static void light1_func(const char *state, STATUS *status)
{
    char g[4], b[4], r[4];
    uint8_t temp[3];
    char *endptr;

    if (!get_text(request_msg, g, "g:", "r:"))
        return;

    if (!get_text(request_msg, r, "r:", "b:"))
        return;

    if (!get_text(request_msg, b, "b:", "/"))
        return;

    temp[0] = (uint8_t)strtol(g, &endptr, 10);
    if (*endptr != '\0')
        return;
    temp[1] = (uint8_t)strtol(r, &endptr, 10);
    if (*endptr != '\0')
        return;
    temp[2] = (uint8_t)strtol(b, &endptr, 10);
    if (*endptr != '\0')
        return;

    status->light1[0] = temp[0];
    status->light1[1] = temp[1];
    status->light1[2] = temp[2];
    reset_spi(status);
    light_1(status->light1[0], status->light1[1], status->light1[2]);
    light_2(status->light2[0], status->light2[1], status->light2[2]);
    ws2812TX();
    strcpy(response_msg, "light-btn1=0");
}

static void light2_func(const char *state, STATUS *status)
{
    char g[4], b[4], r[4];
    uint8_t temp[3];
    char *endptr;

    if (!get_text(request_msg, g, "g:", "r:"))
        return;

    if (!get_text(request_msg, r, "r:", "b:"))
        return;

    if (!get_text(request_msg, b, "b:", "/"))
        return;

    temp[0] = (uint8_t)strtol(g, &endptr, 10);
    if (*endptr != '\0')
        return;
    temp[1] = (uint8_t)strtol(r, &endptr, 10);
    if (*endptr != '\0')
        return;
    temp[2] = (uint8_t)strtol(b, &endptr, 10);
    if (*endptr != '\0')
        return;

    status->light2[0] = temp[0];
    status->light2[1] = temp[1];
    status->light2[2] = temp[2];
    reset_spi(status);
    light_1(status->light1[0], status->light1[1], status->light1[2]);
    light_2(status->light2[0], status->light2[1], status->light2[2]);
    ws2812TX();
    strcpy(response_msg, "light-btn2=0");
}

static void buzz_func(const char *state, STATUS *status)
{
    status->buzzer = !status->buzzer;
    if (status->buzzer)
        strcpy(response_msg, "buzz-btn=1");
    else
        strcpy(response_msg, "buzz-btn=0");
}

static void (*func[])(const char *, STATUS *) = {mode_func,    start_func, reset_func,  toggle1_func, toggle2_func,
                                                 display_func, clear_func, light1_func, light2_func,  buzz_func};

static void processRequest(STATUS *status)
{
    char *find;
    char header[15];

    request_index = 0;
    memset(response_msg, 0, sizeof(response_msg));

    find = strchr(request_msg, '=');
    if (find == NULL)
        return;
    find += 1;

    get_text_before(request_msg, header, "=");
    for (int i = 0; i < 10; i++)
    {
        if (strstr(header, REQUESTS[i]))
        {
            func[i](find, status);
            uartSTR_TX(USART3_CH, response_msg);
        }
    }

    uartSTR_TX(USART3_CH, "");
}
