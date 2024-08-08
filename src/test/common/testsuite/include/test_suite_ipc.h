/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_IPC_H
#define TEST_SUITE_IPC_H

#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
#include "ipc.h"
#endif
#include "test_suite_channel.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define TEST_SUITE_PRIVATE_COMMAND_BUFFER_SIZE      60

#define TEST_SUITE_CORE_IMPLEMENTING_THE_CONSOLE    APPS

#define CORES_CORE_IMPLEMENTING_THE_CONSOLE         CORES_APPS_CORE

#define CMD_TEST_RUNNING                            0
#define CMD_TEST_PASS                               1
#define CMD_TEST_FAIL                               2
#define CMD_TEST_ERROR                              3
#define CMD_TEST_RETURN_CONTROL                     4

#define TEST_SUITE_TIMER_S_TO_US                    1000000
/**
 * @defgroup test_common_testsuite_ipc IPC
 * @ingroup  test_common_testsuite
 * @{
 */

/**
 * @if Eng
 * @brief  Gets test suite ipc functions.
 * @return test suite ipc functions address.
 * @else
 * @brief  获取测试套件ipc功能函数。
 * @return 测试套件ipc功能函数接口地址。
 * @endif
 */
test_suite_channel_funcs_t *test_suite_ipc_funcs_get(void);

#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE

/**
 * @if Eng
 * @brief  Clear the receive flag of echo core.
 * @param  [in]  core core.
 * @else
 * @brief  清除各个核的接收标志
 * @param  [in]  core 核。
 * @endif
 */
void test_suite_commands_echo_clear(cores_t core);

/**
 * @if Eng
 * @brief  Clear the receive flag of all core.
 * @else
 * @brief  清除所有核的接收标志
 * @endif
 */
void test_suite_ipc_clear_core_flag(void);

/**
 * @if Eng
 * @brief  The command is executed on another core, blocking the execution.
 * @param  [in]  core core.
 * @param  [in]  commmand command string.
 * @param  [in]  timeout timeout interval, unit ms.
 * @else
 * @brief  命令在别的核执行，阻塞执行
 * @param  [in]  core 核.
 * @param  [in]  commmand 命令字符.
 * @param  [in]  timeout 超时时间,单位ms.
 * @endif
 */
void test_suite_commands_execute_in_external_core(cores_t core, char *command, uint32_t timeout);

/**
 * @if Eng
 * @brief  Callback function for receiving command responses.
 * @param  [in]  message The ipc_action_t of the received message.
 * @param  [in]  payload_p Data payload of the received mssage.
 * @param  [in]  src Sending core.
 * @param  [in]  id Internal message id. Can be used to detect duplicate delivery and for debug.
 * @return True if the incoming mailslot shluld be released or false if the message was
 *         not (fully) processed and should be redelivered
 * @else
 * @brief  命令的响应接收回调函数.
 * @param  [in]  message 接收消息的ipc_action_t。
 * @param  [in]  payload_p 接收到的消息缓存buffer。
 * @param  [in]  src 发送核.
 * @param  [in]  id 内部消息流水号ID。可用于检测重复交付和调试.
 * @return 如果消息处理完成,应释放本条消息所在内存，则为True
 *         如果消息未完全处理完成,且应重新处理该条消息，则为False
 * @endif
 */
bool test_suite_commands_ipc_message_handler(ipc_action_t message,
    const volatile ipc_payload *payload_p, cores_t src, uint32_t id);

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
