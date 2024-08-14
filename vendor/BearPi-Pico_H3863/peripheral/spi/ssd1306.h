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

#ifndef SSD1306_H
#define SSD1306_H

#include <stddef.h>
#include <stdint.h>
#include "ssd1306_fonts.h"

// #define SSD1306_USE_I2C
#define SSD1306_USE_SPI

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT 64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128
#endif

// some LEDs don't display anything in first two columns

#ifndef SSD1306_BUFFER_SIZE
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)
#endif

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef enum {
    SSD1306_OK = 0x00,
    SSD1306_ERR = 0x01 // Generic error.
} SSD1306_Error_t;

// Struct to store transformations
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SSD1306_t;
typedef struct {
    uint8_t x;
    uint8_t y;
} SSD1306_VERTEX;

// Procedure definitions
void ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_UpdateScreen(void);

char ssd1306_DrawChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_DrawString(char *str, FontDef Font, SSD1306_COLOR color);

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
void ssd1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
void ssd1306_DrawPolyline(const SSD1306_VERTEX *par_vertex, uint16_t par_size, SSD1306_COLOR color);
void ssd1306_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
void ssd1306_DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR par_color);
void ssd1306_DrawBitmap(const uint8_t *bitmap, uint32_t size);
void ssd1306_DrawRegion(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data, uint32_t size);

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void ssd1306_SetContrast(const uint8_t value);
/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void ssd1306_SetDisplayOn(const uint8_t on);
/**
 * @brief Reads DisplayOn state.
 * @return  0: OFF.
 *          1: ON.
 */
uint8_t ssd1306_GetDisplayOn(void);

// Low-level procedures
void ssd1306_Reset(void);
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_WriteData(uint8_t *buffer, size_t buff_size);
SSD1306_Error_t ssd1306_FillBuffer(uint8_t *buf, uint32_t len);
void ssd1306_ClearOLED(void);
void ssd1306_printf(char *fmt, ...);

#endif // __SSD1306_H__