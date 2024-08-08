/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_CONSOLE_H
#define TEST_SUITE_CONSOLE_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_console Console
 * @ingroup  test_common_testsuite
 * @{
 */

#define CONSOLE_CHAR_BACKSPACE (0x08)
#define CONSOLE_CHAR_ENTER     (0x0D)
#define CONSOLE_CHAR_ESC       (0x1B)
#define CONSOLE_CHAR_UP        (0x41)
#define CONSOLE_CHAR_DOWN      (0x42)
#define CONSOLE_CHAR_RIGHT     (0x43)
#define CONSOLE_CHAR_LEFT      (0x44)
#define CONSOLE_CHAR_CSI       (0x5B)
#define CONSOLE_CHAR_DELETE    (0x7F)
#define CONSOLE_CHAR_NONE      (0xFF)

#define CONSOLE_CHAR_PAGE_HOME (0x31)
#define CONSOLE_CHAR_PAGE_END  (0x34)
#define CONSOLE_CHAR_PAGE_UP   (0x35)
#define CONSOLE_CHAR_PAGE_DOWN (0x36)
#define CONSOLE_CHAR_PAGE_7E   (0x7E)

#define CONSOLE_COMMAND_CLEAR    ("[1J")
#define CONSOLE_COMMAND_MOVE_1_1 ("[11H")

/**
 * @if Eng
 * @brief  Testsuite console color.
 * @else
 * @brief  测试套件控制台颜色。
 * @endif
 */
typedef enum {
    TERM_COLOR_RESET = 0,
    TERM_COLOR_BLACK = 30,
    TERM_COLOR_RED,
    TERM_COLOR_GREEN,
    TERM_COLOR_BROWN,
    TERM_COLOR_BLUE,
    TERM_COLOR_PINK,
    TERM_COLOR_CYAN,
    TERM_COLOR_WHITE = 39
} term_color_t;

/**
 * @if Eng
 * @brief  Initialize test suite console.
 * @else
 * @brief  初始化测试套件控制台。
 * @endif
 */
void test_suite_console_init(void);

/**
 * @if Eng
 * @brief  Register console command buffer.
 * @param  [in]  buffer_to_register the buffer register to console.
 * @else
 * @brief  注册控制台的命令输入缓存。
 * @param  [in]  buffer_to_register 要注册给控制台的缓存。
 * @endif
 */
void test_suite_console_register_command_buffer(char *buffer_to_register);

/**
 * @if Eng
 * @brief  Enable console command process，if enabled, allow receive buffer form channel.
 * @else
 * @brief  打开控制台的命令解析，打开状态下允许接收通道传来的命令。
 * @endif
 */
void test_suite_console_enable(void);

/**
 * @if Eng
 * @brief  Disable console command  process，if disabled, not receive buffer form channel.
 * @else
 * @brief  关闭控制台的命令解析，关闭状态下不再接收通道传来的命令。
 * @endif
 */
void test_suite_console_disable(void);

/**
 * @if Eng
 * @brief  Get test suite console status.
 * @return  false:disabled; true:enable.
 * @else
 * @brief  获取测试套件控制台的状态。
 * @return  false:关闭; true:打开。
 * @endif
 */
bool test_suite_console_is_enabled(void);

/**
 * @if Eng
 * @brief  Process the char from the channel.
 * @param  [in]  c the char to process.
 * @else
 * @brief  单字节处理通道传来的消息。
 * @param  [in]  c 要处理的字节。
 * @endif
 */
void test_suite_console_process_char(char c);

/**
 * @if Eng
 * @brief  Set test suite console background color.
 * @param  [in]  color console color type.
 * @else
 * @brief  设置测试控制台背景色。
 * @param  [in]  color 控制台颜色类型。
 * @endif
 */
void test_suite_console_set_color(term_color_t color);

/**
 * @if Eng
 * @brief  Displat test suite test status over console.
 * @param  [in]  status test suite test status.
 * @else
 * @brief  通过控制台显示测试套件当前的测试状态。
 * @param  [in]  status 测试套件的测试状态。
 * @endif
 */
void test_suite_console_display_test_status(int status);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
