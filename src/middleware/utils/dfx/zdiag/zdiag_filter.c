/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: diag filter
 * This file should be changed only infrequently and with great care.
 */
#include "diag_filter.h"
#include "securec.h"
#include "diag_adapt_layer.h"
#include "dfx_adapt_layer.h"

#define ZDIAG_FILTER_GROUP_NUM 10
#define ZDIAG_FILTER_MAX_LEVEL 8
#define ZDIAG_STATE_CONNECT_NOTIFY_CNT 3

typedef struct {
    bool enable;
    uint8_t level;
    diag_addr tool_addr;
    uint32_t last_rev_time;
    uint16_t enable_id[ZDIAG_FILTER_GROUP_NUM];
} zdiag_filter_ctrl_t;

#if defined DIAG_FILTER_BSS_SECTION
STATIC DIAG_FILTER_BSS_SECTION zdiag_filter_ctrl_t g_zdiag_filter_ctrl;
#else
STATIC zdiag_filter_ctrl_t g_zdiag_filter_ctrl;
#endif

STATIC zdiag_filter_notify_hook g_zdiag_notify_process[ZDIAG_STATE_CONNECT_NOTIFY_CNT];

static inline zdiag_filter_ctrl_t *zdiag_get_filter_ctrl(void)
{
    return &g_zdiag_filter_ctrl;
}

__attribute__((weak)) void zdiag_level_proc(uint8_t level)
{
    unused(level);
}

__attribute__((weak)) void diag_highest_level_proc(uint8_t level)
{
    unused(level);
}

diag_addr zdiag_get_connect_tool_addr(void)
{
    zdiag_filter_ctrl_t *ctrl = zdiag_get_filter_ctrl();
    return ctrl->tool_addr;
}

void zdiag_set_enable(bool enable, diag_addr addr)
{
    int i;
    zdiag_filter_ctrl_t *ctrl = zdiag_get_filter_ctrl();
    dfx_log_debug("zdiag_set_enable %d\r\n", enable);
    memset_s(ctrl, sizeof(zdiag_filter_ctrl_t), 0x0, sizeof(zdiag_filter_ctrl_t));
    if (enable) {
        ctrl->enable = enable;
        ctrl->tool_addr = addr;
        g_zdiag_filter_ctrl.last_rev_time = dfx_get_cur_second();
    } else {
        memset_s(ctrl, sizeof(zdiag_filter_ctrl_t), 0x0, sizeof(zdiag_filter_ctrl_t));
    }
    for (i = 0; i < ZDIAG_STATE_CONNECT_NOTIFY_CNT; i++) {
        if (g_zdiag_notify_process[i] != NULL) {
            (void)g_zdiag_notify_process[i](enable);
        }
    }
}

bool zdiag_is_enable(void)
{
    zdiag_filter_ctrl_t *ctrl = zdiag_get_filter_ctrl();
    return ctrl->enable;
}

void zdiag_set_level_enable(uint8_t level, bool enable)
{
    zdiag_filter_ctrl_t *ctrl = zdiag_get_filter_ctrl();
    dfx_log_debug("zdiag_set_level_enable %d %d\r\n", level, enable);
    if (level >= ZDIAG_FILTER_MAX_LEVEL) {
        return;
    }

    if (enable) {
        ctrl->level |= (uint8_t)(1 << level);
    } else {
        ctrl->level &= (uint8_t)(~(1 << level));
    }

    zdiag_level_proc(ctrl->level);
}

void zdiag_set_id_enable(uint32_t id, bool enable)
{
    uint32_t i;
    int32_t free_idx = -1;
    int32_t match_idx = -1;

    zdiag_filter_ctrl_t *ctrl = zdiag_get_filter_ctrl();
    for (i = 0; i < ZDIAG_FILTER_GROUP_NUM; i++) {
        if (ctrl->enable_id[i] == id) {
            match_idx = (int32_t)i;
            break;
        }
        if (ctrl->enable_id[i] == 0 && free_idx == -1) {
            free_idx = (int32_t)i;
        }
    }
    dfx_log_debug("zdiag_set_id_enable %d %d %d %d\r\n", id, enable, match_idx, free_idx);

    if (match_idx != -1 && enable) {
        return;
    }

    if (match_idx != -1 && !enable) {
        ctrl->enable_id[match_idx] = 0;
        return;
    }

    if (free_idx != -1 && enable) {
        ctrl->enable_id[free_idx] = (uint16_t)id;
    }
    return;
}

bool zdiag_log_enable(uint8_t level, uint32_t id)
{
    uint32_t i;
    zdiag_filter_ctrl_t *ctrl = zdiag_get_filter_ctrl();
    if (!ctrl->enable) {
        dfx_log_debug("un_enable_a %d %d\r\n", level, id);
        return false;
    }

    if (level >= ZDIAG_FILTER_MAX_LEVEL) {
        dfx_log_debug("un_enable_b %d %d\r\n", level, id);
        return false;
    }

    if (((1 << level) & ctrl->level) == 0) {
        dfx_log_debug("un_enable_c %d %d\r\n", level, id);
        return false;
    }

    for (i = 0; i < ZDIAG_FILTER_GROUP_NUM; i++) {
        if (ctrl->enable_id[i] == id) {
            return true;
        }
    }
    dfx_log_debug("un_enable_e %d %d\r\n", level, id);
    return false;
}

void zdiag_filter_register_notify_hook(zdiag_filter_notify_hook hook)
{
    int i;
    for (i = 0; i < ZDIAG_STATE_CONNECT_NOTIFY_CNT; i++) {
        if (g_zdiag_notify_process[i] == NULL) {
            g_zdiag_notify_process[i] = hook;
            break;
        }
    }
}

void zdiag_state_beat_heart_pkt_recv(diag_addr peer_addr)
{
    if (g_zdiag_filter_ctrl.tool_addr == peer_addr) {
        g_zdiag_filter_ctrl.last_rev_time = dfx_get_cur_second();
    }
}

uint32_t zdiag_state_get_last_recv_time(diag_addr peer_addr)
{
    if (g_zdiag_filter_ctrl.tool_addr == peer_addr) {
        return g_zdiag_filter_ctrl.last_rev_time;
    }
    return 0;
}

void zdiag_filter_init(void)
{
    memset_s(&g_zdiag_filter_ctrl, sizeof(zdiag_filter_ctrl_t), 0, sizeof(zdiag_filter_ctrl_t));
}
