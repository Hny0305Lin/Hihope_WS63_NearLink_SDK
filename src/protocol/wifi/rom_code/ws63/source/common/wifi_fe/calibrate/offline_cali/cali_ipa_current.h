/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: 离线校准ipa current
 */

#ifndef CALI_IPA_CURRENT_H
#define CALI_IPA_CURRENT_H

#include "cali_outer_hal_if.h"
#include "cali_base_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
// iPa current校准
osal_void cali_ipa_current(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CALI_IPA_CURRENT_H
