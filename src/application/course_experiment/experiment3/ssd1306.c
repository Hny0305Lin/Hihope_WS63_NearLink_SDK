/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <securec.h>
#include "i2c.h"
#include "soc_osal.h"
#include "ssd1306.h"

#define CONFIG_I2C_MASTER_BUS_ID 1
#define I2C_SLAVE2_ADDR 0x3C
#define SSD1306_CTRL_CMD 0x00
#define SSD1306_CTRL_DATA 0x40
#define SSD1306_MASK_CONT (0x1 << 7)
#define DOUBLE 2

void ssd1306_Reset(void)
{
    // Wait for the screen to boot,1ms  The delay here is very important
    osal_mdelay(1);
}

static uint32_t ssd1306_SendData(uint8_t *buffer, uint32_t size)
{
    uint16_t dev_addr = I2C_SLAVE2_ADDR;
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = size;
    uint32_t retval = uapi_i2c_master_write(CONFIG_I2C_MASTER_BUS_ID, dev_addr, &data);
    if (retval != 0) {
        printf("I2cWrite(%02X) failed, %0X!\n", data.send_buf[1], retval);
        return retval;
    }
    return 0;
}

static uint32_t ssd1306_WiteByte(uint8_t regAddr, uint8_t byte)
{
    uint8_t buffer[] = {regAddr, byte};
    return ssd1306_SendData(buffer, sizeof(buffer));
}

// Send a byte to the command register
void ssd1306_WriteCommand(uint8_t byte)
{
    ssd1306_WiteByte(SSD1306_CTRL_CMD, byte);
}

// Send data
void ssd1306_WriteData(uint8_t *buffer, uint32_t buff_size)
{
    uint8_t data[SSD1306_WIDTH * DOUBLE] = {0};
    for (uint32_t i = 0; i < buff_size; i++) {
        data[i * DOUBLE] = SSD1306_CTRL_DATA | SSD1306_MASK_CONT;
        data[i * DOUBLE + 1] = buffer[i];
    }
    data[(buff_size - 1) * DOUBLE] = SSD1306_CTRL_DATA;
    ssd1306_SendData(data, sizeof(data));
}

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

// Screen object
static SSD1306_t SSD1306;

/* Fills the Screenbuffer with values from a given buffer of a fixed length */
SSD1306_Error_t ssd1306_FillBuffer(uint8_t *buf, uint32_t len)
{
    SSD1306_Error_t ret = SSD1306_ERR;
    if (len <= SSD1306_BUFFER_SIZE) {
        memcpy_s(SSD1306_Buffer, len + 1, buf, len);
        ret = SSD1306_OK;
    }
    return ret;
}

