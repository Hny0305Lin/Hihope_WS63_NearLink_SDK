/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: at_mfg.c
 */

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#include "at_wifi_mfg_cmd_talbe.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_CCPRIV_MAX_LEN 10

typedef enum {
    EXT_AT_MFG_SET_XO_TRIM_COARSE,
    EXT_AT_MFG_SET_XO_TRIM_FINE,
    EXT_AT_MFG_GET_XO_TRIM,
    EXT_AT_MFG_SET_FACTOR,
    EXT_AT_MFG_GET_FACTOR,
    EXT_AT_MFG_SET_CALI_POWER,
    EXT_AT_MFG_GET_RSSI_OFFSET,
    EXT_AT_MFG_SET_CURVE_PARAM,
    EXT_AT_MFG_GET_CURVE_PARAM,
    EXT_AT_MFG_SET_LOW_CURVE_PARAM,
    EXT_AT_MFG_GET_LOW_CURVE_PARAM,
    EXT_AT_MFG_SET_RSSI_OFFSET,
    EXT_AT_MFG_SET_TAR_POWER,
    EXT_AT_MFG_GET_TEMP,
    EXT_AT_MFG_SET_EFUSE_POWER,
    EXT_AT_MFG_GET_EFUSE_POWER,
    EXT_AT_MFG_SET_EFUSE_RSSI_COMP,
    EXT_AT_MFG_GET_EFUSE_RSSI_COMP,
    EXT_AT_MFG_SET_EFUSE_CMU_XO_TRIM,
    EXT_AT_MFG_GET_EFUSE_CMU_XO_TRIM,
    EXT_AT_MFG_EFUSE_RCALDATA,
    EXT_AT_MFG_EFUSE_REMAIN,
    EXT_AT_MFG_SET_EFUSE_TEMP,
    EXT_AT_MFG_GET_EFUSE_TEMP,

    EXT_AT_MFG_BUTT
} ext_at_mfg_type_enum;

static at_ret_t parse_at_cmd_with_param(const coarse_args_t *args, td_u32 cmd_type)
{
    td_u32 ret;
    td_s32 argc = convert_bin_to_dec(args->para_map);

    ret = uapi_wifi_mfg_ccpriv(argc, (td_char *)&args->para1, cmd_type);
    return (at_ret_t)ret;
}

static at_ret_t parse_at_cmd(td_u32 cmd_type)
{
    td_s32 argc = 0;
    td_u32 ret;
    td_char *argv[EXT_AT_CCPRIV_MAX_LEN] = {0};

    ret = uapi_wifi_mfg_ccpriv(argc, argv, cmd_type);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_cmu_xo_trim_coarse(const coarse_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_XO_TRIM_COARSE);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_cmu_xo_trim_fine(const fine_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_XO_TRIM_FINE);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_cmu_xo_trim(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_XO_TRIM);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_temp(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_TEMP);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_tar_power(const tarpower_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_TAR_POWER);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_cali_power(const calipower_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_CALI_POWER);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_curve_param(const highcurve_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_CURVE_PARAM);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_curve_param(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_CURVE_PARAM);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_low_curve_param(const lowcurve_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_LOW_CURVE_PARAM);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_low_curve_param(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_LOW_CURVE_PARAM);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_curve_factor(const factor_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_FACTOR);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_curve_factor(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_FACTOR);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_rssi_offset(const rssicomp_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_RSSI_OFFSET);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_rssi_offset(const getrssicomp_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_GET_RSSI_OFFSET);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_efuse_power_info(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_SET_EFUSE_POWER);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_efuse_power_info(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_EFUSE_POWER);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_efuse_rssi_offset(const efuserssi_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_EFUSE_RSSI_COMP);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_efuse_rssi_offset(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_EFUSE_RSSI_COMP);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_rcaldata(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_EFUSE_RCALDATA);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_efuse_temp(const efusetemp_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_SET_EFUSE_TEMP);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_efuse_temp(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_EFUSE_TEMP);
    return (at_ret_t)ret;
}

at_ret_t cmd_set_efuse_cmu_xo_trim(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_SET_EFUSE_CMU_XO_TRIM);
    return (at_ret_t)ret;
}

at_ret_t cmd_get_efuse_cmu_xo_trim(void)
{
    td_u32 ret;

    ret = parse_at_cmd(EXT_AT_MFG_GET_EFUSE_CMU_XO_TRIM);
    return (at_ret_t)ret;
}

at_ret_t cmd_efuse_remain(const remain_args_t *args)
{
    td_u32 ret;

    ret = parse_at_cmd_with_param(args, EXT_AT_MFG_EFUSE_REMAIN);
    return (at_ret_t)ret;
}

#define AT_CCPRIV_MFG_FUNC_NUM (sizeof(at_wifi_mfg_cmd_parse_table) / sizeof(at_wifi_mfg_cmd_parse_table[0]))

td_void los_at_mfg_ccpriv_cmd_register(td_void)
{
    uapi_at_report("los_at_mfg_ccpriv_cmd_register \r\n");
    uapi_at_register_cmd(at_wifi_mfg_cmd_parse_table, AT_CCPRIV_MFG_FUNC_NUM);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif