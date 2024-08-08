/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at common service api header for product \n
 */

#ifndef AT_PRODUCT_H
#define AT_PRODUCT_H

#include "at.h"

/**
 * @defgroup middleware_utils_at_product Product
 * @ingroup  middleware_utils_at
 * @{
 */

/**
 * @if Eng
 * @brief  Declare AT msg queue create function type.
 * @else
 * @brief  声明AT命令消息队列创建接口类型。
 * @endif
 */
typedef void(*at_msg_queue_create_func_t)(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id);

/**
 * @if Eng
 * @brief  Declare AT msg queue write function type.
 * @else
 * @brief  声明AT命令消息队列写接口类型。
 * @endif
 */
typedef uint32_t(*at_msg_queue_write_func_t)(unsigned long queue_id, void *msg_ptr,
                                             uint32_t msg_size, uint32_t timeout);

/**
 * @if Eng
 * @brief  Declare AT msg queue read function type.
 * @else
 * @brief  声明AT命令消息队列读接口类型。
 * @endif
 */
typedef uint32_t(*at_msg_queue_read_func_t)(unsigned long queue_id, void *buf_ptr,
                                            uint32_t *buf_size, uint32_t timeout);

/**
 * @if Eng
 * @brief  Declare AT log function type.
 * @else
 * @brief  声明AT命令日志接口类型。
 * @endif
 */
typedef uint32_t(*at_log_func_t)(const char *buf, uint16_t buf_size, uint8_t level);

/**
 * @if Eng
 * @brief  Declare AT task pause function type.
 * @else
 * @brief  声明AT命令任务暂停接口类型。
 * @endif
 */
typedef void(*at_task_pause_func_t)(void);

/**
 * @if Eng
 * @brief  Declare AT malloc function type.
 * @else
 * @brief  声明AT命令内存申请函数类型。
 * @endif
 */
typedef void*(*at_malloc_func_t)(uint32_t);

/**
 * @if Eng
 * @brief  Declare AT free function type.
 * @else
 * @brief  声明AT命令内存释放函数类型。
 * @endif
 */
typedef void(*at_free_func_t)(void*);

/**
 * @if Eng
 * @brief  Declare AT timeout callback function type.
 * @else
 * @brief  声明AT定时器回调函数类型。
 * @endif
 */
typedef void(*at_timer_callback_func_t)(void *argument);
 
/**
 * @if Eng
 * @brief  Declare AT timeout create function type.
 * @else
 * @brief  声明AT定时器创建函数类型。
 * @endif
 */
typedef void*(*at_timer_create_func_t)(at_timer_callback_func_t callback, void *argument);
 
/**
 * @if Eng
 * @brief  Declare AT timeout start function type.
 * @else
 * @brief  声明AT定时器启动函数类型。
 * @endif
 */
typedef void(*at_timer_start_func_t)(void *timer_handle, uint32_t time_us);
 
/**
 * @if Eng
 * @brief  Declare AT timeout stop function type.
 * @else
 * @brief  声明AT定时器删除函数类型。
 * @endif
 */
typedef void(*at_timer_delete_func_t)(void *timer_handle);
 
/**
 * @if Eng
 * @brief  Declare AT channel creating one mutex function type.
 * @else
 * @brief  声明AT命令创建互斥锁函数类型。
 * @endif
 */
typedef void*(*at_create_mutex_func_t)(void);
 
/**
 * @if Eng
 * @brief  Declare AT channel acquiring one mutex function type.
 * @else
 * @brief  声明AT命令获取互斥锁函数类型。
 * @endif
 */
typedef void(*at_acquire_mutex_func_t)(void*);
 
/**
 * @if Eng
 * @brief  Declare AT channel releasing one mutex function type.
 * @else
 * @brief  声明AT命令释放互斥锁函数类型。
 * @endif
 */
typedef void(*at_release_mutex_func_t)(void*);

/**
 * @if Eng
 * @brief  Declare AT channel write function type.
 * @else
 * @brief  声明AT命令写函数类型。
 * @endif
 */
typedef void(*at_write_func_t)(const char*);

/**
 * @if Eng
 * @brief  Declare AT command attribute handling function type.
 * @else
 * @brief  声明AT命令属性解析处理函数类型。
 * @endif
 */
typedef bool(*at_cmd_attr_func_t)(uint16_t attr);

/**
 * @if Eng
 * @brief  Declare the list of basic functions on which AT depends.
 * @else
 * @brief  声明AT依赖的基础函数清单。
 * @endif
 */
typedef struct at_base_api_t {
    at_malloc_func_t malloc_func;
    at_free_func_t free_func;
    at_msg_queue_create_func_t msg_queue_create_func;
    at_msg_queue_write_func_t msg_queue_write_func;
    at_msg_queue_read_func_t msg_queue_read_func;
    at_task_pause_func_t task_pause_func;
#ifdef CONFIG_AT_SUPPORT_LOG
    at_log_func_t log_func;
#endif
#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    at_cmd_attr_func_t cmd_attr_func;
#endif
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
    at_timer_create_func_t timer_create_func;
    at_timer_start_func_t timer_start_func;
    at_timer_delete_func_t timer_delete_func;
#endif
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
    at_create_mutex_func_t create_mutex_func;
    at_acquire_mutex_func_t acquire_mutex_func;
    at_release_mutex_func_t release_mutex_func;
#endif
} at_base_api_t;

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
/**
 * @if Eng
 * @brief  Register AT command default abort func.
 * @param  [in]  func AT command default abort func.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册AT命令默认打断函数。
 * @param  [in]  func AT命令默认打断函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_cmd_default_abort_register(at_abort_func_t func);
#endif

/**
 * @if Eng
 * @brief  Register at base api func list.
 * @param  [in]  base_api The at base api func list.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册AT所需的基础函数。
 * @param  [in]  base_api 基础函数列表. see @ref at_base_api_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_base_api_register(at_base_api_t base_api);

/**
 * @if Eng
 * @brief  Register write interface for channel.
 * @param  [in]  id The channel id. see @ref at_channel_id_t
 * @param  [in]  func The write interface. see @ref at_write_func_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  为特定通道注册写接口。
 * @param  [in]  id 通道号， 参考 @ref at_channel_id_t
 * @param  [in]  func 写接口， 参考 @ref at_write_func_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_channel_write_register(at_channel_id_t id, at_write_func_t func);

/**
 * @if Eng
 * @brief  Sending data to the AT module through a specific channel.
 * @param  [in]  id The channel id. see @ref at_channel_id_t
 * @param  [in]  data The address of data.
 * @param  [in]  len The length of data.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  通过特定通道向模块发送数据。
 * @param  [in]  id 通道号， 参考 @ref at_channel_id_t
 * @param  [in]  data 数据起始地址。
 * @param  [in]  len 数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_channel_data_recv(at_channel_id_t id, uint8_t* data, uint32_t len);

/**
 * @if Eng
 * @brief  AT module main processing function.
 * @param  [in]  unused Reserved parameters.
 * @else
 * @brief  AT模块主处理函数。
 * @param  [in]  unused 预留参数。
 * @endif
 */
void uapi_at_msg_main(void* unused);

/**
 * @}
 */
#endif
