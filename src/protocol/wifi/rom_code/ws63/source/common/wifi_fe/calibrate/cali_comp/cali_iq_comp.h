/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: power
 */

#ifndef __CALI_IQ_COMP_H__
#define __CALI_IQ_COMP_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "cali_outer_hal_if.h"
#include "fe_rf_dev.h"
#include "cali_base_def.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
osal_void cali_tx_iq_comp(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 band, osal_u8 cali_subband_idx,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth);
osal_void cali_rx_iq_comp(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 band, osal_u8 cali_subband_idx,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __CALI_IQ_COMP_H__ */
