/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx mem read write
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_CMD_MEM_READ_WRITE_H
#define DIAG_CMD_MEM_READ_WRITE_H

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_mem32(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_mem16(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_mem8(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_w1(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_w2(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_w4(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_mem_operate(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
bool diag_cmd_permit_read(uintptr_t start_addr, uintptr_t end_addr);
bool diag_cmd_permit_write(uintptr_t start_addr, uintptr_t end_addr);
#endif