/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag msg
 * This file should be changed only infrequently and with great care.
 */

#include "diag_msg.h"
#include "zdiag_pkt_router.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"
#include "log_file.h"
#include "debug_print.h"

static bool g_offline_log_enable = false;

void uapi_zdiag_set_offline_log_enable(bool enable)
{
#if CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES
    if (g_offline_log_enable == enable) {
        return;
    }

    if (enable) {
        store_file_cfg_t cfg = {0};
        errcode_t ret;

        cfg.mult_files = DFX_OFFLINE_LOG_MUTI_FILE_NUM;
        cfg.cache_size = DFX_OFFLINE_LOG_CACHE_SIZE;
        cfg.enable_cache = DFX_OFFLINE_LOG_CACHE_ENABLE;
        cfg.file_size = DFX_OFFLINE_LOG_FILE_SIZE;
        cfg.path = DFX_OFFLINE_LOG_PATH;
        cfg.name = DFX_OFFLINE_LOG_NAME;

        ret = uapi_logfile_open(STORE_DIAG, (const store_file_cfg_t *)&cfg);
        if (ret != ERRCODE_SUCC && ret != ERRCODE_DFX_LOGFILE_ALREADY_OPEN) {
            dfx_log_err("open logfile %s failed. ret = 0x%x\r\n", cfg.name, ret);
            return;
        }
    } else {
        (void)uapi_logfile_close(STORE_DIAG);
    }
    g_offline_log_enable = enable;
#else
    unused(enable);
    g_offline_log_enable = false;
#endif
}

bool uapi_zdiag_offline_log_is_enable(void)
{
#if CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES
    return g_offline_log_enable;
#else
    return false;
#endif
}

errcode_t diag_msg_proc(uint16_t msg_id, uint8_t *msg, uint32_t msg_len)
{
    switch (msg_id) {
        case DFX_MSG_ID_DIAG_PKT:
            diag_pkt_msg_proc(msg_id, msg, (uint32_t)msg_len);
            return ERRCODE_SUCC;
        default:
            return ERRCODE_NOT_SUPPORT;
    }
}