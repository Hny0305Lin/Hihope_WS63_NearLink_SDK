 /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: dbac algorithm
 */

#ifndef DBAC_CHBA_ROM_H
#define DBAC_CHBA_ROM_H

#ifdef _PRE_WLAN_FEATURE_DBAC
/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "alg_dbac_rom.h"
#include "alg_main_rom.h"
#include "oal_types_device_rom.h"
#include "frw_ext_if_rom.h"
#include "mac_device_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 2 宏定义 */
#define MAX_CHBA_SLOT 32
#define PER_SLOT (MAX_CHBA_SLOT - 1)

typedef enum {
    CHBA_SLOT0 = 0,
    CHBA_SLOT1 = 1,
    CHBA_SLOT2 = 2,
    CHBA_SLOT3 = 3,
    CHBA_SLOT4 = 4,
    CHBA_SLOT5 = 5,
    CHBA_SLOT6 = 6,
    CHBA_SLOT7 = 7,
    CHBA_SLOT8 = 8,
    CHBA_SLOT9 = 9,
    CHBA_SLOT10 = 10,
    CHBA_SLOT11 = 11,
    CHBA_SLOT12 = 12,
    CHBA_SLOT13 = 13,
    CHBA_SLOT14 = 14,
    CHBA_SLOT15 = 15,
    CHBA_SLOT16 = 16,
    CHBA_SLOT17 = 17,
    CHBA_SLOT18 = 18,
    CHBA_SLOT19 = 19,
    CHBA_SLOT20 = 20,
    CHBA_SLOT21 = 21,
    CHBA_SLOT22 = 22,
    CHBA_SLOT23 = 23,
    CHBA_SLOT24 = 24,
    CHBA_SLOT25 = 25,
    CHBA_SLOT26 = 26,
    CHBA_SLOT27 = 27,
    CHBA_SLOT28 = 28,
    CHBA_SLOT29 = 29,
    CHBA_SLOT30 = 30,
    CHBA_SLOT31 = 31,
    CHBA_SLOT_BUTT
} alg_dbac_chba_slot_enum;
typedef osal_u8 alg_dbac_chba_slot_enum_uint8;

#define DISCOVER_SLOT CHBA_SLOT0
#define DISCOVER_END_SLOT CHBA_SLOT1
#define CHBA_DBAC_MAX_TBTT_COUNT 10 /* 一个CHBA周期最多tbtt中断数 */
#define CHBA_DEFALUT_PERIODS_MS 512 /* CHBA默认调度周期时长(ms) */

/* CHBA初始化时默认参数 */
#define CHBA_DEFAULT_DURATION_MS (13) /* CHBA默认duration时长(ms) */
#define CHBA_DEFAULT_INTERVAL_MS (16) /* CHBA默认interval时长(ms) */
#define CHBA_DEFAULT_NOA_DURATION (CHBA_DEFAULT_DURATION_MS << 10) /* CHBA默认noa duration参数(tu) */
#define CHBA_DEFAULT_NOA_INTERVAL (CHBA_DEFAULT_INTERVAL_MS << 10) /* CHBA默认noa interval参数(tu) */

#define get_slot_by_tsf(a) (((a + 100) >> 10) >> 4)
#define get_slot_id_by_tsf(a) (get_slot_by_tsf(a) & PER_SLOT)

#define alg_dbac_bit(x) (1U << (x))

struct chba_dbac_info {
    /* STA抢占CHBA时隙编号bitmap.0：不抢占；其他值：bit_x=1表示slot_x 被抢占 */
    osal_u32 sta_preempt_chba_bitmap;
    osal_u8 chan_switch_flag; /* dbac场景chba切信道标志位 */
    oal_bool_enum_uint8 support_channel_follow;   /* 是否支持CHBA信道跟随 */
    osal_u8 dbac_resume_slot_idx; /* chba dbac恢复发送的slot_idx */
    osal_u8 resv;
};

typedef struct {
    osal_u32 channel_sequence_bitmap; /* CHBA 信道序列工作时隙 */
    struct chba_dbac_info dbac_info;
    /* 对应的dmac_vap_stru指针 */
    dmac_vap_stru *dmac_vap;
} chba_vap_info_stru;

typedef osal_u32 (*alg_dbac_sta_chba_noa_isr_proc_cb)(alg_dbac_mgr_stru *dbac_mgr, oal_bool_enum_uint8 is_absent_start);
typedef osal_u32 (*alg_dbac_parse_type_sta_chba_vap_cb)(const dmac_vap_stru *dmac_vap0, const dmac_vap_stru *dmac_vap1,
    alg_dbac_type_enum_uint8 *dbac_type, osal_u8 *led_vapid, osal_u8 *flw_vapid);
osal_u32 alg_dbac_parse_type_sta_chba_vap(const dmac_vap_stru *dmac_vap0, const dmac_vap_stru *dmac_vap1,
    alg_dbac_type_enum_uint8 *dbac_type, osal_u8 *led_vapid, osal_u8 *flw_vapid);
osal_void alg_dbac_update_sta_chba_mode(alg_dbac_mgr_stru *dbac_mgr,
    dmac_vap_stru *led_vap, dmac_vap_stru *flw_vap);
osal_void alg_dbac_sta_chba_noa_isr_proc(
    alg_dbac_mgr_stru *dbac_mgr, oal_bool_enum_uint8 is_absent_start);
oal_bool_enum_uint8 alg_chba_dbac_should_protect_vip_frame(alg_dbac_mgr_stru *dbac_mgr);
osal_void dmac_chba_dbac_update_preempt_info(dmac_vap_stru *dmac_vap, chba_vap_info_stru *chba_vap_info);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _PRE_WLAN_FEATURE_DBAC */
#endif
