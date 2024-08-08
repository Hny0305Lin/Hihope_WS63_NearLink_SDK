/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: algorithm transfer power control
 */

#ifndef ALG_TPC_ROM_H
#define ALG_TPC_ROM_H

#ifdef _PRE_WLAN_FEATURE_TPC

/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "oal_types_device_rom.h"
#include "wlan_spec_rom.h"
#include "dmac_ext_if_rom.h"
#include "alg_autorate_if_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/******************************************************************************
  2 宏定义
******************************************************************************/
#define ALG_TPC_INVALID_FIX_PWR (0xff)  /* 无效的固定功率值 */
#define ALG_TPC_FIX_CODE_LOW_LEVEL 5
#define ALG_TPC_FIX_CODE_COMPANSATION 3

#define ALG_TPC_40M_RSSI_ADJUST 3
#define ALG_TPC_80M_RSSI_ADJUST 6

/******************************************************************************
  3 枚举定义
******************************************************************************/
/* 数据类型 */
typedef enum {
    ALG_TPC_CFG_INT8 = 0,
    ALG_TPC_CFG_UINT8 = 1,
    ALG_TPC_CFG_UINT16 = 2,
    ALG_TPC_CFG_UINT32 = 3,
    ALG_TPC_CFG_UINT64 = 4,
    ALG_TPC_CFG_TYPE_BUTT
} alg_tpc_cfg_param_type_enum;
typedef osal_u8 alg_tpc_cfg_param_type_enum_uint8;

/******************************************************************************
  4 STRUCT定义
******************************************************************************/
typedef struct {
    alg_param_sync_tpc_stru *param_sync;
    alg_tpc_common_info_stru common_info;
} alg_tpc_stru;

/* 用户功率信息结构体 */
typedef struct {
    dmac_user_stru *user;
    alg_tpc_user_rssi_stru user_rssi_info; /* 用户RSSI信息结构体 */
    alg_tpc_user_distance_stru user_distance_info; /* 用户距离信息结构体 */
#ifdef _PRE_WLAN_TPC_DO_PROBE
    osal_u8 cur_query_pktno;                      /* 当前查询的报文编号 */
    osal_u8 cur_complete_pktno;                    /* 当前完成的报文编号 */
    osal_u8 resv[2];
    alg_tpc_probe_info_stru bebk_tpc_probe_info; /* 该用户BE/BK业务的探测信息 */
    alg_tpc_probe_info_stru vi_tpc_probe_info;   /* 该用户VI业务的探测信息 */
    alg_tpc_probe_info_stru vo_tpc_probe_info;   /* 该用户VO业务的探测信息 */
    alg_tpc_probe_info_stru *tpc_probe_info;     /* 用户业务探测信息指针 */

    osal_u8 aggr_smooth_scale;    /* 聚合的平滑系数 */
    osal_u8 normal_smooth_scale;  /* 非聚合的平滑系数 */
#endif
} alg_tpc_user_pow_info_stru;


/* 设置配置参数的回调函数指针 */
typedef osal_void (*p_alg_tpc_config_param_handle_func)(const dmac_ioctl_alg_param_stru *alg_param);

/* 算法参数配置表结构 */
typedef struct {
    mac_alg_cfg_enum_uint16 alg_cfg_param;
    alg_tpc_cfg_param_type_enum_uint8 param_type;
    osal_u8 rev[3];
    osal_u16 param_offset;

    p_alg_tpc_config_param_handle_func cfg_param_handle;
} alg_tpc_cfg_param_table_stru;

typedef osal_u32 (*alg_tpc_set_tx_tpc_cb)(hal_tx_txop_alg_stru *txop_param, const osal_u8 *pow_level,
    mac_tx_ctl_stru *cb);
typedef osal_u32 (*alg_tpc_set_tpc_mode_cb)(const hal_to_dmac_device_stru *hal_device,
    wlan_tpc_mode_enum_uint8 new_mode);
typedef osal_u32 (*alg_tpc_change_channel_num_process_cb)(dmac_vap_stru *dmac_vap,
    mac_alg_channel_bw_chg_type_uint8 type);
