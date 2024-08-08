/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag usr adapt
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_ADAPT_LAYER_H
#define DIAG_ADAPT_LAYER_H

#include "time.h"
#include "diag_config.h"
#include "diag_common.h"
#include "diag_channel.h"
#include "dfx_adapt_layer.h"

#define DIAG_HSO_MAX_MUX_PKT_SIZE 512
#define DIAG_HSO_VER_DEFAULT 0
#define DIAG_HSO_THIS_VER 0x11

#define USER_CMD_LIST_NUM 10 /* Maximum number of non-system command lists */

static inline uint32_t diag_adapt_get_msg_time(void)
{
    return (uint32_t)uapi_systick_get_ms();
}

static inline uint64_t diag_adapt_get_msg_time_ms(void)
{
    return uapi_systick_get_ms();
}

static inline uint32_t diag_adapt_get_msg_local_time(void)
{
    uint64_t time_ms = uapi_systick_get_ms();
    time_t time_s = (time_t)(time_ms / 1000); /* 1 second = 1000 milliseconds */

    struct tm tm = { 0 };
    if (localtime_r(&time_s, &tm) == NULL) {
        return 0;
    }

    uint32_t local_time = ((uint32_t)tm.tm_mon + 1) << 28; /* 28~31 bit: month */
    /* 1 day = 24 hours, 1 hour = 60 minutes, 1 minute = 60 seconds, 1 second = 100 * 10 milliseconds */
    local_time += (uint32_t)(((tm.tm_mday * 24 + tm.tm_hour) * 60 + tm.tm_min) * 60 + tm.tm_sec) * 100;
    local_time += (uint32_t)((time_ms / 10) % 100); /* 1 second = 100 * 10 milliseconds */
    return local_time;
}

static inline errcode_t diag_adapt_sync_tx_sem_wait(void)
{
    return ERRCODE_FAIL;
}

static inline void diag_adapt_sync_tx_sem_signal(void)
{
    return;
}

#ifdef SUPPORT_DIAG_V2_PROTOCOL
static inline uint8_t diag_adapt_get_default_dst(void)
{
    return DIAG_FRAME_FID_UART;
}

static inline diag_channel_id_t diag_adapt_addr_2_channel_id(diag_frame_fid_t addr)
{
    if (addr == DIAG_FRAME_FID_UART) {
        return DIAG_CHANNEL_ID_0;
    } else if (addr == DIAG_FRAME_FID_BLE_GATT) {
        return DIAG_CHANNEL_ID_1;
    }
    return DIAG_CHANNEL_ID_INVALID;
}

static inline diag_frame_fid_t diag_adapt_channel_id_2_addr(diag_channel_id_t id)
{
    if (id == DIAG_CHANNEL_ID_0) {
        return DIAG_FRAME_FID_UART;
    } else if (id == DIAG_CHANNEL_ID_1) {
        return DIAG_FRAME_FID_BLE_GATT;
    }
    return DIAG_FRAME_FID_MAX;
}

static inline diag_frame_fid_t diag_adapt_get_local_addr(void)
{
    return DIAG_FRAME_FID_LOCAL;
}

#else /* SUPPORT_DIAG_V2_PROTOCOL */

#define DIAG_LOCAL_ADDR             100
#define DIAG_UART_CONNECT_HSO_ADDR  200

static inline uint8_t diag_adapt_get_default_dst(void)
{
    return DIAG_UART_CONNECT_HSO_ADDR;
}

static inline diag_channel_id_t diag_adapt_dst_2_channel_id(diag_addr addr)
{
    if (addr == DIAG_UART_CONNECT_HSO_ADDR) {
        return DIAG_CHANNEL_ID_0;
    }
    return DIAG_CHANNEL_ID_INVALID;
}

static inline diag_addr diag_adapt_get_local_addr(void)
{
    return DIAG_LOCAL_ADDR;
}

static inline diag_addr_attribute_t diag_adapt_addr_2_attribute(diag_addr addr)
{
    if (addr == DIAG_UART_CONNECT_HSO_ADDR) {
        return DIAG_ADDR_ATTRIBUTE_VALID | DIAG_ADDR_ATTRIBUTE_HSO_CONNECT;
    } else if (addr == DIAG_LOCAL_ADDR) {
        return DIAG_ADDR_ATTRIBUTE_VALID;
    } else {
        return 0;
    }
}
#endif /* SUPPORT_DIAG_V2_PROTOCOL */

static inline bool diag_adapt_is_in_unblocking_context(void)
{
    return false;
}

#endif /* DIAG_ADAPT_LAYER_H */
