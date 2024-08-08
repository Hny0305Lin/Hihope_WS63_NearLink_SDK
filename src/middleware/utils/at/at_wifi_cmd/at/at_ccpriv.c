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

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "at_ccpriv.h"

#include <string.h>

#include "at.h"
#include "at_utils.h"
#include "at_cmd_register.h"
#include "at_wifi_ccpriv_table.h"
#include "at_ccpriv_register.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define los_unref_param(P)  ((P) = (P))
#define EXT_AT_CCPRIV_MAX_LEN      10

typedef enum {
    EXT_AT_AL_TX,
    EXT_AT_AL_RX,
    EXT_AT_RX_INFO,
    EXT_AT_SET_COUNTRY,
    EXT_AT_GET_COUNTRY,
    EXT_AT_SET_WLAN0_BW,
    EXT_AT_SET_AP0_BW,
    EXT_AT_SET_MESH0_BW,
    EXT_AT_GET_WLAN0_MESHINFO,
    EXT_AT_GET_MESH0_MESHINFO,
    EXT_AT_SET_TPC,
    EXT_AT_SET_TRC,
    EXT_AT_SET_RATE,
    EXT_AT_SET_ARLOG,
    EXT_AT_GET_VAP_INFO,
    EXT_AT_GET_USR_INFO,
    EXT_AT_SET_RATE_POWER,
    EXT_AT_CAL_TONE,

    EXT_AT_TYPE_BUTT
}ext_at_type_enum;
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*****************************************************************************
  3 函数实现
*****************************************************************************/
at_ret_t at_los_wifi_al_tx(const altx_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_AL_TX);
    return (at_ret_t)ret;
}

at_ret_t at_los_wifi_al_rx(const alrx_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_AL_RX);
    return (at_ret_t)ret;
}

at_ret_t at_los_wifi_rx_info(void)
{
    td_s32 argc = 0;
    td_u32 ret;
    td_char *argv[EXT_AT_CCPRIV_MAX_LEN] = {0};

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, argv, EXT_AT_RX_INFO);
    return (at_ret_t)ret;
}

at_ret_t at_los_wifi_ccpriv(const ccpriv_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_ccpriv\r\n");

    ret = uapi_wifi_ccpriv(argc, (td_char *)&args->para1);
    return (at_ret_t)ret;
}

TD_PRV td_u32 at_uapi_wifi_set_country(td_s32 argc, TD_CONST td_char *argv[])
{
    if (at_param_null_check(argc, argv) == EXT_ERR_FAILURE) {
        return EXT_ERR_FAILURE;
    }
    td_u32 ret = uapi_wifi_at_start(argc, argv, EXT_AT_SET_COUNTRY);
    return ret;
}

TD_PRV td_u32 at_uapi_wifi_get_country(td_s32 argc, TD_CONST td_char *argv[])
{
    td_u32 ret = uapi_wifi_at_start(argc, argv, EXT_AT_GET_COUNTRY);
    return ret;
}

at_ret_t at_los_wifi_set_arlog(const arlog_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_SET_ARLOG);
    return ret;
}

at_ret_t at_los_wifi_get_vap_info(const vapinfo_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_GET_VAP_INFO);
    return ret;
}

at_ret_t at_los_wifi_get_usr_info(const usrinfo_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_GET_USR_INFO);
    return ret;
}

at_ret_t at_los_wifi_set_rate_power(const setrpwr_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_SET_RATE_POWER);
    return ret;
}

at_ret_t at_los_wifi_cal_tone(const caltone_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_u32 ret;

    uapi_at_report("uapi_wifi_at_start \r\n");

    ret = uapi_wifi_at_start(argc, (td_char *)&args->para1, EXT_AT_CAL_TONE);
    return ret;
}

#define AT_MFG_FUNC_NUM (sizeof(at_wifi_mfg_parse_table) / sizeof(at_wifi_mfg_parse_table[0]))

td_void los_at_mfg_cmd_register(td_void)
{
    uapi_at_report("los_at_mfg_cmd_register \r\n");
    uapi_at_register_cmd(at_wifi_mfg_parse_table, AT_MFG_FUNC_NUM);
}

td_void los_at_ccpriv_cmd_register(td_void)
{
#if defined(AT_REGIST_CCPRIV)
    uapi_at_report("los_at_ccpriv_cmd_register \r\n");
    uapi_at_register_cmd(at_wifi_ccpriv_parse_table, 1);
#endif
}
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

