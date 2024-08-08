/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: rx dc校准补偿
 */

#ifndef __CALI_RX_DC_COMP_H__
#define __CALI_RX_DC_COMP_H__

#include "cali_outer_os_if.h"
#include "cali_outer_hal_if.h"
#include "fe_rf_dev.h"
#include "cali_base_def.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void cali_rx_dc_comp(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 band, osal_u8 chn_idx,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
