/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: 离线校准C补偿
 */

#ifndef CALI_C_COMP_H
#define CALI_C_COMP_H

#include "fe_rf_dev.h"
#include "cali_base_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
// c校准补偿
osal_void cali_c_comp(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 band, osal_u8 cali_subband_idx,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CALI_C_COMP_H
