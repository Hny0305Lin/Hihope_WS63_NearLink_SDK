/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of alg_rts_rom.c for outer files to reference
 */

#ifndef ALG_RTS_ROM_H
#define ALG_RTS_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "alg_main_rom.h"
#include "hal_device_rom.h"
#include "dmac_alg_if_part_rom.h"
#include "dmac_user_rom.h"
#include "dmac_alg_rom.h"
#include "mac_device_rom.h"
#include "alg_txbf_rom.h"
#include "hal_device_rom.h"
#include "hal_power_rom.h"
#include "dmac_ext_if_rom.h"
#include "frw_util_rom.h"
#include "hal_alg_rts_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_RTS

/*****************************************************************************
  1 宏定义
*****************************************************************************/
#define ALG_RTS_CFG_RATE_SHIFT 3
#define ALG_RTS_CFG_RATE_BITS 0x7

/*****************************************************************************
  2 枚举定义
*****************************************************************************/
/* RTS mode */
typedef enum {
    ALG_RTS_MODE_ALL_ENABLE    = 0, /* rate[0..3]都开RTS */
    ALG_RTS_MODE_ALL_DISABLE   = 1, /* rate[0..3]都不开RTS */
    ALG_RTS_MODE_RATE0_DYNAMIC = 2, /* rate[0]动态RTS, rate[1..3]都开RTS */
    ALG_RTS_MODE_RATE0_DISABLE = 3, /* rate[0]不开RTS, rate[1..3]都开RTS */
    ALG_RTS_MODE_THRESHOLD     = 4, /* 根据MIB配置的RTS Threshold字节数门限确定是否开RTS */

    ALG_RTS_MODE_BUTT
} alg_rts_mode_enum;
typedef osal_u8 alg_rts_mode_enum_uint8;

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/* rts固定速率结构体 */
typedef struct {
    osal_u8 fixed_rate    : 3;
    osal_u8 is_fixed_rate : 1;
    osal_u8 reserved      : 4;
} alg_rts_fixed_rate_stru;
/* rts_dev结构体 */
typedef struct {
    alg_param_sync_rts_stru *rts_sync_dmac;                    /* 从host同步参数 */
    alg_config_param_sync_rts_stru rts_config_param_sync_dmac; /* 从host同步rts配置命令参数 */

    alg_rts_fixed_rate_stru fix_rate[HAL_TX_RATE_MAX_NUM];     /* 固定速率配置参数 */

    osal_u32 co_intf : 1;                                      /* 干扰检测模块通知RTS算法同频干扰状态 */
    osal_u32 res     : 31;                                     /* 4字节对齐 */

    osal_u32 *rom;
}alg_rts_dev_info_stru;
/* rts的user信息结构体 */
typedef struct {
    osal_u32   first_loss_ratio                 : 11;        /* 首包错误率(千分数) */
    osal_u32   first_pkt_stat_intvl_pktcnt      : 7;         /* 首包统计间隔包计数 */
    osal_u32   first_pkt_stat_rate0_pktcnt      : 8;         /* rate0首包统计间隔包计数 */

    osal_u32   is_valid_user                    : 1;         /* 当前用户是否注册标记位 */
    osal_u32   rate0_use_rts                    : 1;         /* rate0是否使用RTS */
    osal_u32   enable_ampdu                     : 1;         /* 是否支持AMPDU */
    osal_u32   traffic_type                     : 3;         /* 该用户的业务类型 */

    /* 指向其它已分配空间的指针 */
    alg_rts_dev_info_stru   *rts_dev_info;
    dmac_user_stru          *dmac_user;
}alg_rts_user_info_stru;
/* rts_dev结构体rom指针 */
typedef osal_u32 (*alg_rts_dev_init_cb)(alg_rts_dev_info_stru *rts_dev);
/* 重点函数rom钩子预留 */
typedef osal_u32 (*alg_rts_tx_process_cb)(dmac_user_stru *user, mac_tx_ctl_stru *cb, hal_tx_txop_alg_stru *txop_param,
    osal_u32 *ret);
typedef osal_u32 (*alg_rts_tx_comp_process_cb)(dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr_param, osal_u32 *ret);
typedef osal_u32 (*alg_rts_rate_tx_param_cb)(hal_tx_ctrl_desc_rate_stru *per_rate_params, osal_u8 *rts_rate,
    alg_rts_user_info_stru *user_info, osal_u32 *ret);
typedef osal_u32 (*alg_rts_tx_payload_len_notify_cb)(dmac_user_stru *user, hal_tx_txop_alg_stru *txop_param,
    osal_u32 payload_len, osal_u8 ampdu_enable, osal_u32 *ret);
/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_s32 alg_rts_init(osal_void);
osal_void alg_rts_exit(osal_void);
osal_u32 alg_rts_init_user(dmac_vap_stru *vap, dmac_user_stru *user);
osal_u32 alg_rts_exit_user(dmac_vap_stru *vap, dmac_user_stru *user);
osal_u32 alg_rts_tx_process(dmac_user_stru *user, mac_tx_ctl_stru *cb, hal_tx_txop_alg_stru *txop_param);
osal_u32 alg_rts_tx_complete_process(dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr_param);
osal_u32 alg_rts_tx_payload_len_notify(dmac_user_stru *user, hal_tx_txop_alg_stru *txop_param, osal_u32 payload_len,
    osal_u8 ampdu_enable);
osal_u32 alg_rts_co_intf_notify(hal_to_dmac_device_stru *hal_dev, alg_intf_det_notify_info_stru *intf_det_notify);
osal_s32 alg_rts_threshold_sync(dmac_vap_stru *dmac_vap, frw_msg *msg);
/* 配置命令相关的函数调用给外部接口使用 */
osal_u32 alg_rts_config_param(frw_msg *msg);
osal_u32 alg_rts_param_sync(const alg_param_sync_stru *sync);
#endif /* #ifdef _PRE_WLAN_FEATURE_RTS */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_rts_rom.h */
