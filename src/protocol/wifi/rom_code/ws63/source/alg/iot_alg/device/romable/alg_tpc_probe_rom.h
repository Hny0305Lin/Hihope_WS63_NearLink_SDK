/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: algorithm transfer power control
 */

#ifndef ALG_TPC_PROBE_ROM_H
#define ALG_TPC_PROBE_ROM_H

#ifdef _PRE_WLAN_FEATURE_TPC
#ifdef _PRE_WLAN_TPC_DO_PROBE
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
/* 探测参数 */
#define ALG_TPC_MIN_PROBE_INTVL_PKTNUM 8   /* 默认最小探测间隔包数目 */
#define ALG_TPC_MAX_PROBE_INTVL_PKTNUM 128 /* 默认最大探测间隔包数目 */
#define ALG_TPC_PROBE_INTVL_KEEP_TIMES 4   /* 默认探测间隔保持的次数 */
#define ALG_TPC_AGGR_PER_SMOOTH_SCALE 3    /* 聚合PER平滑因子 */
#define ALG_TPC_NORMAL_PER_SMOOTH_SCALE 5  /* 非聚合PER平滑因子 */
#define ALG_TPC_PER_SMOOTH_SCALE_MIN 2     /* 最小PER平滑因子 */
#define ALG_TPC_CUR_PER_REDUCE_TH 5        /* 本次PER会调整平滑PER门限 */

/* PER门限 */
#define ALG_TPC_INVALID_PER 0xffff /* 无效PER */
#define ALG_TPC_BEST_RATE_PER_LIMIT_TO_DOWN (40)
#define ALG_TPC_BEST_RATE_PER_LIMIT_TO_UP (80) // (100)
#define ALG_TPC_PROBE_RATE_PER_LIMIT_STOP_DOWN (ALG_TPC_BEST_RATE_PER_LIMIT_TO_UP)
#define ALG_TPC_PROBE_RATE_PER_LIMIT_STOP_UP (ALG_TPC_BEST_RATE_PER_LIMIT_TO_DOWN)
#define ALG_TPC_PER_MARGIN (30) /* TPC PER相对增加量阈值 */
#define ALG_TPC_MAX_PROBE_WAIT_COUNT 4 /* 等待探测结果的最大发送完成中断次数(避免中断丢失导致状态异常) */

/******************************************************************************
  3 枚举定义
******************************************************************************/
/* 算法状态 */
typedef enum {
    ALG_TPC_STATE_NORMAL = 0,     /* TPC工作在最优功率的状态 */
    ALG_TPC_STATE_PROBE_UP = 1,   /* TPC向上探测的状态 */
    ALG_TPC_STATE_PROBE_DOWN = 2, /* TPC向下探测的状态 */
    ALG_TPC_STATE_BUTT
} alg_tpc_state_enum;
typedef osal_u8 alg_tpc_state_enum_uint8;

/* TPC帧类型 */
typedef enum {
    ALG_TPC_FRAME_BEST = 0,  /* TPC以最优功率发帧 */
    ALG_TPC_FRAME_PROBE = 1, /* TPC以探测功率发帧 */
    ALG_TPC_FRAME_FIX = 2,   /* TPC以固定功率发帧(只针对data0用固定功率, data1~3仍用最大功率) */
    ALG_TPC_FRAME_MAX = 3,   /* TPC以最大功率发帧，用于autorate的探测帧 */
    ALG_TPC_FRAME_BUTT
} alg_tpc_frame_type_enum;
typedef osal_u8 alg_tpc_frame_type_enum_uint8;

/******************************************************************************
  4 STRUCT定义
******************************************************************************/
/* 支持TPC功率调整的速率集合,区分元素protocol,bw */
typedef struct {
    osal_u8 alg_tpc_11b_bw_target_index;
    osal_u8 alg_tpc_11g_11a_bw_target_index;
    osal_u8 alg_tpc_11n_20mhz_target_index;
    osal_u8 alg_tpc_11n_40mhz_target_index;

    osal_u8 alg_tpc_11ac_20mhz_target_index;
    osal_u8 alg_tpc_11ac_40mhz_target_index;
    osal_u8 alg_tpc_11ac_80mhz_target_index;
    osal_u8 alg_tpc_11ac_160mhz_target_index;
} alg_tpc_target_rate_index_stru;

