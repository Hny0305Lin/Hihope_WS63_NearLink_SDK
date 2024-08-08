/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved. \n
 *
 * Description: Provides the data struct define of dpu api \n
 */

#ifndef SOC_DPU_TYPE_H
#define SOC_DPU_TYPE_H

#include "td_base.h"
#include "soc_gfx_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup dpu_type dpu_type
 * @ingroup  gpu_type
 * @{
 */

/**
 * @if Eng
 * @brief  An invalid layer ID is returned when a layer fails to be created.
 * @else
 * @brief  创建图层失败时，返回非法图层ID
 * @endif
 */
#define EXT_DPU_INVALID_HANDLE 0xFFFFFFFF /* 非法handle ID */

/**
 * @if Eng
 * @brief  Layer ID.
 * @else
 * @brief  图层的ID号
 * @endif
 */
typedef enum {
    EXT_DPU_LAYER_ID1 = 0, /*!< @if Eng Graphics layer G0, supported.
                                @else   图形层G0, 支持 @endif */
    EXT_DPU_LAYER_ID2,     /*!< @if Eng Graphics layer G1 supported.
                                @else   图形层G1 支持 @endif */
    EXT_DPU_LAYER_ID3,     /*!< @if Eng Graphics layer G3, not supported.
                                @else   图形层G3, 不支持 @endif */
    EXT_DPU_LAYER_ID4,     /*!< @if Eng Graphics layer G4, not supported.
                                @else   图形层G4, 不支持 @endif */
    EXT_DPU_LAYER_ID_MAX   /*!< @if Eng Reserved bit, invalid input.
                                @else   保留位, 非法输入 @endif */
} ext_dpu_layer_id;

/**
 * @if Eng
 * @brief  buffer mode.
 * @else
 * @brief  buffer模式
 * @endif
 */
typedef enum {
    EXT_DPU_BUFFER_MODE_NORMAL = 0, /*!< @if Eng Normal mode(double canvas display mode).
                                         @else   NORMAL模式（双canvas送显模式） */
    EXT_DPU_BUFFER_MODE_SINGLE,     /*!< @if Eng Single mode(single canvas display mode).
                                         @else   SINGLE模式（单canvas送显模式） */
    EXT_DPU_BUFFER_MODE_MAX         /*!< @if Eng Reserved bit, invalid input.
                                         @else   保留位, 非法输入 @endif */
} ext_dpu_buf_mode;

/**
 * @if Eng
 * @brief  Relative Z Order of Layers.
 * @else
 * @brief  图层的相对Z序
 * @endif
 */
typedef enum {
    EXT_DPU_ZORDER_MODE_MOVETOP = 0, /*!< @if Eng Moves the layer to the topmost layer.
                                          @else   指定的图层移到最顶层 @endif */
    EXT_DPU_ZORDER_MODE_MOVEUP,      /*!< @if Eng Moves the layer up.
                                          @else   指定的图层向上移动 @endif */
    EXT_DPU_ZORDER_MODE_MOVEBOTTOM,  /*!< @if Eng Moves the layer to the bottom layer.
                                          @else   指定的图层移到最底层 @endif */
    EXT_DPU_ZORDER_MODE_MOVEDOWN,    /*!< @if Eng Moves the layer down.
                                          @else   指定的图层向下移动 @endif */
    EXT_DPU_ZORDER_MODE_MAX          /*!< @if Eng Reserved bit, invalid input.
                                          @else   保留位, 非法输入 @endif */
} ext_dpu_zorder_mode;

/**
 * @if Eng
 * @brief  Working status of the layer.
 * @else
 * @brief  图层的工作状态
 * @endif
 */
typedef enum {
    EXT_DPU_STATE_IDLE = 0,  /*!< @if Eng The layer is idle.
                                  @else   图层处于空闲状态 @endif */
    EXT_DPU_STATE_BUSY,      /*!< @if Eng The layer is in the display state.
                                  @else   图层处于送显状态 @endif */
    EXT_DPU_STATE_MAX        /*!< @if Eng Reserved bit, invalid input.
                                  @else   保留位, 非法输入 @endif */
} ext_dpu_state_mode;

/**
 * @if Eng
 * @brief  Colorkey attribute of a layer.
 * @else
 * @brief  图层的colorkey属性
 * @endif
 */
typedef struct {
    td_bool key_en;    /*!< @if Eng The flag of key enable.
                            @else   使能标记 @endif */
    td_u32 key_value;  /*!< @if Eng The pixel value equal to key_value will be ignored.
                            @else   内存中像素的值等于key_value将被透掉 @endif */
} ext_dpu_colorkey;

/**
 * @if Eng
 * @brief  Layer information.
 * @else
 * @brief  图层信息
 * @endif
 */
typedef struct {
    td_u32 width;                         /*!< @if Eng Layer width.
                                               @else   图层宽度 @endif */
    td_u32 height;                        /*!< @if Eng Layer height.
                                               @else   图层高度 @endif */
    ext_gfx_fmt layer_fmt;                /*!< @if Eng Color format.
                                               @else   颜色格式 @endif */
    ext_dpu_layer_id layer_id;            /*!< @if Eng Layer ID.
                                               @else   层的ID @endif */
    ext_dpu_buf_mode buf_mode;            /*!< @if Eng Buffer mode.
                                               @else   buffer模式 @endif */
    ext_gfx_compress_mode compress_mode;  /*!< @if Eng Not supported(default value: 0).
                                               @else   当前不支持（默认为0） @endif */
} ext_dpu_layer_info;

/**
 * @if Eng
 * @brief  Updates the information about the displayed content.
 * @else
 * @brief  更新显示内容的信息
 * @endif
 */
typedef struct {
    td_u32 width;                          /*!< @if Eng Width of the surface,Unit: number of pixels.
                                                @else   surface宽度, 单位：像素数 @endif */
    td_u32 height;                         /*!< @if Eng Height of the surface,Unit: number of pixels.
                                                @else   surface高度, 单位：像素数 @endif */
    td_u32 stride;                         /*!< @if Eng Buffer interval distance.
                                                @else   buffer行间距 @endif */
    td_u32 uv_stride;                      /*!< @if Eng Indicates the line spacing of UV data. This parameter
                                                is valid only when the G1 output format is YUV420SP.
                                                @else   uv数据行间距, 仅G1送显格式为yuv420sp时有效 */
    td_u32 uv_offset;                      /*!< @if Eng UV data offset. This parameter is valid only when
                                                the G1 output format is YUV420SP.
                                                @else   uv数据偏移量, 仅G1送显格式为yuv420sp时有效 @endif */
    td_u32 size;                           /*!< @if Eng Memory size,Unit: byte. size = stride * height.
                                                @else   内存大小, 单位：字节。size = stride * height @endif */
    td_u32 phy_addr;                       /*!< @if Eng The physical memory address for storing displays data.
                                                @else   显示数据存放的物理地址 @endif */
    td_uchar *vir_addr;                    /*!< @if Eng The virtual memory address for storing displays data.
                                                @else   显示数据存放的虚拟地址 @endif */
    ext_rect *update_rect;                 /*!< @if Eng Update Region.
                                                @else   更新区域 @endif */
    ext_gfx_fmt color_fmt;                 /*!< @if Eng Color Formatting.
                                                @else   颜色格式 @endif */
    td_bool premulti_en;                   /*!< @if Eng Premultiplication enable flag.
                                                @else   预乘使能标记 @endif */
    ext_gfx_compress_mode compress_mode;   /*!< @if Eng Data Compression Type.
                                                @else   数据压缩类型 @endif */
} ext_dpu_surface;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_DPU_TYPE_H */
