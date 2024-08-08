/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description:gmmu api interface
 */

#ifndef SOC_GMMU_API_H
#define SOC_GMMU_API_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup gmmu_type gmmu_type
 * @ingroup  gpu_api
 * @{
 */

/*-------------------------------- macro define -------------------------------------------*/

#define GMMU_BUF_CNT 2

/*-------------------------------- struct define ------------------------------------------*/

/**
 * @if Eng
 * @brief  Gmmu memory attributes.
 * @else
 * @brief  gmmu内存属性
 * @endif
 */
typedef struct gmmu_attr_struct {
    td_u8 bpp;        /*!< @if Eng Pixel Depth.
                           @else   像素深度 @endif */
    td_u32 width;     /*!< @if Eng Memory Width.
                           @else   内存宽度 @endif */
    td_u32 height;    /*!< @if Eng Memory height.
                           @else   内存高度 @endif */
    td_u8  offset;    /*!< @if Eng Pixel offset for fine tuning lut contours.
                           @else   像素偏移，用于微调lut轮廓 @endif */
} *gmmu_attr_ptr;

/**
 * @if Eng
 * @brief  gmmu buffer attribute.
 * @else
 * @brief  gmmu buffer属性
 * @endif
 */
typedef struct gmmu_buf_struct {
    td_u32 stride;               /*!< @if Eng Gmmu buffer line spacing.
                                      @else   gmmu buffer行间距 @endif */
    td_u32 size;                 /*!< @if Eng Pixel Depth.
                                      @else   gmmu buffer 大小@endif */
    td_u32 buffer[GMMU_BUF_CNT]; /*!< @if Eng Pixel Depth.
                                      @else   gmmu buffer地址 @endif */
} *gmmu_buf_ptr;

/**
 * @}
 */

/**
 * @defgroup gmmu_api gmmu_api
 * @ingroup  gpu_api
 * @{
 */

/*-------------------------------- func declares ------------------------------------------*/

/**
 * @if Eng
 * @brief  Initializing the Gmmu Memory.
 * @param  [in]  Width, height, and pixel depth required for memory initialization.
 * @retval EXT_SUCCESS   Success.
 * @retval Other         Failure.
 * @else
 * @brief  Gmmu 内存初始化
 * @param  [in]  gmmu_attr 内存初始化需要的宽高，像素深度信息
 * @retval EXT_SUCCESS   成功
 * @retval Other         失败
 * @endif
 */
td_s32  uapi_gmmu_init(const gmmu_attr_ptr gmmu_attr);

/**
 * @if Eng
 * @brief  Gmmu Memory Deinitialization.
 * @else
 * @brief  Gmmu 内存去初始化
 * @endif
 */
td_void uapi_gmmu_deinit(td_void);

/**
 * @if Eng
 * @brief  Gmmu buffer application.
 * @param  [in]  gmmu_buf Memory information about the gmmu buffer
 * @else
 * @brief  Gmmu 内存申请
 * @param  [in]  gmmu_buf gmmu buffer的内存信息
 * @endif
 */
td_void uapi_gmmu_alloc(gmmu_buf_ptr gmmu_buf);

/**
 * @if Eng
 * @brief  Gmmu buffer free.
 * @param  [in]  gmmu_buf Memory information about the gmmu buffer
 * @else
 * @brief  Gmmu 内存释放
 * @param  [in]  gmmu_buf gmmu buffer的内存信息
 * @endif
 */
td_void uapi_gmmu_free(gmmu_buf_ptr gmmu_buf);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_GMMU_API_H */
