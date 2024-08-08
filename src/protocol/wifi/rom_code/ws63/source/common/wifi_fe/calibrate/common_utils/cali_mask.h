/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 校准mask相关
 */

#ifndef __CALI_MASK_H__
#define __CALI_MASK_H__

#include "cali_outer_os_if.h"
#include "cali_outer_hal_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define CALI_DATA_MASK_UPLOAD_ON 0x1  // 校准后数据上报
#define CALI_DATA_MASK_DOWNLOAD_ON 0x2  // 接收下发的数据
#define CALI_DATA_MASK_ALL_CALI 0x4  // 执行上电校准
#define CALI_DATA_MASK_WITHOUT_PWR 0x8 // 不做上电功率校准
#define CALI_DATA_MASK_ONLINE_CALI 0x10 // 动态校准开关
#define CALI_DATA_MASK_DEFAULT 0x17 // 默认开启上电功率 动态校准

osal_void cali_set_cali_mask(const osal_u8 *param);
osal_u32 cali_get_cali_mask(wlan_channel_band_enum_uint8 band);
osal_void cali_set_cali_data_mask(const osal_u8 *param);
osal_u8 cali_get_cali_data_mask(osal_void);
osal_void cali_set_cali_done_flag(osal_u8 is_done);
osal_u8 cali_get_cali_done_flag(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
