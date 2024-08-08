/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of HAL external public interfaces .
 */

#ifndef HAL_EXT_IF_ROM_H
#define HAL_EXT_IF_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if_rom.h"
#include "oam_ext_if.h"
#include "frw_ext_if_rom.h"
#include "wlan_types_rom.h"
#include "hal_common_ops_rom.h"
#include "hal_ext_if_device_rom.h"
#ifdef _PRE_WLAN_FEATURE_AMPDU
#include "hal_ampdu_rom.h"
#endif
#include "hal_power_save_rom.h"
#ifdef _PRE_WLAN_FEATURE_INTF_DET
#include "hal_ext_intf_det_rom.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_EXT_IF_ROM_H

static INLINE__ osal_u16 hal_reg_read16(osal_u32 addr)
{
    return *(volatile osal_u16 *)(uintptr_t)addr;
}

static INLINE__ osal_void hal_reg_write16(osal_u32 addr, osal_u16 val)
{
    *((volatile osal_u16 *)(uintptr_t)addr) = (val);
}

static INLINE__ osal_u32 hal_reg_read(osal_u32 addr)
{
    return *(volatile osal_u32 *)(uintptr_t)addr;
}

static INLINE__ osal_void hal_reg_write(osal_u32 addr, osal_u32 val)
{
    *((volatile osal_u32 *)(uintptr_t)addr) = (val);
}

static INLINE__ osal_u32 hal_reg_read_bit(osal_u32 addr, osal_u8 offset, osal_u8 bits)
{
    osal_u32 value = hal_reg_read(addr);

    return (value >> offset) & (((osal_u32)1 << bits) - 1);
}

static INLINE__ osal_void hal_reg_write_bits(osal_u32 reg_addr, osal_u8 offset, osal_u8 bits, osal_u32 reg_val)
{
    osal_u32 value = hal_reg_read(reg_addr);
    value &= ~((((osal_u32)1 << bits) - 1) << offset);   /* 先将对应bits位的值清0 */
    value |= ((reg_val & (((osal_u32)1 << bits) - 1)) << offset); /* 将对应bits值改写为配置值 */
    hal_reg_write(reg_addr, value);
}

/*****************************************************************************
 功能描述  : 写表寄存器的值
*****************************************************************************/
static INLINE__ osal_void hal_reg_read_tbl(hal_reg32_cfg_stru *reg_tbl, osal_u8 tbl_len)
{
    osal_u8 index;
    for (index = 0; index < tbl_len; index++) {
        reg_tbl[index].value = hal_reg_read(reg_tbl[index].address);
    }
}

/*****************************************************************************
 功能描述  : 读取表寄存器的值
*****************************************************************************/
static INLINE__ osal_void hal_reg_write_tbl(const hal_reg32_cfg_stru *reg_tbl, osal_u8 tbl_len)
{
    osal_u8 index;
    for (index = 0; index < tbl_len; index++) {
        hal_reg_write(reg_tbl[index].address, reg_tbl[index].value);
    }
}

static INLINE__ osal_void hal_reg_write16_tbl(const hal_reg16_cfg_stru *reg_tbl, osal_u8 tbl_len)
{
    osal_u8 index;
    for (index = 0; index < tbl_len; index++) {
        hal_reg_write16(reg_tbl[index].address, reg_tbl[index].value);
    }
}

hal_cfg_cap_info_stru *hal_device_get_cfg_custom_addr(osal_void);
osal_void hal_rf_cali_set_calidata_mask(const osal_u8 *param);

// mac_err
typedef struct {
    osal_void      (* func)(hal_to_dmac_device_stru  *hal_device, osal_u32 para);
    osal_u32    param;
} hal_error_proc_stru;

hal_error_proc_stru get_err_proc_func(osal_u8 error_id);

osal_void  hal_device_process_mac_error_status(osal_u32 error1_irq_state, osal_u32 error2_irq_state);

osal_void hal_mac_error_msg_report(hal_to_dmac_device_stru *hal_device, hal_mac_error_type_enum_uint8 error_type);

// vap tbtt bcn common user
osal_s32 hal_main_init(osal_void);

osal_void  hal_main_exit(osal_void);

osal_void hal_add_vap(hal_to_dmac_device_stru *hal_device_base,
    wlan_vap_mode_enum_uint8 vap_mode, osal_u8 hal_vap_id, hal_to_dmac_vap_stru **hal_vap);

