/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: logen校准补偿
 */

#ifndef __CALI_LOGEN_COMP_H__
#define __CALI_LOGEN_COMP_H__

#include "osal_types.h"
#include "cali_outer_hal_if.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void cali_logen_comp(const hal_rf_dev *rf_dev,
    wlan_channel_band_enum_uint8 band,
    osal_u8 cali_subband_idx,
    wlan_channel_bandwidth_enum_uint8 bw);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
