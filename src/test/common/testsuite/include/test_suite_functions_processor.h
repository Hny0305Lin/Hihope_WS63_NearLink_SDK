/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_FUNCTIONS_PROCESSOR_H
#define TEST_SUITE_FUNCTIONS_PROCESSOR_H

#include <stdint.h>
#include "common_def.h"
#include "test_suite.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_funcs_processor Functions Processor
 * @ingroup  test_common_testsuite
 * @{
 */

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
errcode_t test_suite_function_add_func(char *name, char *description, test_suite_function_callback_t func);

/**
 * @if Eng
 * @brief  Init testsuite function list.
 * @else
 * @brief  初始化测试用例函数列表。
 * @endif
 */
void test_suite_function_init(void);

/**
 * @if Eng
 * @brief  Deinit function that delete all of the stored testsuit functions.
 * @else
 * @brief  删除所有已注册的测试函数接口。
 * @endif
 */
void test_suite_function_deinit(void);

/**
 * @if Eng
 * @brief  Generic function that list all of the stored testsuit functions out of the channel.
 * @else
 * @brief  通过通道输出所有已注册的功能测试接口。
 * @endif
 */
void test_suite_function_list_func(void);

/**
 * @if Eng
 * @brief  Executes the command.
 * @param  [in]  command The command to execute.
 * @else
 * @brief  执行命令。
 * @param  [in]  command 要处理的命令。
 * @endif
 */
void test_suite_function_execute_command(char *command);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
