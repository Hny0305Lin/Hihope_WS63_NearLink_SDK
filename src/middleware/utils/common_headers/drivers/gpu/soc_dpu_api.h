/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved. \n
 *
 * Description: Provides dpu interface api \n
 */

#ifndef SOC_DPU_API_H
#define SOC_DPU_API_H

#include "soc_dpu_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup dpu_api dpu_api
 * @ingroup  gpu_api
 * @{
 */

/**
 * @if Eng
 * @brief  Create a display layer and return the layer handle.
 * @param  [in]  layer_info Basic Layer Information.
 * @param  [out] layer Layer handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  创建显示图层，返回对应图层的handle
 * @param  [in]  layer_info 图层基本信息
 * @param  [out] layer 图层句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_create_layer(const ext_dpu_layer_info *layer_info, td_handle *layer);

/**
 * @if Eng
 * @brief  Destroying a Layer.
 * @param  [in]  layer Layer handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  销毁图层
 * @param  [in]  layer 图层句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_destroy_layer(td_handle layer);

/**
 * @if Eng
 * @brief  Sets the global alpha of a layer.
 * @param  [in]  layer Layer handle.
 * @param  [in]  alpha alpha value (0 ~ 255).
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层全局alpha
 * @param  [in]  layer 图层句柄
 * @param  [in]  alpha 图层alpha值(0~255)
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_set_layer_alpha(td_handle layer, td_u8 alpha);

/**
 * @if Eng
 * @brief  Get the global alpha of a layer.
 * @param  [in]  layer Layer handle.
 * @param  [out] alpha alpha value(0~255).
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  获取图层全局alpha
 * @param  [in]  layer 图层句柄
 * @param  [out] alpha 图层alpha值(0~255)
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_get_layer_alpha(td_handle layer, td_u8 *alpha);

/**
 * @if Eng
 * @brief  Set the start position of the layer.
 * @param  [in]  layer Layer handle.
 * @param  [in]  pos_x Layer start x.
 * @param  [in]  pos_y Layer start y.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层起始位置
 * @param  [in]  layer 图层句柄
 * @param  [in]  pos_x 图层起始x坐标
 * @param  [in]  pos_y 图层起始y坐标
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_set_layer_pos(td_handle layer, td_s32 pos_x, td_s32 pos_y);

/**
 * @if Eng
 * @brief  Get the start position of the layer.
 * @param  [in]  layer Layer handle.
 * @param  [out] *pos_x Layer start x.
 * @param  [out] *pos_y Layer start y.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  获取图层起始位置
 * @param  [in]  layer 图层句柄
 * @param  [out] *pos_x 图层起始x坐标
 * @param  [out] *pos_y 图层起始y坐标
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_get_layer_pos(td_handle layer, td_s32 *pos_x, td_s32 *pos_y);

/**
 * @if Eng
 * @brief  Sets the relative Z order of a layer.
 * @param  [in]  layer Layer handle.
 * @param  [in]  zorder relative Z order of a layer.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层的相对Z序
 * @param  [in]  layer 图层句柄
 * @param  [in]  zorder 图层的相对Z序
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_set_layer_zorder(td_handle layer, ext_dpu_zorder_mode zorder);

/**
 * @if Eng
 * @brief  Get the Z order of a layer.
 * @param  [in]  layer Layer handle.
 * @param  [out] zorder relative Z order of a layer.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层的相对Z序
 * @param  [in]  layer 图层句柄
 * @param  [out] zorder 图层的相对Z序
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_get_layer_zorder(td_handle layer, td_u32 *zorder);

/**
 * @if Eng
 * @brief  Display Layers.
 * @param  [in]  layer Layer handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  显示图层
 * @param  [in]  layer 图层句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_show_layer(td_handle layer);

/**
 * @if Eng
 * @brief  Hide Layers.
 * @param  [in]  layer Layer handle.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  隐藏图层
 * @param  [in]  layer 图层句柄
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_hide_layer(td_handle layer);

/**
 * @if Eng
 * @brief  Setting Layer Information.
 * @param  [in]  layer Layer handle.
 * @param  [in]  Layer information to update.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层信息
 * @param  [in]  layer 图层句柄
 * @param  [in]  surface 要更新的图层信息
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_set_layer_info(td_handle layer, const ext_dpu_surface *surface);

/**
 * @if Eng
 * @brief  Setting Layer Information.
 * @param  [in]  layer Layer handle.
 * @param  [out] surface Current Layer Information.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层信息
 * @param  [in]  layer 图层句柄
 * @param  [out] surface 当前图层信息
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_get_layer_info(td_handle layer, ext_dpu_surface *surface);

/**
 * @if Eng
 * @brief  Set the colorkey of the layer.
 * @param  [in]  layer Layer handle.
 * @param  [in]  colorkey Layer colorkey configuration.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置图层colorkey
 * @param  [in]  layer 图层句柄
 * @param  [in]  colorkey 图层colorkey配置
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_set_layer_colorkey(td_handle layer, const ext_dpu_colorkey *colorkey);

/**
 * @if Eng
 * @brief  Get the colorkey of a layer
 * @param  [in]  layer Layer handle.
 * @param  [out] colorkey Colorkey configuration of the current layer.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  获取图层colorkey
 * @param  [in]  layer 图层句柄
 * @param  [out] colorkey 当前图层colorkey配置
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_get_layer_colorkey(td_handle layer, ext_dpu_colorkey *colorkey);

/**
 * @if Eng
 * @brief  Layer refresh display.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  图层刷新显示
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_refresh_layers(td_void);

/**
 * @if Eng
 * @brief  Wait until the layer display is complete.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  等待图层显示完成
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_wait_layer_vsync(td_void);

/**
 * @if Eng
 * @brief  Set Wait Flag.
 * @param  [in]  layer Layer handle.
 * @param  [in]  True with a single buffer, false with double buffers.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  设置等待标记
 * @param  [in]  layer 图层句柄
 * @param  [in]  vblank 单buffer送显为true，双buffer送显为false
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_set_layer_vblank(td_handle layer, td_bool vblank);

/**
 * @if Eng
 * @brief  Get Waiting Flag.
 * @param  [in]  layer Layer handle.
 * @param  [out] True with a single buffer, false with double buffers.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  获取等待标记
 * @param  [in]  layer 图层句柄
 * @param  [out] vblank 单buffer送显为true，双buffer送显为false
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32 uapi_dpu_get_layer_vblank(td_handle layer, td_bool *vblank);

/**
 * @if Eng
 * @brief  Querying the Display Completion Status of a Layer.
 * @param  [in]  layer Layer handle.
 * @retval 0   Idle
 * @retval 1   Busy
 * @retval Other          Failure.
 * @else
 * @brief  查询图层的显示完成状态
 * @param  [in]  layer 图层句柄
 * @retval 0   空闲
 * @retval 1   忙碌
 * @retval Other          失败
 * @endif
 */
td_s32 uapi_dpu_query_layer_state(td_handle layer);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_DPU_API_H */
