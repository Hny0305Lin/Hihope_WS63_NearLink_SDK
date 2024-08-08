/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of alg_autorate_rom.h
 */

#ifndef ALG_AUTORATE_BASIC_ROM_H
#define ALG_AUTORATE_BASIC_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "alg_autorate_if_rom.h"
#include "alg_autorate_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_AUTORATE
/*****************************************************************************
  1 全局变量声明
*****************************************************************************/
/* 算法内部模块使用 */
extern wlan_he_ltf_type_enum_uint8 g_ltf_and_gi[OAL_BUTT][WLAN_HE_GI_TYPE_BUTT];
extern alg_autorate_dev_stru g_ast_autorate_dev_info;  /* !!!从HOST传过来的参数，dev不允许修改赋值 */
/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* AUTORATE DEBUG INFO LOG */
#define alg_ar_dbg_log0(vap_id, fmt)             \
    do {                                                    \
        if (g_ast_autorate_dev_info.enable_ar_log == OSAL_TRUE) {  \
            oam_warning_log0(vap_id, OAM_SF_AUTORATE, fmt); \
        }                                                   \
    } while (0)

#define alg_ar_dbg_log1(vap_id, fmt, p1)             \
    do {                                                        \
        if (g_ast_autorate_dev_info.enable_ar_log == OSAL_TRUE) {      \
            oam_warning_log1(vap_id, OAM_SF_AUTORATE, fmt, p1); \
        }                                                       \
    } while (0)

#define alg_ar_dbg_log2(vap_id, fmt, p1, p2)             \
    do {                                                            \
        if (g_ast_autorate_dev_info.enable_ar_log == OSAL_TRUE) {          \
            oam_warning_log2(vap_id, OAM_SF_AUTORATE, fmt, p1, p2); \
        }                                                           \
    } while (0)

#define alg_ar_dbg_log3(vap_id, fmt, p1, p2, p3)             \
    do {                                                                \
        if (g_ast_autorate_dev_info.enable_ar_log == OSAL_TRUE) {              \
            oam_warning_log3(vap_id, OAM_SF_AUTORATE, fmt, p1, p2, p3); \
        }                                                               \
    } while (0)

#define alg_ar_dbg_log4(vap_id, fmt, p1, p2, p3, p4)             \
    do {                                                                    \
        if (g_ast_autorate_dev_info.enable_ar_log == OSAL_TRUE) {                  \
            oam_warning_log4(vap_id, OAM_SF_AUTORATE, fmt, p1, p2, p3, p4); \
        }                                                                   \
    } while (0)

#define ALG_AR_GLA_PROTOCOL_SHIFT           8
#define ALG_AR_GLA_BW_SHIFT                 4
/*****************************************************************************
  1 函数声明
*****************************************************************************/
void alg_ar_probe_finish_update_info(alg_autorate_user_stru *user_info, oal_bool_enum_uint8 update_bestrate);
osal_u32 alg_ar_probe_finish_process(alg_autorate_user_stru *user_info);
alg_autorate_dev_stru *alg_ar_get_device_rate_info(void);
alg_autorate_vap_stru *alg_ar_get_vap_rate_info(osal_u8 vap_id);
alg_autorate_vap_stru *alg_ar_get_vap_rate_info_by_dmac_vap(const dmac_vap_stru *dmac_vap);
alg_autorate_user_stru *alg_ar_get_user_rate_info(dmac_user_stru *dmac_user);
oal_bool_enum_uint8 alg_autorate_is_enable(osal_void);
osal_u8 alg_ar_get_hal_rate_by_wlan_11b_rate(wlan_11b_rate_index_enum_uint8 rate_index);
osal_u8 alg_ar_get_hal_rate_by_wlan_legacy_rate(wlan_legacy_ofdm_mcs_enum_uint8 rate_index);
osal_u8 alg_ar_get_wlan_legacy_rate_by_hal_rate(osal_u8 tx_param_mcs);
osal_u8 alg_autorate_get_hal_nss_rate(wlan_protocol_enum_uint8 protocol, wlan_nss_enum_uint8 nss,
    osal_u8 rate_index);
