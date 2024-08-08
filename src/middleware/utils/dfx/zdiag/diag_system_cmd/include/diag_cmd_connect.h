/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag connect cmd.
 * This file should be changed only infrequently and with great care.
 */
#ifndef __DIAG_CMD_CONNECT_H__
#define __DIAG_CMD_CONNECT_H__

#include "errcode.h"
#include "diag.h"
#include "diag_cmd_connect_st.h"

errcode_t diag_cmd_hso_connect_disconnect(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option);
#endif
