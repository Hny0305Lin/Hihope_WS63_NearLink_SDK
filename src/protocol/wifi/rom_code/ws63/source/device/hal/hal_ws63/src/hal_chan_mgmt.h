/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_phy.c.
 */

#ifndef HAL_CHAN_MGMT_H
#define HAL_CHAN_MGMT_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_chan_mgmt_rom.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CHAN_MGMT_H

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_WIFI_DEVICE

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CHAN_MGMT_H

osal_u32 hh503_set_bandwidth_mode_cb(hal_to_dmac_device_stru *hal_device,
    wlan_channel_bandwidth_enum_uint8 bandwidth);
#ifndef _PRE_WLAN_ATE
osal_u32 hal_scan_params_handle_cb(hal_to_dmac_device_stru *hal_device, hal_scan_params_stru *hal_scan_params,
    const hal_scan_info_stru *hal_scan_info);
osal_u32 hal_scan_pause_handle_cb(hal_to_dmac_device_stru *hal_device,
    hal_scan_pause_type_enum_uint8 hal_scan_pause_type);
#endif
osal_void hal_set_radar_config_enable(oal_bool_enum_uint8 value);
oal_bool_enum_uint8 hal_get_radar_config_enable(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