wlan_phy_protocol_enum_uint8 alg_wlan_protocol_to_su_phy_protocol_mode(wlan_protocol_enum_uint8 en_wlan_pro);
wlan_phy_protocol_enum_uint8 alg_ar_get_phy_protocol_by_rate_info(alg_autorate_user_stru *user_info,
    alg_autorate_basic_rate_info_stru *rate_info, osal_u8 rate_index, wlan_gi_type_enum_uint8 gi_type);
wlan_protocol_enum_uint8 alg_wlan_phy_protocol_mode_to_wlan_protocol(
    wlan_phy_protocol_enum_uint8 phy_protocol);
wlan_nss_enum_uint8 alg_ar_get_ht_nss_by_mcs_index(osal_u8 rate_index);
osal_u32 alg_autorate_tx_param_nss_mcs_analysis(osal_u8 nss_rate, wlan_phy_protocol_enum protocol_mode,
    wlan_nss_enum_uint8 *nss, osal_u8 *rate_index);
const alg_rate_table_stru *alg_rate_table_obtain(wlan_protocol_enum_uint8 en_protocol,
    alg_ar_bw_cap_enum_uint8 en_bw, wlan_nss_enum_uint8 en_nss);
void alg_ar_get_rate_kbps(alg_autorate_basic_rate_info_stru *rate_info, osal_u8 rate_index,
    const alg_rate_table_stru *rate_item, oal_bool_enum_uint8 dcm, osal_u32 *rate_kbps);
osal_u32 alg_ar_rate_stat_calc_goodput(const alg_rate_table_stru *rate_item,
    alg_autorate_basic_rate_info_stru *rate_info, osal_u8 rate_index, osal_u16 aggr_subfrm, osal_u32 *goodput);
osal_u32 alg_ar_get_theory_goodput(alg_autorate_basic_rate_info_stru *rate_info, osal_u8 rate_index,
    const alg_rate_table_stru *rate_item, osal_u32 *theory_goodput);
oal_bool_enum_uint8 alg_ar_is_legacy_protocol(wlan_protocol_enum_uint8 protocol);
osal_bool alg_get_vap_user_support_ldpc(const dmac_vap_stru *vap, const dmac_user_stru *user);
osal_bool alg_get_user_cur_protocol_bw_support_short_gi(alg_autorate_user_stru *user_info,
    wlan_protocol_enum_uint8  cur_protocol, alg_ar_bw_cap_enum_uint8  cur_bandwidth);
void alg_autorate_notify_best_rate_change(alg_autorate_user_stru *user_info);
osal_u8 alg_ar_get_highest_bit_one(osal_u16 bitmap);
osal_u8 alg_ar_get_lowest_bit_one(osal_u16 bitmap);
osal_u8 alg_ar_get_max_rate_avail(alg_autorate_user_stru *user_info, wlan_protocol_enum_uint8 protocol,
    wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw);
osal_u8 alg_ar_get_min_rate_avail(alg_autorate_user_stru *user_info, wlan_protocol_enum_uint8 protocol,
    wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw);
oal_bool_enum_uint8 alg_ar_rate_index_available(alg_autorate_user_stru *user_info,
    wlan_protocol_enum_uint8 protocol, wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw, osal_u8 rate_index);
oal_bool_enum_uint8 alg_ar_rate_set_index_available(alg_autorate_user_stru *user_info,
    wlan_protocol_enum_uint8 protocol, wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw, osal_u8 rate_index);
osal_u8 alg_ar_get_bigger_rate_index(alg_autorate_user_stru *user_info, wlan_protocol_enum_uint8 protocol,
    wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw, osal_u8 base_rate_index);
osal_u8 alg_ar_get_smaller_rate_index(alg_autorate_user_stru *user_info, wlan_protocol_enum_uint8 protocol,
    wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw, osal_u8 base_rate_index);
