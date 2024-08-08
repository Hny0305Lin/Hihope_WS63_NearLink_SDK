/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: HAL Layer Specifications.
 */

#ifndef HAL_COMMON_OPS_ROM_H
#define HAL_COMMON_OPS_ROM_H

/*****************************************************************************
    1 头文件包含
*****************************************************************************/
#include "oal_types_device_rom.h"
#include "wlan_spec_rom.h"
#include "wlan_types_rom.h"
#include "frw_ext_if_rom.h"
#include "wlan_msg_rom.h"
#include "hal_commom_ops_type_rom.h"
#include "hal_common_ops_device_rom.h"
#include "hal_ops_common_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 支持最多32个配置点 */
enum {
    DCACHE_FLUSH_TX = 0,            // 数据发送设置描述符
    DCACHE_FLUSH_TX_ISR,            // TX完成中断处理
    DCACHE_FLUSH_TX_ISR_MSG,        // TX完成中断消息处理
    DCACHE_FLUSH_RX_ALLOC_DSCR,     // RX分配描述符
    DCACHE_FLUSH_ADD_DSCR,          // 添加描述符
    DCACHE_FLUSH_DRX_ISR,           // RX数据帧中断
    DCACHE_FLUSH_CRX_ISR,           // RX管理帧中断
    DCACHE_FLUSH_RX_ISR_MSG,        // RX中断消息处理
    DCACHE_FLUSH_BEACON,            // BEACON帧发送
    DCACHE_FLUSH_PROTECT,           // 保护帧NULL data帧发送
    DCACHE_FLUSH_COEX,              // 蓝牙共存帧
    DCACHE_FLUSH_QUEUE_CLEAR,       // clear硬件队列
    DCACHE_FLUSH_QUEUE_FLUSH,       // FLUSH硬件队列
    DCACHE_FLUSH_RECYCLE,           // 流控算法回收描述符
    DCACHE_FLUSH_BUTT
};

/*****************************************************************************
  7.2 基准接收描述符定义
*****************************************************************************/
#pragma pack(push, 1)
typedef struct {
    osal_u8 rts_count  : 3; /* RTS的实际发送次数 */
    osal_u8 data_count : 3; /* DATA的实际发送次数 */
    osal_u8 resv : 2;
} hal_rank_alg_result_stru;
#pragma pack(pop)

typedef struct {
    osal_u8                            data_send_cnt;
    osal_u8                            rts_succ;
    osal_u8                            cts_failure;
    osal_s8                            last_ack_rssi;
    osal_u8                            mpdu_num;
    osal_u8                            error_mpdu_num;
    osal_u8                            last_rate_rank;
    osal_u8                            tid;
    hal_tx_queue_type_enum_uint8       ac;
    osal_u16                           mpdu_len;
    osal_u8                            is_retried;
    osal_u8                            bandwidth;
    osal_u8                            sounding_mode;           /* 表示该帧sounding类型 */
    osal_u8                            status;                  /* 该帧的发送结果 */
    osal_u8                            ampdu_enable;            /* 表示该帧是否为AMPDU聚合帧 */
    osal_u16                           origin_mpdu_lenth;       /* mpdu长度 */
    osal_u8                            last_tb_flag;            /* tb发送的标记 */

    osal_u64                           ampdu_result;
    hal_channel_matrix_dsc_stru        tx_dsc_chnl_matrix;      /* 发送描述符中的信道矩阵信息 */
    hal_tx_ctrl_desc_rate_stru         per_rate[HAL_TX_RATE_MAX_NUM];
    osal_u32                           ampdu_length;
    hal_tx_txop_tpc_stru               tpc;

    osal_u32                           now_time_ms;
    osal_u8                            normal_pkt_num;
    osal_u8                            first_succ_cnt;    /* 第一次发送速率级别成功的帧数 */
    osal_u8                            first_fail_cnt;    /* 第一次发送速率级别失败的帧数 */
    hal_rank_alg_result_stru           rank_result[HAL_TX_RATE_MAX_NUM];
} hal_tx_dscr_ctrl_one_param;

