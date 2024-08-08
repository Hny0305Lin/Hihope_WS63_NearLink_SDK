/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of alg_autorate_if_rom.h
 */

#ifndef ALG_AUTORATE_IF_ROM_H
#define ALG_AUTORATE_IF_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "alg_main_rom.h"
#include "hal_device_rom.h"
#include "dmac_alg_if_part_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_AUTORATE

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  6 函数声明
*****************************************************************************/
osal_u32 alg_autorate_init(osal_void);
void alg_autorate_exit(osal_void);
osal_u32 alg_autorate_get_tx_best_rate(dmac_user_stru *user, osal_u32 *rate_kbps,
    alg_autorate_rate_detail_stru *rate_detail);
osal_u32 alg_autorate_get_rate_kbps(hal_tx_ctrl_desc_rate_stru *per_rate, osal_u32 *rate_kbps,
    osal_u32 *aggr_bytes_4ms);
osal_u32 alg_autorate_get_ru_rate_kbps(alg_autorate_ru_rate_detail_stru *rate, osal_u32 *rate_kbps);
osal_u32 alg_autorate_get_rx_rate_by_rate_table(
    dmac_user_stru *user, dmac_rx_ctl_stru *cb_ctrl, osal_u32 *rate_kbps);
osal_u32 alg_autorate_change_user_bandwidth_process(dmac_vap_stru *vap, dmac_user_stru *user);
osal_u32 alg_autorate_delete_ba_fail(dmac_user_stru *user);
oal_bool_enum_uint8 alg_autorate_is_enable(osal_void);
osal_u32 alg_autorate_config(dmac_vap_stru *dmac_vap, dmac_ioctl_alg_param_stru *alg_param);

#ifdef _PRE_WLAN_FEATURE_INTF_DET
osal_u32 alg_autorate_intf_change_process(hal_to_dmac_device_stru *hal_device,
    alg_intf_det_notify_info_stru *intf_det_notify);
#endif
osal_u32 alg_autorate_tx_complete_process(dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr);
osal_u32 alg_rate_stat_goodput(osal_u16 us_per, osal_u32 theory_goodput);

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
osal_s32 alg_autorate_intrf_mode_proc(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user, mac_tx_ctl_stru *cb,
    hal_tx_txop_alg_stru *txop_alg);
osal_s32 alg_autorate_config_intrf_mode_param(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_ar_config_set_11b_tx_count(dmac_vap_stru *dmac_vap, frw_msg *msg);
#endif
osal_u32 alg_autorate_param_sync(const alg_param_sync_stru *sync);
typedef osal_u32 (*alg_autorate_config_cb)(dmac_vap_stru *dmac_vap, dmac_ioctl_alg_param_stru *alg_param);
osal_bool alg_get_vap_user_support_ldpc(const dmac_vap_stru *vap, const dmac_user_stru *user);
#endif /* #ifdef _PRE_WLAN_FEATURE_AUTORATE */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_autorate_rom.h */