osal_u32 alg_ar_set_schedule_fix_rate_info(dmac_user_stru *user, alg_autorate_user_stru *user_info, osal_u8 tid_id);
osal_u32 alg_ar_set_schedule_rate_info(dmac_user_stru *user, alg_autorate_user_stru *user_info, osal_u8 tid_id);
osal_u32 alg_autorate_add_vap(dmac_vap_stru *dmac_vap);
osal_u32 alg_autorate_start_vap(dmac_vap_stru *dmac_vap);
osal_u32 alg_autorate_exit_vap(dmac_vap_stru *dmac_vap);
osal_void alg_autorate_dev_para_init(osal_void);
void alg_ar_init_user_basic_info(alg_autorate_user_stru *user_info,
    alg_autorate_dev_stru *dev_autorate, dmac_user_stru *user, dmac_vap_stru *vap);
void alg_ar_init_user_legacy_rate_set_capability(alg_autorate_user_stru *user_info);
void alg_ar_init_user_ht_rate_set_capability(alg_autorate_user_stru *user_info);
void alg_ar_init_user_vht_rate_set_capability(alg_autorate_user_stru *user_info);
void alg_ar_init_user_he_rate_set_capability(alg_autorate_user_stru *user_info);
void alg_ar_init_user_rate_set_capability(alg_autorate_user_stru *user_info);
osal_u8 alg_ar_init_user_get_start_rate_index_by_protocol(alg_autorate_user_stru *user_info,
    wlan_protocol_enum_uint8 protocol, wlan_nss_enum_uint8 nss, alg_ar_bw_cap_enum_uint8 bw);
osal_u8 alg_ar_init_user_get_start_rate_index(alg_autorate_user_stru *user_info);
void alg_ar_init_user_probe_info(alg_autorate_user_stru *user_info);
void alg_ar_fill_retry_rank_with_er_base_info(alg_autorate_tx_rate_rank_stru *each_stable_rank,
    alg_autorate_user_stru *user_rate_info);
void alg_ar_fill_retry_rank_with_er_dcm_rate(osal_u8 rank_index,
    alg_autorate_tx_rate_rank_stru *stable_rank, alg_autorate_user_stru *user_rate_info);
void alg_ar_get_user_tx_rate_rank_info(
    alg_autorate_user_stru *user_info, osal_u8 max_rank_index, alg_autorate_tx_rate_rank_stru *tx_rate_rank);
osal_u32 alg_ar_init_user_rate_stat_info(alg_autorate_user_stru *user_info, osal_bool is_same_stat);
osal_u32 alg_ar_init_user_best_rate_info(alg_autorate_user_stru *user_info,
    alg_autorate_basic_rate_info_stru *rate_info);
osal_u32 alg_ar_init_user_rate_info(alg_autorate_user_stru *user_info,
    alg_autorate_basic_rate_info_stru *rate_info);
osal_u32 alg_ar_init_user_rate(alg_autorate_user_stru *user_info);
osal_u32 alg_autorate_init_user(dmac_vap_stru *vap, dmac_user_stru *user);
osal_u32 alg_autorate_exit_user(dmac_vap_stru *vap, dmac_user_stru *user);
osal_void alg_autorate_change_protocol_mode(dmac_user_stru *user, wlan_protocol_enum_uint8 new_protocol);
void alg_ar_init_bcast_data_tx_param(dmac_vap_stru *dmac_vap, alg_autorate_tx_fix_rate_rank_stru *tx_param);
void alg_ar_init_ucast_mgmt_tx_param(dmac_vap_stru *dmac_vap, alg_autorate_tx_fix_rate_rank_stru *tx_param);
void alg_ar_init_bcast_mgmt_tx_param(dmac_vap_stru *dmac_vap, alg_autorate_tx_fix_rate_rank_stru *tx_param);
osal_u32 alg_autorate_get_tx_best_rate_fix_mode(alg_autorate_dev_stru *device_info,
    alg_autorate_basic_rate_info_stru *basic_rate_info);
#endif /* #ifdef _PRE_WLAN_FEATURE_AUTORATE */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_autorate_rom.h */
