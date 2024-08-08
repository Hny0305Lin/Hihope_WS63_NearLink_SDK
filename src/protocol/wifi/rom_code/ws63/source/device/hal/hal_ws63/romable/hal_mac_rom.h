/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_mac_rom.c.
 */

#ifndef HAL_MAC_ROM_H
#define HAL_MAC_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "wlan_types_rom.h"
#include "oam_ext_if.h"
#include "frw_ext_if_rom.h"
#include "hal_ext_if_rom.h"
#include "hal_mac_reg.h"
#include "hal_chip_rom.h"
#include "hal_device_rom.h"
#include "hal_soc_rom.h"
#include "hal_device_fsm_rom.h"
#include "frw_rom_cb_rom.h"
#include "fe_extern_if_device.h"
#include "hal_mac_reg_field.h"
#include "hal_reset_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define HH503_SIFSTIME 16
#define HH503_ACK_CTS_FRAME_LEN 14
#define HH503_LONG_PREAMBLE_LEN_A 16
#define HH503_LONG_PREAMBLE_LEN_B 144
#define HH503_PLCP_HEADER_LEN_A 4
#define HH503_PLCP_HEADER_LEN_B 48
/* 以下为PHY头时间 单位us */
#define HH503_LONG_PREAMBLE_TIME 192
#define HH503_SHORT_PREAMBLE_TIME 96
#define HH503_LEGACY_OFDM_TIME 20
#define HH503_HT_PHY_TIME 40
#define HH503_VHT_PHY_TIME 44

/* RTS时间定义，包括PHY头 帧长/速率+phy头时间 */
#define HH503_RTSTIME_11B_LONG 221
#define HH503_RTSTIME_NON_11B 27

/* CTS时间定义，包括PHY头帧长/速率+phy头时间 */
#define HH503_CTSACKTIME_11B_LONG 212
#define HH503_CTSACKTIME_NON_11B 25

/* BA时间定义，包括PHY头帧长/速率+phy头时间 */
#define HH503_BATIME_11B_LONG 238
#define HH503_BATIME_NON_11B 30

/* 过滤寄存器初始值 */
#define HH503_MAC_CFG_INIT 0xC7BDFEDA

#define HH503_EIFSTIME_GONLY \
    (HH503_SIFSTIME + HH503_ACK_CTS_FRAME_LEN * 8 + HH503_PLCP_HEADER_LEN_A + HH503_LONG_PREAMBLE_LEN_A)
#define HH503_EIFSTIME_GMIXED \
    (HH503_SIFSTIME + HH503_ACK_CTS_FRAME_LEN * 8 + HH503_PLCP_HEADER_LEN_B + HH503_LONG_PREAMBLE_LEN_B)

#define HH503_LUT_READ_OPERN 0x0
#define HH503_LUT_WRITE_OPERN 0x1
#define HH503_LUT_REMOVE_OPERN 0x2
/* Time (in units of 10us) to wait for CE-LUT update operation to complete */
#define HH503_NUM_CE_LUT_UPDATE_ATTEMPTS 2

/*****************************************************************************/
/* Channel Access Timer Management Registers                                 */
/*****************************************************************************/
#define HH503_MAC_SEC_CHAN_SLOT_COUNT_INIT_VAL_FREQ_5 0x00000001
#define HH503_MAC_SEC_CHAN_SLOT_COUNT_INIT_VAL_FREQ_2 0x00000002

#define HH503_PHY_TX_MODE_DEFAULT 0x00000000

#ifdef _PRE_WLAN_DFT_STAT
/* 维测需要读取硬件寄存器的值，函数定义放在DFT的预编译宏下 */
#define HAL_DFT_REG_OTA_LEN 1024 /* 寄存器OTA上报的最大限制 */
#endif

#define BSRP_VAP_MAX 2

#define HH503_PA_LUT_UPDATE_TIMEOUT 2000
#define HH503_TX_SUSPEND_UPDATE_TIMEOUT 5000

#define P2P_PERIOD_NOA_COUNT 255

#define HH503_MAC_CFG_BIP_REPLAY_FAIL_FLT_EN_MASK    0x80000000
#define HH503_MAC_CFG_CCMP_REPLAY_FAIL_FLT_EN_MASK    0x4000000
#define HH503_MAC_CFG_OTHER_CTRL_FRAME_FLT_EN_MASK    0x400
#define HH503_MAC_CFG_BCMC_MGMT_OTHER_BSS_FLT_EN_MASK    0x10
#define HH503_MAC_CFG_VAP0_BSSID_H_LEN    16
#define HH503_MAC_CFG_VAP0_BSSID_H_OFFSET    0
#define HH503_MAC_CFG_VAP0_BSSID_H_MASK    0xffff
#define HH503_MAC_CFG_VAP0_BSSID_L_LEN    32
#define HH503_MAC_CFG_VAP0_BSSID_L_OFFSET    0
#define HH503_MAC_CFG_VAP0_BSSID_L_MASK    0xffffffff

#define HH503_LUT_RETRY_INTERVAL 10
#define HH503_LUT_OPER_READ  0
#define HH503_LUT_OPER_WRITE 1
#define HH503_LUT_OPER_DEL   2
#define HH503_LUT_OPER_CLR   3

#define HH503_PN_FRM_TYPE_MCBC_DATA  0
#define HH503_PN_FRM_TYPE_UC_QOS     1
#define HH503_PN_FRM_TYPE_UC_NQOS    2
#define HH503_PN_FRM_TYPE_UC_MGMT    3
#define HH503_PN_FRM_TYPE_MCBC_MGMT  4

#define HH503_MAX_USR_NUM  8

#define HH503_KEY_IGTK 0
#define HH503_KEY_PTK  1
#define HH503_KEY_GTK  2
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  STRUCT定义
*****************************************************************************/
typedef struct {
    osal_u32 tpc_ch0           : 8,
             dpd_tpc_lv_ch0    : 2,
             cfr_idx           : 2,
             reseved           : 20;
} hh503_tx_resp_phy_mode_reg_stru;

typedef union {
    struct {
        osal_u32 tx_vector_tpc_ch0 : 8;
        osal_u32 tx_vector_freq_bandwidth_mode : 2;
        osal_u32 tx_vector_smoothing : 1;
        osal_u32 tx_vector_channel_code : 1;
        osal_u32 reserved : 20;
    } bits;
    osal_u32 u32;
} hh503_tx_phy_mode_reg_stru;

typedef union {
    struct {
        osal_u32 tx_vector_he_ltf_type : 2;
        osal_u32 tx_vector_ant_sel : 2;
        osal_u32 tx_vector_tpc_dpd_lv_0 : 2;
        osal_u32 tx_vector_pe_duration : 3;
        osal_u32 tx_vector_dcm : 1;
        osal_u32 tx_vector_mcs_rate : 4;
        osal_u32 tx_vector_protocol_mode : 2;
        osal_u32 tx_vector_he_flag : 1;
        osal_u32 tx_vector_preamble_boost_flag : 1;
        osal_u32 tx_vector_preamble : 1;
        osal_u32 tx_vector_gi_type : 2;
        osal_u32 tx_vector_cfr_idx : 2;
        osal_u32 reserve           : 9;
    } bits;
    osal_u32 u32;
} hh503_tx_phy_rate_stru;

typedef union {
    struct {
        osal_u32 mcs_rate : 4;
        osal_u32 protocol_mode : 2;
        osal_u32 reserve : 2;
    } bits;
    osal_u8 u8;
} hal_phy_rate_stru;

typedef osal_void (*p_hh503_dft_report_all_reg_state_cb)(hal_to_dmac_device_stru *hal_device);
typedef osal_void (*p_hh503_enable_radar_det_cb)(osal_u8 enable);
typedef osal_void (*p_hh503_set_psm_listen_interval_count_cb)(hal_to_dmac_vap_stru *hal_vap,
    osal_u16 interval_count);

typedef struct {
    p_hh503_dft_report_all_reg_state_cb hh503_dft_report_all_reg_state;
    p_hh503_enable_radar_det_cb hh503_enable_radar_det;
} hal_mac_rom_cb;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
 功能描述  : 获取当前发送完成中断上报的MPDU个数
 函 数 名  : hh503_get_vap_internal
 功能描述  : HAL内部获取VAP
