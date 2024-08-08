/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_device_fsm_rom.c.
 */

#ifndef HAL_DEVICE_FSM_ROM_H
#define HAL_DEVICE_FSM_ROM_H

/*****************************************************************************
    1 头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_common_ops_rom.h"
#include "hal_ext_if_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define MAC_HAL_FSM_TIMER_IDLE_TIMEOUT        20
/*****************************************************************************
    函数声明
*****************************************************************************/
osal_void hal_device_fsm_attach(hal_to_dmac_device_stru *hal_device);
osal_void hal_device_fsm_detach(hal_to_dmac_device_stru *hal_device);
osal_void hal_device_handle_event(hal_to_dmac_device_stru *hal_device, osal_u16 type, osal_u16 datalen, osal_u8 *data);
osal_u32 hal_device_fsm_trans_to_state(hal_dev_fsm_stru *hal_dev_fsm, osal_u8 state, osal_u16 event);
osal_void hal_device_state_idle_entry(osal_void *ctx);
osal_void hal_device_state_idle_exit(osal_void *ctx);
osal_u32 hal_device_state_idle_event(osal_void *ctx, osal_u16 event, osal_u16 event_data_len, osal_void *event_data);
osal_void hal_device_state_work_entry(osal_void *ctx);
osal_void hal_device_state_work_exit(osal_void *ctx);
osal_u32 hal_device_state_work_event(osal_void *ctx, osal_u16 event, osal_u16 event_data_len, osal_void *event_data);
osal_void hal_device_state_scan_entry(osal_void *ctx);
osal_void hal_device_state_scan_exit(osal_void *ctx);
osal_u32 hal_device_state_scan_event(osal_void *ctx, osal_u16 event, osal_u16 event_data_len, osal_void *event_data);

osal_u8 hal_device_get_mac_pa_switch(osal_void);
osal_void hal_device_set_mac_pa_switch(osal_u8 val);
osal_void hal_device_work_sub_state_trans(hal_to_dmac_device_stru *hal_device);

osal_u8 hal_dev_fsm_get_curr_state(osal_void);
osal_s32 hal_dev_fsm_sync_state(osal_u8 curr_state, osal_u8 sub_state);
osal_void hal_device_set_pm_bitmap(hal_to_dmac_device_stru *hal_device, osal_u8 vap_id,
    hal_device_work_sub_state_info state_trans_to);
osal_void hal_dev_pm_work_sub_state(hal_to_dmac_device_stru *hal_device, osal_u8 vap_id,
    hal_device_work_sub_state_info state_trans_to);

osal_void hal_device_enter_rf_sleep_mode_mark(hal_to_dmac_device_stru *hal_device, oal_bool_enum_uint8 ps_mode_ever);
hal_work_sub_state_uint8 hal_dev_fsm_get_curr_substate(osal_void);
osal_void hal_dev_sync_mac_pa_switch_to_dev(osal_u8 mac_pa_switch);
osal_void hal_device_state_init_entry(osal_void *ctx);
osal_void hal_device_state_init_exit(osal_void *ctx);
osal_u32 hal_device_state_init_event(osal_void *ctx, osal_u16 event, osal_u16 event_data_len, osal_void *event_data);
osal_void hal_device_handle_event(hal_to_dmac_device_stru *hal_device, osal_u16 type, osal_u16 datalen, osal_u8 *data);
osal_u32 hal_device_fsm_trans_to_state(hal_dev_fsm_stru *hal_dev_fsm, osal_u8 state, osal_u16 event);
osal_void hal_device_fsm_info(hal_to_dmac_device_stru *hal_device);
osal_u8 hal_device_get_mac_pa_switch(osal_void);
osal_void hal_device_set_mac_pa_switch(osal_u8 val);
osal_void hal_dev_change_work_sub_state(hal_work_sub_state_uint8 work_sub_state, osal_u8 hal_vap_id);
osal_void hal_dev_sync_work_vap_bitmap(osal_u32 work_vap_bitmap);
osal_void hal_pm_enable_front_end_to_dev(hal_to_dmac_device_stru *hal_dev, osal_bool enable_paldo);
osal_void hal_dev_sync_work_rf_to_dev(osal_u8 rf_id);
osal_void hal_dev_req_sync_pm_bitmap(osal_u8 hal_vap_id, osal_u8 work_sub_state);

typedef osal_u32 (*hal_device_scan_param_cb)(hal_to_dmac_device_stru *hal_device, hal_scan_params_stru *hal_scan_params,
    const hal_scan_info_stru *hal_scan_info);
typedef osal_u32 (*hal_device_scan_pause_cb)(hal_to_dmac_device_stru *hal_device,
    hal_scan_pause_type_enum_uint8 hal_san_pause_type);

typedef osal_void (*hal_fsm_hmsg2d_init_rx_dscr_cb)(hal_to_dmac_device_stru *hal_device);
typedef osal_void (*hal_fsm_hmsg2d_destory_rx_dscr_cb)(hal_to_dmac_device_stru *hal_device);
typedef osal_void (*hal_fsm_hmsg2d_idle_to_sleep_prepare_cb)(osal_void);
typedef osal_void (*hal_fsm_hmsg2d_front_end_to_dev_cb)(hal_to_dmac_device_stru *hal_dev, osal_bool enable_paldo);
typedef osal_void (*hal_fsm_hmsg2d_work_vap_bitmap_cb)(osal_u32 work_vap_bitmap);
typedef osal_void (*hal_fsm_hmsg2d_sync_pm_bitmap_cb)(osal_u8 hal_vap_id, osal_u8 work_sub_state);
typedef osal_void (*hal_fsm_hmsg2d_work_to_dev_cb)(osal_u8 rf_id);

osal_void hal_fsm_idle_to_sleep_prepare(osal_void *dmac_vap, osal_void *msg);
osal_s32 hal_dev_fsm_init_rx_dscr(osal_void *dmac_vap, osal_void *msg);
osal_s32 hal_dev_fsm_destroy_rx_dscr(osal_void *dmac_vap, osal_void *msg);
osal_void hal_dev_sync_sub_work_to_rf(hal_to_dmac_device_stru *hal_dev, osal_u8 rf_id);

typedef osal_void (*hal_device_hw_init_cb)(hal_to_dmac_device_stru *hal_device);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_device_fsm_rom.h */
