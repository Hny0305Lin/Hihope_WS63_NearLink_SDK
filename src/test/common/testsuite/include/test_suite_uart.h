/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_UART_H
#define TEST_SUITE_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "uart.h"
#include "test_suite_channel.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    TEST_USB_SERIAL,
    TEST_CHIP_UART
} test_uart_mode_t;

/**
 * @defgroup test_common_testsuite_uart UART
 * @ingroup  test_common_testsuite
 * @{
 */
#ifdef SLEM_CARKEY
#define TEST_SUITE_UART_BAUD_RATE   921600
#else
#define TEST_SUITE_UART_BAUD_RATE   115200
#endif
#define NEWLINE                     "\r\n"

/**
 * @if Eng
 * @brief  Initialize test suite UART.
 * @else
 * @brief  初始化测试套件的UART。
 * @endif
 */
void test_suite_uart_init(void);

/**
 * @if Eng
 * @brief  Deinitialize test suite UART.
 * @else
 * @brief  去初始化测试套件的UART。
 * @endif
 */
void test_suite_uart_deinit(void);

/**
 * @if Eng
 * @brief  reset the baud rate of test suite UART.
 * @else
 * @brief  重置测试套件的UART的波特率。
 * @endif
 */
void test_suite_uart_reset_baud_rate(void);

/**
 * @if Eng
 * @brief  Sends a single char over the Test Suite UART.
 * @param  [in]  c char to send.
 * @else
 * @brief  通过测试套件指定的UART发送单字节。
 * @param  [in]  c 要发送的单个字节。
 * @endif
 */
void test_suite_uart_send_char(char c);

/**
 * @if Eng
 * @brief  Sends a string over the Test Suite UART.
 * @param  [in]  str string to send.
 * @else
 * @brief  通过测试套件指定的UART发送字符串。
 * @param  [in]  str 要发送的字符串。
 * @endif
 */
void test_suite_uart_send(const char *str);

/**
 * @if Eng
 * @brief  Sends a formatted char* out of the Test Suite UART.
 * @param  [in]  str string to send.
 * @else
 * @brief  通过测试套件指定的UART格式化发送字符串。
 * @param  [in]  str 要发送的字符串。
 * @endif
 */
void test_suite_uart_sendf(const char *str, ...);

/**
 * @if Eng
 * @brief  Sends a string over the Test Suite UART and adds "\r\n" at the end.
 * @param  [in]  str string to send.
 * @else
 * @brief  通过测试套件指定的UART发送字符串并且换行。
 * @param  [in]  str 要发送的字符串。
 * @endif
 */
void test_suite_uart_send_line(const char *str);

/**
 * @if Eng
 * @brief  Gets test suite uart functions.
 * @return test suite uart functions address.
 * @else
 * @brief  获取测试套件uart功能函数。
 * @return 测试套件uart功能函数接口地址。
 * @endif
 */
test_suite_channel_funcs_t *test_suite_uart_funcs_get(void);

/**
 * @if Eng
 * @brief  Testsuite uart callback function.
 * @param [in] buffer The data buffer that uart receive.
 * @param [in] length The data length.
 * @param [in] error The uart error flag.
 * @else
 * @brief  测试套件uart回调函数。
 * @param [in] buffer 测试套件uart接收数据地址。
 * @param [in] length 测试套件uart接收数据长度。
 * @param [in] error  测试套件uart接收错误标志。
 * @endif
 */
void test_suite_uart_rx_callback(const void *buffer, uint16_t length, bool error);

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
void test_suite_switch_serial_mode(test_uart_mode_t mode);
#endif
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
