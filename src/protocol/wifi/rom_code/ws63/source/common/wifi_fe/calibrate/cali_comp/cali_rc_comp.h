/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: rx dc校准补偿
 */

#ifndef __CALI_RC_COMP_H__
#define __CALI_RC_COMP_H__

#include "cali_outer_os_if.h"
#include "cali_outer_hal_if.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void cali_rc_comp(const hal_rf_dev *rf, wlan_channel_band_enum_uint8 freq, osal_u8 chn_idx,
    wlan_channel_bandwidth_enum_uint8 rf_bw);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
