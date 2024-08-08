/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file for cali_iq.c.
 */

#ifndef __CALI_IQ_H__
#define __CALI_IQ_H__

#include "cali_outer_os_if.h"
#include "cali_outer_hal_if.h"
#include "cali_base_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_CALI_IQ
osal_void cali_tx_iq(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);
osal_void cali_rx_iq(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __CALI_IQ_H__ */
