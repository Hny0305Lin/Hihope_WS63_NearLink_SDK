/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file
 */

#ifndef MSG_DEVICE_MANAGE_ROM_H
#define MSG_DEVICE_MANAGE_ROM_H

#include "osal_types.h"
#include "hal_commom_ops_type_rom.h"
#include "wlan_types_base_rom.h"

/* 同步需要从ko侧同步到device侧的dmac_device结构体成员 */
typedef struct {
    osal_u8 up_vap_num : 4; /* 维护当前整个device up状态vap个数 */
    osal_u8 mac_vap_id : 4; /* 多vap共存时，保存睡眠前的mac vap id */

    /* 当前扫描状态，根据此状态处理obss扫描和host侧下发的扫描请求，以及扫描结果的上报处理 */
    mac_scan_state_enum_uint8 curr_scan_state : 4;
    osal_u8 resv1                             : 4;
    osal_u8 p2p_ps_pause;
    osal_u8 resv2;
} mac_update_device_stru;

/* 同步需要从ko侧同步到device侧的hal_device结构体成员 */
typedef struct {
    osal_u8 promis_switch : 1;
    osal_u8 ampdu_tx_hw   : 1;
    osal_u8 al_tx_flag    : 3;
    osal_u8 al_rx_flag    : 3;
    osal_u8 current_chan_number;
    osal_u8 ax_vap_cnt;
    osal_u8 fix_power_level;
    mac_channel_stru wifi_channel_status;
    osal_u32 al_tx_pkt_len; /* 常发模式发送报文大小 */
} mac_update_hal_device_stru;

#endif