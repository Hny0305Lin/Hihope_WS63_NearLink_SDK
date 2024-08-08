/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: serial
 *
 * Create: 2021-03-09
 */

#include <stdbool.h>
#include <string.h>
#include "boot_def.h"
#include "boot_delay.h"
#include "boot_serial.h"

#define UART_GETC_DELAY_TIME                    2
#define UART_GETS_DELAY_TIME                    2000
#define UART_DELAY_TIME                         (8 * 1000000)

#define HIBURN_UART_RX_BUFFER_SIZE              1

uart_bus_t g_hiburn_uart = UART_BUS_NONE;
bool g_uart_mute = false;
bool g_hiburn_uart_enabled = false;

void serial_set_mute(void)
{
    g_uart_mute = true;
}

static bool check_uart_mute(void)
{
    return g_uart_mute;
}

/* 取消禁言 */
void serial_cancel_mute(void)
{
    g_uart_mute = false;
}

void serial_putc(const char c)
{
    bool is_mute = check_uart_mute();
    if (is_mute == true) {
        return;
    }
    uapi_uart_write(HIBURN_CODELOADER_UART, (uint8_t *)&c, sizeof(int8_t), UART_GETC_DELAY_TIME);
}

void serial_puts(const char *s)
{
    bool is_mute = check_uart_mute();
    if (is_mute == true) {
        return;
    }
    uapi_uart_write(HIBURN_CODELOADER_UART, (uint8_t *)s, strlen(s), UART_GETS_DELAY_TIME);
}

void serial_put_buf(const char *buffer, int32_t length)
{
    bool is_mute;

    is_mute = check_uart_mute();
    if (is_mute == true) {
        return;
    }

    uapi_uart_write(HIBURN_CODELOADER_UART, (uint8_t *)buffer, (uint32_t)length, UART_GETS_DELAY_TIME);
}

uint8_t serial_getc(void)
{
    uint8_t getc = 0;
    uapi_uart_read(HIBURN_CODELOADER_UART, (uint8_t *)&getc, sizeof(getc), UART_GETC_DELAY_TIME);
    return getc;
}


void serial_puthex(uint32_t h, bool print_all)
{
    int32_t i;
    uint8_t c;
    char mark = 0;
    uint32_t temp = h;

    bool is_mute = check_uart_mute();
    if (is_mute == true) {
        return;
    }

    serial_puts("0x");

    for (i = 0; i < BITS_PER_BYTE; i++) {
        c = (temp >> 28) & 0x0F; /* u32 right shift 28 */

        if (c >= DECIMAL) {
            c = (c - DECIMAL) + 'A';
        } else {
            c = c + '0';
        }

        if (print_all) {
            serial_putc(c);
            temp = temp << 4; /* u32 left shift 4 */
            continue;
        }

        /* 如果不是最后一个数且之前数字都为0 */
        if ((mark == 0) && (i != BITS_PER_BYTE - 1)) {
            if (c != '0') {
                mark = 1;
                serial_putc(c);
            }
        } else {
            serial_putc(c);
        }

        temp = temp << 4; /* u32 left shift 4 */
    }
}

uint32_t serial_getc_timeout(uint32_t timeout_us, uint8_t *ch)
{
    uint32_t cnt = 0;

    while (cnt < timeout_us) {
        if (uapi_uart_rx_fifo_is_empty(HIBURN_CODELOADER_UART) == false) {
            *ch = serial_getc();
            return ERRCODE_SUCC;
        }
        cnt++;
    }
    return ERRCODE_FAIL;
}
#ifndef ATE_CLOSE_DEBUG
void boot_put_errno(uint32_t print_errno)
{
    serial_puts("\nerrno=");
    serial_puthex(print_errno, 0);
    serial_puts("\n");
    mdelay(RESET_DELAY_MS);
}

void boot_puthex(uint32_t h, bool print0)
{
    serial_puts(" ");
    serial_puthex(h, print0);
}

void boot_msg0(const char *s)
{
    serial_puts(s);
    serial_puts("\r\n");
}

void boot_msg1(const char *s, uint32_t h)
{
    serial_puts(s);
    serial_puthex(h, 0);
    serial_puts("\r\n");
}

void boot_msg2(const char *s, uint32_t h1, uint32_t h2)
{
    serial_puts(s);
    serial_puthex(h1, 0);
    serial_puts(" ");
    serial_puthex(h2, 0);
    serial_puts("\r\n");
}

void boot_msg4(const char *s, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4)
{
    serial_puts(s);
    serial_puthex(h1, 0);
    serial_puts(" ");
    serial_puthex(h2, 0);
    serial_puts(" ");
    serial_puthex(h3, 0);
    serial_puts(" ");
    serial_puthex(h4, 0);
    serial_puts("\r\n");
}

void hiburn_uart_init(uart_param_stru uart_param, uart_bus_t uart_bus)
{
    uart_pin_config_t uart_pins;
    uart_attr_t uart_line_config;
    uart_buffer_config_t uart_buffer_config;
    uint8_t g_hiburn_rx_buffer[HIBURN_UART_RX_BUFFER_SIZE] = { 0 };
    UNUSED(uart_bus);

    g_hiburn_uart = HIBURN_CODELOADER_UART;
    // TX configuration
    uart_pins.tx_pin = HIBURN_UART_TX;
    uart_pins.rts_pin = PIN_NONE;

    // RX configuration
    uart_pins.rx_pin = HIBURN_UART_RX;
    uart_pins.cts_pin = PIN_NONE;

    uart_line_config.baud_rate = uart_param.baudrate;
    uart_line_config.data_bits = uart_param.databit - 5; // 映射需要减5
    uart_line_config.parity = uart_param.parity;
    if (uart_line_config.parity == 1) { // 1代表奇校验
        uart_line_config.parity = UART_PARITY_ODD;
    } else if (uart_line_config.parity == 2) { // 2代表偶校验
        uart_line_config.parity = UART_PARITY_EVEN;
    } else {
        uart_line_config.parity = UART_PARITY_NONE;
    }
    uart_line_config.stop_bits = uart_param.stopbit - 1;

    uart_buffer_config.rx_buffer_size = HIBURN_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_hiburn_rx_buffer;

    (void)uapi_uart_init(HIBURN_CODELOADER_UART, &uart_pins, &uart_line_config, NULL, &uart_buffer_config);
    uart_port_unregister_irq(HIBURN_CODELOADER_UART);
    g_hiburn_uart_enabled = true;
}

void hiburn_uart_deinit(void)
{
    uapi_uart_deinit(g_hiburn_uart);
    g_hiburn_uart_enabled = false;
}
#endif