typedef osal_u32 (*alg_tpc_tx_complete_process_cb)(dmac_user_stru *dmac_user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr_param);
typedef osal_u32 (*alg_tpc_tx_query_cb)(hal_to_dmac_device_stru *hal_device,
    alg_tpc_user_pow_info_stru *user_tpc_pow_info,
    hal_user_pow_info_stru *hal_user_pow_info, mac_tx_ctl_stru *cb);
typedef osal_u32 (*alg_tpc_user_distance_judge_cb)(const alg_tpc_user_rssi_stru *user_rssi_info,
    alg_tpc_user_distance_stru *user_distance_info, const dmac_user_stru *dmac_user, osal_s8 tpc_rssi_20_m);
typedef osal_u32 (*alg_tpc_distance_state_has_changed_cb)(dmac_user_stru *dmac_user,
    alg_tpc_user_distance_stru *user_distance_info, wlan_user_distance_enum_uint8 old_distance);
typedef osal_u32 (*alg_tpc_set_vap_spec_frame_tx_power_cb)(dmac_vap_stru *dmac_vap);
typedef osal_u32 (*alg_tpc_init_user_cb)(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user);
typedef osal_u32 (*alg_tpc_tx_process_cb)(dmac_user_stru *dmac_user, mac_tx_ctl_stru *cb,
    hal_tx_txop_alg_stru *txop_param);
typedef osal_u32 (*alg_tpc_rx_process_cb)(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user, oal_dmac_netbuf_stru *buf,
    hal_rx_statistic_stru *rx_stats);
typedef osal_u32 (*alg_tpc_rx_mgmt_process_cb)(const dmac_vap_stru *vap, dmac_user_stru *user,
    oal_dmac_netbuf_stru *buf);
typedef osal_u32 (*alg_tpc_update_power_cb)(dmac_vap_stru *vap);

/******************************************************************************
  5 函数声明
******************************************************************************/
osal_u32 alg_tpc_init(osal_void);
osal_u32 alg_tpc_exit(osal_void);
osal_u32 alg_tpc_init_user(dmac_vap_stru *vap, dmac_user_stru *user);
osal_u32 alg_tpc_exit_user(dmac_vap_stru *vap, dmac_user_stru *user);
osal_u32 alg_tpc_tx_process(dmac_user_stru *dmac_user, mac_tx_ctl_stru *cb, hal_tx_txop_alg_stru *txop_param);
osal_u32 alg_tpc_rx_process(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user, oal_dmac_netbuf_stru *buf,
    hal_rx_statistic_stru *rx_stats);
osal_u32 alg_tpc_rx_mgmt_process(const dmac_vap_stru *vap, dmac_user_stru *user, oal_dmac_netbuf_stru *buf);
osal_u32 alg_tpc_tx_complete_process(dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr_param);
osal_u32 alg_tpc_param_sync(const alg_param_sync_stru *sync);
osal_u32 alg_tpc_config_param(const dmac_vap_stru *dmac_vap, frw_msg *msg);
alg_tpc_stru *alg_tpc_get_tpc(osal_void);
osal_void alg_tpc_set_tpc_mode(const hal_to_dmac_device_stru *hal_device, wlan_tpc_mode_enum_uint8 new_mode);
osal_void alg_tpc_set_anti_intrf_flag(osal_u8 enable, osal_u16 cfg_flag);
osal_void alg_tpc_near_distance_rssi_handle(dmac_user_stru *user, alg_tpc_user_distance_stru *dist_info,
    osal_u8 best_rate_index);
#ifdef _PRE_WLAN_FEATURE_INTF_DET
osal_u32 alg_tpc_intf_change_process(hal_to_dmac_device_stru *hal_device,
    alg_intf_det_notify_info_stru *intf_det_notify);
#endif
#ifdef _PRE_WLAN_FEATURE_TEMP_PROTECT
osal_u32 alg_tpc_over_temp_process(hal_to_dmac_device_stru *hal_device, osal_u8 flag);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
#endif /* end of alg_tpc_rom.h */

