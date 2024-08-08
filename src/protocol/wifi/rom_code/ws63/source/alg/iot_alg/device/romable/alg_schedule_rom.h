/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
* Description: algorithm schedule rom
* Author:
*/
#ifndef ALG_SCHEDULE_ROM_H
#define ALG_SCHEDULE_ROM_H

#ifdef _PRE_WLAN_FEATURE_SCHEDULE
/******************************************************************************
1 头文件包含
******************************************************************************/
#include "hal_device_rom.h"
#include "wlan_spec_rom.h"
#include "dmac_ext_if_rom.h"
#include "dmac_alg_if_part_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    osal_u32 tx_complete_bytes;   /* 每个用户发送完成后释放的mpdu长度和 */
    osal_u32 last_tx_complete_bytes;   /* 上一次的每个用户发送完成后释放的mpdu长度和 */
    osal_u32 schedule_tx_time;          /* 每个用户发送时间 */
    osal_u32 last_schedule_tx_time;      /* 上一次的每个用户发送时间 */
    osal_u16 assoc_id;
    osal_u8  lut_index;
    osal_u8  resv;
} alg_sch_dmac_user_info;
typedef struct {
    osal_u8  dmac_log;                   /* dmac调度侧维测日志 */
    osal_u8  time_debug;                 /* dmac调度时间维测日志 */
    osal_u8  resv[2];

    osal_u32 bcast_air_time;            /* 广播消耗的空口时间 */
    osal_u32 bcast_tx_bytes;            /* 广播报文发送字节数 */

    osal_u32 stat_sch[WLAN_ASSOC_USER_MAX_NUM][WLAN_TIDNO_BUTT]; /* 统计device出包 */
    alg_param_sync_sch_stru *param_sync;    /* host侧同步参数 */
    frw_timeout_stru schedule_dmac_timer;  /* 定时上报定时器 */
}alg_dmac_sch_stru;

osal_s32 alg_schedule_dmac_init(osal_void);
osal_void alg_schedule_dmac_exit(osal_void);
osal_u32 alg_schedule_dmac_config(const dmac_vap_stru *dmac_vap, const dmac_ioctl_alg_param_stru *alg_param);
osal_u32 alg_sch_param_sync(const alg_param_sync_stru *sync);
osal_u32 (*alg_schedule_get_stat_sch(osal_void))[WLAN_TIDNO_BUTT];
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* #ifdef _PRE_WLAN_FEATURE_INTF_DET */
#endif /* end of alg_schedule_rom.h */