*****************************************************************************/
static INLINE__ osal_void hh503_get_vap_internal(const hal_device_stru *device, osal_u8 vap_id,
    hal_to_dmac_vap_stru **hal_vap)
{
    /* 检查传入的vap id是否合理 */
    if (vap_id >= HAL_MAX_VAP_NUM) {
        *hal_vap = OSAL_NULL;
        return;
    }
    *hal_vap = (hal_to_dmac_vap_stru *)(device->vap_list[vap_id]);
}

static INLINE__ osal_u32 hh503_get_subband_index(wlan_channel_band_enum_uint8 band, osal_u8 channel_idx,
    osal_u8 *subband_idx)
{
    if (band == WLAN_BAND_5G) {
        /* ws73不支持5G */
        return OAL_FAIL;
    }
    if ((band == WLAN_BAND_2G) && (channel_idx >= HAL_2G_CHANNEL_NUM + 1)) {
        return OAL_FAIL;
    }
    *subband_idx = channel_idx;
    return OAL_SUCC;
}

/*****************************************************************************
 ROM回调函数类型定义
*****************************************************************************/
typedef osal_void (*hh503_cb_mac_init_hw)(osal_void);
typedef osal_void (*hh503_set_prot_resp_frame_chain_ext_cb)(hal_to_dmac_device_stru *hal_device,
    osal_u8 chain_val);
typedef osal_void (*hh503_set_extlna_chg_cfg_ext_cb)(hal_to_dmac_device_stru *hal_device,
    oal_bool_enum_uint8 extlna_chg_bypass);
typedef osal_void (*hh503_set_primary_channel_ext_cb)(hal_to_dmac_device_stru *hal_device, osal_u8 channel_num,
    wlan_channel_band_enum_uint8 band, osal_u8 channel_idx, wlan_channel_bandwidth_enum_uint8 bandwidth);
typedef osal_void (*hh503_reset_hw_macphy_cb)(osal_bool hw_reset);
osal_void hh503_set_dup_rts_cts_mode(osal_void);
osal_void hh503_ce_clear_all(osal_void);
osal_void hal_tsf_pulse_adjust(osal_void);
osal_void hh503_get_freq_factor(osal_u8 freq_band, osal_u8 channel_num, osal_u32 *freq_factor);
osal_void hh503_set_psm_dtim_count_cb(hal_to_dmac_vap_stru *hal_vap, osal_u8 dtim_count);
osal_u32  hh503_enable_ce(osal_void);
osal_void hh503_set_ctrl_frm_pow_code(osal_u32 phy_mode_addr, osal_u32 data_rate_addr, osal_u32 pow_code);
osal_void hh503_set_extlna_chg_cfg_ext(hal_to_dmac_device_stru *hal_device, oal_bool_enum_uint8 extlna_chg_bypass);
osal_u32 hh503_wait_mac_done_timeout(osal_u32 reg_addr, osal_u32 bitmask, osal_u32 reg_busy_value,
    osal_u32 wait_10us_cnt);
osal_void hal_device_set_rx_filter1_reg(osal_bool filter);

/*****************************************************************************
 功能描述  : 根据tx描述符更新beacon_rate寄存器域
*****************************************************************************/
osal_void hal_chan_update_beacon_rate(hh503_tx_phy_rate_stru *beacon_rate, const hal_tx_ctrl_desc_rate_stru *tx_dscr);

/*****************************************************************************
 功能描述  : 恢复硬件发送
*****************************************************************************/
osal_void hal_set_machw_tx_resume(osal_void);
osal_void hal_initialize_machw_common(osal_void);
/*****************************************************************************
 功能描述  : 配置MAC PHY ADC的频率
*****************************************************************************/
osal_u8 hal_set_machw_phy_adc_freq(hal_to_dmac_device_stru *hal_device,
    wlan_channel_bandwidth_enum_uint8 bandwidth);

#ifdef _PRE_WLAN_FEATURE_CHBA
osal_void hal_sync_update_tsf(osal_u8 vap_id, osal_bool enable);
#endif
osal_void hal_vap_set_multi_bssid(osal_u8 *trans_bssid, osal_u8 maxbssid_indicator);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_mac_rom.h */
