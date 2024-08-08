/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_CHANNEL_H
#define TEST_SUITE_CHANNEL_H

#include <stdint.h>
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_channel Channel
 * @ingroup  test_common_testsuite
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize channel for testsuite.
 * @else
 * @brief  通道层的初始化接口。
 * @endif
 */
typedef void (*test_suite_channel_init_t)(void);

/**
 * @if Eng
 * @brief  Initialize channel for testsuite.
 * @else
 * @brief  通道层的初始化接口。
 * @endif
 */
typedef void (*test_suite_channel_deinit_t)(void);

/**
 * @if Eng
 * @brief  Channel sends char interface.
 * @param  [in]  c char to send.
 * @else
 * @brief  通道层的字节发送接口。
 * @param  [in]  c 要发送的单个字节。
 * @endif
 */
typedef void (*test_suite_channel_send_char_t)(char c);

/**
 * @if Eng
 * @brief  Channel sends string interface.
 * @param  [in]  str string to send.
 * @else
 * @brief  通道层的字符串发送接口。
 * @param  [in]  str 要发送的字符串。
 * @endif
 */
typedef void (*test_suite_channel_send_t)(const char *str);

/**
 * @if Eng
 * @brief  Channel sends format string interface.
 * @param  [in]  str string to send.
 * @else
 * @brief  通道层的格式化字符串发送接口。
 * @param  [in]  str 要发送的字符串。
 * @endif
 */
typedef void (*test_suite_channel_sendf_t)(const char *str, ...);

/**
 * @if Eng
 * @brief  Channel sends string and "\r\n" interface.
 * @param  [in]  str string to send.
 * @else
 * @brief  通道层的字符串发送并换行接口。
 * @param  [in]  str 要发送的字符串。
 * @endif
 */
typedef void (*test_suite_channel_send_line_t)(const char *str);

typedef struct {
    test_suite_channel_init_t       init;              /*!< @if Eng Init channel interface.
                                                            @else   通道层的初始化接口。 @endif */
    test_suite_channel_deinit_t     deinit;            /*!< @if Eng Deinit channel interface.
                                                            @else   通道层的去初始化接口。 @endif */
    test_suite_channel_send_char_t  send_char;         /*!< @if Eng Channel sends char interface.
                                                            @else   通道层发送字节接口。 @endif */
    test_suite_channel_send_t       send;              /*!< @if Eng Channel sends string interface.
                                                            @else   通道层发送字符串接口。 @endif */
    test_suite_channel_sendf_t      sendf;             /*!< @if Eng Channel sends format string interface.
                                                            @else   通道层发送格式化字符串接口。 @endif */
    test_suite_channel_send_line_t  send_line;         /*!< @if Eng Channel sends string and "\r\n" interface.
                                                            @else   通道层发送字符串并换行接口。 @endif */
} test_suite_channel_funcs_t;

/**
 * @if Eng
 * @brief  Initialize test suite channel.
 * @param  [in]  funcs test suite channel funtions.
 * @else
 * @brief  初始化测试套件的通道。
 * @param  [in]  funcs 测试套件通道函数接口。
 * @endif
 */
void test_suite_channel_init(test_suite_channel_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Deinit test suite channel.
 * @else
 * @brief  去初始化测试套件的逻辑通道。
 * @endif
 */
void test_suite_channel_deinit(void);

/**
 * @if Eng
 * @brief  Get test suite channel functions.
 * @retval channel functions.
 * @else
 * @brief  获取逻辑通道的函数接口。
 * @retval 通道函数接口。
 * @endif
 */
test_suite_channel_funcs_t *test_suite_channel_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif