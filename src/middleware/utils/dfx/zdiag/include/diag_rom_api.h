/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag romable
 * This file should be changed only infrequently and with great care.
 */
#ifndef __DIAG_ROM_API_H__
#define __DIAG_ROM_API_H__
#include "diag.h"
typedef errcode_t (*diag_report_sys_msg_handler)(uint32_t module_id, uint32_t msg_id, const uint8_t *buf,
    uint16_t buf_size, uint8_t level);

typedef struct {
    diag_report_sys_msg_handler report_sys_msg;
} diag_rom_api_t;

void diag_rom_api_register(const diag_rom_api_t *api);
#endif