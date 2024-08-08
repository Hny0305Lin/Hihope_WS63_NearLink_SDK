/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */
#include "diag_rom_api.h"
#include "errcode.h"

diag_rom_api_t g_diag_rom_api;

errcode_t uapi_diag_report_sys_msg(uint32_t module_id, uint32_t msg_id, const uint8_t *buf,
                                   uint16_t buf_size, uint8_t level)
{
    if (g_diag_rom_api.report_sys_msg != NULL) {
        return g_diag_rom_api.report_sys_msg(module_id, msg_id, buf, buf_size, level);
    }
    return ERRCODE_FAIL;
}

void diag_rom_api_register(const diag_rom_api_t *api)
{
    g_diag_rom_api = *api;
}