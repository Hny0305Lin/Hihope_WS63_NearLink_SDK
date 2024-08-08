 /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: dbac algorithm
 */

#ifndef ALG_DBAC_ROM_H
#define ALG_DBAC_ROM_H

#ifdef _PRE_WLAN_FEATURE_DBAC
/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "oal_types_device_rom.h"
#include "frw_ext_if_rom.h"
#include "mac_device_rom.h"
#include "alg_main_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/******************************************************************************
  2 宏定义
******************************************************************************/
/* 在slot结束前提前暂停VAP的时间 */
#define DBAC_MAX_SLOT_NUM                   40  /* 按照1000ms 最大40个slot */

#define DBAC_MAX_BEACON_INTERVAL            3000
#define DBAC_DEFAULT_LED_SLOT_RATIO         50
#define DBAC_SLOT_RATIO_MULTIPLE            100
#define DBAC_SLOT_RATIO_LED_MIN             20
#define DBAC_SLOT_RATIO_LED_MAX             80
#define CFG_DBAC_BEACON_INTERVAL_TU         (100U)
#define CFG_DBAC_SCHED_PERIOD_TU            (CFG_DBAC_BEACON_INTERVAL_TU >> 1)
#define CFG_DBAC_LED_VAP_DWELL_TU           (CFG_DBAC_SCHED_PERIOD_TU >> 1)

#define DBAC_NOA_CHECK_OFFSET               1000    /* us */

#define DBAC_DUAL_STA_MARGIN 5 /* ms, 双STA模式时tbtt与切信道之间的最小距离 */
#define DBAC_DUAL_STA_SAMETIME_THRES (DBAC_DUAL_STA_MARGIN * 2)
#define DBAC_DUAL_STA_MODE_UPDATE_THRES (40 * 4) /* 1个时隙长度25ms，25ms*40*4=4S，过多久后更新双STA的模式 */
#define DBAC_STA_GO_UPDATE_THRES (16)        /* 1个时隙长度25ms，25ms*40*4=4S，过多久后重设NoA */
#define DBAC_STA_GO_SYNC_CNT 7                   /* GO多久与STA的tsf同步一次，单位beacon周期 */

#define DBAC_US_TO_TU_FRACTION_BITS 10  /* us转换为tu */

#define dbac_us2tu(us) ((us) >> DBAC_US_TO_TU_FRACTION_BITS)
#define dbac_tu2us(tu) ((tu) << DBAC_US_TO_TU_FRACTION_BITS)

#define DBAC_TBTT_PREV_MARGIN 3    /* 单位ms */
#define DBAC_TBTT_POST_MARGIN 3    /* 单位ms */

#define alg_dbac_annother_vap_index(en_vap_index) \
    (((en_vap_index) == CFG_DBAC_LED_VAP_IDX) ? CFG_DBAC_FLW_VAP_IDX : CFG_DBAC_LED_VAP_IDX)

#define DBAC_PERIODIC_NOA_COUNT 255
#define DBAC_FIRST_TWO_SLOT 2
#define DBAC_ODD_SLOT_FLAG 1
/******************************************************************************
  3 枚举定义
******************************************************************************/
/* 双sta模式下的场景区分 */
typedef enum {
    ALG_DBAC_DUAL_STA_EQBI_SAME_TIME,      /* beacon周期相同 tbtt相距小于阈值 */
    ALG_DBAC_DUAL_STA_EQBI_SAME_SUBPERIOD, /* beacon周期相同 tbtt相距大于10ms且在同一个子周期内 */
    ALG_DBAC_DUAL_STA_EQBI_DIFF_SUBPERIOD, /* beacon周期相同 tbtt相距大于10ms且不在同一个子周期内 */
    ALG_DBAC_DUAL_STA_NEBI,                /* beacon周期不同 */

    ALG_DBAC_DUAL_STA_MODE_BUTT,
} alg_dbac_dual_sta_mode_enum;
typedef osal_u8 alg_dbac_dual_sta_mode_enum_uint8;

/******************************************************************************
  ４ STRUCT定义
******************************************************************************/
struct tag_alg_dbac_mgr_stru;
typedef osal_u32 (*alg_dbac_sched_func)(struct tag_alg_dbac_mgr_stru *dbac_mgr);

/* slot结构体 */
typedef struct {
    osal_u16 time;                   /* 时隙长度, 单位tu */
    alg_dbac_vap_idx_enum_uint8 idx; /* 此时隙所属的VAP */
    osal_u8 resv;
} alg_dbac_slot_stru;

typedef struct tag_alg_dbac_sched_mgr_stru {
    /* 调度管理器关键参数 */
    osal_u16 dbac_period;               /* DBAC时隙分配周期，规定等于led vap的beacon周期 */
    osal_u16 sched_period;              /* 时隙分配子周期 */

    osal_u16 sched_period_half;         /* 时隙分配子周期的一半 即1:1时隙分配时1个时隙的长度 */
    osal_u16 flw_vap_bi;                    /* flw vap的beacon周期, 只在双sta beacon周期不同时使用 */

    osal_u16 led_beacon_offset_tu;      /* led vap tbtt相对于dbac周期开始的偏移 */
    osal_u16 flw_beacon_offset_tu;      /* flw vap tbtt相对于dbac周期开始的偏移, 双sta模式下不使用 */

    /* 时隙分配结果 */
    osal_u8 sched_period_num;           /* 一个DBAC时隙分配周期中 子周期的个数 */
    osal_u8 slot_num;                 /* 一个DBAC时隙分配周期内的时隙个数 */
    osal_u8 cur_slot;                 /* 当前时隙编号 */
    osal_u8 resv0;

    osal_u16 slot_stats;              /* 每个时隙(25ms)++ */
    osal_u8 slot_ratio[CFG_DBAC_VAP_IDX_BUTT];     /* 每个频带的时隙比例(%) */

    alg_dbac_slot_stru slot_time[DBAC_MAX_SLOT_NUM];    /* 每个时隙的时长 单位tu */

    oal_bool_enum_uint8 fix_ratio;  /* 是否固定时隙比例的标志 */
    oal_bool_enum_uint8 fix_ratio_update;           /* 固定比例是否调整 */
    alg_dbac_dual_sta_mode_enum_uint8 dual_sta_mode; /* 双sta模式，只在双sta时隙分配时使用 */
    oal_bool_enum_uint8 flw_tbtt_present;   /* 此次调度是否flw tbtt在线(双STA场景存在轮流tbtt在线的场景) */

    oal_bool_enum_uint8 noa_need_update;             /* NoA是否需要更新 仅在sta+go模式下使用 */
    osal_u8 flw_tbtt_slot_num;                       /* 等时隙分配时 flw vap tbtt时刻所在时隙编号,编号从0开始 */
    osal_u8 bit_gc_slot_preempting  : 1;
    osal_u8 bit_gc_slot_protect_flag  : 1;
    osal_u8 bit_gc_slot_protect_period  : 6;
    osal_u8 resv1;
} alg_dbac_sched_mgr_stru;

/* DBAC状态定义 */
typedef enum {
    DBAC_STATE_NOT_INIT = 0,
    DBAC_STATE_INITED,
    DBAC_STATE_SWITCH_READY,
    DBAC_STATE_BUTT
} alg_dbac_state_enum;
typedef osal_u8 alg_dbac_state_enum_uint8;

/* DBAC状态定义 */
typedef enum {
    DBAC_TYPE_NONE = 0,  /* 初始化，不存在的类型 */
    DBAC_TYPE_STA_GO,    /* STA+GO异频共存 */
    DBAC_TYPE_STA_CL,    /* STA+CL异频共存 */
    DBAC_TYPE_STA_CL_NOA, /* STA+CL异频共存并且CL侧有count 255的NOA */
    DBAC_TYPE_STA_AP,     /* 为支持STA + SOFTAP信道跟随中间态 */
    DBAC_TYPE_STA_CHBA, /* STA+CHBA异信道共存 */

    DBAC_TYPE_BUTT
} alg_dbac_type_enum;
typedef osal_u8 alg_dbac_type_enum_uint8;

typedef enum {
    DBAC_TX_NETBUF_CHECK = 0,
    DBAC_TX_CB_CHECK,
    DBAC_TX_PAUSE_CHECK,
    DBAC_TX_VAP_ID_CHECK,

    DBAC_TX_BUTT
} alg_dbac_tx_check_enum;
typedef osal_u8 alg_dbac_tx_check_enum_uint8;

