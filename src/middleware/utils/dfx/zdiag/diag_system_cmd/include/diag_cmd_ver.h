/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag ver cmd.
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_CMD_VER_H__
#define __DIAG_CMD_VER_H__

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_ver(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
#endif
