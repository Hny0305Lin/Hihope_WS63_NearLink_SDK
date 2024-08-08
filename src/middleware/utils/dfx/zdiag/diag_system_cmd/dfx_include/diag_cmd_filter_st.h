/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx filter st
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_CMD_FILTER_ST_H
#define DIAG_CMD_FILTER_ST_H

#include <stdint.h>

typedef struct {
    uint16_t module_id;     /* Module ID, when MSG is SYS or USR, mark the subcategories under the two categories */
    uint8_t is_dest_module; /* Is it the target module */
    uint8_t switch_code;    /* Turns the configuration on or off */
} diag_cmd_msg_cfg_req_stru_t;

#endif