/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: get resource info function
 */
#ifndef DIAG_CMD_GET_RES_INFO_H
#define DIAG_CMD_GET_RES_INFO_H

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_get_res_info(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size,
                                diag_option_t *option);
#endif // !DIAG_CMD_GET_RES_INFO_H