osal_void hal_get_hal_vap(const hal_to_dmac_device_stru *hal_device_base, osal_u8 vap_id,
    hal_to_dmac_vap_stru **hal_vap);

osal_void hal_del_vap(hal_to_dmac_device_stru *hal_device, osal_u8 vap_id);

osal_u8  hal_device_find_all_up_vap(const hal_to_dmac_device_stru *hal_device, osal_u8 *vap_id, osal_u8 lenth);
hal_to_dmac_device_stru *hal_chip_get_hal_device(osal_void);

osal_void  hal_vap_beacon_suspend(const hal_to_dmac_vap_stru *hal_vap);

osal_void  hal_vap_beacon_resume(const hal_to_dmac_vap_stru *hal_vap);

osal_void hal_vap_send_beacon_pkt(const hal_to_dmac_vap_stru *hal_vap, const hal_beacon_tx_params_stru *params);

osal_void hal_vap_set_machw_beacon_period(const hal_to_dmac_vap_stru *hal_vap, osal_u16 beacon_period);

osal_void hal_register_alg_isr_hook(struct tag_hal_to_dmac_device_stru *hal_to_dmac_device,
    hal_isr_type_enum_uint8 isr_type, hal_alg_noify_enum_uint8 alg_notify, p_hal_alg_isr_func func);

osal_void hal_vap_tsf_get_32bit(const hal_to_dmac_vap_stru *hal_vap, osal_u32 *tsf_lo);

osal_void hal_enable_tsf_tbtt(hal_to_dmac_vap_stru *hal_vap, oal_bool_enum_uint8 dbac_enable);

osal_void hal_vap_read_tbtt_timer(const hal_to_dmac_vap_stru *hal_vap, osal_u32 *value);

osal_void hal_vap_get_beacon_period(const hal_to_dmac_vap_stru *hal_vap, osal_u32 *beacon_period);

osal_void hal_vap_tsf_get_64bit(const hal_to_dmac_vap_stru *hal_vap, osal_u32 *tsf_high, osal_u32 *tsf_lo);

osal_void hal_vap_write_tbtt_timer(const hal_to_dmac_vap_stru *hal_vap, osal_u32 value);
osal_void hal_vap_tsf_set_64bit(const hal_to_dmac_vap_stru *hal_vap, osal_u32 tsf_hi, osal_u32 tsf_lo);

static INLINE__ osal_u8 get_hal_dev_current_scan_idx(hal_to_dmac_device_stru *hal_device)
{
    return hal_device->hal_scan_params.start_chan_idx + hal_device->hal_scan_params.scan_chan_idx;
}

osal_u8 hal_dev_fsm_get_curr_state(osal_void);

static INLINE__ osal_bool hal_device_state_is_busy(osal_void)
{
    return (hal_dev_fsm_get_curr_state() == HAL_DEVICE_WORK_STATE) ||
        (hal_dev_fsm_get_curr_state() == HAL_DEVICE_SCAN_STATE) ? OSAL_TRUE : OSAL_FALSE;
}
osal_void hal_device_inc_assoc_user_nums(hal_to_dmac_device_stru  *hal_device);
osal_void hal_device_dec_assoc_user_nums(hal_to_dmac_device_stru  *hal_device);
// power tpc
osal_void hal_pow_get_spec_frame_data_rate_idx(osal_u8 rate,  osal_u8 *rate_idx);
osal_u16 hal_pow_get_pow_table_tpc_code(osal_u8 band, osal_u8 rate, osal_u8 bw, osal_u8 pwr_lvl);
osal_u16 hal_pow_get_pow_table_tpc_code_for_dscr(osal_u8 band, osal_u8 mcs, osal_u8 protocol_mode, osal_u8 bw,
    osal_u8 pwr_lvl);
// forward
osal_void hal_gp_set_mpdu_count(osal_u32 netbuf_reserve, osal_u32 large_free, osal_u32 short_free, osal_u32 mgmt_free);
osal_void  hal_clear_tx_hw_queue(hal_to_dmac_device_stru *hal_device);
osal_void hal_clear_hw_fifo(hal_to_dmac_device_stru *hal_device);
oal_bool_enum_uint8 hal_is_hw_tx_queue_empty(hal_to_dmac_device_stru *hal_device);
osal_void hal_flush_tx_complete_irq(hal_to_dmac_device_stru *hal_dev);
osal_void hal_flush_rx_queue_complete_irq(hal_to_dmac_device_stru *hal_dev, hal_rx_dscr_queue_id_enum_uint8 queue_num);

