/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: traffic control schedule algorithm
 */

#ifndef ALG_TX_TB_ROM_H
#define ALG_TX_TB_ROM_H

/*****************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "alg_main_rom.h"
#include "hal_device_rom.h"
#include "dmac_alg_if_part_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ALG_TX_TB_AGGR_INVALID 0xFF
#define ALG_TX_TB_AGGR_MAX 12
#define ALG_TX_TB_UP_VAP_CNT 2
#define ALG_TX_TB_IN_AGGR_BYTES_TH 200
#define ALG_SCH_HE_PHY_HEAD_M_HE_TIME_US 40 /* L-STF(8) + L-LTF(8) + L-SIG(4) + RL-SIG(4) + HE-SIG-A(8) + HE-STF(8) */
#define alg_tx_tb_phy_gi_to_wlan_gi(phy_gi) \
    ((phy_gi) == WLAN_PHY_HE_GI_3DOT2US ? WLAN_GI_LONG : \
    (phy_gi) == WLAN_PHY_HE_GI_DOT8US ? WLAN_GI_SHORT : \
    (phy_gi) == WLAN_PHY_HE_GI_1DOT6US ? WLAN_GI_MID : WLAN_HE_GI_TYPE_BUTT)
typedef enum {
    ALG_TX_DEFAULT  = 0, /* 默认状态 */
    ALG_TX_TB_PRE = 1, /* tx tb前 */
    ALG_TX_TB = 2, /* tx tb状态 */
    ALG_TX_TB_STATE_BUTT
} alg_tx_tb_state_enum;
typedef struct {
    alg_param_sync_tx_tb_stru *sync_para;
    osal_u8 tx_tb_state : 7;
    osal_u8 aggr_con_flag : 1;
    osal_u8 vap_idx;
    osal_u16 he_trig_count;
    frw_timeout_stru tx_tb_timer;
    osal_u32 *rom;
} alg_tx_tb_info_stru;
#define alg_tx_tb_print(...)
osal_void alg_tx_tb_init(osal_void);
osal_void alg_tx_tb_exit(osal_void);
osal_u32 alg_tx_tb_param_sync(const alg_param_sync_stru *sync);
osal_u8 alg_tx_tb_compute_tid_aggr_num(osal_u8 vap_id, const dmac_tid_stru *txtid);
alg_tx_tb_info_stru* alg_tx_tb_get_params(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif