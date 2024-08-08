/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: zdiag adpat psd
 * This file should be changed only infrequently and with great care.
 */
#include "diag.h"
#include "diag_adapt_psd.h"

psd_enable_callback g_psd_enable_callback = NULL;

errcode_t diag_cmd_psd_enable(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    psd_enable_callback psd_enable_cb = uapi_zdiag_psd_enable_get_cb();
    if (psd_enable_cb != NULL) {
        psd_enable_cb(cmd_id, cmd_param, cmd_param_size, option);
    }
    return ERRCODE_SUCC;
}

void uapi_zdiag_psd_enable_register(psd_enable_callback cb)
{
    g_psd_enable_callback = cb;
}

void uapi_zdiag_psd_enable_unregister(void)
{
    g_psd_enable_callback = NULL;
}

psd_enable_callback uapi_zdiag_psd_enable_get_cb(void)
{
    return g_psd_enable_callback;
}