// one pkt
osal_void hal_get_one_packet_status(hal_one_packet_debug_stru *status);
osal_void hal_one_packet_start(hal_to_dmac_device_stru *hal_device,
    const hal_one_packet_cfg_stru *cfg);
osal_void hal_one_packet_stop(hal_to_dmac_device_stru *hal_device);
osal_void hal_send_one_packet(hal_to_dmac_device_stru *device, hal_one_packet_cfg_stru *one_packet_cfg,
    hal_one_packet_status_stru *status, osal_u16 wait_timeout);

// dbac
osal_u32 hal_gp_get_dbac_vap_stop_bitmap(osal_void);
osal_void hal_gp_set_dbac_vap_stop_bitmap(osal_u32 val);

osal_void hal_disable_machw_phy_and_pa(osal_void);
osal_void  hal_enable_machw_phy_and_pa(const hal_to_dmac_device_stru *hal_device);
osal_void  hal_recover_machw_phy_and_pa(hal_to_dmac_device_stru *hal_device);

// slp
osal_void hal_gp_set_slp_tx_ctrl(osal_u32 val);
osal_u32 hal_gp_get_slp_tx_ctrl(osal_void);

// protocol
osal_void hal_get_bcn_rate(const hal_to_dmac_vap_stru *hal_vap, osal_u8 *data_rate);

osal_void hal_reset_nav_timer(osal_void);

osal_void  hal_device_get_fix_rate_pow_code_idx(const hal_tx_ctrl_desc_rate_stru *rate_param,
    osal_u8 *rate_pow_idx);

// channel mgmt
osal_void  hal_set_freq_band(hal_to_dmac_device_stru *hal_device, wlan_channel_band_enum_uint8 band);
osal_void  hal_set_bandwidth_mode(hal_to_dmac_device_stru *hal_device,
    wlan_channel_bandwidth_enum_uint8 bandwidth);
osal_void  hal_set_primary_channel(hal_to_dmac_device_stru *hal_device, osal_u8 channel_num,
    wlan_channel_band_enum_uint8 band, osal_u8 channel_idx, wlan_channel_bandwidth_enum_uint8 bandwidth);

osal_void hal_chan_measure_start(hal_chan_measure_stru *meas);

osal_void hal_get_ch_statics_result(hal_ch_statics_irq_event_stru *ch_statics);

// csi
osal_void hal_csi_set_sw_buff_index(osal_u8 cur_index);
osal_void hal_csi_set_data_base_addr(uintptr_t base_addr);
osal_void hal_csi_set_buff_step(osal_u16 buff_step);
osal_void hal_csi_set_buff_num(osal_u8 buff_num);

// pm
#ifdef _PRE_PM_TBTT_OFFSET_PROBE
osal_void hal_tbtt_offset_probe_destroy(hal_to_dmac_vap_stru *hal_vap);
osal_void hal_tbtt_offset_probe_tbtt_cnt_incr(hal_to_dmac_vap_stru *hal_vap);
osal_void hal_tbtt_offset_probe_beacon_cnt_incr(hal_to_dmac_vap_stru *hal_vap);
#endif
osal_void hal_pm_vote2platform(hal_pm_mode_enum_uint8 pm_mode);

osal_void hal_pm_servid_unregister(osal_void);

osal_void hal_pm_servid_register(osal_void);

#define HAL_PM_DEF_TBTT_OFFSET              10000        /* tbtt提前量默认值单位us */
#define HAL_PM_DEF_EXT_TSF_OFFSET           12500       /* 外置tsf唤醒提前量单位us */

// dscr
osal_void  hal_tx_get_dscr_status(const hal_tx_dscr_stru *tx_dscr, osal_u8 *status);

osal_void  hal_tx_get_dscr_send_rate_rank(const hal_tx_dscr_stru *tx_dscr, osal_u8 *send_rate_rank);

osal_void hal_tx_get_vap_id(const hal_to_dmac_device_stru *hal_device,
    const hal_tx_dscr_stru *tx_dscr, osal_u8 *vap_id);

osal_void hal_tx_fill_basic_ctrl_dscr(const hal_to_dmac_device_stru *hal_device,
    hal_tx_dscr_stru *tx_dscr, hal_tx_mpdu_stru *mpdu);

osal_void hal_tx_seqnum_set_dscr(hal_tx_dscr_stru *tx_dscr, osal_u16 seq_num);

