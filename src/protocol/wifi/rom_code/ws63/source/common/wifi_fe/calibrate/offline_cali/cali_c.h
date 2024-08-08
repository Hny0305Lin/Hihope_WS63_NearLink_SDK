/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: 离线校准C
 */

#ifndef CALI_C_H
#define CALI_C_H

#include "cali_outer_hal_if.h"
#include "cali_base_def.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
// r校准
osal_void cali_c(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CALI_C_H