typedef struct {
    osal_u8 mac_vap_id;
    osal_u8 hal_vap_id;
    osal_u16 beacon_period;

    dmac_vap_stru *dmac_vap;
    hal_to_dmac_vap_stru *hal_vap;
} alg_dbac_switch_info;

typedef struct {
    alg_dbac_switch_info switch_info[CFG_DBAC_VAP_IDX_BUTT]; /* 0-leader */
    alg_dbac_type_enum_uint8 dbac_type;             /* DBAC场景 */
    alg_dbac_vap_idx_enum_uint8 sta_dbac_idx;
    osal_u8 resv[2];
} alg_dbac_cfg_stru;

typedef struct tag_alg_dbac_mgr_stru {
    alg_param_sync_dbac_stru *dbac_para;
    osal_u32 fcs_timer;                         /* fcs切换信道定时器handle */
    osal_u32 led_tbtt_delay_timer;              /* led vap tbtt delay定时器 */
    osal_u32 flw_tbtt_delay_timer;              /* flw vap tbtt delay定时器 */
    mac_fcs_cfg_stru fcs_cfg[CFG_DBAC_VAP_IDX_BUTT];    /* FCS配置 */
    alg_dbac_cfg_stru dbac_cfg;
    alg_dbac_sched_mgr_stru sched_mgr;             /* 分时调度统计 */

    alg_dbac_state_enum_uint8 dbac_state;           /* DBAC状态 */
    oal_bool_enum_uint8 dbac_pause : 1;             /* 是否暂停DBAC */
    oal_bool_enum_uint8 dbac_schedule_log : 1;      /* dbac schedule log */
    oal_bool_enum_uint8 dbac_linkloss_log : 1;      /* dbac log */
    oal_bool_enum_uint8 vip_frame_protect_cnt : 5;
    osal_s8 sta_ap_sync_cnt;                       /* 多少个tbtt GO同步一次tsf */
    osal_u8 resv;
    osal_u32 tick;

    alg_dbac_vap_stats_info vap_stats[CFG_DBAC_VAP_IDX_BUTT];  /* DBAC VAP统计信息 */
    alg_dbac_dev_stats_info dev_stats;
} alg_dbac_mgr_stru;

typedef osal_void (*alg_dbac_timer_isr_callback)(osal_u32 param);
typedef osal_u32 (*alg_dbac_sched_dual_sta_policy_cb)(alg_dbac_mgr_stru *dbac_mgr);
typedef osal_u32 (*alg_dbac_sched_sta_go_policy_cb)(alg_dbac_mgr_stru *dbac_mgr);
typedef osal_u32 (*alg_dbac_start_work_cb)(dmac_device_stru *dmac_device, hal_to_dmac_device_stru *hal_device,
    alg_dbac_mgr_stru *dbac_mgr, dmac_vap_stru *dmac_vap);
typedef osal_u32 (*alg_dbac_stop_auto_work_cb)(dmac_vap_stru *dmac_vap, dmac_device_stru *device,
    hal_to_dmac_device_stru *hal_device, alg_dbac_mgr_stru *dbac_mgr);
typedef osal_u32 (*alg_dbac_sync_sta_ap_tbtt_cb)(alg_dbac_mgr_stru *dbac_mgr, osal_bool is_led_tbtt);
typedef osal_bool (*alg_dbac_is_dbac_not_ready_cb)(osal_void);
typedef osal_u32 (*alg_dbac_prepare_fcs_config_cb)(mac_fcs_cfg_stru *config, alg_dbac_mgr_stru *dbac_mgr,
    const dmac_vap_stru *src_vap, const dmac_vap_stru *dst_vap);
typedef osal_u32 (*alg_dbac_parse_2vap_type_cb)(const dmac_vap_stru *dmac_vap0, const dmac_vap_stru *dmac_vap1,
    alg_dbac_type_enum_uint8 *dbac_type, osal_u8 *led_vapid, osal_u8 *flw_vapid);
typedef osal_u32 (*alg_dbac_tx_process_cb)(dmac_user_stru *dmac_user,
    mac_tx_ctl_stru *cb, hal_tx_txop_alg_stru *txop_param);
typedef osal_u32 (*alg_dbac_rx_process_cb)(dmac_vap_stru *vap, dmac_user_stru *user, oal_dmac_netbuf_stru *netbuf,
    hal_rx_statistic_stru *rx_stats);
