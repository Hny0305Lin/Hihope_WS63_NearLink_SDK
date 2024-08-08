/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At register header \n
 * Author:
 */

#ifndef AT_RADAR_CMD_REGISTER_H
#define AT_RADAR_CMD_REGISTER_H

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_RADAR_CMD_MAX_LEN   128

uint32_t uapi_at_radar_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num);
void at_radar_cmd_register(void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
