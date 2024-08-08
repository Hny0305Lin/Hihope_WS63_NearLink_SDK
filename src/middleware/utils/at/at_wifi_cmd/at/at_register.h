/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __AT_REGISTER_H__
#define __AT_REGISTER_H__

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_DEFAULT_IFNAME_LOCALHOST    "lo"
#define EXT_DEFAULT_IFNAME_AP           "ap0"
#define EXT_DEFAULT_IFNAME_STA          "wlan0"
#define EXT_DEFAULT_IFNAME_MESH         "mesh0"

td_void los_at_general_cmd_register(td_void);
td_u32 at_wifi_ifconfig(td_s32 argc, TD_CONST td_char **argv);
td_u32 osShellDhcps(td_s32 argc, TD_CONST td_char **argv);
td_void at_send_recv_data(td_char *data);
td_void at_sys_cmd_register(td_void);

extern td_u32 get_rf_cmu_pll_param(td_s16 *high_temp, td_s16 *low_temp, td_s16 *compesation);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
