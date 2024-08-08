/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At register header \n
 */

#ifndef AT_BT_CMD_REGISTER_H
#define AT_BT_CMD_REGISTER_H

#include <td_base.h>
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_BT_CMD_MAX_LEN   128

td_u32 uapi_at_bt_register_cmd(TD_CONST at_cmd_entry_t *cmd_tbl, td_u16 cmd_num);
td_void at_bt_cmd_register(td_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
