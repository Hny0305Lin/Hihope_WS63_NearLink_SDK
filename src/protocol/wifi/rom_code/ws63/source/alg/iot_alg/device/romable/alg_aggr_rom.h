/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: auto-aggr algorithm rom.
 */

#ifndef ALG_AGGR_ROM_H
#define ALG_AGGR_ROM_H

#ifdef _PRE_WLAN_FEATURE_AUTOAGGR

/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "hal_device_rom.h"
#include "dmac_ext_if_rom.h"
#include "frw_msg_rom.h"
#include "dmac_alg_if_part_rom.h"
#include "alg_common_rom.h"
#include "alg_probe_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/******************************************************************************
  2 宏定义
******************************************************************************/
#define ALG_AGGR_4MS_TO_250US_SHIFT       4     /* 基准聚合时间4ms相对于最小聚合时间250us的移位值(2^n) */
#define ALG_AGGR_SHORT_STAT_SHIFT         3     /* 默认短期统计的除2位移值 */
#define ALG_AGGR_LONG_STAT_SHIFT          4     /* 默认长期统计的除2位移值 */

#define alg_aggr_stat_smooth_short_per(_avg_per, _instant_per, _scale)                      \
    ((((osal_u16)((_avg_per) * ((1ul << (ALG_AGGR_SHORT_STAT_SHIFT + (_scale))) - 1))) >> \
       (ALG_AGGR_SHORT_STAT_SHIFT + (_scale))) + ((_instant_per) >> (ALG_AGGR_SHORT_STAT_SHIFT + (_scale))))

#define alg_aggr_stat_smooth_long_per(_avg_per, _instant_per, _scale) \
    ((((osal_u16)((_avg_per) * ((1ul << (ALG_AGGR_LONG_STAT_SHIFT + (_scale))) - 1)))>> \
       (ALG_AGGR_LONG_STAT_SHIFT + (_scale))) + ((_instant_per) >> (ALG_AGGR_LONG_STAT_SHIFT + (_scale))))

/*
    通过当前最优速率获取4ms对应的字节数限制：
    其中aggr_bytes_limit_4ms = (best_rate_kbps * 4ms) / 8bits = best_rate_kbps * 1/2
*/
#define alg_aggr_get_best_rate_bytes_limit_4ms(best_rate_kbps)    ((best_rate_kbps) >> 1)

#define alg_aggr_param_offset(_elem) (alg_stru_elem_offset(alg_aggr_stru, _elem))
#define alg_aggr_calc_cfg_id_offest(alg_cfg_id) ((alg_cfg_id) - MAC_ALG_CFG_AGGR_START)
/******************************************************************************
  3 枚举定义
******************************************************************************/
typedef enum {
    ALG_AGGR_AUTO_MODE              = 0,    /* 使用auto模式获取聚合时间 */
    ALG_AGGR_FIXED_MODE             = 1,    /* 使用fixed模式配置聚合时间 */
    ALG_AGGR_MODE_BUTT
} alg_aggr_mode_enum;

typedef enum {
    ALG_AGGR_MIN_AGGR_TIME_IDX         = 0,               /* 聚合自适应最小聚合时间索引 */
    ALG_AGGR_500US_TIME_IDX            = 1,               /* 500us对应的聚合传输时间索引 */
    ALG_AGGR_1MS_TIME_IDX              = 2,               /* 1ms对应的聚合传输时间索引 */
    ALG_AGGR_2MS_TIME_IDX              = 3,               /* 2ms对应的聚合传输时间索引 */
    ALG_AGGR_4MS_TIME_IDX              = 4,               /* 4ms对应的聚合传输时间索引 */
    ALG_AGGR_MAX_AGGR_TIME_IDX         = 5,               /* 聚合自适应最大聚合时间索引 */
    ALG_AGGR_TIME_IDX_BUTT
} alg_aggr_time_index_enum;
typedef osal_u8 alg_aggr_time_idx_enum_uint8;