/*****************************************************************************
    10.2 对外暴露的配置接口
*****************************************************************************/
osal_void hal_set_sifs_resp_rate(oal_bool_enum_uint8 rate_restrict);
osal_void hh503_tx_init_dscr_queue(hal_to_dmac_device_stru *device);
osal_void hal_tx_ctrl_dscr_link(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr_prev,
    hal_tx_dscr_stru *tx_dscr);


osal_void hal_get_tx_dscr_next(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr,
    hal_tx_dscr_stru **tx_dscr_next);
osal_void hal_tx_ctrl_dscr_unlink(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_u16 hal_tx_seqnum_get_dscr(hal_tx_dscr_stru *tx_dscr);
osal_u8 hal_tx_get_dscr_seqno_sw_generate(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_void hal_tx_data_set_tsf(hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr, osal_u16 tsf);

osal_void hal_tx_set_dscr_seqno_sw_generate(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr,
    osal_u8 sw_seqno_generate);
osal_void hal_tx_set_dscr_seq_ctl_hw_bypass(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr,
    osal_u8 seq_ctl_hw_bypass);
osal_void hal_tx_set_dscr_tx_pn_hw_bypass(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr,
    osal_u8 tx_pn_hw_bypass);
osal_u8 hal_tx_get_dscr_seq_ctl_hw_bypass(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_u8 hal_tx_get_dscr_tx_pn_hw_bypass(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_u8 hal_tx_get_dscr_mac_head_len(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);

osal_void hal_tx_get_dscr_ctrl_one_param(const hal_to_dmac_device_stru *hal_device, const hal_tx_dscr_stru *tx_dscr,
    hal_tx_dscr_ctrl_one_param *tx_dscr_one_param);
osal_void hal_tx_retry_clear_dscr(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI
osal_void hal_tx_set_pdet_en(const hal_to_dmac_device_stru *hal_device,
    hal_tx_dscr_stru *tx_dscr, oal_bool_enum_uint8 pdet_en_flag);
osal_void hal_dyn_cali_tx_pow_ch_set(hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_void hal_dyn_cali_tx_pa_ppa_swtich(oal_bool_enum_uint8 ppa_working);
osal_void hal_dyn_cali_al_tx_config_amend(hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
#endif
osal_void hal_tx_get_dscr_chiper_type(hal_tx_dscr_stru *tx_dscr, osal_u8 *chiper_type, osal_u8 *chiper_key_id);

osal_void hal_tx_put_dscr(hal_tx_queue_type_enum_uint8 tx_queue_type, hal_tx_dscr_stru *tx_dscr);
osal_u32  hal_get_tx_q_status(osal_u8 qnum);
osal_u32  hal_get_tx_q_status_empty(osal_u8 qnum);
osal_void hal_tx_ctrl_dscr_ampdu_info(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr,
    hal_tx_ppdu_feature_stru *ppdu_feature);
osal_u32  hal_get_tx_multi_q_status(osal_void);
osal_void hal_rx_get_info_dscr(hal_to_dmac_device_stru *hal_device,
    osal_u32 *rx_dscr, hal_rx_ctl_stru *rx_ctl, hal_rx_status_stru *rx_status, hal_rx_statistic_stru *rx_statistics);
osal_void hal_rx_record_frame_status_info(hal_to_dmac_device_stru *hal_device,
    osal_u32 *rx_dscr, hal_rx_dscr_queue_id_enum_uint8 queue_id);

#ifdef _PRE_WLAN_RF_AUTOCALI
osal_void hh503_rf_cali_auto_switch(osal_u32 switch_mask);
osal_void hh503_rf_cali_auto_mea_done(osal_u8 freq, osal_u8 chn_idx, osal_u8 cali_type, osal_u8 cali_state);
#endif

osal_void hh503_set_sr_vfs_clear(osal_void);
osal_void hh503_set_psr_statistics_clear(osal_void);
osal_void hh503_set_sr_statistics_clear(osal_void);
osal_void hh503_set_psr_ctrl(oal_bool_enum_uint8 flag);
osal_void hh503_set_psrt_htc(osal_void);
osal_void hh503_set_psr_offset(osal_s32 offset);
osal_void hh503_set_bss_color_enable(oal_bool_enum_uint8 flag);
osal_void hh503_set_srg_pd_etc(osal_u32 pd, osal_u32 pw);
osal_void hh503_set_non_pd(osal_u32 pd, osal_u32 pw);
osal_void hh503_set_srg_ctrl(osal_void);
osal_void hh503_set_srg_partial_bssid_bitmap(osal_u32 bitmap_low, osal_u32 bitmap_high);
osal_void hh503_set_srg_bss_color_bitmap(osal_u32 bitmap_low, osal_u32 bitmap_high);
osal_void hh503_set_srg_ele_off(osal_void);
osal_void hh503_set_sr_ctrl(oal_bool_enum_uint8 flag);
osal_void hh503_color_rpt_clr(osal_void);
osal_void hh503_color_rpt_en(oal_bool_enum_uint8 flag);
osal_void hh503_color_area_get(mac_color_area_stru *color_area);

osal_void hh503_set_dev_support_11ax(hal_to_dmac_device_stru *hal_device, osal_u8 reg_value);
osal_void hh503_set_mac_backoff_delay(osal_u32 back_off_time);
osal_void hh503_set_mu_edca_lifetime(osal_u8 bk, osal_u8 be, osal_u8 vi, osal_u8 vo);
osal_void hh503_set_mu_edca_aifsn(osal_u8 bk, osal_u8 be, osal_u8 vi, osal_u8 vo);
osal_void hh503_set_mu_edca_func_en(osal_u8 bk, osal_u8 be, osal_u8 vi, osal_u8 vo);
osal_void hh503_close_mu_edca_func(osal_void);
osal_void hh503_set_affected_acs(osal_u8 affected_acs);
osal_void hh503_set_mu_edca_cw(osal_u8 ac_type, osal_u8 cwmax, osal_u8 cwmin);
osal_void hh503_set_bss_color(const hal_to_dmac_vap_stru *hal_vap, osal_u8 bss_color);
osal_void hh503_set_partial_bss_color(const hal_to_dmac_vap_stru *hal_vap, osal_u8 partial_bss_color);
osal_void hh503_set_phy_aid(osal_u16 aid);
osal_void hh503_set_queue_size_in_ac(osal_u32 queue_size, osal_u8 vap_id, osal_u16 ac_order);
osal_void hh503_set_ndp_status(osal_u8 status);

osal_void hh503_vap_set_twt(const hal_to_dmac_vap_stru *hal_vap, const twt_reg_param_stru *twt_param,
    osal_u8 count, osal_u8 trigger);
osal_void hh503_cali_twt(const hal_to_dmac_vap_stru *hal_vap);
osal_void hal_vap_set_multi_bssid(osal_u8 *trans_bssid, osal_u8 maxbssid_indicator);
osal_void hh503_set_he_rom_en(oal_bool_enum_uint8 rx_om);

osal_void hh503_set_machw_rx_buff_addr(osal_u32 rx_dscr, hal_rx_dscr_queue_id_enum_uint8 queue_num);
osal_u8   hal_set_machw_rx_buff_addr_sync(hal_to_dmac_device_stru *hal_device,
    osal_u32 *rx_dscr, hal_rx_dscr_queue_id_enum_uint8 queue_num);
osal_void  hal_rx_add_dscr(hal_to_dmac_device_stru *hal_device, hal_rx_dscr_queue_id_enum_uint8 queue_num);
osal_void hal_rx_pre_add_dscr(hal_to_dmac_device_stru *hal_device, hal_rx_dscr_queue_id_enum_uint8 queue_num,
    osal_u16 add_num);
osal_void hh503_rx_update_dscr(hal_to_dmac_device_stru *hal_device, hal_rx_dscr_queue_id_enum_uint8 queue_num);
osal_void hal_rx_recycling_dscr(hal_to_dmac_device_stru *hal_device, hal_rx_dscr_queue_id_enum_uint8 queue_num,
    osal_u16 dscr_thr);
oal_bool_enum_uint8 hal_is_machw_enabled(osal_void);
osal_void hh503_set_machw_tx_suspend(osal_void);
osal_void hal_set_machw_tx_resume(osal_void);
osal_void hh503_reset_phy_machw(hal_reset_hw_type_enum_uint8 type, osal_u8 sub_mod, osal_u8 reset_phy_reg,
    osal_u8 reset_mac_reg);
osal_void hal_initialize_machw(osal_void);
osal_u8 hal_set_machw_phy_adc_freq(hal_to_dmac_device_stru *hal_device,
    wlan_channel_bandwidth_enum_uint8 bandwidth);
osal_void hal_set_rx_multi_ant(const hal_to_dmac_device_stru *hal_device, osal_u8 phy_chain);

osal_void hh503_set_tx_sequence_num(machw_tx_sequence_stru tx_seq, osal_u32 val_write, osal_u8 vap_index);
osal_void hh503_get_tx_seq_num(osal_u8 lut_index, osal_u8 tid, osal_u8 qos_flag, osal_u8 vap_index, osal_u16 *val_read);
#ifdef _PRE_WLAN_FEATURE_AMPDU_TX_HW
osal_void hh503_save_tx_ba_para(hal_ba_para_stru *ba_para);
osal_void hh503_get_tx_ba_para(hal_ba_para_stru *ba_para);
#endif

osal_void hh503_disable_machw_ack_trans(osal_void);
osal_void hh503_enable_machw_ack_trans(osal_void);
osal_void hal_disable_machw_cts_trans(osal_void);
osal_void hal_enable_machw_cts_trans(osal_void);
osal_void hal_initialize_phy(osal_void);
osal_void hh503_initialize_rf_sys(hal_to_dmac_device_stru *hal_device);
osal_void hh503_fpga_rf_config(osal_void);
osal_void hh503_initialize_rf_sys_fpga(hal_to_dmac_device_stru *hal_device);
osal_void hal_pow_sw_initialize_tx_power(hal_to_dmac_device_stru *hal_device);
osal_void hh503_pow_initialize_tx_power(hal_to_dmac_device_stru *hal_device);
osal_void hh503_pow_set_rf_regctl_enable(hal_to_dmac_device_stru *hal_device, oal_bool_enum_uint8 rf_linectl);
osal_void hal_pow_set_resp_frame_tx_power(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 band, osal_u8 chan_idx);
osal_void hh503_tb_mcs_tx_power(wlan_channel_band_enum_uint8 band);
osal_void hh503_tb_tx_power_init(wlan_channel_band_enum_uint8 band);

osal_void hh503_cali_send_func(hal_to_dmac_device_stru *hal_device,
    osal_u8 *cali_data, osal_u16 frame_len, osal_u16 remain);
osal_u32  hh503_get_mac_int_status(osal_void);
osal_void hh503_clear_mac_int_status(osal_u32 status);
osal_void hh503_get_mac_error_int_status(hal_error_state_stru *state);
osal_void hh503_unmask_mac_init_status(osal_u32 status);
osal_void hh503_show_irq_info(hal_to_dmac_device_stru *hal_device, osal_u8 param);
osal_void hh503_clear_irq_stat(hal_to_dmac_device_stru *hal_device_base);

osal_void hh503_config_eifs_time(wlan_protocol_enum_uint8 protocol);
osal_void hal_unregister_alg_isr_hook(hal_to_dmac_device_stru *hal_device, hal_isr_type_enum_uint8 isr_type,
    hal_alg_noify_enum_uint8 alg_notify);
osal_void hh503_one_packet_get_status(hal_one_packet_status_stru *status);
osal_void hh503_reg_info(hal_to_dmac_device_stru *hal_device, osal_u32 addr, osal_u32 *val);
osal_void hh503_reg_info16(hal_to_dmac_device_stru *hal_device, osal_u32 addr, osal_u16 *val);

#ifdef _PRE_WLAN_ONLINE_DPD
osal_void hal_dpd_cfr_set_tpc(hal_to_dmac_device_stru *hal_device, osal_u8 tpc);
osal_void hal_dpd_cfr_set_bw(hal_to_dmac_device_stru *hal_device, osal_u8 bw);
osal_void hh503_dpd_cfr_set_mcs(hal_to_dmac_device_stru *hal_device, osal_u8 mcs);
osal_void hal_dpd_cfr_set_freq(hal_to_dmac_device_stru *hal_device, osal_u8 freq);
#endif
osal_u32  hal_get_all_tx_q_status(osal_void);
osal_void hal_dmac_ce_del_key(const hal_security_key_stru *security_key);
osal_void hh503_ce_del_peer_macaddr(osal_u8 lut_idx);
osal_void hal_get_ch_measurement_result_ram(hal_ch_statics_irq_event_stru *ch_statics);
osal_void hal_get_ch_measurement_result(hal_ch_statics_irq_event_stru *ch_statics);
osal_void  hh503_get_spec_frm_rate(hal_to_dmac_device_stru *hal_device);
osal_void hh503_set_spec_frm_phy_tx_mode(const hal_to_dmac_device_stru *hal_device,
    osal_u8 band, osal_u8 subband_idx);
osal_void hal_set_resp_pow_level(osal_s8 near_distance_rssi, osal_s8 far_distance_rssi);
osal_void hh503_rf_regctl_enable_set_regs(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 freq_band, osal_u8 cur_ch_num, wlan_channel_bandwidth_enum_uint8 bandwidth);

#ifdef _PRE_WLAN_FEATURE_TXBF
osal_void hal_set_sta_membership_status_63_32(osal_u32 value);
osal_void hal_set_sta_membership_status_31_0(osal_u32 value);
osal_void hal_set_sta_user_p_63_48(osal_u32 value);
osal_void hal_set_sta_user_p_47_32(osal_u32 value);
osal_void hal_set_sta_user_p_31_16(osal_u32 value);
osal_void hal_set_sta_user_p_15_0(osal_u32 value);
#endif
/* phy debug信息中trailer信息上报设置寄存器 */
osal_void hh503_rf_tone_transmit_entrance(const hal_to_dmac_device_stru *hal_device, osal_u16 data_len,
    osal_u8 chain_idx);
osal_void hh503_rf_tone_transmit_exit(const hal_to_dmac_device_stru *hal_device);

osal_void hh503_set_extlna_chg_cfg(hal_to_dmac_device_stru *hal_device, oal_bool_enum_uint8 extlna_chg_bypass);
#ifdef _PRE_WLAN_FEATURE_INTF_DET
osal_void hh503_get_txrx_frame_time(hal_ch_mac_statics_stru *ch_statics);
#endif
osal_void hh503_set_80m_resp_mode(hal_to_dmac_device_stru *hal_device, osal_u8 debug_en);
osal_void hh503_get_mac_statistics_data(hal_mac_key_statis_info_stru *mac_key_statis);
osal_void hh503_get_phy_statistics_data(hal_mac_key_statis_info_stru *mac_key_statis);
osal_void hh503_get_mac_rx_statistics_data(hal_mac_rx_mpdu_statis_info_stru *mac_rx_statis);
osal_void hh503_get_mac_tx_statistics_data(hal_mac_tx_mpdu_statis_info_stru *mac_tx_statis);
osal_void hh503_hw_stat_clear(osal_void);
osal_void hh503_set_ddc_en(osal_u32 reg_value);
oal_bool_enum_uint8 hh503_get_dyn_bypass_extlna_pm_flag(const hal_to_dmac_device_stru *hal_device);
#ifdef _PRE_WLAN_FEATURE_CCA_OPT
osal_void hh503_set_ed_high_th(osal_s8 ed_high_20_reg_val, osal_s8 ed_high_40_reg_val,
    oal_bool_enum_uint8 is_default_th);
#endif
osal_void hh503_set_psm_listen_interval(const hal_to_dmac_vap_stru *hal_vap, osal_u16 interval);
osal_void hh503_set_psm_listen_interval_count(hal_to_dmac_vap_stru *hal_vap, osal_u16 interval_count);
osal_void hh503_set_tbtt_offset(const hal_to_dmac_vap_stru *hal_vap, osal_u16 offset);
#ifdef _PRE_WLAN_FEATURE_POWERSAVE
osal_void hh503_set_psm_ext_tbtt_offset(const hal_to_dmac_vap_stru *hal_vap, osal_u16 offset);
#endif
osal_void hh503_set_psm_beacon_period(const hal_to_dmac_vap_stru *hal_vap, osal_u32 beacon_period);
osal_void hh503_set_beacon_timeout(osal_u16 value);
osal_void hh503_set_sta_dtim_period(const hal_to_dmac_vap_stru *hal_vap, osal_u32 dtim_period);
osal_u8 hh503_get_psm_dtim_count(const hal_to_dmac_vap_stru *hal_vap);
osal_void hh503_set_psm_dtim_count(hal_to_dmac_vap_stru *hal_vap, osal_u8 dtim_count);
#ifdef _PRE_WLAN_FEATURE_MAC_PARSE_TIM
osal_void hh503_mac_set_bcn_tim_pos(const hal_to_dmac_vap_stru *hal_vap, osal_u16 pos);
osal_void hh503_pm_set_mac_parse_tim(osal_u8 flag);
#endif
osal_void hh503_enable_machw_edca(osal_void);
osal_void hal_get_tx_dscr_field(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_void hh503_set_phy_max_bw_field(hal_to_dmac_device_stru *hal_device, osal_u32 data,
    hal_phy_max_bw_sect_enmu_uint8 sect);
osal_void hh503_rf_test_disable_al_tx(hal_to_dmac_device_stru *hal_device);
osal_void hh503_rf_test_enable_al_tx(hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);
osal_void hal_set_rx_filter_reg(osal_u32 rx_filter_command);
osal_void hh503_clear_mac_error_int_status(const hal_error_state_stru *status);
#define HAL_VAP_LEVEL_FUNC_EXTERN
osal_void hh503_vap_set_machw_prot_params(hal_to_dmac_vap_stru *hal_vap,
    hal_tx_phy_mode_one_stru *phy_tx_mode, hal_tx_ctrl_desc_rate_stru *data_rate);

osal_void hh503_vap_set_macaddr(const hal_to_dmac_vap_stru *hal_vap, const osal_u8 *mac_addr, osal_u16 mac_addr_len);
osal_void hal_sync_update_tsf(osal_u8 vap_id, osal_bool enable);
osal_void hh503_tx_enable_peer_sta_ps_ctrl(const osal_u8 lut_index);
osal_void hh503_tx_disable_peer_sta_ps_ctrl(const osal_u8 lut_index);
osal_void hh503_tx_enable_resp_ps_bit_ctrl(osal_u8 lut_index);
osal_void hh503_tx_disable_resp_ps_bit_ctrl(osal_u8 lut_index);
osal_void hh503_tx_enable_resp_ps_bit_ctrl_all(osal_void);
osal_void hh503_tx_disable_resp_ps_bit_ctrl_all(osal_void);
osal_void hh503_pm_wlan_get_service_id(hal_to_dmac_vap_stru  *hal_vap, osal_u8 *service_id);

osal_u32 hh503_rf_get_pll_div_idx(wlan_channel_band_enum_uint8 band, osal_u8  channel_idx,
    wlan_channel_bandwidth_enum_uint8 bandwidth, osal_u8  *pll_div_idx);
osal_void hh503_get_cali_info(hal_to_dmac_device_stru *hal_device, osal_u8 *param);

#ifdef _PRE_WLAN_FEATURE_FTM
osal_u64  hh503_get_ftm_tod(osal_void);
osal_u64  hh503_get_ftm_toa(osal_void);
osal_u64  hh503_get_ftm_t2(osal_void);
osal_u64  hh503_get_ftm_t3(osal_void);
osal_void hh503_get_ftm_ctrl_status(osal_u32 *ftm_status);
osal_void hh503_get_ftm_config_status(osal_u32 *ftm_status);
osal_void hh503_set_ftm_config_status(osal_u32 ftm_status);
osal_u8   hh503_get_ftm_dialog(osal_void);
osal_void hh503_set_ftm_pulse_enable_status(osal_u32 ftm_status);
#endif

#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI
osal_void hal_rf_init_dyn_cali_reg_conf(hal_to_dmac_device_stru *hal_device);
osal_u32  hh503_get_tx_pdet_by_pow(hal_to_dmac_device_stru * const hal_device,
    hal_pdet_info_stru * const pdet_info, hal_dyn_cali_usr_record_stru * const user_pow, osal_s16 *exp_pdet);
osal_void hh503_rf_dyn_cali_enable(hal_to_dmac_device_stru *device);
osal_void hh503_rf_dyn_cali_disable(hal_to_dmac_device_stru *device);
osal_void hh503_config_set_dyn_cali_dscr_interval(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 band, osal_u16 param_val);
osal_void hh503_rf_cali_realtime_entrance(hal_to_dmac_device_stru * const hal_device,
    hal_pdet_info_stru * const pdet_info,
    hal_dyn_cali_usr_record_stru * const user_pow,  hal_tx_dscr_stru * const base_dscr);
osal_void hh503_init_dyn_cali_tx_pow(hal_to_dmac_device_stru *hal_device);
#endif

osal_void hal_flush_rx_complete_irq(hal_to_dmac_device_stru *hal_dev);
oal_bool_enum_uint8 hh503_check_mac_int_status(osal_void);
osal_void hal_device_init_vap_pow_code(hal_to_dmac_device_stru *hal_device, hal_vap_pow_info_stru *vap_pow_info,
    hal_pow_set_type_enum_uint8 type, const mac_channel_stru *channel);
osal_void hal_set_hw_en_reg_cfg(osal_u8 hw_en);

osal_void hh503_adjust_tx_power(osal_u8 ch, osal_s8 power);
osal_void hh503_restore_tx_power(osal_u8 ch);

osal_void hh503_cfg_txop_cycle(osal_u16 txop_limit);
osal_void hh503_cfg_lifetime(osal_u8 ac, osal_u16 lifetime);
osal_void hh503_bl_sifs_en(osal_u8 sifs_en);
osal_void hh503_rts_info(osal_u32 rate, osal_u32 phy_mode, osal_u8* addr1);
osal_void hh503_bl_cfg_txop_en(osal_u8 txop_en);

osal_void hh503_cfg_tx_rts_mac_cnt(osal_u8 max_cnt);
osal_void hal_rf_set_trx_type(hal_rf_trx_type_enum trx_type);

osal_void hh503_set_rx_framefilter(osal_bool enable);

osal_void hh503_gp_set_mpdu_count(osal_u32 val);

osal_void hh503_set_sec_bandwidth_offset(osal_u8 bandwidth);
osal_void hh503_psm_clear_mac_rx_isr(osal_void);
osal_void hh503_set_mac_aid(const hal_to_dmac_vap_stru *hal_vap, osal_u16 aid);
osal_void hh503_vap_get_machw_txop_limit_ac(wlan_wme_ac_type_enum_uint8 ac, osal_u16 *txop);
osal_void hh503_set_tx_queue_suspend_mode(osal_u8 mask, osal_u8 mode);
osal_void hal_flush_dcache(osal_u8 flush_pos);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
