/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: Header file for cali_logen.c.
 */

#ifndef __CALI_LOGEN_H__
#define __CALI_LOGEN_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "cali_outer_hal_if.h"
#include "cali_base_def.h"
#include "cali_accum_data.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*************************************************************************/
#define CALI_LOGEN_CAP_CODE_LEN                (4)
#define CALI_CAP_CALI_ACCUM_INIT_VAL           (-30000)
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_void cali_logen_path(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 bw, osal_u8 chain);
osal_void cali_logen_recover(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 bw, osal_u8 chain);
osal_void cali_logen(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 bw, osal_u8 chain, osal_u8 cali_subband_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __COMMON_ALG_RF_CALI_RC_PKTRAM_H__ */