void ssd1306_Init_CMD(void)
{
    ssd1306_WriteCommand(0xA4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    ssd1306_WriteCommand(0xD3); // -set display offset - CHECK
    ssd1306_WriteCommand(0x00); // -not offset

    ssd1306_WriteCommand(0xD5); // --set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(0xF0); // --set divide ratio

    ssd1306_WriteCommand(0xD9); // --set pre-charge period
    ssd1306_WriteCommand(0x11); // 0x22 by default

    ssd1306_WriteCommand(0xDA); // --set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(0x02);
#elif (SSD1306_HEIGHT == 64)
    ssd1306_WriteCommand(0x12);
#elif (SSD1306_HEIGHT == 128)
    ssd1306_WriteCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    ssd1306_WriteCommand(0xDB); // --set vcomh
    ssd1306_WriteCommand(0x30); // 0x20,0.77xVcc, 0x30,0.83xVcc

    ssd1306_WriteCommand(0x8D); // --set DC-DC enable
    ssd1306_WriteCommand(0x14); //
    ssd1306_SetDisplayOn(1);    // --turn on SSD1306 panel
}

// Initialize the oled screen
void ssd1306_Init(void)
{
    // Reset OLED
    ssd1306_Reset();
    // Init OLED
    ssd1306_SetDisplayOn(0); // display off

    ssd1306_WriteCommand(0x20); // Set Memory Addressing Mode
    ssd1306_WriteCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    ssd1306_WriteCommand(0xB0); // Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    ssd1306_WriteCommand(0xC0); // Mirror vertically
#else
    ssd1306_WriteCommand(0xC8); // Set COM Output Scan Direction
#endif

    ssd1306_WriteCommand(0x00); // ---set low column address
    ssd1306_WriteCommand(0x10); // ---set high column address

    ssd1306_WriteCommand(0x40); // --set start line address - CHECK

    ssd1306_SetContrast(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_WriteCommand(0xA0); // Mirror horizontally
#else
    ssd1306_WriteCommand(0xA1); // --set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    ssd1306_WriteCommand(0xA7); // --set inverse color
#else
    ssd1306_WriteCommand(0xA6); // --set normal color
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    ssd1306_WriteCommand(0xFF);
#else
    ssd1306_WriteCommand(0xA8); // --set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(0x1F); //
#elif (SSD1306_HEIGHT == 64)
    ssd1306_WriteCommand(0x3F); //
#elif (SSD1306_HEIGHT == 128)
    ssd1306_WriteCommand(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
    ssd1306_Init_CMD();
    // Clear screen
    ssd1306_Fill(Black);

    // Flush buffer to screen
    ssd1306_UpdateScreen();

    // Set default values for screen object
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    SSD1306.Initialized = 1;

    osal_printk("SSD1306 Init SUCC!\r\n");
}

// Fill the whole screen with the given color
void ssd1306_Fill(SSD1306_COLOR color)
{
    /* Set memory */
    uint32_t i;

    for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

// Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(void)
{
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages

    uint8_t cmd[] = {
        0X21, // 设置列起始和结束地址
        0X00, // 列起始地址 0
        0X7F, // 列终止地址 127
        0X22, // 设置页起始和结束地址
        0X00, // 页起始地址 0
        0X07, // 页终止地址 7
    };
    uint32_t count = 0;
    uint8_t data[sizeof(cmd) * DOUBLE + SSD1306_BUFFER_SIZE + 1] = {};

    // copy cmd
    for (uint32_t i = 0; i < sizeof(cmd) / sizeof(cmd[0]); i++) {
        data[count++] = SSD1306_CTRL_CMD | SSD1306_MASK_CONT;
        data[count++] = cmd[i];
    }

    // copy frame data
    data[count++] = SSD1306_CTRL_DATA;
    memcpy_s(&data[count], SSD1306_BUFFER_SIZE + 1, SSD1306_Buffer, SSD1306_BUFFER_SIZE);
    count += sizeof(SSD1306_Buffer);

    // send to i2c bus
    uint32_t retval = ssd1306_SendData(data, count);
    if (retval != 0) {
        printf("ssd1306_UpdateScreen send frame data filed: %d!\r\n", retval);
    }
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
    SSD1306_COLOR color1 = color;
    // Check if pixel should be inverted
    if (SSD1306.Inverted) {
        color1 = (SSD1306_COLOR)!color1;
    }

    // Draw in the right color
    uint32_t c = 8; // 8
    if (color == White) {
        SSD1306_Buffer[x + (y / c) * SSD1306_WIDTH] |= 1 << (y % c);
    } else {
        SSD1306_Buffer[x + (y / c) * SSD1306_WIDTH] &= ~(1 << (y % c));
    }
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char ssd1306_DrawChar(char ch, FontDef Font, SSD1306_COLOR color)
{
    uint32_t i, b, j;

    // Check if character is valid
    uint32_t ch_min = 32;  // 32
    uint32_t ch_max = 126; // 126
    if ((uint32_t)ch < ch_min || (uint32_t)ch > ch_max) {
        return 0;
    }

    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.FontWidth) || SSD1306_HEIGHT < (SSD1306.CurrentY + Font.FontHeight)) {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for (i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - ch_min) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++) {
            if ((b << j) & 0x8000) {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
char ssd1306_DrawString(char *str, FontDef Font, SSD1306_COLOR color)
{
    // Write until null-byte
    char *str1 = str;
    while (*str1) {
        if (ssd1306_DrawChar(*str1, Font, color) != *str1) {
            // Char could not be written
            return *str1;
        }
        // Next char
        str1++;
    }

    // Everything ok
    return *str1;
}

// Position the cursor
void ssd1306_SetCursor(uint8_t x, uint8_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

// Draw line by Bresenhem's algorithm
void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
{
    uint8_t x = x1;
    uint8_t y = y1;
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;
    ssd1306_DrawPixel(x2, y2, color);
    while ((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(x1, y1, color);
        error2 = error * DOUBLE;
        if (error2 > -deltaY) {
            error -= deltaY;
            x += signX;
        } else {
            /* nothing to do */
        }
        if (error2 < deltaX) {
            error += deltaX;
            y += signY;
        } else {
            /* nothing to do */
        }
    }
}

// Draw polyline
void ssd1306_DrawPolyline(const SSD1306_VERTEX *par_vertex, uint16_t par_size, SSD1306_COLOR color)
{
    uint16_t i;
    if (par_vertex != 0) {
        for (i = 1; i < par_size; i++) {
            ssd1306_DrawLine(par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
        }
    } else {
        /* nothing to do */
    }
    return;
}

// Draw circle by Bresenhem's algorithm
void ssd1306_DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR par_color)
{
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t b = 2;
    int32_t err = b - b * par_r;
    int32_t e2;

    if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT) {
        return;
    }

    do {
        ssd1306_DrawPixel(par_x - x, par_y + y, par_color);
        ssd1306_DrawPixel(par_x + x, par_y + y, par_color);
        ssd1306_DrawPixel(par_x + x, par_y - y, par_color);
        ssd1306_DrawPixel(par_x - x, par_y - y, par_color);
        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * b + 1);
            if (-x == y && e2 <= x) {
                e2 = 0;
            } else {
                /* nothing to do */
            }
        } else {
            /* nothing to do */
        }
        if (e2 > x) {
            x++;
            err = err + (x * b + 1);
        } else {
            /* nothing to do */
        }
    } while (x <= 0);

    return;
}

// Draw rectangle
void ssd1306_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
{
    ssd1306_DrawLine(x1, y1, x2, y1, color);
    ssd1306_DrawLine(x2, y1, x2, y2, color);
    ssd1306_DrawLine(x2, y2, x1, y2, color);
    ssd1306_DrawLine(x1, y2, x1, y1, color);
}

void ssd1306_DrawBitmap(const uint8_t *bitmap, uint32_t size)
{
    unsigned int c = 8;
    uint8_t rows = size * c / SSD1306_WIDTH;
    if (rows > SSD1306_HEIGHT) {
        rows = SSD1306_HEIGHT;
    }
    for (uint8_t y = 0; y < rows; y++) {
        for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {
            uint8_t byte = bitmap[(y * SSD1306_WIDTH / c) + (x / c)];
            uint8_t bit = byte & (0x80 >> (x % c));
            ssd1306_DrawPixel(x, y, bit ? White : Black);
        }
    }
}

void ssd1306_DrawRegion(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data, uint32_t size)
{
    uint32_t stride = w;
    uint8_t h = w; // 字体宽高一样
    uint8_t width = w;
    if (x + w > SSD1306_WIDTH || y + h > SSD1306_HEIGHT || w * h == 0) {
        printf("%dx%d @ %d,%d out of range or invalid!\r\n", w, h, x, y);
        return;
    }

    width = (width <= SSD1306_WIDTH ? width : SSD1306_WIDTH);
    h = (h <= SSD1306_HEIGHT ? h : SSD1306_HEIGHT);
    stride = (stride == 0 ? w : stride);
    unsigned int c = 8;

    uint8_t rows = size * c / stride;
    for (uint8_t i = 0; i < rows; i++) {
        uint32_t base = i * stride / c;
        for (uint8_t j = 0; j < width; j++) {
            uint32_t idx = base + (j / c);
            uint8_t byte = idx < size ? data[idx] : 0;
            uint8_t bit = byte & (0x80 >> (j % c));
            ssd1306_DrawPixel(x + j, y + i, bit ? White : Black);
        }
    }
}

void ssd1306_SetContrast(const uint8_t value)
{
    const uint8_t kSetContrastControlRegister = 0x81;
    ssd1306_WriteCommand(kSetContrastControlRegister);
    ssd1306_WriteCommand(value);
}

void ssd1306_SetDisplayOn(const uint8_t on)
{
    uint8_t value;
    if (on) {
        value = 0xAF; // Display on
        SSD1306.DisplayOn = 1;
    } else {
        value = 0xAE; // Display off
        SSD1306.DisplayOn = 0;
    }
    ssd1306_WriteCommand(value);
}

uint8_t ssd1306_GetDisplayOn(void)
{
    return SSD1306.DisplayOn;
}

int g_ssd1306_current_loc_v = 0;
#define SSD1306_INTERVAL_V (15)

void ssd1306_ClearOLED(void)
{
    ssd1306_Fill(Black);
    g_ssd1306_current_loc_v = 0;
}

void ssd1306_printf(char *fmt, ...)
{
    char buffer[20];
    int ret = 0;
    if (fmt) {
        va_list argList;
        va_start(argList, fmt);
        ret = vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer), fmt, argList);
        if (ret < 0) {
            printf("buffer is null\r\n");
        }
        va_end(argList);
        ssd1306_SetCursor(0, g_ssd1306_current_loc_v);
        ssd1306_DrawString(buffer, Font_7x10, White);

        ssd1306_UpdateScreen();
    }
    g_ssd1306_current_loc_v += SSD1306_INTERVAL_V;
}