typedef osal_u32 (*alg_dbac_tbtt_isr_cb)(osal_u8 vap_id, osal_void *arg);
typedef osal_u32 (*alg_dbac_noa_isr_cb)(osal_u8 vap_id, osal_void *arg, oal_bool_enum_uint8 absent_start);
typedef osal_u32 (*alg_dbac_tbtt_delay_isr_cb)(osal_bool is_led_tbtt);
typedef osal_u32 (*alg_dbac_set_noa_cb)(alg_dbac_mgr_stru *dbac_mgr);
typedef osal_u32 (*alg_dbac_sched_restart_dynamic_proc_cb)(alg_dbac_mgr_stru *dbac_mgr, const dmac_vap_stru *led_vap,
    hal_to_dmac_vap_stru *led_hal_vap, dmac_vap_stru *flw_vap, hal_to_dmac_vap_stru *flw_hal_vap);
typedef osal_u8 (*alg_dbac_resume_select_vap_idx_cb)(const alg_dbac_mgr_stru *dbac_mgr);
typedef osal_s32 (*alg_dbac_fcs_event_handle_cb)(dmac_vap_stru *dmac_vap, frw_msg *msg);
typedef osal_u32 (*alg_dbac_timer_isr_cb)(osal_u32 param);
typedef osal_u32 (*alg_dbac_update_sta_chba_mode_cb)(alg_dbac_mgr_stru *dbac_mgr,
    dmac_vap_stru *led_vap, dmac_vap_stru *flw_vap);
typedef osal_u32 (*alg_dbac_tx_netbuf_resverve_cb)(alg_dbac_vap_idx_enum_uint8 dbac_vap_type);

typedef osal_u32 (*dmac_chba_dbac_calc_preempt_bitmap_cb)(osal_u32 channel_seq_bitmap,
    osal_u32 *tbtt_timer_array, osal_u32 tbtt_timer_count);
/******************************************************************************
  ５ 函数声明
******************************************************************************/
osal_u32 alg_dbac_init(osal_void);
osal_u32 alg_dbac_exit(osal_void);
osal_u32 alg_dbac_param_sync(const alg_param_sync_stru *sync);
osal_s32 alg_dbac_set_resume(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_dbac_set_pause(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_dbac_set_state_down(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_dbac_update_fcs_config(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 alg_dbac_fcs_event_handle(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_void alg_dbac_post_fcs_event(const alg_dbac_mgr_stru *dbac_mgr, const mac_fcs_cfg_stru *fcs_cfg);
osal_bool alg_dbac_is_delay_switch_chan(osal_u8 chan_number);
alg_dbac_mgr_stru *alg_dbac_get_dbac_stru(osal_void);
osal_void alg_dbac_stop_timer(osal_void);
osal_bool alg_dbac_is_need_fcs(alg_dbac_sched_mgr_stru *sched_mgr, osal_u8 curr_slot);
osal_void alg_dbac_assign_slot(alg_dbac_slot_stru *slot_time, osal_u16 time, osal_u8 vap_idx);
osal_u32 alg_dbac_stop_vap(dmac_vap_stru *dmac_vap);
osal_u32 alg_dbac_start_vap(dmac_vap_stru *dmac_vap);
osal_void alg_dbac_log_info(const alg_dbac_mgr_stru *dbac_mgr);
osal_void alg_dbac_shedule_log_info_condition(const alg_dbac_mgr_stru *dbac_mgr);
osal_void alg_dbac_shedule_log_info(const alg_dbac_mgr_stru *dbac_mgr);
osal_s32 alg_dbac_wait_send_disassoc(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_bool alg_dbac_is_dbac_not_ready_or_paused(osal_void);
osal_u32 alg_dbac_config(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_u32 alg_dbac_record_2vap_fcs_config(alg_dbac_mgr_stru *dbac_mgr, osal_u8 led_vapid, osal_u8 flw_vapid);
osal_void alg_dbac_start_timer(osal_u32 us, alg_dbac_timer_isr_callback timer_isr);
osal_void alg_dbac_timer_isr(osal_u32 param);
osal_void alg_dbac_sync_sta_ap_tbtt(alg_dbac_mgr_stru *dbac_mgr, osal_bool is_led_tbtt);
osal_void alg_dbac_stop_all_timer(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _PRE_WLAN_FEATURE_DBAC */
#endif
