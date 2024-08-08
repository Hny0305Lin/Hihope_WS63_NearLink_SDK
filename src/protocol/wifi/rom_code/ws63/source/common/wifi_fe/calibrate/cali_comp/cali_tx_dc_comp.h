/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: TX_DC校准补偿
 */

#ifndef __CALI_TX_DC_COMP_H__
#define __CALI_TX_DC_COMP_H__

#include "osal_types.h"
#include "cali_outer_hal_if.h"
#include "cali_base_def.h"
#include "fe_rf_dev.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void cali_tx_dc_comp(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 band, osal_u8 cali_subband_idx,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
