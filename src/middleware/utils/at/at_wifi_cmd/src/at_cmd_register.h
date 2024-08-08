/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: AT commands interface.
 */

#ifndef __AT_CMD_H__
#define __AT_CMD_H__

#include <td_base.h>
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_CMD_MAX_LEN   128

td_u32 uapi_at_register_cmd(TD_CONST at_cmd_entry_t *cmd_tbl, td_u16 cmd_num);
td_void at_sys_cmd_register(td_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
