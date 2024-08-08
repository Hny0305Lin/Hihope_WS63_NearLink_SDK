/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: traffic control schedule algorithm
 */

#ifndef ALG_TRAFFIC_CTL_ROM_H
#define ALG_TRAFFIC_CTL_ROM_H

#include "alg_main_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏和STRUCT定义
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_TRAFFIC_CTL
#define ALG_TRAFFIC_CTL_DETECT_INIT                   0
#define ALG_TRAFFIC_CTL_DETECT_ALLOC                  1
#define ALG_TRAFFIC_CTL_DETECT_FREE                   2

typedef struct {
    osal_u8 thres[HAL_RX_DSCR_QUEUE_ID_BUTT];
    osal_u8 resv[1];
} alg_traffic_rx_restore_stru;

typedef struct {
    frw_timeout_stru traffic_ctl_timer;  /* 拥塞控制算法定时器 */
    alg_traffic_tx_ctl_stru tx_ctl;      /* 发送拥塞控制阈值和参数 */
    alg_traffic_stats_stru stats;        /* 拥塞控制统计 */
    alg_traffic_rx_restore_stru rx_res;  /* 接收描述符队列预留长度 */
    alg_param_sync_traffic_ctl_stru *param_sync;
} alg_traffic_ctl_stru;

/*****************************************************************************
  3 函数声明
*****************************************************************************/
typedef osal_u32 (*dmac_traffic_ctl_config)(const dmac_ioctl_alg_param_stru *alg_param);
typedef osal_u32 (*dmac_traffic_ctl_timer)(osal_void *void_code);
typedef osal_u32 (*dmac_traffic_ctl_downlink)(dmac_vap_stru *vap, oal_dmac_netbuf_stru *buf,
    dmac_txrx_output_type_enum_uint8 *output);
typedef osal_u32 (*dmac_traffic_ctl_free)(osal_void);
typedef osal_u32 (*dmac_traffic_ctl_alloc)(oal_dmac_netbuf_stru *netbuf);
typedef osal_u32 (*dmac_traffic_ctl_user)(dmac_vap_stru *vap, dmac_user_stru *user);
typedef osal_u32 (*dmac_traffic_ctl_vap)(dmac_vap_stru *vap);
typedef osal_u32 (*dmac_traffic_rx_thres)(alg_traffic_ctl_stru *tc,
    hal_to_dmac_device_stru *hal_device, hal_rx_dscr_queue_id_enum_uint8 queue_id);
typedef osal_u32 (*dmac_traffic_tx_notify)(alg_traffic_ctl_stru *tc, osal_u32 large_free, osal_u32 short_free,
    osal_u32 mgmt_free);

alg_traffic_ctl_stru *alg_traffic_ctl_get(osal_void);
osal_void alg_traffic_ctl_init(osal_void);
osal_void alg_traffic_ctl_exit(osal_void);
osal_u32 alg_traffic_ctl_config_param(const dmac_ioctl_alg_param_stru *alg_param, frw_msg *msg);
osal_u32 alg_traffic_ctl_sync_param(const alg_param_sync_stru *sync);
#endif /* #ifdef _PRE_WLAN_FEATURE_TRAFFIC_CTL */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_traffic_ctl_rom.h */