/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_EFUSE_OPT_H
#define BOOT_EFUSE_OPT_H

#include "boot_uart_auth.h"

uint32_t loader_burn_efuse(const uart_ctx *cmd_ctx);
uint32_t loader_read_efuse(const uart_ctx *cmd_ctx);

#endif