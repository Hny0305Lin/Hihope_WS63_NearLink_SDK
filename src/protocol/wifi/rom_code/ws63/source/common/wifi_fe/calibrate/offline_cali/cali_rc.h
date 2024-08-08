/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file for common_alg_rf_cali_rc_pktram.c.
 */

#ifndef __CALI_RC_H__
#define __CALI_RC_H__
#include "osal_types.h"
#include "cali_outer_hal_if.h"
#include "cali_base_def.h"
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CALI_H

#define CALI_20M_BANDWIDTH_ADJUST   129 /* ACI刷新20M的RC CODE缩放值 */
#define CALI_BANDWIDTH_MASK_20M_ADJUST   198 /* TX MASK 20M的RC CODE缩放值 */
#define CALI_BANDWIDTH_CODE_MAX   0x7F /* rc code校准值max */
#define CALI_RC_AMP_FACTOR          (511)
#define CALI_RF_CALI_RC_RIGHT_SHIFT (8)
#define CALI_RC_2500KHZ             (2500)
#define CALI_RC_20000KHZ            (20000)
#define CALI_RC_40000KHZ            (40000)
#define CALI_SA_WAIT_TIMEOUT        (100)

/* BW校准算法产生单音信号的种类 */
typedef enum {
    CALI_RC_TONE_2P5M,
    CALI_RC_TONE_20M,
    CALI_RC_TONE_40M,

    CALI_RC_TONE_BUTT
} cali_rc_tone_enum;
typedef osal_u8 cali_rc_tone_enum_uint8;

osal_void cali_rc_path(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain);
osal_void cali_rc_recover(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain);
osal_void cali_rc(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);
osal_void cali_adjust_bandwidth(const hal_device_stru *device, wlan_channel_band_enum_uint8 band);
osal_void cali_rc_get_tone_sa_report(const hal_device_stru *device,
    cali_rc_tone_enum_uint8 tone_idx, osal_u32 *sa_val);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __COMMON_ALG_RF_CALI_RC_PKTRAM_H__ */
