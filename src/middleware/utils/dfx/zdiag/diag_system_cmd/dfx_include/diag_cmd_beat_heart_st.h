/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx beat heart st
 * This file should be changed only infrequently and with great care.
 */

#ifndef __DIAG_CMD_BEAT_HEART_ST_H__
#define __DIAG_CMD_BEAT_HEART_ST_H__

#include <stdint.h>

typedef struct {
    uint32_t dir;
    uint32_t random_data;
} diag_beat_heart_cmd_ind_t;

#define DIAG_HEART_BEAT_DIR_UP (0x1)
#define DIAG_HEART_BEAT_DIR_DOWN (0x2)
#endif