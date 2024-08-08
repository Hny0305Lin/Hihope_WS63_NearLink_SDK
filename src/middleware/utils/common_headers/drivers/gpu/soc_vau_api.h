/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved. \n
 *
 * Description: Provides vau interface api \n
 */

#ifndef SOC_VAU_API_H
#define SOC_VAU_API_H

#include "soc_vau_type.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup vau_api vau_api
 * @ingroup  gpu_api
 * @{
 */

/**
 * @if Eng
 * @brief  Create a task and return the handle of the task.
 * @retval >0   Success.
 * @retval <=0  Failure.
 * @else
 * @brief  创建任务，返回对应任务的handle
 * @retval >0   成功
 * @retval <=0  失败
 * @endif
 */
td_handle uapi_vau_create(td_void);

/**
 * @if Eng
 * @brief  Cancel the Task.
 * @param  [in]  handle Task handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  取消任务
 * @param  [in]  handle 任务句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_cancel(td_handle handle);

/**
 * @if Eng
 * @brief  Destroy the Task.
 * @param  [in]  handle Task handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  销毁任务
 * @param  [in]  handle 任务句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_destroy(td_handle handle);

/**
 * @if Eng
 * @brief  Quick Fill Interface.
 * @param  [in]  handle Task handle.
 * @param  [in]  surface Layer Fill Information.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  快速填充接口
 * @param  [in]  handle 任务句柄
 * @param  [in]  surface 图层填充信息
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_fill(td_handle handle, ext_vau_surface *surface);

/**
 * @if Eng
 * @brief  Drawing Interfaces.
 * @param  [in]  handle Task handle.
 * @param  [in]  surface_list Task parameters.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  绘制接口
 * @param  [in]  handle 任务句柄
 * @param  [in]  surface_list 任务参数表
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_render(td_handle handle, const ext_vau_surface_list *surface_list);

/**
 * @if Eng
 * @brief  Multi-layer compose interface.
 * @param  [in]  handle Task handle.
 * @param  [in]  surface_list Task parameters.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  多层叠加接口
 * @param  [in]  handle 任务句柄
 * @param  [in]  surface_list 任务参数表
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_compose(td_handle handle, const ext_vau_surface_list *surface_list);

/**
 * @if Eng
 * @brief  Submit the task.
 * @param  [in]  handle Task handle.
 * @param  [in]  is_sync Synchronize Tasks.
 * @param  [in]  time_out Timeout Wait Time.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  任务提交
 * @param  [in]  handle 任务句柄
 * @param  [in]  is_sync 是否同步任务
 * @param  [in]  time_out 超时等待时间
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_submit(td_handle handle, td_bool is_sync, td_u32 time_out);

/**
 * @if Eng
 * @brief  Compressed header parsing.
 * @param  [in]  Compressed header streams.
 * @param  [in]  size Compression Header Size.
 * @param  [in]  info Compressed header parsing information.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  压缩头解析
 * @param  [in]  head 压缩头码流
 * @param  [in]  size 压缩头大小
 * @param  [in]  info 压缩头解析信息
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_parse_cmp_header(td_u8 *head, td_u32 size, ext_vau_cmp_header *info);

/**
 * @if Eng
 * @brief  Wait for task to completed.
 * @param  [in]  handle Task handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  等待任务完成
 * @param  [in]  handle 任务句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_vau_wait_for_done(td_handle handle);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_VAU_API_H */
