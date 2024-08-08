/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_dscr_rom.c.
 */

#ifndef HAL_DSCR_ROM_H
#define HAL_DSCR_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_common_ops_rom.h"
#include "hal_mac_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 接收描述符个数的宏定义 */
#define HH503_HW_RX_DSCR_LIST_ZERO 0
#define HAL_RX_MACHW_REG_TIMEOUT_COUNT 10
#define HAL_TX_DSCR_RATE_NUM 4
#define HAL_DSCR_BASE_ADDR 0xA
#define HH503_MAC_BUS_MAX_2ADDR_OFFSET 20
/*****************************************************************************
  STRUCT定义
*****************************************************************************/

/*****************************************************************************
  7.1 发送描述符定义
*****************************************************************************/
/* 描述符第10行结构体 */
typedef struct {
    osal_u32 peer_ps_chk_disable : 1;
    osal_u32 tsf_timestamp : 15;

    osal_u32 mac_header_length : 6;
    osal_u32 obss_pd_tx_disable : 1;
    osal_u32 sub_msdu_num : 1;
    osal_u32 retry_flag_hw_bypass : 1;
    osal_u32 duration_hw_bypass : 1;
    osal_u32 seq_ctl_hw_bypass : 1;
    osal_u32 timestamp_hw_bypass : 1;
    osal_u32 tx_addba_ssn_bypass : 1;
    osal_u32 tx_pn_hw_bypass : 1;
    osal_u32 tx_fcs_gen_err_en : 1;
    osal_u32 software_retry : 1;
} hh503_tx_mac_mode_one_stru;

/* 描述符第11行结构体 */
typedef struct {
    osal_u32 mpdu_num : 6; /* ampdu中mpdu的个数 */
    osal_u32 tx_vap_idx : 2;
    osal_u32 tx_ampdu_session_index : 4;
    osal_u32 reserved_5 : 5;
    osal_u32 he_max_pe_fld : 2;
    osal_u32 pdet_en : 1;
    osal_u32 ciper_protocol_type : 4;
    osal_u32 cipher_key_id : 3;
    osal_u32 ra_lut_idx : 3;
    osal_u32 uplink_flag : 1;
    osal_u32 ampdu_enable : 1;
} hh503_tx_mac_mode_two_stru;

/* tx dscr word14-17 */
typedef struct {
    osal_u32 rate : 4; /* 上面定义的速率结构体位域 */
    osal_u32 freq_bw : 2;  /* hal_channel_assemble_enum */
    osal_u32 protocol_mode : 3;
    osal_u32 he_dcm : 1; /* wlan_phy_dcm_status */
    osal_u32 preamble_mode : 1; /* wlan_phy_preamble_type */
    osal_u32 gi_type : 2; /* wlan_phy_he_gi_type_enum/wlan_phy_he_gi_type_enum */
    osal_u32 ltf_type : 2;
    osal_u32 fec_coding : 1;
    osal_u32 tpc_ch0 : 8;
    osal_u32 rts_rate_rank : 3;
    osal_u32 rts_cts_protect_mode : 1;
    osal_u32 dpd_tpc_lv : 2;
    osal_u32 cfr_idx : 2;
} hh503_tx_ctrl_desc_rate_tpc_stru;

typedef struct {
    /*  word0 */
    osal_u32 tx_intr_seq : 3;
    osal_u32 send_flag : 1;
    osal_u32 data_count_0 : 3;
    osal_u32 data_count_1 : 3;
    osal_u32 data_count_2 : 3;
    osal_u32 data_count_3 : 3;
    osal_u32 last_ack_rssi : 8;
    osal_u32 last_trig_based_flag : 2; /* 最后发送成功时是否使用trig baseed 的格式 */
    osal_u32 send_rate_rank : 2;
    osal_u32 status : 4;

    /*  word1 */
    osal_u32 tx_tb_count : 4;
    osal_u32 rts_count_0 : 3;
    osal_u32 rts_count_1 : 3;
    osal_u32 rts_count_2 : 3;
    osal_u32 rts_count_3 : 3;
    osal_u32 reserved_0 : 4;
    osal_u32 ba_ssn : 12;

    /*  word2-3 */
    osal_u32 ba_bitmap_word1; /* 接收到的BA的bitmap高32位,optional */
    osal_u32 ba_bitmap_word0; /* 接收到的BA的bitmap低32位,optional */

    /*  word4 */
    osal_u32 tx_pn_msb : 16;
    osal_u32 tx_pn_fld_vld : 1;
    osal_u32 seq_num_fld_vld : 1;
    osal_u32 hw_retry_flag : 1;
    osal_u32 reserved_1 : 1;
    osal_u32 seq_num_fld : 12;

    /*  word5 */
    osal_u32 tx_pn_lsb;
} hh503_tx_ctrl_dscr_one_stru; /* rom */

/* 第6~12行 */
typedef struct tag_hh503_tx_ctrl_two_dscr_stru {
    /*  word6 */
    osal_u32 next_dscr_addr : 20; /* 填给硬件的下一个描述符物理地址 判空，低20bit为空即为空 */
    osal_u32 reserved_2 : 12;

    /* word7 */
    osal_u32 sub_msdu0_buff_addr : 20; /* 基地址寄存器初始化 */
    osal_u32 sub_msdu0_len : 12;
    /* word8 */
    osal_u32 sub_msdu1_buff_addr : 20; /* 基地址寄存器初始化 */
    osal_u32 sub_msdu1_len : 12;
    /* word9 */
    osal_u32 mac_hdr_start_addr : 20;
    osal_u32 reserved_3 : 2;
    osal_u32 ant_set : 2;
    osal_u32 reserved_4 : 8;
    /* word10 */
    hh503_tx_mac_mode_one_stru tx_mac_mode_one;
    /* word11 */
    hh503_tx_mac_mode_two_stru tx_mac_mode_two;
    /* word12 */
    osal_u32 partial_aid : 9;
    osal_u32 group_id_bss_color : 6;
    osal_u32 preamble_boost_flag : 1;
    osal_u32 ch_bandwidth_in_non_ht_exist : 1;
    osal_u32 dyn_bandwidth_in_non_ht_exist : 1;
    osal_u32 dyn_bandwidth_in_non_ht : 1;
    osal_u32 txop_ps_not_allowed : 1;
    osal_u32 lsig_txop : 1;
    osal_u32 long_nav_enable : 1;
    osal_u32 min_mpdu_len : 10;
} hh503_tx_ctrl_dscr_two_stru; /* rom */

/* 第13-17 行 */
typedef struct {
    /* word13 */
    osal_u32 ampdu_len : 16;
    osal_u32 cts_protect : 1;
    osal_u32 smoothing : 1;
    osal_u32 rts_tpc_rank : 2;
    osal_u32 tx_count3 : 3;
    osal_u32 tx_count2 : 3;
    osal_u32 tx_count1 : 3;
    osal_u32 tx_count0 : 3;
    /* word14 ~ 17 */
    hh503_tx_ctrl_desc_rate_tpc_stru rate_tpc[HAL_TX_DSCR_RATE_NUM];
} hh503_tx_ctrl_dscr_three_stru; /* rom */

/*****************************************************************************
  7.2 接收描述符定义
*****************************************************************************/
typedef struct {
    osal_u32 *next_rx_dscr;         /* 后一个描述符的地址(物理地址) */
    osal_u32 mac_hdr_start_addr;     /* 当前描述符对应的帧的帧头地址 */
    osal_u32 mac_payload_start_addr; /* 当前描述符对应的帧的payload地址 */
} hh503_rx_buffer_addr_stru;

typedef union {
    struct {
        osal_u8 legacy_rate : 4;
        osal_u8 reserved1 : 2;
        osal_u8 protocol_mode : 2;
    } legacy_rate; /* 11a/b/g的速率集定义 */
    struct {
        osal_u8 ht_mcs : 6;
        osal_u8 protocol_mode : 2;
    } ht_rate; /* 11n的速率集定义 */
    struct {
        osal_u8 vht_mcs : 4;
        osal_u8 nss_mode : 2;
        osal_u8 protocol_mode : 2;
    } vht_nss_mcs; /* 11ac的速率集定义 */
    struct {
        osal_u8 he_mcs : 4;
        osal_u8 nss_mode : 2;
        osal_u8 protocol_mode : 2;
    } he_nss_mcs; /* 11ax的速率集定义 */
} rx_nss_rate_union;

/*
 * rx_num_buffer_in_mpdu 对应一个AMSDU，标记分为几个Buffer携带
 * 需要累加uc_mac_hdr_len和uc_frame_len，告知软件，或者描述符中删除mac hdr len，frame len不变
 */
