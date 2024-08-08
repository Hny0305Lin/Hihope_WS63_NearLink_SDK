/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: power
 */

#ifndef __CALI_TX_PWR_COMP_H__
#define __CALI_TX_PWR_COMP_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "cali_outer_hal_if.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CALI_H

osal_void cali_tx_power_comp(const hal_rf_dev *rf_dev, wlan_channel_band_enum_uint8 band, osal_u8 cali_subband_idx,
    wlan_channel_bandwidth_enum_uint8 bw);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
