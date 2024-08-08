/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_api API
 * @ingroup  test_common_testsuite
 * @{
 */
#define TEST_PARAM_ARGC_1 1
#define TEST_PARAM_ARGC_2 2
#define TEST_PARAM_ARGC_3 3
#define TEST_PARAM_ARGC_4 4
#define TEST_PARAM_ARGC_5 5
#define TEST_PARAM_ARGC_6 6
#define TEST_PARAM_ARGC_7 7
#define TEST_PARAM_4_BYTES 4
#define TEST_PARAM_8_BYTES 8

#define add_function(x, y, z) uapi_test_suite_add_function(x, y, z)

/**
 * @if Eng
 * @brief  Test function pointer interface.
 * @param  [in]  argc The input parameter number.
 * @param  [in]  argv The input paramter list.
 * @retval ERRCODE_SUCC Test success.
 * @retval Other        Test failure. For details, see @ref errcode_t.
 * @else
 * @brief  功能测试函数回调指针。
 * @param  [in]  argc 入参个数。
 * @param  [in]  argv 入参列表。
 * @retval ERRCODE_SUCC 测试成功。
 * @retval Other        测试失败。 参考 see @ref errcode_t 。
 * @endif
 */
typedef int (*test_suite_function_callback_t)(int32_t argc, char *argv[]);

/**
 * @if Eng
 * @brief  Initialize test suite resources.
 * @else
 * @brief  初始化测试套件资源。
 * @endif
 */
void uapi_test_suite_init(void);

/**
 * @if Eng
 * @brief  Deinitialize test suite resources.
 * @else
 * @brief  去初始化测试套件资源。
 * @endif
 */
void uapi_test_suite_deinit(void);

/**
 * @if Eng
 * @brief  Adds a function to the testsuite functions list.
 * @param  [in]  name The external 'user' name of the function.
 * @param  [in]  description A brief description of the function.
 * @param  [in]  func function pointer to the function.
 * @retval ERRCODE_SUCC The function has been added.
 * @retval Other        Too many functions have been added. For details, see @ref errcode_t.
 * @else
 * @brief  添加需要功能测试到测试套件的列表中。
 * @param  [in]  name 测试的功能函数名字。
 * @param  [in]  description 简要的功能测试描述。
 * @param  [in]  func 功能测试函数接口。
 * @retval ERRCODE_SUCC 函数添加成功。
 * @retval Other        函数添加超过最大数量。 参考 see @ref errcode_t 。
 * @endif
 */
errcode_t uapi_test_suite_add_function(char *name, char *description, test_suite_function_callback_t func);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif