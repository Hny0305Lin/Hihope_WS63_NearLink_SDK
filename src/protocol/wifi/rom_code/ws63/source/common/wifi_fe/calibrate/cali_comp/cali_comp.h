/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 校准补偿相关的处理
 */

#ifndef __CALI_COMP_H__
#define __CALI_COMP_H__

#include "cali_outer_os_if.h"
#include "cali_outer_hal_if.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// 设置信道时，应用该信道对应的补偿值（所有补偿）
osal_void cali_apply_compensation(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 freq,
    osal_u8 ch_idx, wlan_channel_bandwidth_enum_uint8 bandwidth);

// 对某一个校准项应用预置补偿（某些校准项在校准前要先应用其他校准项的补偿）
osal_void cali_apply_other_compensation(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 freq,
    osal_u8 ch_idx, wlan_channel_bandwidth_enum_uint8 bandwidth, osal_u8 cali_type);

osal_u8 cali_get_cali_subband_idx_by_chan_num(wlan_channel_band_enum_uint8 band, osal_u8 ch);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
