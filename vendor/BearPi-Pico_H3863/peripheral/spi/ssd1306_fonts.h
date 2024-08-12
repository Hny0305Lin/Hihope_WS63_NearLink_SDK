/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
/**
 * @file ssd1306_fonts.h Fonts for monochrome/rgb oled display
 */

#ifndef SSD1306_FONTS_H
#define SSD1306_FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_FONTS FONTS: Supported LCD fonts
 * @{
 */

extern const unsigned char g_f6X8[][6];
extern const unsigned char g_f8X16[];

typedef struct {
    const unsigned char FontWidth; /*!< Font width in pixels */
    unsigned char FontHeight;      /*!< Font height in pixels */
    const unsigned short *data;    /*!< Pointer to data font data array */
} FontDef;

extern FontDef Font_7x10;
extern FontDef Font_6x8;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#ifdef __cplusplus
}
#endif

#endif // SSD1306_FONTS_H