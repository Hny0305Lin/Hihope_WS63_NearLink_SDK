/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "securec.h"
#include "test_suite_console.h"
#include "test_suite_task.h"
#include "test_suite_uart.h"
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
#include "gadget/usbd_acm.h"
#endif

#ifdef SUPPORT_AUDIO_LIEYIN_TOOL
#define TEST_SUITE_UART_RX_BUFFER_SIZE  512
#else
#define TEST_SUITE_UART_RX_BUFFER_SIZE  128
#endif
#define TEST_SUITE_UART_LEN_MAX         512
#define TEST_SUITE_UART_QUEUE_SIZE      1024

static uart_bus_t g_test_suite_uart = TEST_SUITE_UART_BUS;
static uint8_t g_test_suite_uart_rx_buffer_test[TEST_SUITE_UART_RX_BUFFER_SIZE];

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
static test_uart_mode_t g_test_uart_mode = TEST_CHIP_UART;
#endif

void test_suite_uart_init(void)
{
    uart_buffer_config_t uart_buffer_config;

    uart_pin_config_t uart_pin_config = {
        .tx_pin = TEST_SUITE_UART_TX_PIN,
        .rx_pin = TEST_SUITE_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uart_attr_t uart_line_config = {
        .baud_rate = TEST_SUITE_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_buffer_config.rx_buffer_size = TEST_SUITE_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_test_suite_uart_rx_buffer_test;
    (void)uapi_uart_init(TEST_SUITE_UART_BUS, &uart_pin_config, &uart_line_config, NULL, &uart_buffer_config);

#ifdef SUPPORT_AUDIO_LIEYIN_TOOL
    uapi_uart_register_rx_callback(g_test_suite_uart, UART_RX_CONDITION_MASK_IDLE, 1, \
                                   test_suite_uart_rx_callback);
#else
    uapi_uart_register_rx_callback(g_test_suite_uart, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE, 1, \
                                   test_suite_uart_rx_callback);
#endif
}

void test_suite_uart_deinit(void)
{
    if (g_test_suite_uart == TEST_SUITE_UART_BUS) {
        uapi_uart_deinit(g_test_suite_uart);
    }
}

void test_suite_uart_reset_baud_rate(void)
{
    uart_attr_t uart_line_config = {
        .baud_rate = TEST_SUITE_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    while (uapi_uart_has_pending_transmissions(TEST_SUITE_UART_BUS)) {};
    uapi_uart_set_attr(TEST_SUITE_UART_BUS, &uart_line_config);
}

static void test_suite_uart_write(const char *buffer, uint16_t length)
{
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
    if (g_test_uart_mode == TEST_USB_SERIAL) {
        usb_serial_write(0, buffer, length);
    } else {
        uapi_uart_write(g_test_suite_uart, (uint8_t *)buffer, (uint32_t)length, 0);
    }
#else
    uapi_uart_write(g_test_suite_uart, (uint8_t *)buffer, (uint32_t)length, 0);
#endif
}

void test_suite_uart_send_char(char c)
{
    test_suite_uart_write(&c, 1);
}

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
void test_suite_switch_serial_mode(test_uart_mode_t mode)
{
    g_test_uart_mode = mode;
}
#endif

void test_suite_uart_send(const char *str)
{
    test_suite_uart_write(str, (uint16_t)strlen(str));
}

void test_suite_uart_sendf(const char *str, ...)
{
    static char s[TEST_SUITE_UART_LEN_MAX];  /* This needs to be large enough to store the string */
    int32_t str_len;

    va_list args;
    va_start(args, str);
    str_len = vsprintf_s(s, sizeof(s), str, args);
    va_end(args);

    if (str_len < 0) {
        return;
    }

    test_suite_uart_send(s);
}

void test_suite_uart_send_line(const char *str)
{
    test_suite_uart_send(str);
    test_suite_uart_send("\r\n");
}

test_suite_channel_funcs_t *test_suite_uart_funcs_get(void)
{
    static test_suite_channel_funcs_t test_suite_uart_funcs = {
        .init = test_suite_uart_init,
        .deinit = test_suite_uart_deinit,
        .send_char = test_suite_uart_send_char,
        .send = test_suite_uart_send,
        .sendf = test_suite_uart_sendf,
        .send_line = test_suite_uart_send_line,
    };
    return &test_suite_uart_funcs;
}

void test_suite_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    if (length == 0 || error) {
        return;
    }

    if (test_suite_console_is_enabled()) {
        test_suite_write_msgqueue(buffer, length);
    }
}
