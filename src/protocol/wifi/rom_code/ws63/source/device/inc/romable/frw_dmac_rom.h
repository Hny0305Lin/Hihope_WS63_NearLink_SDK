/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of dmac public interface.
 * Author:
 */

#ifndef FRW_DMAC_ROM_H
#define FRW_DMAC_ROM_H

#include "osal_types.h"
#include "oal_net_rom.h"
#include "dmac_ext_if_rom.h"
#include "frw_msg_rom.h"
#include "dmac_pkt_debug_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define frw_dbg(fmt, ...)

#define TIME_OUT_MS 2000

typedef osal_s32 (*dmac_frw_msg_callback)(dmac_vap_stru *dmac_vap, frw_msg *msg);
typedef struct {
    frw_msg msg;
    struct osal_list_head list;
    osal_s32 cb_return;
    osal_u16 msg_id;
    osal_u8 wait_cond : 1;
    osal_u8 sync : 1;
    osal_u8 wait_cond_thread : 1;
    osal_u8 pool_used : 1;
    osal_u8 pool_idx : 4;
    osal_u8 vap_id;
    osal_u16 time_out;
    osal_u16 seq;
    osal_atomic wait_fail;
} frw_msg_node;

typedef struct {
    struct osal_list_head list;
    osal_u32 cnt;
    osal_u32 seq;
    osal_spinlock lock;
} frw_msg_que;

typedef struct {
    dmac_frw_msg_callback msg_cb;
} frw_msg_item;

typedef struct {
    osal_u16 msg_id_min;
    osal_u16 msg_id_max;
    frw_msg_item *msg_table;
    frw_msg_que que[FRW_POST_PRI_NUM];
#ifdef _PRE_OS_VERSION_LITEOS
    osal_wait que_wait; /* 异步事件 */
#endif
} frw_ctrl;

/*
     约定：对于dmac侧而言，无论是配置消息，还是数据帧/管理帧：
           send： dmac -> wal/hmac
           post： dmac -> dmac
           rcv ： wal/hmac - > dmac
           对于数据帧/管理帧
           rx:    air -> dmac
           tx:    dmac -> air
*/

/*  msg_id：配置消息ID;
    用于从device侧发起的消息/配置，且消息/配置一定会被发往host侧
    d2h没有同步消息；
*/
osal_s32 frw_send_cfg_to_host(osal_u16 msg_id, osal_u8 vap_id, frw_msg *msg);

/* 用于wal->hmac的异步配置 */
osal_s32 dmac_frw_post_async_msg(osal_u16 msg_id, osal_u8 pri, osal_u8 vap_id, frw_msg *msg, osal_u32 seq);

/*
   netbuf: 数据帧报文指针，调用者申请，HCC释放；
   发送数据帧到hmac侧的接口
   调用者要填写优先级，cb域具体填写位置待定
*/
osal_u32 frw_dmac_msg_hook_register(osal_u16 msg_id, dmac_frw_msg_callback msg_cb);
osal_void frw_dmac_msg_hook_unregister(osal_u16 msg_id);

osal_s32 dmac_frw_netbuf_exec_callback(dmac_vap_stru *dmac_vap, osal_u8 data_type,
    osal_u16 netbuf_type, frw_msg *msg);

osal_u32 dmac_send_msg_to_host(osal_u8 vap_id, osal_u16 msg_id, osal_u8 *data, osal_u32 data_len);
osal_s32 dmac_frw_msg_exec_callback(osal_u16 msg_id, osal_u8 vap_id, frw_msg *msg);
osal_s32 dmac_hal_post_async_msg(osal_u16 msg_id, osal_u8 pri, osal_u8 vap_id, osal_u8 *data, osal_u32 data_len);
osal_s32 dmac_frw_msg_exec(osal_u16 msg_id, osal_u8 vap_id, osal_u8 *data, osal_u32 data_len);
frw_ctrl *get_dmac_frw_ctrl(osal_void);
osal_void frw_dump_stats(osal_void);

osal_void frw_thread_process_msg(osal_void);
#ifdef _PRE_OS_VERSION_LITEOS
osal_u8 frw_thread_condition_check(osal_void);
osal_wait *frw_thread_get_wait(osal_void);
#endif

osal_s32 dmac_config_flush_event_msg(dmac_vap_stru *dmac_vap, frw_msg *msg);
frw_msg_node* frw_fetch_msg_node(osal_void);
osal_void frw_free_msg_node(frw_msg_node *node);
osal_void frw_thread_process_msg_node(frw_msg_node *node, osal_bool drop);
osal_s32 frw_send_mgmt_to_host(oal_dmac_netbuf_stru *netbuf, osal_u32 buf_len, osal_u32 msg_type, osal_u32 vap_id);
osal_s32 frw_send_data_to_host(oal_dmac_netbuf_stru *netbuf, osal_u32 buf_len, osal_u32 msg_type, osal_u32 vap_id);
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
typedef osal_u32 (*frw_rx_msg_cb)(osal_u16 msg_id, osal_u8 vap_id, osal_u32 seq, osal_s32 dev_cb_ret, frw_msg *msg);
typedef osal_u32 (*frw_rx_netbuf_cb)(oal_dmac_netbuf_stru *netbuf, osal_u32 payload_len);
#endif
#define OSAL_FRW_CB_CONTINUE 1
typedef osal_u32 (*frw_recv_msg_callback)(frw_msg_adapt *msg_adapt);
typedef osal_u32 (*frw_hcc_flowctrl_type_callback)(osal_u8 data_type, osal_u16 *fc_flag, osal_u8 *queue_id);
typedef osal_s32 (*frw_msg_exec_callback_hook)(osal_u16 msg_id, osal_u8 vap_id, frw_msg *msg);
device_frw_stat *get_frw_device_stat_info(osal_void);
typedef osal_void (*frw_event_flush_cb)(osal_u16 event_type);
osal_u8 is_frw_queue_empty(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // FRW_DMAC_H