/* 算法内部模块使用 */
extern const osal_u16 g_aggr_time_list[ALG_AGGR_TIME_LIST_NUM];
extern const osal_u16 g_aggr_scale_list[ALG_AGGR_TIME_LIST_NUM];

/******************************************************************************
  4 STRUCT定义
******************************************************************************/
/* 聚合自适应算法结构体 */
typedef struct {
    osal_bool enable;                     /* 是否使能聚合自适应算法 */
    osal_bool dbg_aggr_log;               /* 使能聚合优化日志 */
    osal_bool serial_aggr_log;            /* 使能聚合优化测试用串口日志 */
    osal_bool voice_aggr;                 /* Voice是否支持聚合的标志 */

    /* 聚合优化相关 */
    osal_u8 aggr_mode;                     /* 当前聚合自适应的模式（FIXED, AUTO） */
    osal_u8 fixed_aggr_time_idx;           /* 固定档位聚合时间对应的g_aaggr_time_list数组下标 */
    osal_u8 intf_adjch_status;             /* 邻频干扰状态 */
    osal_u8 aggr_stat_num;                 /* 聚合时间探测的统计delta PER的包数目 */

    osal_u8 aggr_stat_shift;               /* 聚合时间探测的统计delta PER的包数目除2位移值 */
    osal_u8 max_aggr_num;                  /* 规格限定最大聚合个数 */
    osal_u8 max_aggr_time_idx;             /* 约束的最大聚合时间对应的g_aggr_time_list数组下标 */
    osal_u8 min_aggr_time_idx;             /* 最小聚合传输时间索引 */

    alg_param_sync_aggr_stru *host_param_sync;   /* 从host侧同步的算法阈值参数结构体 */

    osal_u8 *rom;
} alg_aggr_stru;

/* 聚合优化相关信息 */
typedef struct {
    osal_u32 normal_aggr_mpdu_bytes;     /* 正常聚合时间对应的聚合MPDU的最大byptes */
    osal_u8  normal_max_aggr_num;        /* 默认聚合时间下最大聚合字节数对应的聚合数目 */
    osal_u8  aggr_probe_state : 3;       /* 聚合探测状态类型alg_aggr_probe_state_enum_uint8 */
    osal_u8  aggr_probe_direct : 2;      /* 聚合探测方向,类型alg_aggr_probe_direct_enum_uint8 */
    osal_u8  aggr_time_idx : 3;          /* 当前最优聚合传输时间数组索引,类型alg_aggr_time_idx_enum_uint8 */
    osal_u16 aggr_time_us;               /* 当前最优聚合传输时间(单位:us) */

    osal_u8  aggr_probe_intvl_cnt;       /* 聚合时间尝试对应的包间隔计数 */
    osal_u8  aggr_up_stat_cnt;           /* 向上探测聚合时间传输的统计次数 */
    osal_u8  aggr_down_stat_cnt;         /* 向下探测聚合时间传输的统计次数 */
    osal_u8  probe_pktno_wait_cnt;       /* 探测报文已发送但暂未收到完成中断的等待次数 */

    osal_u16 aggr_probe_pktno : 12; /* 聚合探测报文的编号 */
    osal_u16 cur_aggr_idx : 4;      /* 当前使用的聚合时间索引，正常是最优聚合传输时间数组索引，探测时取探测聚合时间索引 */
    osal_u16 probe_aggr_idx : 4;    /* 探测聚合时间索引类型alg_aggr_time_idx_enum_uint8 */
    osal_u16 resv : 12;

    // : per的维护考虑增加老化机制，参考autorate
    osal_u16 short_per[ALG_AGGR_TIME_LIST_NUM];              /* 最优速率短期平均的PER(扩大1024倍) */
    osal_u16 long_per[ALG_AGGR_TIME_LIST_NUM];               /* 最优速率长期平均的PER(扩大1024倍) */
    osal_u32 aggr_last_used_time_ms[ALG_AGGR_TIME_LIST_NUM]; /* 各聚合时间档位最后的使用的时间 */

    osal_s32 aggr_up_delta_per;     /* 向上聚合探测的PER变化量 = aggr_up_per - aggr_normal_per (扩大2^20) */
    osal_s32 aggr_down_delta_per;   /* 向下聚合探测的PER变化量 = aggr_normal_per - aggr_down_per (扩大2^20) */

    osal_u32 up_aggr_mpdu_bytes;   /* 向上聚合探测对应的聚合MPDU的最大byptes */

    osal_u8 *rom;
} alg_aggr_user_info_stru;

