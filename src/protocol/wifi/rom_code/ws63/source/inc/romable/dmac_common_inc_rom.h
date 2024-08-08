/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of ws83 device and host public interface.
 */

#ifndef __DMAC_COMMON_INC_ROM_H__
#define __DMAC_COMMON_INC_ROM_H__

#include "osal_types.h"
#include "frw_msg_rom.h"
#include "wlan_msg_rom.h"
#include "hal_ops_common_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef struct {
    osal_u32 addr;
    osal_u32 width;
    osal_u32 val;
} reg_info;

typedef struct {
    osal_u8 syn_id;
    osal_u8 value;
} hcc_custom_data;

typedef struct {
    osal_u8 ac_num;
} tx_comp_schedule;

enum reg_addr_width {
    REG_ADDR_16BITS,
    REG_ADDR_32BITS,
};

#define BEACON_BUFF_LEN 768
typedef struct {
    osal_u8              beacon_data[BEACON_BUFF_LEN];
    hal_tx_txop_alg_stru tx_param;
    osal_u32             pkt_len;
    osal_u32             tx_chain_mask;
} tx_beacon_data;

typedef struct {
    osal_u8 null_qosnull_frame[32];     /* null&qos null,取最大长度32 */
    osal_u16 cfg_coex_tx_vap_index : 4; /* 03新增premmpt帧配置参数 */
    osal_u16 cfg_coex_tx_qos_null_tid : 4;
    osal_u16 bit_rsv : 3;
    osal_u16 cfg_coex_tx_peer_index : 5;
} btcoex_null_preempt_stru;

#ifndef _PRE_WLAN_FEATURE_CENTRALIZE
osal_u32 frw_rx_custom_post_data_function(hcc_queue_type queue_id, osal_u8 stype, osal_u8 *buf, osal_u32 len,
                                          osal_u8 *user_param);
#else
osal_u32 frw_rx_custom_post_data_function(osal_u8 queue_id, osal_u8 stype, osal_u8 *buf, osal_u32 len,
                                          osal_u8 *user_param);
#endif
static INLINE__ osal_void frw_msg_init(osal_u8 *data, osal_u32 data_len, osal_u8 *rsp, osal_u32 rsp_buf_len,
    frw_msg *msg)
{
    msg->data_len = data_len;
    msg->data = data;
    msg->rsp = rsp;
    msg->rsp_buf_len = rsp_buf_len;
    msg->rsp_len = 0;
}

osal_s32 frw_send_msg_to_device(osal_u8 vap_id, osal_u16 msg_id, frw_msg *msg, osal_bool sync);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // __DMAC_COMMON_INC_ROM_H__
