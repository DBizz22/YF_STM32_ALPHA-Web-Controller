#include "stdbool.h"
// #include "stm32f10x.h"
#include <stdint.h>

//
typedef struct time
{
    bool enabled;
    uint16_t msecs;
    int8_t secs;
    int8_t mins;
    int8_t hrs;
} TIME;

static bool update_time_up(TIME *t)
{
    if (!t->enabled)
        return false;
    if (t->msecs < 1000)
        return false;
    if (t->msecs >= 1000)
        t->msecs -= 1000;
    t->secs++;
    if (t->secs == 60)
    {
        t->mins++;
        t->secs = 0;
    }
    if (t->mins == 60)
    {
        t->hrs++;
        t->mins = 0;
    }
    if (t->hrs == 24)
    {
        t->hrs = 0;
    }
    return true;
}

static bool update_time_down(TIME *t)
{
    if (!t->enabled)
        return false;
    if (t->hrs == 0 && t->mins == 0 && t->secs == 0)
        return false;
    if (t->msecs < 1000)
        return false;

    if (t->msecs >= 1000)
        t->msecs -= 1000;
    t->secs--;
    if (t->secs < 0)
    {
        t->mins--;
        t->secs = 59;
    }
    if (t->mins < 0)
    {
        if (t->hrs > 0)
            t->hrs--;
        t->mins = 59;
    }

    return true;
}

static void set_time(TIME *t, int8_t hr, int8_t min, int8_t sec)
{
    if (hr < 0 || min < 0 || sec < 0)
        return;

    t->msecs = 0;
    if (sec >= 60)
    {
        min += sec / 60;
        sec %= 60;
    }
    if (min >= 60)
    {
        hr += min / 60;
        min %= 60;
    }
    if (hr >= 24)
        hr %= 24;

    t->secs = sec;
    t->hrs = hr;
    t->mins = min;
}

static void reset_time(TIME *t)
{
    t->msecs = 0;
    t->secs = 0;
    t->mins = 0;
    t->hrs = 0;
}
