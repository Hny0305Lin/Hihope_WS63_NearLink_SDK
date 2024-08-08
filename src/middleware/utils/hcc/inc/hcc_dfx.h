/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc dfx completion.
 * Author:
 * Create: 2021-05-13
 */

#ifndef HCC_DFX_HEADER
#define HCC_DFX_HEADER

#include "td_type.h"
#include "hcc_comm.h"
#include "hcc_cfg_comm.h"
#include "hcc_types.h"
#include "hcc_cfg_comm.h"
#include "soc_osal.h"

typedef struct _hcc_service_stat_ {
    /* 业务申请内存成功、失败的总次数, 业务内存只有RX方向需要hcc申请 */
    td_u32 alloc_succ_cnt; /* 回调业务alloc返回成功次数 */
    td_u32 alloc_cb_cnt;   /* 回调业务alloc次数 */
    td_u32 alloc_fail_cnt; /* 申请内存失败的次数: 其中包含参数错误引起未调用回调场景 */

    /* 回调业务free的次数 */
    td_u32 free_cnt;

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
    /* 回调业务start_subq的次数 */
    td_u32 start_subq_cnt;
    /* 回调业务stop_subq的次数 */
    td_u32 stop_subq_cnt;
#endif
    /* 回调业务 rx_proc 统计 */
    td_u32 rx_cnt; /* 调用次数 */
    td_u32 exp_rx_cnt; /* 实际应调用次数 */

    td_u32 bus_tx_succ; /* 通道发送成功包数量 */

    td_u32 rx_err_cnt;
} hcc_service_stat;

typedef struct _hcc_queue_stat_ {
    /* 队列申请内存成功、失败次数 */
    /* 释放次数无法记录，数据可能会不准确，暂不记录 */
    td_u32 alloc_succ_cnt;
    td_u32 alloc_fail_cnt;

    td_u32 total_pkts;  // 当前队列处理帧的数量，成功传递给对端 或 成功传递给业务的包数量
    td_u32 loss_pkts; // 当前队列丢帧数量
} hcc_queue_stat;

typedef struct _hcc_bus_stat_ {
    td_u32 total_tx_pkts;  // 发送成功的数量
    td_u32 loss_tx_pkts; // 发送丢帧数量
} hcc_bus_stat;

typedef struct _hcc_inner_stat_ {
    td_u32 unc_alloc_succ[HCC_DIR_COUNT];
    td_u32 unc_alloc_fail[HCC_DIR_COUNT];
    td_u32 unc_free_cnt;
    td_u32 mem_free_cnt;
    td_u32 srv_rx_cnt;
    td_u16 tx_fail_exit_cnt;
    td_u16 proc_packet_overnum_cnt;
} hcc_inner_stat;

typedef struct _hcc_dfx_ {
    osal_spinlock hcc_dfx_lock;
#ifdef CONFIG_HCC_SUPPORT_DFX_SRV
    hcc_service_stat *service_stat;
#endif
    hcc_queue_stat *queue_stat[HCC_DIR_COUNT];
    hcc_inner_stat inner_stat;
    td_u8 que_max_cnt;
    td_u8 srv_max_cnt;
    td_u16 rsv;
} hcc_dfx;

td_void hcc_proc_overnum_inc(td_void *hcc);
td_void hcc_tx_fail_inc(td_void *hcc);
td_void hcc_dfx_queue_info_print(td_u8 chl, hcc_queue_dir dir, hcc_queue_type q_id);
td_void hcc_dfx_service_info_print(td_u8 chl, hcc_service_type service_type);
td_void hcc_bus_dfx_statics_print(td_u8 chl);
#endif /* HCC_DFX_HEADER */
