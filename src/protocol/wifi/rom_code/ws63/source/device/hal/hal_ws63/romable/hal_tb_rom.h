/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_coex_reg.c.
 */

#ifndef HAL_TB_ROM_H
#define HAL_TB_ROM_H

/*****************************************************************************
  其他头文件包含
*****************************************************************************/
#include "hal_common_ops_rom.h"
#include "hal_power_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 收到TRIGGER帧携带的参数 */
typedef struct {
    wlan_bw_cap_enum_uint8 rpt_trig_bw;          /* 上报trigger里的带宽 */
    osal_u8              rpt_trig_type;            /* trigger的类型 */
    osal_u8              rpt_trig_mcs;             /* mcs：0~7 */
    osal_u8              rpt_trig_nss;             /* nss：0~1 */
    osal_u8              rpt_trig_target_rssi;     /* trigger:0~90映射到-110dbm~-20dBm, trs:-90 + 2*Fval */
    osal_u8              rpt_trig_ap_tx_power;     /* trigger:0~60映射到-20dbm~40dBm, trs:-20 + 2*Fval */
    osal_u8              set_tpc;
    osal_u8              rate_idx;
} hal_rpt_trig_frm_params_stru;

osal_void hh503_irq_rx_triger_frm_param_isr(hal_device_stru *device, osal_u8 data);
typedef osal_void (*hh503_dispatch_trig_event_cb)(hal_device_stru *device, hal_trig_common_info_field_stru *common_info,
    hal_trig_user_info_field_stru *user_info);

typedef osal_void (*hal_pow_he_trig_frame_update_code_cb)(hal_to_dmac_device_stru *hal_device,
    hal_rpt_trig_frm_params_stru *trig_para, osal_u8 is_trs);
osal_u8 hal_get_ru_size(osal_u8 ru_allocation);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