osal_void hal_tx_get_dscr_seq_num(hal_tx_dscr_stru *tx_dscr, osal_u16 *seq_num);

osal_void hal_tx_ucast_data_set_dscr(hal_to_dmac_device_stru *device, hal_tx_dscr_stru *tx_dscr,
    hal_tx_txop_alg_stru *txop_alg);

osal_void hal_tx_csi_set_rts_dscr(hal_tx_dscr_stru *tx_dscr);

osal_void  hal_tx_set_dscr_status(hal_tx_dscr_stru *tx_dscr, osal_u8 status);

osal_void hal_rx_get_dscr_status(osal_u32 *rx_dscr, osal_u8 *dscr_status);

osal_void hal_rx_free_dscr_list(hal_to_dmac_device_stru *hal_device,
    hal_rx_dscr_queue_id_enum_uint8 queue_num, osal_u32 *rx_dscr);

osal_void hal_rx_get_netbuffer_addr_dscr(osal_u32 *rx_dscr, oal_dmac_netbuf_stru **rx_next_amsdu_dscr);

osal_void hal_rx_init_dscr_queue(hal_to_dmac_device_stru *device, osal_u8 set_hw);

osal_void hal_rx_destroy_dscr_queue(hal_to_dmac_device_stru *device);

osal_void hal_al_rx_destroy_dscr_queue(hal_to_dmac_device_stru *hal_device);

osal_void hal_al_rx_init_dscr_queue(hal_to_dmac_device_stru *hal_device, osal_u16 size);

osal_void hal_tx_set_dscr_ba_ssn(hal_tx_dscr_stru *tx_dscr, osal_u16 ba_ssn);

osal_void hal_tx_set_dscr_ba_bitmap(hal_tx_dscr_stru *tx_dscr, osal_u32 word0, osal_u32 word1);

// wow to be removed
osal_void hal_ce_get_key(const hal_security_key_stru *security_key);
osal_void hal_ce_add_key(hal_security_key_stru *security_key);
osal_void hal_vap_get_gtk_rx_lut_idx(const hal_to_dmac_vap_stru *hal_vap, osal_u8 *lut_idx);
osal_void hal_gp_set_sdp_chnl_switch_off(osal_u32 val);
osal_void hal_gp_clear_dbac_vap_stop_bitmap(osal_void);
hal_cipher_protocol_type_enum_uint8 hal_cipher_suite_to_ctype(wlan_ciper_protocol_type_enum_uint8 cipher_suite);
wlan_ciper_protocol_type_enum_uint8 hal_ctype_to_cipher_suite(hal_cipher_protocol_type_enum_uint8 cipher_type);

osal_void hal_set_rf_limit_power(wlan_channel_band_enum_uint8 band, osal_u8 power);

osal_void  hal_pow_set_user_resp_frame_tx_power(hal_to_dmac_device_stru *hal_device, osal_u8 lut_index,
    osal_u8 rssi_distance);
osal_void hal_pow_set_band_spec_frame_tx_power(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 band, osal_u8 chan_idx);

osal_void hal_set_bcn_phy_tx_mode(const hal_to_dmac_vap_stru *hal_vap, osal_u32 pow_code);

osal_void hal_pow_set_four_rate_tx_dscr_power(hal_tx_txop_alg_stru *txop_param, const osal_u8 *pow_level,
    hal_tx_txop_tpc_stru *tpc);

osal_void hal_pow_get_data_rate_idx(osal_u8 protocol_mode, osal_u8 mcs, wlan_bw_cap_enum_uint8 bw,
    osal_u8 *rate_idx);

static INLINE__ osal_bool hal_m2s_check_btcoex_on(const hal_to_dmac_device_stru *hal_device)
{
    osal_u8 tmp = *(osal_u8 *)&(hal_device->hal_m2s_fsm.m2s_mode);
    if ((tmp & WLAN_M2S_TRIGGER_MODE_BTCOEX) != 0) {
        return OSAL_TRUE;
    } else {
        return OSAL_FALSE;
    }
}

static INLINE__ osal_u8 get_hal_m2s_mode_tpye(const hal_to_dmac_device_stru *hal_device)
{
    return (*(osal_u8 *)&(hal_device->hal_m2s_fsm.m2s_mode));
}

