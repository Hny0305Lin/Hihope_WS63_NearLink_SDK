/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx beat heart
 * This file should be changed only infrequently and with great care.
 */
#ifndef __DIAG_CMD_BEAT_HEART_H__
#define __DIAG_CMD_BEAT_HEART_H__

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_beat_heart(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_beat_heart_init(void);
void diag_beat_heart_process(void);
#endif
