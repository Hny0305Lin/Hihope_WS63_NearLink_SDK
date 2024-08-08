/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: diag offline log enable
 */

#ifndef DIAG_CMD_OFFLINE_LOG_H
#define DIAG_CMD_OFFLINE_LOG_H

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_set_offline_log(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif /* DIAG_CMD_OFFLINE_LOG_H */