static INLINE__ osal_bool hal_m2s_check_rssi_on(const hal_to_dmac_device_stru *hal_device)
{
    osal_u8 tmp = *(osal_u8 *)&(hal_device->hal_m2s_fsm.m2s_mode);
    if ((tmp & WLAN_M2S_TRIGGER_MODE_RSSI) != 0) {
        return OSAL_TRUE;
    } else {
        return OSAL_FALSE;
    }
}

static INLINE__ osal_bool hal_m2s_check_command_on(const hal_to_dmac_device_stru *hal_device)
{
    osal_u8 tmp = *(osal_u8 *)&(hal_device->hal_m2s_fsm.m2s_mode);
    if ((tmp & WLAN_M2S_TRIGGER_MODE_COMMAND) != 0) {
        return OSAL_TRUE;
    } else {
        return OSAL_FALSE;
    }
}

osal_void hal_pow_set_pow_code_idx_in_tpc(hal_tx_txop_tpc_stru *tpc,
    const osal_u32 *tpc_code, osal_u16 tpc_code_size);

osal_u8 hal_pow_get_legacy_rate(hal_tx_ctrl_desc_rate_stru *tx_dscr);

osal_void hal_device_set_pow_al_tx(const hal_to_dmac_device_stru *hal_device,
    osal_u8 band, hal_tx_txop_alg_stru *txop_alg);

osal_u32  hal_device_find_one_up_vap(hal_to_dmac_device_stru *hal_device, osal_u8 *mac_vap_id);
osal_u32  hal_device_find_another_up_vap(hal_to_dmac_device_stru *hal_device,
    osal_u8 vap_id_self, osal_u8 *mac_vap_id);

/********** for ws63   **********/
#define HAL_ANT_SWITCH_RSSI_MGMT_ENABLE             BIT0
#define HAL_ANT_SWITCH_RSSI_DATA_ENABLE             BIT1

/********** for uranus   **********/
osal_void hal_tx_dscr_get_rate(const hal_tx_dscr_stru *tx_dscr, osal_u8 rate_index, hal_tx_dscr_rate_stru *tx_rate);
osal_void hal_tx_dscr_get_protocol_mode(const hal_tx_dscr_stru *tx_dscr, osal_u8 rate_index, osal_u8 *protocol_mode);
/********** for uranus   **********/
/*****************************************************************************
 功能描述  : 释放保存mac寄存器的netbuf
*****************************************************************************/
osal_void hal_reset_free_reg_save(osal_void);
/*****************************************************************************
 功能描述  : mac寄存器恢复
*****************************************************************************/
osal_void hal_reset_restore_mac_reg(osal_void);

/*****************************************************************************
 功能描述  : MAC和PHY寄存器DMA方式恢复
*****************************************************************************/
osal_void hal_reset_dma_restore_reg(osal_void);
/*****************************************************************************
 函 数 名  : hal_pm_enable_front_end
 功能描述  : 低功耗唤醒前端操作
*****************************************************************************/
osal_void hal_pm_enable_front_end(hal_to_dmac_device_stru  *hal_device, osal_u8 enable_paldo);
/*****************************************************************************
 功能描述  : chip级别中断初始化(bt coex中断)
*****************************************************************************/
osal_void hal_chip_irq_init(osal_void);
hal_to_dmac_device_stru *hal_get_hal_to_dmac_device(osal_void);
osal_u32 hal_device_find_one_up_hal_vap(hal_to_dmac_device_stru *hal_device,  osal_u8 *vap_id);
/*****************************************************************************
 功能描述  : chip级别中断去初始化(bt coex中断)
*****************************************************************************/
osal_void hal_irq_init(osal_void);
osal_void hal_chip_irq_exit(osal_void);
osal_void hal_irq_exit(osal_void);
/*****************************************************************************
 功能描述  : 屏蔽指定中断
*****************************************************************************/
osal_void hal_mask_interrupt(osal_u32 offset);
osal_void hal_pm_set_bcn_rf_chain(hal_to_dmac_vap_stru *hal_vap, osal_u8 bcn_rf_chain);

/*****************************************************************************
 函 数 名  : hal_vap_get_machw_txop_limit_vivo
 功能描述  : 获取VI、VO的TXOP上限(单位: 微秒)
*****************************************************************************/
static INLINE__ osal_void  hal_vap_get_machw_txop_limit_ac(osal_u8 ac, osal_u16 *txop)
{
    hal_public_hook_func(_vap_get_machw_txop_limit_ac)(ac, txop);
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_ext_if_rom.h */
