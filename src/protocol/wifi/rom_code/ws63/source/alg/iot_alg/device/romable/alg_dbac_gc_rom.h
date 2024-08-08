 /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: dbac algorithm
 */

#ifndef ALG_DBAC_GC_ROM_H
#define ALG_DBAC_GC_ROM_H

#ifdef _PRE_WLAN_FEATURE_DBAC
/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "oal_types_device_rom.h"
#include "frw_ext_if_rom.h"
#include "mac_device_rom.h"
#include "alg_main_rom.h"
#include "alg_dbac_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef osal_u32 (*alg_dbac_noa_para_change_notify_cb)(dmac_vap_stru *dmac_gc);
typedef oal_bool_enum_uint8 (*alg_dbac_need_preempt_gc_slot_cb)(alg_dbac_mgr_stru *dbac_mgr,
    alg_dbac_sched_mgr_stru *sched_mgr, dmac_vap_stru *check_vap);
osal_u32 alg_dbac_sta_cl_noa_entry(alg_dbac_mgr_stru *dbac_mgr, dmac_vap_stru *sta_vap, dmac_vap_stru *gc_vap);
oal_bool_enum_uint8 alg_dbac_need_preempt_gc_slot(alg_dbac_mgr_stru *dbac_mgr,
    alg_dbac_sched_mgr_stru *sched_mgr, dmac_vap_stru *check_vap);
osal_u32 alg_dbac_sta_cl_noa_update_slots(alg_dbac_mgr_stru *dbac_mgr, dmac_vap_stru *sta_vap, dmac_vap_stru *gc_vap);
osal_void  alg_dbac_sta_gc_noa_isr_proc(alg_dbac_mgr_stru *dbac_mgr, oal_bool_enum_uint8 absent_start);
osal_u32 alg_dbac_sta_cl_noa_adjustment(alg_dbac_mgr_stru *dbac_mgr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _PRE_WLAN_FEATURE_DBAC */
#endif