/* 设置配置参数的回调函数指针 */
typedef osal_void (*aggr_config_param_handle_func)(osal_void);

/* 算法参数配置表结构 */
typedef struct {
    osal_u8 cfg_id_offest;
    alg_cfg_param_type_enum_uint8 param_type;
    osal_u16 param_offset;
    aggr_config_param_handle_func cfg_param_handle;
} alg_aggr_cfg_param_table_stru;

/******************************************************************************
  5 函数声明
******************************************************************************/
osal_s32  alg_aggr_init(osal_void);
osal_void alg_aggr_exit(osal_void);
osal_void alg_aggr_init_user_info(alg_aggr_user_info_stru *aggr_info, osal_u8 aggr_time_idx, dmac_user_stru *dmac_user);
alg_aggr_stru *alg_aggr_get_dev_info(osal_void);
osal_void alg_aggr_update_max_mpdu_bytes(alg_aggr_user_info_stru *aggr_info, hal_tx_dscr_ctrl_one_param *tx_dscr);
osal_u16 alg_aggr_get_instant_per(hal_tx_dscr_ctrl_one_param *tx_dscr);
osal_u32 alg_aggr_config_param(const dmac_vap_stru *dmac_vap, const dmac_ioctl_alg_param_stru *alg_param);
#ifdef _PRE_WLAN_TPC_DO_PROBE
osal_bool alg_aggr_get_voice_switch(osal_void);
#endif
osal_void alg_aggr_config_stat_num(osal_void);
osal_u32 alg_aggr_param_sync(const alg_param_sync_stru *sync);
osal_u32 alg_aggr_probe_deadlock(dmac_user_stru *dmac_user, osal_u8 lock_type);

typedef osal_u32 (*alg_aggr_init_cb)(osal_void);
typedef osal_u32 (*alg_aggr_exit_cb)(osal_void);
typedef osal_u32 (*alg_aggr_set_cfg_cb)(const dmac_ioctl_alg_param_stru *alg_param, osal_u32 *ret);
typedef osal_u32 (*alg_aggr_init_user_info_cb)(alg_aggr_user_info_stru *aggr_info, osal_u8 aggr_time_idx,
    dmac_user_stru *dmac_user);
typedef osal_u32 (*alg_aggr_normal_proc_cb)(dmac_user_stru *dmac_user, alg_aggr_user_info_stru *aggr_info,
    mac_tx_ctl_stru *cb, hal_tx_dscr_ctrl_one_param *tx_dscr);
typedef osal_u32 (*alg_aggr_set_sch_aggr_cb)(dmac_user_stru *dmac_user, osal_u8 aggr_time_idx, osal_u8 tid_id,
    osal_u32 *ret);
typedef osal_u32 (*alg_aggr_tx_comp_cb)(dmac_user_stru *dmac_user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr, osal_u32 *ret);
typedef osal_u32 (*alg_aggr_brate_update_cb)(dmac_user_stru *dmac_user, osal_u32 rate_kbps,
    alg_autorate_rate_detail_stru *rate_detail, osal_u32 *ret);
typedef osal_u32 (*alg_aggr_tx_payload_len_chk_cb)(dmac_user_stru *user, hal_tx_txop_alg_stru *txop_param,
    osal_u32 payload_len, osal_u8 ampdu_enable, osal_u32 *ret);
typedef osal_u32 (*alg_aggr_adjintf_notify_cb)(hal_to_dmac_device_stru *hal_device,
    alg_intf_det_notify_info_stru *intf_det_notify, osal_u32 *ret);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* #ifdef _PRE_WLAN_FEATURE_AUTOAGGR */
#endif /* end of alg_aggr_rom.h */
