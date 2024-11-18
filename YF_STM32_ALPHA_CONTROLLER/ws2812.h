#ifndef WS2812_H_
#define WS2812_H_
#include "stdbool.h"
#include "stm32f10x.h"

#define NO_COLORS 29
#define BLACK                                                                                                          \
    {                                                                                                                  \
        0, 0, 0                                                                                                        \
    }
#define WHITE                                                                                                          \
    {                                                                                                                  \
        255, 255, 255                                                                                                  \
    }
#define RED                                                                                                            \
    {                                                                                                                  \
        0, 255, 0                                                                                                      \
    }
#define LIME                                                                                                           \
    {                                                                                                                  \
        255, 0, 0                                                                                                      \
    }
#define BLUE                                                                                                           \
    {                                                                                                                  \
        0, 0, 255                                                                                                      \
    }
#define YELLOW                                                                                                         \
    {                                                                                                                  \
        255, 255, 0                                                                                                    \
    }
#define CYAN                                                                                                           \
    {                                                                                                                  \
        255, 0, 255                                                                                                    \
    }
#define MAGENTA                                                                                                        \
    {                                                                                                                  \
        0, 255, 255                                                                                                    \
    }
#define SILVER                                                                                                         \
    {                                                                                                                  \
        192, 192, 192                                                                                                  \
    }
#define GRAY                                                                                                           \
    {                                                                                                                  \
        128, 128, 128                                                                                                  \
    }
#define MAROON                                                                                                         \
    {                                                                                                                  \
        0, 128, 0                                                                                                      \
    }
#define OLIVE                                                                                                          \
    {                                                                                                                  \
        128, 128, 0                                                                                                    \
    }
#define GREEN                                                                                                          \
    {                                                                                                                  \
        128, 0, 0                                                                                                      \
    }
#define PURPLE                                                                                                         \
    {                                                                                                                  \
        0, 128, 128                                                                                                    \
    }
#define TEAL                                                                                                           \
    {                                                                                                                  \
        128, 0, 128                                                                                                    \
    }
#define NAVY                                                                                                           \
    {                                                                                                                  \
        0, 0, 128                                                                                                      \
    }
#define GOLD                                                                                                           \
    {                                                                                                                  \
        215, 255, 0                                                                                                    \
    }
#define ORANGE                                                                                                         \
    {                                                                                                                  \
        165, 255, 0                                                                                                    \
    }
#define CHOCO                                                                                                          \
    {                                                                                                                  \
        105, 210, 30                                                                                                   \
    }
#define FOREST                                                                                                         \
    {                                                                                                                  \
        139, 34, 34                                                                                                    \
    }
#define INDIGO                                                                                                         \
    {                                                                                                                  \
        0, 75, 130                                                                                                     \
    }
#define BROWN                                                                                                          \
    {                                                                                                                  \
        42, 165, 42                                                                                                    \
    }
#define FIREBRICK                                                                                                      \
    {                                                                                                                  \
        34, 178, 34                                                                                                    \
    }
#define TEAL                                                                                                           \
    {                                                                                                                  \
        128, 0, 128                                                                                                    \
    }
#define ORCHID                                                                                                         \
    {                                                                                                                  \
        112, 218, 214                                                                                                  \
    }
#define BEIGE                                                                                                          \
    {                                                                                                                  \
        245, 245, 220                                                                                                  \
    }
#define SIENNA                                                                                                         \
    {                                                                                                                  \
        82, 160, 45                                                                                                    \
    }
#define LAVENDER                                                                                                       \
    {                                                                                                                  \
        230, 230, 250                                                                                                  \
    }
#define DIM                                                                                                            \
    {                                                                                                                  \
        105, 105, 105                                                                                                  \
    }

void light_1(uint8_t green, uint8_t red, uint8_t blue);
void light_2(uint8_t green, uint8_t red, uint8_t blue);
void ws2812TX(void);
void process(void);

#endif
