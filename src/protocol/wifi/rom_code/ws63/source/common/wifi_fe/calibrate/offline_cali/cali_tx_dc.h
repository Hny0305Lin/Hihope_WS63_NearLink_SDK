/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 离线校准TX_DC
 */

#ifndef __CALI_TX_DC_H__
#define __CALI_TX_DC_H__

#include "osal_types.h"
#include "cali_outer_hal_if.h"
#include "cali_complex_num.h"
#include "cali_base_def.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

cali_complex_stru cali_tx_dc_calc_comp_val(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_complex_stru compensate, cali_complex_stru dc3);
osal_void cali_check_adjust_tx_dc_comp_val(cali_complex_stru *tx_dc_comp);
osal_void cali_data_save_tx_dc_val(const hal_device_stru *device,
    osal_u8 chn_idx, osal_u8 txdc_lvl_idx, cali_complex_stru *tx_dc_comp);
osal_void cali_tx_dc(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
