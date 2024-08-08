/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_LOG_H
#define TEST_SUITE_LOG_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_log Log
 * @ingroup  test_common_testsuite
 * @{
 */

/**
 * @if Eng
 * @brief  Register Test Suite Channel to the log.
 * @else
 * @brief  将测试套件的逻辑通道注册给记录。
 * @endif
 */
void test_suite_log_get_channel_funcs(void);

/**
 * @if Eng
 * @brief  Logs a char over Test Suite Channel.
 * @param  [in]  data the string to log.
 * @else
 * @brief  通过测试套件通道记录一个字符。
 * @param  [in]  data 要记录的字符。
 * @endif
 */

void test_suite_log_char(char data);

/**
 * @if Eng
 * @brief  Logs a string over Test Suite Channel.
 * @param  [in]  str the string to log.
 * @else
 * @brief  通过测试套件通道记录一个字符串。
 * @param  [in]  str 要记录的字符串。
 * @endif
 */
void test_suite_log_string(const char *str);


/**
 * @if Eng
 * @brief  Logs a string with argument over Test Suite Channel.
 * @param  [in]  str the string with argument to log.
 * @else
 * @brief  通过测试套件通道记录一个字符串。
 * @param  [in]  str 要记录的字符串。
 * @endif
 */
void test_suite_log_stringf(const char *str, ...);

/**
 * @if Eng
 * @brief  Logs a string over Test Suite Channel and adds "\r\n" at the end.
 * @param  [in]  line the string to log.
 * @else
 * @brief  通过测试套件通道记录一个字符串并换行。
 * @param  [in]  line 要记录的字符串。
 * @endif
 */
void test_suite_log_line(char *line);

/**
 * @if Eng
 * @brief  Log latest test result.
 * @param  [in]  result latest test result. For details, see @ref errcode_t.
 * @else
 * @brief  记录最后一次测试的结果。
 * @param  [in]  result 最近一次的测试结果。 参考 see @ref errcode_t 。
 * @endif
 */
void test_suite_log_set_test_result(int result);

/**
 * @if Eng
 * @brief  Get latest test result.
 * @retval ERRCODE_SUCC Test success.
 * @retval Other        Test failed. For details, see @ref errcode_t.
 * @else
 * @brief  获取最后一次的测试结果。
 * @retval ERRCODE_SUCC 测试成功。
 * @retval Other        测试失败。 参考 see @ref errcode_t 。
 * @endif
 */
int test_suite_log_get_test_result(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
