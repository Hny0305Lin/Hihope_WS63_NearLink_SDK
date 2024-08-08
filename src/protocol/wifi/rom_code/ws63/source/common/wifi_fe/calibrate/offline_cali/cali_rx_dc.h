/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 离线校准RX_DC
 */

#ifndef __CALI_RX_DC_H__
#define __CALI_RX_DC_H__

#include "cali_outer_hal_if.h"
#include "cali_complex_num.h"
#include "cali_base_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// rx dc校准
osal_void cali_rx_dc_calc_vn2_vn3(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_complex_stru *vn2, cali_complex_stru *vn3);
osal_void cali_switch_fix_bit_data(cali_complex_stru *complex_data);
osal_void cali_data_save_digital_rx_dc_val(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    osal_u8 chn_idx, cali_complex_stru *vn3);
osal_void cali_rxdc_calc_dcoc_code(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_complex_stru vcomp1, osal_u16 *rx_dc_comp);
osal_void cali_calc_rx_dc_analog_comp_val(const hal_device_stru *device,
    wlan_channel_band_enum_uint8 band, cali_complex_stru vn2, osal_u8 chn_idx);
osal_void cali_rx_dc(const hal_device_stru *device,
    wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 cali_bandwidth,
    osal_u8 chain,
    osal_u8 chn_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
