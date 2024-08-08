/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag dfx
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_DFX_ST_H__
#define __ZDIAG_DFX_ST_H__
#include <stdint.h>
#define DIAG_DFX_SUPPORT_MAX_CHANNEL        4
#define DIAG_DFX_SUPPORT_MAX_PKT_MEM_TYPE   2

typedef struct {
    uint32_t case_id;
    uint32_t data[3];
} diag_dfx_cmd_req_st_t;

typedef struct {
    uint32_t case_id;
    uint32_t data[3];
} diag_dfx_cmd_ind_st_t;

typedef struct {
    uint32_t put_msg_2_cache_fail_times;
    uint16_t dfx_msg_q_num;
    uint16_t dfx_msg_q_peak_num;
    uint16_t transmit_msg_q_num;
    uint16_t transmit_msg_q_peak_num;
    uint32_t dfx_msg_process_max_time;
    uint32_t transmit_msg_process_max_time;
    uint32_t send_local_q_fail;
    uint32_t send_local_q_success;
    uint32_t transmit_msg_rev_cnt;      /* transmit 消息接收总次数 */
    uint32_t msg_rev_cnt;               /* dfx 消息接收总次数 */
    uint32_t diag_pkt_msg_rev_cnt;      /* diag pkt 消息接收次数 */
    uint32_t beat_heart_msg_rev_cnt;    /* 心跳消息接收次数 */
    uint32_t channel_receive_data_cnt[DIAG_DFX_SUPPORT_MAX_CHANNEL];    /* 通道接收数据字节数 */
    uint32_t channel_receive_frame_cnt[DIAG_DFX_SUPPORT_MAX_CHANNEL];   /* 通道接收数据帧数 */
    uint32_t mem_pkt_alloc_size[DIAG_DFX_SUPPORT_MAX_PKT_MEM_TYPE];     /* 总计分配内存字节数 */
    uint32_t mem_pkt_free_size[DIAG_DFX_SUPPORT_MAX_PKT_MEM_TYPE];      /* 总计释放内存字节数 */
    uint32_t alloc_mem_size;            /* 当前使用内存字节数 */
    uint32_t alloc_mem_peak_size;       /* 使用内存峰值字节数 */
    uint32_t conn_excep_cnt;
    uint32_t conn_bu_cnt;
} zdiag_dfx_stat_t;
#endif
