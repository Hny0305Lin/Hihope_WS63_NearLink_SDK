/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: hal tx.
 */

#ifndef HAL_EXT_INTF_DET_ROM_H
#define HAL_EXT_INTF_DET_ROM_H
/*****************************************************************************
    1 头文件包含
*****************************************************************************/
#include "hal_device_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_INTF_DET
osal_void hal_set_counter1_clear(osal_void);
osal_void hal_set_mac_clken(oal_bool_enum_uint8 wctrl_enable);
osal_void hal_chan_measure_start(hal_chan_measure_stru *meas);
osal_void hh503_get_txrx_frame_time(hal_ch_mac_statics_stru *ch_statics);
osal_void hh503_irq_channel_statistics_isr(hal_device_stru *hal_device, osal_u8 data);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_ext_intf_det_rom.h */