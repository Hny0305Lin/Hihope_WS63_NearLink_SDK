/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag dfx
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_DFX_H__
#define __ZDIAG_DFX_H__

#include "errcode.h"
#include "zdiag_dfx_st.h"
#include "diag.h"

zdiag_dfx_stat_t *uapi_zdiag_get_dfx_stat(void);
errcode_t diag_dfx_cmd(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

static inline void diag_dfx_channel_receive_data(uint32_t chan_id, uint32_t cnt)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    if (chan_id < DIAG_DFX_SUPPORT_MAX_CHANNEL) {
        g_zdiag_dfx_stat->channel_receive_data_cnt[chan_id] += cnt;
    }
}

static inline void diag_dfx_channel_receive_frame(uint32_t chan_id, uint32_t cnt)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    if (chan_id < DIAG_DFX_SUPPORT_MAX_CHANNEL) {
        g_zdiag_dfx_stat->channel_receive_frame_cnt[chan_id] += cnt;
    }
}

static inline void diag_dfx_alloc_pkt(uint32_t pkt_type, uint32_t size)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    if (pkt_type < DIAG_DFX_SUPPORT_MAX_PKT_MEM_TYPE) {
        g_zdiag_dfx_stat->mem_pkt_alloc_size[pkt_type] += size;
    }
}

static inline void diag_dfx_free_pkt(uint32_t pkt_type, uint32_t size)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    if (pkt_type < DIAG_DFX_SUPPORT_MAX_PKT_MEM_TYPE) {
        g_zdiag_dfx_stat->mem_pkt_free_size[pkt_type] += size;
    }
}

static inline void zdiag_dfx_put_msg_2_cache_fail(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->put_msg_2_cache_fail_times++;
}

static inline void diag_dfx_msg_process_time(uint32_t time)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    if (g_zdiag_dfx_stat->dfx_msg_process_max_time < time) {
        g_zdiag_dfx_stat->dfx_msg_process_max_time = time;
    }
}

static inline void diag_transmit_msg_process_time(uint32_t time)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    if (g_zdiag_dfx_stat->transmit_msg_process_max_time < time) {
        g_zdiag_dfx_stat->transmit_msg_process_max_time = time;
    }
}

static inline void zdiag_dfx_send_local_q_success(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->send_local_q_success++;
}

static inline void zdiag_dfx_send_local_q_fail(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->send_local_q_fail++;
}

static inline void zdiag_dfx_rev_msg(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->msg_rev_cnt++;
}

static inline void diag_dfx_transmit_rev_msg(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->transmit_msg_rev_cnt++;
}

static inline void zdiag_dfx_rev_pkt_msg(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->diag_pkt_msg_rev_cnt++;
}

static inline void zdiag_dfx_rev_beat_herat_msg(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->beat_heart_msg_rev_cnt++;
}
static inline void zdiag_dfx_conn_except(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->conn_excep_cnt++;
}
static inline void zdiag_dfx_conn_bu_inc(void)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->conn_bu_cnt++;
}

static inline void diag_dfx_mem_alloc(uint32_t size)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->alloc_mem_size += size;
    if (g_zdiag_dfx_stat->alloc_mem_size > g_zdiag_dfx_stat->alloc_mem_peak_size) {
        g_zdiag_dfx_stat->alloc_mem_peak_size = g_zdiag_dfx_stat->alloc_mem_size;
    }
}

static inline void diag_dfx_mem_free(uint32_t size)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->alloc_mem_size -= size;
}

static inline void diag_dfx_current_msg_num(uint16_t msg_num)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->dfx_msg_q_num = msg_num;
    if (msg_num > g_zdiag_dfx_stat->dfx_msg_q_peak_num) {
        g_zdiag_dfx_stat->dfx_msg_q_peak_num = msg_num;
    }
}

static inline void diag_dfx_current_transmit_msg_num(uint16_t msg_num)
{
    zdiag_dfx_stat_t *g_zdiag_dfx_stat = uapi_zdiag_get_dfx_stat();
    g_zdiag_dfx_stat->transmit_msg_q_num = msg_num;
    if (msg_num > g_zdiag_dfx_stat->transmit_msg_q_peak_num) {
        g_zdiag_dfx_stat->transmit_msg_q_peak_num = msg_num;
    }
}

#endif