/* VAP信息结构体 */
typedef struct {
#ifdef _PRE_WLAN_TPC_DO_PROBE
    alg_tpc_state_enum_uint8 probe_state : 3;         /* 当前用户当前业务类型的探测状态 */
    oal_bool_enum_uint8 probe_query : 1;              /* 探测功率已被查询使用过 */
    oal_bool_enum_uint8 probe_update : 1;             /* 探测功率的结果已更新 */
    oal_bool_enum_uint8 allow_autorate_down_rate : 1; /* 是否允许autorate降速 */
    oal_bool_enum_uint8 perform_ok_old : 1;
    oal_bool_enum_uint8 resv : 1;
    osal_u8 target_rate_index;    /* 该业务的目标速率索引最小值(一般为速率集的最高速率) */
#else
    alg_tpc_state_enum_uint8 probe_state : 3;         /* 当前用户当前业务类型的探测状态 */
    oal_bool_enum_uint8 resv : 5;
#endif
    osal_u8 best_rate_index;      /* 该业务当前Autorate选出的最优速率索引，AC最大速率索引表数目72 */
    osal_u8 best_pow_idx;         /* 最优功率索引 */
#ifdef _PRE_WLAN_TPC_DO_PROBE
    osal_u8 old_best_pow_idx;
    osal_u8 cur_probe_pow_idx;    /* 当前探测功率索引 */
    osal_u8 perf_ok_min_pow_idx;  /* 性能达到要求的最小功率索引 */
    osal_u8 probe_intvl_pktcnt;   /* 探测间隔计数 */

    osal_u8 probe_intvl_pktnum;   /* 探测间隔包数目 */
    osal_u8 probe_intvl_keep_cnt; /* 探测间隔保持计数 */
    osal_u8 probe_wait_count;     /* 探测发送完成中断的等待计数 */
    osal_u8 min_per_pow_idx;      /* 最小PER对应的功率索引 */

    osal_u16 probe_min_per;       /* 探测过程中的最小PER(千分数) */
    osal_u8 probe_pktno;          /* 探测报文编号 */
    osal_u8 resv1;                /* 数组大小 */

    osal_u16 smooth_per[HAL_POW_ADJUST_LEVEL_NUM]; /* 各个功率等级的统计信息 */
#else
    osal_u8 resv1;                /* 数组大小 */
#endif
} alg_tpc_probe_info_stru;

/******************************************************************************
  5 函数声明
******************************************************************************/
typedef osal_u32 (*alg_tpc_probe_state_process_cb)(const alg_tpc_user_pow_info_stru *user_pow_info,
    alg_tpc_probe_info_stru *probe, const hal_tx_dscr_ctrl_one_param *tx_dscr_param);
osal_void alg_tpc_non_probe_state_process(const alg_tpc_user_pow_info_stru *user_pow_info,
    hal_user_pow_info_stru *hal_user_pow_info, alg_tpc_probe_info_stru *probe_info,
    hal_tx_dscr_ctrl_one_param *tx_dscr_param);
osal_void alg_tpc_init_all_probe_info(const alg_tpc_user_pow_info_stru *user_pow_info);
osal_void alg_tpc_probe_init(osal_void);
osal_void alg_tpc_probe_exit(osal_void);
osal_void alg_tpc_get_probe_pow_frame_type(alg_tpc_user_pow_info_stru *user_pow_info, mac_tx_ctl_stru *cb,
    alg_tpc_frame_type_enum_uint8 *frame_type);
osal_void alg_tpc_best_rate_change_process(alg_tpc_user_pow_info_stru *user_pow_info, osal_u8 best_rate_index);
osal_void alg_tpc_tx_complete_probe_process(const alg_tpc_user_pow_info_stru *user_pow_info,
    dmac_user_stru *dmac_user, alg_tpc_probe_info_stru *probe_info,
    hal_tx_dscr_ctrl_one_param *tx_dscr_param)
osal_void alg_tpc_init_all_user_probe_info(const hal_to_dmac_device_stru *hal_device);
osal_u32 alg_tpc_get_traffic_rate_info(alg_tpc_user_pow_info_stru *user_pow_info,
    wlan_wme_ac_type_enum_uint8 traffic_type);
osal_void alg_tpc_update_power_per(const alg_tpc_user_pow_info_stru *user_pow_info,
    alg_tpc_probe_info_stru *probe_info, const hal_tx_dscr_ctrl_one_param *tx_dscr_param, osal_u8 pow_idx);
osal_void alg_tpc_best_rate_change_process(alg_tpc_user_pow_info_stru *user_pow_info, osal_u8 best_rate_index);
osal_void alg_tpc_get_pow_frame_type(alg_tpc_user_pow_info_stru *user_pow_info, mac_tx_ctl_stru *cb,
    alg_tpc_frame_type_enum_uint8 *frame_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
#endif
#endif /* end of alg_tpc_probe_rom.h */

