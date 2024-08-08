/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_device_fsm_rom.c.
 */

#ifndef __HAL_DEVICE_FSM_H__
#define __HAL_DEVICE_FSM_H__

/*****************************************************************************
    1 头文件包含
*****************************************************************************/
#include "hal_common_ops.h"
#include "hal_device_fsm_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    函数声明
*****************************************************************************/
osal_void wlan_msg_h2d_init_rx_dscr(hal_to_dmac_device_stru *hal_device);
osal_void wlan_msg_h2d_destory_rx_dscr(hal_to_dmac_device_stru *hal_device);
osal_void wlan_msg_h2d_sync_sub_work_to_rf(osal_u8 rf_id);
osal_void wlan_msg_h2d_req_sync_pmbit(osal_u8 hal_vap_id, osal_u8 work_sub_state);
osal_void wlan_msg_h2d_sync_work_vap_state(osal_u32 work_vap_bitmap);
osal_void wlan_msg_h2d_enable_front_end(hal_to_dmac_device_stru *hal_dev, osal_bool enable_paldo);
osal_void wlan_msg_h2d_idel_sleep_prepare(osal_void);

osal_void hal_device_idle_to_sleep_prepare(hal_to_dmac_device_stru *hal_device, hal_dev_fsm_stru *hal_dev_fsm);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_device_fsm.h */
