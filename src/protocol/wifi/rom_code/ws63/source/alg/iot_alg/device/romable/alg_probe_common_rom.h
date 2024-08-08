/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: algorithm probe common rom
 */

#ifndef ALG_PROBE_COMMON_ROM_H
#define ALG_PROBE_COMMON_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "dmac_user_rom.h"
#include "dmac_alg_if_part_rom.h"
#include "alg_main_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct {
    alg_lock_probe_type_enum_uint8 en_probe_lock; /* user共用的探测锁 */
    oal_bool_enum_uint8 lock_flag;                /* user置探测锁标志 */
    osal_u16 lock_times;                          /* 统计探测锁期间发包次数 */
    osal_u32 lock_timestamp;                      /* 记录user置探测锁时间戳 */
} alg_common_user_info_stru;
typedef struct {
    alg_param_sync_common_stru *sync_param;
} alg_common_dev_info_stru;
/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_u32 alg_common_init(void);
osal_u32 alg_common_exit(void);
osal_u32 alg_common_init_user(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user);
osal_u32 alg_common_exit_user(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user);
osal_u32 alg_common_tx_complete_proc(dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr);
osal_u32 alg_common_user_qry_probe_lock(osal_u16 user_lut_idx, alg_lock_probe_type_enum_uint8 *pen_lock_type);
osal_u32 alg_common_user_lock_probe(osal_u16 user_lut_idx, alg_lock_probe_type_enum_uint8 en_lock_type);
void alg_common_user_unlock_probe(osal_u16 user_lut_idx, alg_lock_probe_type_enum_uint8 en_lock_type);
osal_u32 alg_common_param_sync(const alg_param_sync_stru *sync);
typedef osal_u32 (*alg_common_init_cb)(void);
typedef osal_u32 (*alg_common_exit_cb)(void);
typedef osal_u32 (*alg_common_init_user_cb)(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user);
typedef osal_u32 (*alg_common_exit_user_cb)(dmac_vap_stru *dmac_vap, dmac_user_stru *dmac_user);
typedef osal_u32 (*alg_common_tx_complete_cb)(dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_tx_dscr_ctrl_one_param *tx_dscr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_common.h */