typedef struct {
    /*  word3 */
    osal_u8 vap_index : 5; /* 本次中断对应的VAP的索引  */
    osal_u8 rsp_flag : 1;  /* beaforming时，是否上报信道矩阵的标识，0:上报，1:不上报 */
    osal_u8 rx_ofdma_flag : 1;
    osal_u8 he_flag : 1;

    osal_s8 rssi_dbm; /* PHY接收当前帧的RSSI值    */

    rx_nss_rate_union nss_rate;

    osal_u8 ciper_type : 4; /* 加密类型 */
    osal_u8 status : 4;     /* 接收状态 */
    /* word4 */
    osal_u16 rx_frame_len; /* 帧长信息:包括帧头和帧头 */
    osal_u8 extended_spatial_streams : 2;
    osal_u8 smoothing : 1;
    osal_u8 freq_bandwidth_mode : 4;
    osal_u8 preabmle : 1;

    osal_u8 channel_code : 1;
    osal_u8 stbc : 2;
    osal_u8 gi_type : 2;
    osal_u8 ampdu_flag : 1;
    osal_u8 sounding_mode : 2;
    /* word5 */
    osal_u8 msdu_nums_in_cur_rx_buffer; /* 当前buffer中msdu的数目 */

    osal_u8 amsdu_flag : 1;        /* AMSDU标识 */
    osal_u8 buffer_start_flag : 1; /* AMSDU中，首个MSDU标识 */
    osal_u8 frag_flag : 1;         /* MSDU分段标识 */
    osal_u8 reserved2 : 1;
    osal_u8 buffer_nums_in_cur_rx_mpdu : 4; /* 当前MPDU占用BUFFER的个数 */

    osal_u8 mac_hdr_len : 6; /* MAC帧头长度 */
    osal_u8 rx_bf_flag : 1;
    osal_u8 rx_mu_mimo : 1;

    osal_u8 rx_sub_mpdu_index;
    /* word6 */
    osal_u32 rx_tsf_timestamp : 24;
    osal_u32 he_ltf_type : 2;
    osal_u32 dcm_en : 1;
    osal_u32 rx_interrupt_seq_num : 4;
    osal_u32 last_mpdu_flag : 1;

    /* word7 */
    osal_u32 rpt_snr_ant0 : 8;
    osal_u32 rpt_aver_evm_ant0 : 12;
    osal_u32 rx_ce_pn : 8;
    osal_u32 rx_peer_index : 4;
} hh503_rx_status_dscr_stru;


/* 接收描述符中维测信息 通过MAC寄存器选择上报rx_vector or rx_trailer */
typedef struct {
    /* 第8~12行 */
    osal_u32 rpt_test_0word;
    osal_u32 rpt_test_1word;
    osal_u32 rpt_test_2word;
    osal_u32 rpt_test_3word;
    osal_u32 rpt_test_4word;
} hh503_rx_debug_dscr_stru;

/*****************************************************************************
  函数声明
*****************************************************************************/
osal_u32 hal_tx_dscr_get_real_addr(osal_u32 *tx_dscr);
osal_u32 *hal_tx_dscr_get_sw_addr(osal_u32 *tx_dscr);
osal_void hh503_tx_wur_frame_set_dscr(hal_tx_dscr_stru *tx_dscr);

typedef osal_void (*hh503_tx_wur_frame_set_dscr_cb)(hal_tx_dscr_stru *tx_dscr);

typedef osal_u32 (*hal_rx_record_frame_status_cb)(hal_to_dmac_device_stru *hal_device, osal_u32 *rx_dscr,
    hal_rx_dscr_queue_id_enum_uint8 queue_id);

osal_u32 hh503_get_real_addr(osal_u32 addr);

osal_u16 hh503_get_rx_dscr_addr(osal_u32 **rx_dscr, osal_u32 **small_rx_dscr);

osal_void hh503_get_tx_dscr_addr(hal_tx_dscr_stru **tx_dscr);

osal_u32 hh503_rx_set_ctrl_dscr(hal_rx_dscr_stru *rx_dscr, oal_dmac_netbuf_stru *netbuf);

osal_void hh503_rx_set_status_dscr(hal_rx_dscr_stru *rx_dscr, hal_rx_status_enum_uint8 status);

osal_void hal_rx_add_dscr(hal_to_dmac_device_stru *hal_device, hal_rx_dscr_queue_id_enum_uint8 queue_num);

osal_void hal_get_tx_dscr_field(const hal_to_dmac_device_stru *hal_device, hal_tx_dscr_stru *tx_dscr);

osal_void hal_dump_all_tx_dscr(hal_to_dmac_device_stru *hal_device, osal_u32 param);

osal_u32 hal_get_tx_dscr_seq_num(hal_tx_dscr_stru *tx_dscr);
osal_void hal_set_tx_dscr_seq_num(hal_tx_dscr_stru *tx_dscr, osal_u32 seq_num_fld);

osal_u32 hal_get_tx_dscr_pn_msb(hal_tx_dscr_stru *tx_dscr);
osal_void hal_set_tx_dscr_pn_msb(hal_tx_dscr_stru *tx_dscr, osal_u32 tx_pn_msb);

osal_u32 hal_get_tx_dscr_pn_lsb(hal_tx_dscr_stru *tx_dscr);
osal_void hal_set_tx_dscr_pn_lsb(hal_tx_dscr_stru *tx_dscr, osal_u32 tx_pn_lsb);
osal_void hh503_rx_ctrl_dscr_link(hal_rx_dscr_stru *rx_dscr_prev, hal_rx_dscr_stru *rx_dscr);
osal_u32 hh503_rx_alloc_netbuf_and_dscr(hal_to_dmac_device_stru *hal_device,
    hal_rx_dscr_queue_id_enum_uint8 queue_num);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* end of hal_dscr_rom.h */
