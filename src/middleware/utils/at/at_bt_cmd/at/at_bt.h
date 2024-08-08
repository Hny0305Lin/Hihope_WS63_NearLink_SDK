/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At bt header \n
 */

#ifndef __AT_BT_H__
#define __AT_BT_H__

#include "td_base.h"
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define EXT_RECONN_PERIOD_MIN 1

TD_CONST char *at_ssid_txt(TD_CONST td_u8 *ssid, td_u32 ssid_len);
td_void los_at_bt_cmd_register(td_void);
td_void los_at_sta_factory_test_cmd_register(td_void);
td_void los_at_mesh_cmd_register(td_void);

typedef enum {
    MPC_BT_SUCC = 0x00,
    MPC_BT_FAIL = 0x01,
} mpc_bt_err_code;

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of at_bt.h */
