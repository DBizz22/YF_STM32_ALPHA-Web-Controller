#include "st7735.h"
#include "fonts.h"
#include "my_dma.h"
#include "my_gpio.h"
#include "my_spi.h"
#include "my_systick.h"
#include "my_timer.h"
#include "stm32f10x.h"
#include <stdbool.h>

#define SPI1_DATAFRAME8 SPI1->CR1 &= ~SPI_CR1_DFF
#define SPI1_DATAFRAME16 SPI1->CR1 |= SPI_CR1_DFF
#define SPI2_DATAFRAME8 SPI2->CR1 &= ~SPI_CR1_DFF
#define SPI2_DATAFRAME16 SPI2->CR1 |= SPI_CR1_DFF

void ST7735_select(const ST7735_DISPLAY *display)
{
    pinOFF(display->CS);
}

void ST7735_deselect(const ST7735_DISPLAY *display)
{
    pinON(display->CS);
}

void ST7735_CONFIG(const uint8_t clk, const ST7735_DISPLAY *display)
{
    initSPI(display->spi, SPEED1, false);
    outMODE(display->RST);
    outMODE(display->DC);
    outMODE(display->BLK);
    pinON(display->BLK); // back light
    pinON(display->CS);  // chip select
    pinON(display->RST); // reset is inactive

    systickCONFIG(clk);
    startTICK();
}

void ST7735_WaitLastData(const ST7735_DISPLAY *display)
{
    if (display->spi == SPI_1)
    {
        while ((SPI1->SR & SPI_SR_TXE) == RESET)
            ;
        while ((SPI1->SR & SPI_SR_BSY) != RESET)
            ;
    }
    else if (display->spi == SPI_2)
    {
        while ((SPI2->SR & SPI_SR_TXE) == RESET)
            ;
        while ((SPI2->SR & SPI_SR_BSY) != RESET)
            ;
    }
}

void ST7735_SendCommand(uint8_t data, const ST7735_DISPLAY *display)
{
    pinOFF(display->DC);
    if (display->spi == SPI_1)
        spi1_TX(data);
    else if (display->spi == SPI_2)
        spi2_Tx(data);
    // ST7735_WaitLastData();
}

void ST7735_SendData(uint8_t data, const ST7735_DISPLAY *display)
{
    pinON(display->DC);
    if (display->spi == SPI_1)
        spi1_TX(data);
    else if (display->spi == SPI_2)
        spi2_Tx(data);
    // ST7735_WaitLastData();
}

void ST7735_SendPixel(uint16_t data, const ST7735_DISPLAY *display)
{
    pinON(display->DC);
    if (display->spi == SPI_1)
        spi1_TX(data);
    else if (display->spi == SPI_2)
        spi2_Tx(data);
    // ST7735_WaitLastData();
}

/*void ST7735_SendDataMultiple(uint8_t *data, uint32_t num)
{
  for (uint32_t i = 0; i < num; i++)
  {
    ST7735_SendData(*data);
    data++;
  }

  ST7735_WaitLastData();
}*/

