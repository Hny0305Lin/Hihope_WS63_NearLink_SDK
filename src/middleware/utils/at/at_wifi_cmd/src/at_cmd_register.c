/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
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

#include "at_cmd_register.h"

#include "at_ccpriv.h"
#include "at_wifi.h"
#include "at_register.h"
#include "at.h"
#include "at_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_u32 uapi_at_register_cmd(TD_CONST at_cmd_entry_t *cmd_tbl, td_u16 cmd_num)
{
    td_u32 ret = EXT_ERR_FAILURE;

    ret = uapi_at_cmd_table_register(cmd_tbl, (uint32_t)cmd_num, EXT_AT_CMD_MAX_LEN);
    if (ret != 0) {
        uapi_at_report_to_single_channel(0, "uapi_at_register_cmd \r\n");
    }

    return ret;
}

td_void at_sys_cmd_register(td_void)
{
    los_at_mfg_cmd_register();
    los_at_ccpriv_cmd_register();
    los_at_general_cmd_register();
    los_at_wifi_cmd_register();
    los_at_sta_factory_test_cmd_register();
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    los_at_mfg_ccpriv_cmd_register();
#endif
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
