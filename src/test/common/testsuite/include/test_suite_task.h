/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifndef TEST_SUITE_TASK_H
#define TEST_SUITE_TASK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef SUPPORT_AUDIO_LIEYIN_TOOL
#define CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE   512
#define TEST_SUITE_MSG_QUEUE_MAX_SIZE CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE
#else
#define TEST_SUITE_MSG_QUEUE_MAX_SIZE 128
#define CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE   300
#endif

/**
 * @defgroup test_common_testsuite_task Task
 * @ingroup  test_common_testsuite
 * @{
 */

/**
 * @if Eng
 * @brief  Write data to message queue.
 * @param  [in]  buffer_addr data address.
 * @param  [in]  buffer_size data size.
 * @else
 * @brief  将数据填给消息队列。
 * @param  [in]  buffer_addr 数据地址。
 * @param  [in]  buffer_size 数据大小。
 * @endif
 */
int32_t test_suite_write_msgqueue(const uint8_t *buffer_addr, uint16_t buffer_size);

/**
 * @if Eng
 * @brief  Create test suite task and message queue.
 * @else
 * @brief  创建测试套件任务及其消息队列。
 * @endif
 */
void test_suite_task_init(void);

/**
 * @if Eng
 * @brief  Destory test suite task and message queue.
 * @else
 * @brief  删除测试套件任务及其消息队列。
 * @endif
 */
void test_suite_task_destroy(void);

/**
 * @if Eng
 * @brief  Set test suite task status.
 * @param  [in]  status test suite task status, false:running; true:exit.
 * @else
 * @brief  设置测试套件运行状态。
 * @param  [in]  status 测试套件运行状态，false:运行中; true:退出。
 * @endif
 */
void test_suite_task_set_exit(bool status);

/**
 * @if Eng
 * @brief  Get test suite task status.
 * @return test suite task status, false:running; true:exit.
 * @else
 * @brief  设置测试套件运行状态。
 * @return 测试套件运行状态, false:运行中; true:退出。
 * @endif
 */
bool test_suite_task_is_exit(void);

/**
 * @if Eng
 * @brief  This function should be called in the main loop, if nonos.
 * @else
 * @brief  无OS的条件下，应该在主循环中调用该函数。
 * @endif
 */
void test_suite_nonos_loop(void);

/**
 * @if Eng
 * @brief  Write data to command queue.
 * @param  [in]  buffer_addr data address.
 * @param  [in]  buffer_size data size.
 * @else
 * @brief  将数据填给命令帧的消息队列。
 * @param  [in]  buffer_addr 数据地址。
 * @param  [in]  buffer_size 数据大小。
 * @endif
 */
int32_t test_suite_write_cmd_queue(const uint8_t *buffer_addr, uint16_t buffer_size);

/**
 * @if Eng
 * @brief  Receive data from command queue.
 * @param  [in]  buffer_addr data address.
 * @param  [in]  buffer_size data size.
 * @else
 * @brief  接收命令帧消息队列的数据。
 * @param  [in]  buffer_addr 数据地址。
 * @param  [in]  buffer_size 数据大小。
 * @endif
 */
int32_t test_suite_receive_cmd_queue(char *buffer_addr, uint32_t *buffer_size);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