void ST7735_Init(const uint8_t clk, const ST7735_DISPLAY *display)
{
    ST7735_CONFIG(clk, display);
    startSPI(display->spi);

    ST7735_select(display);
    pinON(display->RST);
    delayMS(5);
    pinOFF(display->RST);
    delayMS(10);
    pinON(display->RST);
    delayMS(120);

    ST7735_SendCommand(ST7735_SWRESET, display);
    delayMS(120);
    ST7735_SendCommand(ST7735_SLPOUT, display);
    delayMS(120);
    ST7735_SendCommand(ST7735_FRMCTR1, display);
    ST7735_SendData(0x05, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendCommand(ST7735_FRMCTR2, display);
    ST7735_SendData(0x05, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendCommand(ST7735_FRMCTR3, display);
    ST7735_SendData(0x05, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendData(0x05, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendData(0x3C, display);
    ST7735_SendCommand(ST7735_INVCTR, display);
    ST7735_SendData(0x03, display);
    ST7735_SendCommand(ST7735_PWCTR1, display);
    ST7735_SendData(0x28, display);
    ST7735_SendData(0x08, display);
    ST7735_SendData(0x04, display);
    ST7735_SendCommand(ST7735_PWCTR2, display);
    ST7735_SendData(0xC0, display);
    ST7735_SendCommand(ST7735_PWCTR3, display);
    ST7735_SendData(0x0D, display);
    ST7735_SendData(0x00, display);
    ST7735_SendCommand(ST7735_PWCTR4, display);
    ST7735_SendData(0x8D, display);
    ST7735_SendData(0x2A, display);
    ST7735_SendCommand(ST7735_PWCTR5, display);
    ST7735_SendData(0x8D, display);
    ST7735_SendData(0xEE, display);
    ST7735_SendCommand(ST7735_VMCTR1, display);
    ST7735_SendData(0x1A, display);
    ST7735_SendCommand(ST7735_INVOFF, display);
    ST7735_SendCommand(ST7735_MADCTL, display);
    ST7735_SendData(0xC0, display);
    ST7735_SendCommand(ST7735_COLMOD, display);
    ST7735_SendData(0x05, display);
    ST7735_SendCommand(ST7735_GMCTRP1, display);
    ST7735_SendData(0x04, display);
    ST7735_SendData(0x22, display);
    ST7735_SendData(0x07, display);
    ST7735_SendData(0x0A, display);
    ST7735_SendData(0x2E, display);
    ST7735_SendData(0x30, display);
    ST7735_SendData(0x25, display);
    ST7735_SendData(0x2A, display);
    ST7735_SendData(0x28, display);
    ST7735_SendData(0x26, display);
    ST7735_SendData(0x2E, display);
    ST7735_SendData(0x3A, display);
    ST7735_SendData(0x00, display);
    ST7735_SendData(0x01, display);
    ST7735_SendData(0x03, display);
    ST7735_SendData(0x13, display);
    ST7735_SendCommand(ST7735_GMCTRN1, display);
    ST7735_SendData(0x04, display);
    ST7735_SendData(0x16, display);
    ST7735_SendData(0x06, display);
    ST7735_SendData(0x0D, display);
    ST7735_SendData(0x2D, display);
    ST7735_SendData(0x26, display);
    ST7735_SendData(0x23, display);
    ST7735_SendData(0x27, display);
    ST7735_SendData(0x27, display);
    ST7735_SendData(0x25, display);
    ST7735_SendData(0x2D, display);
    ST7735_SendData(0x3B, display);
    ST7735_SendData(0x00, display);
    ST7735_SendData(0x01, display);
    ST7735_SendData(0x04, display);
    ST7735_SendData(0x13, display);
    ST7735_SendCommand(ST7735_NORON, display);
    ST7735_SendCommand(ST7735_DISPON, display);
    delayMS(500);
    ST7735_deselect(display);
}

/*void ST7735_SetColAddr(uint16_t cStart, uint16_t cStop)
{
    uint8_t data[4];

    data[0] = (cStart & 0xFF00) >> 8;
    data[1] = cStart & 0x00FF;
    data[2] = (cStop & 0xFF00) >> 8;
    data[3] = cStop & 0x00FF;

    ST7735_select();
    ST7735_SendCommand(ST7735_CASET);
    ST7735_SendDataMultiple(data, 4);
    ST7735_deselect();
}*/

/*void ST7735_SetRowAddr(uint16_t rStart, uint16_t rStop)
{
    uint8_t data[4];

    data[0] = (rStart & 0xFF00) >> 8;
    data[1] = rStart & 0x00FF;
    data[2] = (rStop & 0xFF00) >> 8;
    data[3] = rStop & 0x00FF;

    ST7735_select();
    ST7735_SendCommand(ST7735_RASET);
    ST7735_SendDataMultiple(data, 4);
    ST7735_deselect();
}*/

void ST7735_SetAddressWindow(const ST7735_DISPLAY *display, uint8_t cStart, uint8_t rStart, uint8_t cStop,
                             uint8_t rStop)
{
    // ST7735_SetColAddr(cStart, cStop - 1);
    // ST7735_SetRowAddr(rStart, rStop - 1);
    ST7735_select(display);
    ST7735_SendCommand(ST7735_CASET, display);
    ST7735_SendData(0x00, display);
    ST7735_SendData(cStart, display);
    ST7735_SendData(0x00, display);
    ST7735_SendData(cStop - 1, display);
    ST7735_SendCommand(ST7735_RASET, display);
    ST7735_SendData(0x00, display);
    ST7735_SendData(rStart, display);
    ST7735_SendData(0x00, display);
    ST7735_SendData(rStop - 1, display);
    ST7735_SendCommand(ST7735_RAMWR, display);
    ST7735_WaitLastData(display);
    ST7735_deselect(display);
}

void ST7735_DrawPixel(const ST7735_DISPLAY *display, uint8_t x, uint8_t y, uint16_t color)
{
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;

    ST7735_select(display);

    ST7735_SetAddressWindow(display, x, y, x + 1, y + 1);
    // uint8_t data[] = { color >> 8, color & 0xFF };
    // initSPI(display->spi,SPEED1,true);
    if (display->spi == SPI_1)
        SPI1_DATAFRAME16;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME16;
    ST7735_SendPixel(color, display);

    ST7735_deselect(display);
    if (display->spi == SPI_1)
        SPI1_DATAFRAME8;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME8;
}

void ST7735_DrawRect(const ST7735_DISPLAY *display, uint8_t cStart, uint8_t rStart, uint8_t cStop, uint8_t rStop,
                     uint16_t color)
{
    // ST7735_SetColAddr(cStart, cStop - 1);
    // ST7735_SetRowAddr(rStart, rStop - 1);
    uint32_t size = (cStop - cStart) * (rStop - rStart);
    ST7735_SetAddressWindow(display, cStart, rStart, cStop, rStop);

    if (display->spi == SPI_1)
        SPI1_DATAFRAME16;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME16;

    ST7735_select(display);
    for (uint32_t i = 0; i < size; i++)
    {
        ST7735_SendPixel(color, display);
    }
    ST7735_WaitLastData(display);
    ST7735_deselect(display);
    if (display->spi == SPI_1)
        SPI1_DATAFRAME8;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME8;
}

void ST7735_SetBackgroundImage(const ST7735_DISPLAY *display, uint16_t cStart, uint16_t rStart, uint16_t cStop,
                               uint16_t rStop, const uint16_t *image)
{
    uint32_t size = (cStop - cStart) * (rStop - rStart);

    ST7735_SetAddressWindow(display, cStart, rStart, cStop, rStop);

    if (display->spi == SPI_1)
    {
        init2DMA(DMA1_CH3, size, (uint32_t)image, (uint32_t)&SPI1->DR);
        DMA1_Channel3->CCR |= (0x01 << 8);
        DMA1_Channel3->CCR |= (0x01 << 10);
    }
    else if (display->spi == SPI_2)
    {
        init2DMA(DMA1_CH5, size, (uint32_t)image, (uint32_t)&SPI2->DR);
        DMA1_Channel5->CCR |= (0x01 << 8);
        DMA1_Channel5->CCR |= (0x01 << 10);
    }

    // spiCONFIG(SPI_1,SPEED1,true);
    if (display->spi == SPI_1)
        SPI1_DATAFRAME16;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME16;

    pinON(display->DC);
    ST7735_select(display);

    if (display->spi == SPI_1)
    {
        startDMA(DMA1_CH3, false);
        while (DMA1_Channel3->CNDTR != 0)
            ;
    }
    else if (display->spi == SPI_2)
    {
        startDMA(DMA1_CH5, false);
        while (DMA1_Channel5->CNDTR != 0)
            ;
    }

    ST7735_WaitLastData(display);
    ST7735_deselect(display);

    if (display->spi == SPI_1)
    {
        stopDMA(DMA1_CH3);
    }
    else if (display->spi == SPI_2)
    {
        stopDMA(DMA1_CH5);
    }

    if (display->spi == SPI_1)
        SPI1_DATAFRAME8;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME8;
}

static void ST7735_WriteChar(const ST7735_DISPLAY *display, uint16_t x, uint16_t y, char ch, FontDef font,
                             uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ST7735_SetAddressWindow(display, x, y, x + font.width, y + font.height); // maybe swap parameters
    if (display->spi == SPI_1)
        SPI1_DATAFRAME16;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME16;

    ST7735_select(display);
    for (i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++)
        {
            if ((b << j) & 0x8000)
            {
                // uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_SendPixel(color, display);
            }
            else
            {
                // uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_SendPixel(bgcolor, display);
            }
        }
    }
    ST7735_deselect(display);
    if (display->spi == SPI_1)
        SPI1_DATAFRAME8;
    else if (display->spi == SPI_2)
        SPI2_DATAFRAME8;
}

void ST7735_WriteString(const ST7735_DISPLAY *display, uint16_t x, uint16_t y, const char *str, FontDef font,
                        uint16_t color, uint16_t bgcolor)
{
    while (*str)
    {
        if (x + font.width >= ST7735_WIDTH)
        {
            x = 0;
            y += font.height;
            if (y + font.height >= ST7735_HEIGHT)
            {
                break;
            }

            if (*str == ' ')
            {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(display, x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
}

void ST7735_InvertColors(const ST7735_DISPLAY *display, bool invert)
{
    ST7735_select(display);
    ST7735_SendCommand(invert ? ST7735_INVON : ST7735_INVOFF, display);
    ST7735_deselect(display);
}

void ST7735_FillScreen(const ST7735_DISPLAY *display, uint16_t color)
{
    ST7735_DrawRect(display, 0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}
