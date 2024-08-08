/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At plt function \n
 */

#include <string.h>

#include "gpio.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"
#include "watchdog.h"
#ifdef CONFIG_MIDDLEWARE_SUPPORT_FTM
#include "factory.h"
#endif
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#include "factory.h"
#include "sfc.h"
#include "xo_trim_porting.h"
#endif
#ifdef CONFIG_DRIVER_SUPPORT_TSENSOR
#include "tsensor.h"
#endif
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
#include "idle_config.h"
#endif
#include "at_cmd.h"
#include "at_utils.h"
#include "at_plt_cmd_register.h"
#include "at_plt_cmd_table.h"
#include "debug_print.h"
#include "time64.h"
#include "version_porting.h"
#include "hal_reboot.h"
#include "at_plt.h"

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
#include "nv.h"
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
#include "upg_porting.h"
#include "upg_ab.h"
#endif
#include "efuse.h"
#include "efuse_porting.h"
#ifdef CONFIG_DFX_SUPPORT_PRINT
#include "dfx_print.h"
#endif
#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
#include "exception.h"
#endif

#define CONVERT_HALF 2
static td_s32 at_plt_convert_bin_to_dec(td_s32 pbin)
{
    td_s32 result = 0;
    td_s32 temp = pbin;

    while (temp != 0) {
        result += temp % CONVERT_HALF;
        temp /= CONVERT_HALF;
    }

    return result;
}

TD_PRV td_u32 at_plt_check_mac_elem(TD_CONST td_char elem)
{
    if (elem >= '0' && elem <= '9') {
        return EXT_ERR_SUCCESS;
    } else if (elem >= 'A' && elem <= 'F') {
        return EXT_ERR_SUCCESS;
    } else if (elem >= 'a' && elem <= 'f') {
        return EXT_ERR_SUCCESS;
    } else if (elem == ':') {
        return EXT_ERR_SUCCESS;
    }

    return EXT_ERR_FAILURE;
}

static td_u32 at_plt_cmd_strtoaddr(TD_CONST td_char *param, td_uchar *mac_addr, td_u32 addr_len)
{
    td_u32 cnt;
    td_char *tmp1 = (td_char *)param;
    td_char *tmp2 = TD_NULL;
    td_char *tmp3 = TD_NULL;

    for (cnt = 0; cnt < 17; cnt++) {    /* 17 */
        if (at_plt_check_mac_elem(param[cnt]) != EXT_ERR_SUCCESS) {
            return EXT_ERR_FAILURE;
        }
    }

    for (cnt = 0; cnt < (addr_len - 1); cnt++) {
        tmp2 = (char*)strsep(&tmp1, ":");
        if (tmp2 == TD_NULL) {
            return EXT_ERR_AT_INVALID_PARAMETER;
        }
        mac_addr[cnt] = (td_uchar)strtoul(tmp2, &tmp3, 16); /* 16 */
    }

    if (tmp1 == TD_NULL) {
        return EXT_ERR_AT_INVALID_PARAMETER;
    }
    mac_addr[cnt] = (td_uchar)strtoul(tmp1, &tmp3, 16); /* 16 */
    return EXT_ERR_SUCCESS;
}

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
#define AT_PLT_NV_WRITE_MAX_LENGTH 1024
typedef enum {
    PLT_NV_ATTR_NORMAL = 0,
    PLT_NV_ATTR_PERMANENT = 1,
    PLT_NV_ATTR_ENCRYPTED = 2,
    PLT_NV_ATTR_NON_UPGRADE = 4
} plt_nv_attr;
STATIC uint32_t plt_nv_get_value(const nvwrite_args_t *args, uint8_t *value);
#endif
#define  DATE_BASE_YEAR     1900
#define  leap_year(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
static const int32_t g_mon_lengths[2][12] = { /* 2: 2 Column,Contains leap year; 12: 12 months */
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

#define AT_PLT_FUNC_NUM (sizeof(at_plt_cmd_parse_table) / sizeof(at_plt_cmd_parse_table[0]))

void los_at_plt_cmd_register(void)
{
    print_str("los_at_plt_cmd_register EXCUTE\r\n");
    uapi_at_plt_register_cmd(at_plt_cmd_parse_table, AT_PLT_FUNC_NUM);
}

at_ret_t plt_nv_read(const nvread_args_t *args)
{
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    uint16_t nv_value_length = 0;
    uint8_t nv_value[AT_PLT_NV_WRITE_MAX_LENGTH] = {0};
    nv_key_attr_t nv_attr = {0};
    errcode_t ret = uapi_nv_read_with_attr((uint16_t)args->key_id, AT_PLT_NV_WRITE_MAX_LENGTH, &nv_value_length,
        nv_value, &nv_attr);
    if (ret != ERRCODE_SUCC) {
        print_str("plt_nv_read failed, ret = [0x%x]\r\n", ret);
        return ret;
    }
    if (nv_attr.permanent == true) {
        print_str("NV[0x%x] is permanent\r\n", args->key_id);
    }
    if (nv_attr.encrypted == true) {
        print_str("NV[0x%x] is encrypted\r\n", args->key_id);
    }
    if (nv_attr.non_upgrade == true) {
        print_str("NV[0x%x] is non_upgrade\r\n", args->key_id);
    }
    for (int i = 0; i < nv_value_length; i++) {
        print_str("nv_value[%d] = [0x%x]\r\n", i, nv_value[i]);
    }
#else
    unused(args);
#endif
    return AT_RET_OK;
}

#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
STATIC uint32_t plt_nv_get_value(const nvwrite_args_t *args, uint8_t *value)
{
    errno_t ret;
    td_u8 *tmp = TD_NULL;
    tmp = (td_u8 *)malloc(args->length);
    if (tmp == TD_NULL) {
        free(tmp);
        return AT_RET_SYNTAX_ERROR;
    }

    at_str_to_hex((char *)(args->value), args->length + args->length, tmp); /* 2:偏移2个字符 */
    ret = memcpy_s(value, AT_PLT_NV_WRITE_MAX_LENGTH + 1, tmp, args->length + 1);
    free(tmp);
    if (ret != EOK) {
        return EXT_ERR_MEMCPYS_FAIL;
    }
    return EXT_ERR_SUCCESS;
}
#endif
at_ret_t plt_nv_write(const nvwrite_args_t *args)
{
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    if (args->length == 0) {
        return AT_RET_CMD_PARA_ERROR;
    }
    uint8_t write_value[AT_PLT_NV_WRITE_MAX_LENGTH] = {0};
    if (plt_nv_get_value(args, write_value) != EXT_ERR_SUCCESS) {
        return AT_RET_CMD_PARA_ERROR;
    }
    nv_key_attr_t nv_attr = {0};
    switch (args->attr) {
        case PLT_NV_ATTR_PERMANENT:
            nv_attr.permanent = 1;
            break;
        case PLT_NV_ATTR_ENCRYPTED:
            nv_attr.encrypted = 1;
            break;
        case PLT_NV_ATTR_NON_UPGRADE:
            nv_attr.non_upgrade = 1;
            break;
        default:
            break;
    }
    errcode_t ret = uapi_nv_write_with_attr((uint16_t)args->key_id, write_value, (uint16_t)args->length, &nv_attr,
        NULL);
    if (ret != ERRCODE_SUCC) {
        print_str("plt_nv_write failed, ret = [0x%x]\r\n", ret);
        return ret;
    }
    for (uint32_t i = 0; i < args->length; i++) {
        print_str("nv_value[%d] = [0x%x]\r\n", i, write_value[i]);
    }
#else
    unused(args);
#endif
    return AT_RET_OK;
}

#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
#define UPG_AB_REGION_CFG_ADDR 0x3FF000
#define UPG_AB_REGION_CFG_SIZE 0x1000
#define UPG_AB_CONFIG_CHECK 0x70746C6C
typedef struct {
    uint32_t  check_num;    /* check number */
    uint32_t  run_region;   /* run region */
} upg_ab_config_t;
#endif
at_ret_t upg_ab_set_run_region(const absetrun_args_t *args)
{
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
    errcode_t ret  = ws63_upg_init();
    if (ret != ERRCODE_SUCC && ret != ERRCODE_UPG_ALREADY_INIT) {
        print_str("ab upg set run region init failed, ret : 0x%x\r\n", ret);
    }
    upg_ab_config_t config = {0};
    ret = upg_flash_read(UPG_AB_REGION_CFG_ADDR, sizeof(upg_ab_config_t), (uint8_t *)&config);
    if (ret != ERRCODE_SUCC || config.check_num != UPG_AB_CONFIG_CHECK) {
        print_str("ab upg set run region read failed, ret : 0x%x\r\n", ret);
        return AT_RET_PROGRESS_BLOCK;
    }
    config.run_region = args->run_region;
    ret = upg_flash_write(UPG_AB_REGION_CFG_ADDR, sizeof(upg_ab_config_t), (const uint8_t *)&config, true);
    if (ret != ERRCODE_SUCC) {
        print_str("ab upg set run region write failed, ret : 0x%x\r\n", ret);
        return ret;
    }
#else
    unused(args);
#endif
    return AT_RET_OK;
}

#define MAC_ADDR_EFUSE 0
#define MAC_ADDR_NV 1
#define SLE_MAC_ADDR 2
#define SET_EFUSE_MAC_PARAM_CNT 2
#ifndef WIFI_MAC_LEN
#define WIFI_MAC_LEN 6
#endif

/*****************************************************************************
 功能描述  :设置efuse mac地址
*****************************************************************************/
at_ret_t set_efuse_mac_addr(const efusemac_args_t *args)
{
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    td_uchar mac_addr[WIFI_MAC_LEN] = {0};

    if (argc != SET_EFUSE_MAC_PARAM_CNT || strlen((const char *)args->mac_addr) != 17) { /* 17 mac string len */
        return AT_RET_SYNTAX_ERROR;
    }

    td_u32 ret = at_plt_cmd_strtoaddr((const char *)args->mac_addr, mac_addr, WIFI_MAC_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    if ((mac_addr[0] & 0x1) == 0x1) {
        osal_printk("set mac error: multicast mac addr not aviable!!\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
    switch (args->mac_type) {
        case MAC_ADDR_EFUSE:
            ret = efuse_write_mac(mac_addr, WIFI_MAC_LEN);
            if (ret != ERRCODE_SUCC) {
                osal_printk("SET EFUSE MAC ERROR, ret : 0x%x\r\n", ret);
                return AT_RET_CMD_PARA_ERROR;
            }
            break;
        case SLE_MAC_ADDR:
            ret = efuse_write_sle_mac(mac_addr, WIFI_MAC_LEN);
            if (ret != ERRCODE_SUCC) {
                osal_printk("SET SLE EFUSE MAC ERROR, ret : 0x%x\r\n", ret);
                return AT_RET_CMD_PARA_ERROR;
            }
            break;
        case MAC_ADDR_NV:
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
            ret = uapi_nv_write(NV_ID_SYSTEM_FACTORY_MAC, mac_addr, WIFI_MAC_LEN);
            if (ret != ERRCODE_SUCC) {
                osal_printk("SET NV MAC ERROR, ret : 0x%x\r\n", ret);
                return AT_RET_CMD_PARA_ERROR;
            }
#else
            return AT_RET_CMD_PARA_ERROR;
#endif
            break;
        default:
            return AT_RET_CMD_PARA_ERROR;
            break;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
 功能描述  :获取efuse mac地址
*****************************************************************************/
at_ret_t get_efuse_mac_addr(void)
{
    td_uchar mac_addr[WIFI_MAC_LEN] = {0};
    td_uchar null_mac_addr[WIFI_MAC_LEN] = {0};
    td_uchar efuse_left_count = 0;
    errcode_t ret;

#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
    uint16_t nv_mac_length;
    ret = uapi_nv_read(NV_ID_SYSTEM_FACTORY_MAC, WIFI_MAC_LEN, &nv_mac_length, mac_addr);
    if (ret != ERRCODE_SUCC || nv_mac_length != WIFI_MAC_LEN) {
        osal_printk("GET NV MAC ERROR, ret : 0x%x\r\n", ret);
    }
    osal_printk("+EFUSEMAC: NV MAC " EXT_AT_MACSTR "\r\n", ext_at_mac2str(mac_addr));
#endif
    ret = efuse_read_mac(mac_addr, WIFI_MAC_LEN, &efuse_left_count);
    if (ret == ERRCODE_SUCC) {
        osal_printk("+EFUSEMAC: EFUSE MAC " EXT_AT_MACSTR "\r\n", ext_at_mac2str(mac_addr));
    } else {
        osal_printk("+EFUSEMAC: EFUSE MAC " EXT_AT_MACSTR "\r\n", ext_at_mac2str(null_mac_addr));
    }
    osal_printk("+EFUSEMAC: Efuse mac chance(s) left: %d times.\r\n", efuse_left_count);

    ret = efuse_read_sle_mac(mac_addr, WIFI_MAC_LEN);
    if (ret == ERRCODE_SUCC) {
        osal_printk("+EFUSEMAC: EFUSE SLE MAC " EXT_AT_MACSTR "\r\n", ext_at_mac2str(mac_addr));
    } else {
        osal_printk("+EFUSEMAC: EFUSE SLE MAC " EXT_AT_MACSTR "\r\n", ext_at_mac2str(null_mac_addr));
    }
    osal_printk("OK\r\n");

    return AT_RET_OK;
}

#define BOOT_PORTING_RESET_REG      0x40002110
#define BOOT_PORTING_RESET_VALUE    0x4

at_ret_t plt_reboot(void)
{
    hal_reboot_chip();
    return AT_RET_OK;
}

at_ret_t at_query_ver_cmd(void)
{
    print_version();
    return AT_RET_OK;
}

at_ret_t at_query_tsensor_temp(void)
{
#ifdef CONFIG_DRIVER_SUPPORT_TSENSOR
    td_u32 ret;
    td_u32 i;
    td_s8 temp = 0;

    for (i = 0; i < 3; i++) { /* loop 3 times */
        ret = uapi_tsensor_get_current_temp(&temp);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
    }

    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("+RDTEMP:ret0x%x.\r\n", ret);
        return ret;
    }

    osal_printk("+RDTEMP:%d\r\n", temp);
#endif
    return AT_RET_OK;
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
static td_u32 get_rf_cmu_pll_param(const td_s16 *high_temp, const td_s16 *low_temp, const td_s16 *compesation)
{
    unused(high_temp);
    unused(low_temp);
    unused(compesation);
    cmu_xo_trim_temp_comp_print();
    return EXT_ERR_SUCCESS;
}
#endif

at_ret_t at_query_xtal_compesation(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_u32 ret;
    td_s16 high_temp_threshold = 0;
    td_s16 low_temp_threshold = 0;
    td_s16 pll_compesation = 0;

    ret = get_rf_cmu_pll_param(&high_temp_threshold, &low_temp_threshold, &pll_compesation);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    osal_printk("+XTALCOM:%d,%d,%d\r\n", high_temp_threshold, low_temp_threshold, pll_compesation);
#endif

    return AT_RET_OK;
}

at_ret_t at_factory_erase(void)
{
#ifdef CONFIG_MIDDLEWARE_SUPPORT_FTM
    errcode_t ret;
    mfg_factory_config_t factory_mode_cfg;

    memset_s(&factory_mode_cfg, sizeof(factory_mode_cfg), 0, sizeof(factory_mode_cfg));
    ret = mfg_flash_read((uint8_t *)(&factory_mode_cfg), sizeof(mfg_factory_config_t));
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    if (factory_mode_cfg.factory_mode != 0x0) {
        osal_printk("factory mode, can not erase\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
    ret = mfg_flash_erase();
    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("at_factory_erase:: uapi_flash_erase failed, ret :%x\r\n", ret);
        return ret;
    }
    /* set normal mode after erase factory. inorder to let start success after reboot. */
    factory_mode_cfg.factory_mode = 0x0;
    factory_mode_cfg.factory_valid = MFG_FACTORY_INVALID;
    ret = mfg_flash_write((const uint8_t *)(&factory_mode_cfg), sizeof(factory_mode_cfg));
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    osal_printk("+FTMERASE:erase addr:0x%x, size:0x%x OK.\r\n", factory_mode_cfg.factory_addr_start,
        factory_mode_cfg.factory_size);
#endif

    return AT_RET_OK;
}

at_ret_t at_factory_mode_read(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    osal_printk("factory mode\r\n");
#else
    osal_printk("non_factory mode\r\n");
#endif
    return AT_RET_OK;
}

#ifdef CONFIG_MIDDLEWARE_SUPPORT_FTM

static td_u32 at_setup_factory_mode_switch(td_s32 argc, const factory_mode_args_t *args)
{
    errcode_t ret;
    mfg_factory_config_t factory_mode_cfg;
    mfg_region_config_t img_info;
    osal_printk("+FTM SWITCH: start\r\n");

    if (argc != 1) {
        return AT_RET_SYNTAX_ERROR;
    }

    /* switch_mode:0 normal_mode;  switch_mode:1 factory_test_mode */
    td_u8 switch_mode = (td_u8)(args->para1);
    if (switch_mode != 0x0 && switch_mode != 0x1) {
        return AT_RET_SYNTAX_ERROR;
    }

    memset_s(&factory_mode_cfg, sizeof(factory_mode_cfg), 0, sizeof(factory_mode_cfg));
    memset_s(&img_info, sizeof(img_info), 0, sizeof(img_info));
    ret = mfg_get_region_config(&img_info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = mfg_flash_read((uint8_t *)(&factory_mode_cfg), sizeof(mfg_factory_config_t));
    if (ret != ERRCODE_SUCC) {
        osal_printk("at_setup_factory_mode_switch:: read failed, ret :%x ", ret);
        return ERRCODE_FAIL;
    }

    ret = mfg_factory_mode_switch(img_info, switch_mode, &factory_mode_cfg);
    if (ret != ERRCODE_SUCC) {
        osal_printk("at_setup_factory_mode_switch:: mfg_factory_mode_switch failed, ret :%x ", ret);
        return ERRCODE_FAIL;
    }

    ret = mfg_flash_write((const uint8_t *)(&factory_mode_cfg), sizeof(factory_mode_cfg));
    if (ret != ERRCODE_SUCC) {
        osal_printk("ftm config init write failed, ret :%x ", ret);
        return ERRCODE_FAIL;
    }
    osal_printk("FTM SWTICH:0x%x, size:0x%x OK.\r\n", factory_mode_cfg.factory_addr_switch,
        factory_mode_cfg.factory_switch_size);
    return AT_RET_OK;
}
#endif

at_ret_t at_factory_mode_switch(const factory_mode_args_t *args)
{
#ifdef CONFIG_MIDDLEWARE_SUPPORT_FTM
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_factory_mode_switch(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
// start of at cmd: uart log level

static td_u32 at_setup_loglevel_cmd(td_s32 argc, const loglevel_args_t *args)
{
#ifdef CONFIG_DFX_SUPPORT_PRINT
    td_u8 log_level;

    if (argc == 0) { /* argc 0 */
        osal_printk("+LOGL:%u\r\n", dfx_print_get_level());
        return AT_RET_OK;
    }
    if (argc != 1) { /* argc 1 */
        return AT_RET_SYNTAX_ERROR;
    }

    log_level = (td_u8)(args->para1);
    if (log_level >= DFX_PRINT_LEVEL_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    dfx_print_set_level(log_level);
    osal_printk("+LOGL:%d\r\n", log_level);
#else
    unused(argc);
    unused(args);
#endif
    return AT_RET_OK;
}
#endif

at_ret_t at_get_log_level(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    if (at_setup_loglevel_cmd(0, NULL) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    return AT_RET_OK;
}

at_ret_t at_set_log_level(const loglevel_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_loglevel_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
#define UAPI_MAX_SLEEP_MODE 2

td_u8 g_sleep_mode;

static errcode_t uapi_set_sleep_mode(td_u8 sleep_mode)
{
    g_sleep_mode = sleep_mode;
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    bool open_pm = (g_sleep_mode == 0) ? false : true;
    idle_set_open_pm(open_pm);
#endif
    return ERRCODE_SUCC;
}

static td_u8 uapi_get_sleep_mode(td_void)
{
    return g_sleep_mode;
}

static td_u32 at_setup_sleepmode_cmd(td_s32 argc, const sleepmode_args_t *args)
{
    td_u8 sleep_mode;
    td_u32 ret;

    if (argc != 1) { /* argc 2 */
        osal_printk("+SLP:%d\r\n", uapi_get_sleep_mode());
        return AT_RET_OK;
    }

    sleep_mode = (td_u8)(args->para1);
    if (sleep_mode > UAPI_MAX_SLEEP_MODE) {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = uapi_set_sleep_mode(sleep_mode);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("+SLP:%d\r\n", sleep_mode);

    return AT_RET_OK;
}
#endif

at_ret_t at_get_sleep_mode(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    if (at_setup_sleepmode_cmd(0, NULL) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    return AT_RET_OK;
}

at_ret_t at_set_sleep_mode(const sleepmode_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_sleepmode_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#define UAPI_UART_PORT_MAX 2
#define UAPI_UART_PORT_NUM 3

at_ret_t at_set_uart_port(const uartport_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);

    osal_printk("+SETUART :%d\r\n", argc);
    if (argc != UAPI_UART_PORT_NUM) { /* argc 3 */
        return AT_RET_SYNTAX_ERROR;
    }

    if (args->para1 > UAPI_UART_PORT_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (args->para2 > UAPI_UART_PORT_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (args->para3 > UAPI_UART_PORT_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("+SETUART:%d,%d,%d\r\n", args->para1, args->para2, args->para3);
#else
    unused(args);
#endif

    return AT_RET_OK;
}

// gpio ops
#ifdef CONFIG_DRIVER_SUPPORT_GPIO
static td_u32 at_setup_gpiodir_cmd(td_s32 argc, const gpiodir_args_t *args)
{
    pin_t io_num;
    gpio_direction_t io_dir;
    pin_mode_t io_mode;
    td_u32 ret;

    if (argc != 2) { /* argc 2 */
        return AT_RET_SYNTAX_ERROR;
    }

    io_num = (pin_t)(args->para1);
    if (io_num > PIN_NONE) {
        osal_printk("+RDGPIO:invalid io,%d\r\n", io_num);
        return AT_RET_SYNTAX_ERROR;
    }

    io_mode = uapi_pin_get_mode(io_num);
    if (io_mode != 0) {
        return ERRCODE_PIN_INVALID_PARAMETER;
    }

    io_dir = (gpio_direction_t)(args->para2);
    if (io_dir > GPIO_DIRECTION_OUTPUT) {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = uapi_gpio_set_dir((pin_t)io_num, io_dir);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("+GPIODIR:%d,%d\r\n", io_num, io_dir);

    return AT_RET_OK;
}
#endif

at_ret_t at_set_gpio_dir(const gpiodir_args_t *args)
{
#ifdef CONFIG_DRIVER_SUPPORT_GPIO
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_gpiodir_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#ifdef CONFIG_DRIVER_SUPPORT_GPIO
static td_u32 at_setup_iogetmode_cmd(td_s32 argc, const getiomode_args_t *args)
{
    pin_t io_num;
    pin_mode_t io_mode;
    pin_pull_t io_pull_stat; /* record io_pull */
    pin_drive_strength_t io_capalibity; /* record io_driver_strength */

    if (argc != 1) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_num = (pin_t)(args->para1);
    if (io_num > PIN_NONE) {
        osal_printk("+GETIOMODE:invalid io,%d\r\n", io_num);
        return AT_RET_SYNTAX_ERROR;
    }

    io_mode = uapi_pin_get_mode(io_num);
    if (io_mode >= PIN_MODE_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_pull_stat = uapi_pin_get_pull(io_num);
    if (io_pull_stat > PIN_PULL_MAX) { /* HAL_PIO_PULL_MAX */
        return AT_RET_SYNTAX_ERROR;
    }

    io_capalibity = uapi_pin_get_ds(io_num);
    if (io_capalibity > PIN_DS_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    osal_printk("+GETIOMODE:%d,%d,%d,%d\r\n", io_num, io_mode, io_pull_stat, io_capalibity);
    return AT_RET_OK;
}
#endif

at_ret_t at_get_iomode(const getiomode_args_t *args)
{
#ifdef CONFIG_DRIVER_SUPPORT_GPIO
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_iogetmode_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#ifdef CONFIG_DRIVER_SUPPORT_GPIO
static td_u32 at_setup_iosetmode_cmd(td_s32 argc, const setiomode_args_t *args)
{
    pin_t io_num;
    pin_mode_t io_mode;
    pin_pull_t io_pull_stat; /* record io_pull */
    pin_drive_strength_t io_capalibity; /* record io_driver_strength */
    td_u32 ret;

    if (argc < 3 || argc > 4) { /* argc 3/4 */
        return AT_RET_SYNTAX_ERROR;
    }

    io_num = (pin_t)(args->para1);
    if (io_num > PIN_NONE) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_mode = (pin_mode_t)(args->para2);
    if (io_mode >= PIN_MODE_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_pull_stat = (pin_pull_t)(args->para3); /* argc 2 */
    if (io_pull_stat >= PIN_PULL_MAX) { /* HAL_PIO_PULL_MAX */
        return AT_RET_SYNTAX_ERROR;
    }

    if (argc == 3) { /* argc 3 */
        io_capalibity = PIN_DS_MAX - 1;
    } else {
        io_capalibity = (pin_drive_strength_t)(args->para4); /* argc 3 */
    }

    ret = uapi_pin_set_mode(io_num, io_mode);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_CMD_ATTR_NOT_ALLOW;
    }

    ret = uapi_pin_get_pull(io_num);
    if (ret != PIN_PULL_MAX) {
        ret = uapi_pin_set_pull(io_num, io_pull_stat);
        if (ret != EXT_ERR_SUCCESS) {
            return AT_RET_CMD_ATTR_NOT_ALLOW;
        }
    }

    ret = uapi_pin_get_ds(io_num);
    if (ret != PIN_DS_MAX) {
        ret = uapi_pin_set_ds(io_num, io_capalibity);
        if (ret != EXT_ERR_SUCCESS) {
            return AT_RET_CMD_ATTR_NOT_ALLOW;
        }
    }

    osal_printk("+SETIOMODE:%d,%d,%d,%d\r\n", io_num, io_mode, io_pull_stat, io_capalibity);
    return AT_RET_OK;
}
#endif

at_ret_t at_set_iomode(const setiomode_args_t *args)
{
#ifdef CONFIG_DRIVER_SUPPORT_GPIO
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_iosetmode_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#ifdef CONFIG_DRIVER_SUPPORT_GPIO
static td_u32 at_setup_gpiowt_cmd(td_s32 argc, const wrgpio_args_t *args)
{
    pin_t io_num;
    gpio_level_t io_level;
    pin_mode_t io_mode;
    gpio_direction_t io_dir = GPIO_DIRECTION_OUTPUT;
    td_u32 ret;

    if (argc != 2) { /* argc 2 */
        return AT_RET_SYNTAX_ERROR;
    }

    io_num = (pin_t)(args->para1);
    if (io_num > PIN_NONE) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_level = (gpio_level_t)(args->para2);
    if (io_level > GPIO_LEVEL_HIGH) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_mode = uapi_pin_get_mode(io_num);
    if (io_mode >= PIN_MODE_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_dir = uapi_gpio_get_dir(io_num);
    if (io_dir != GPIO_DIRECTION_OUTPUT) {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = uapi_gpio_set_val((pin_t)io_num, io_level);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("+WRGPIO:%d,%d,%d\r\n", io_num, io_dir, io_level);

    return EXT_ERR_SUCCESS;
}
#endif

at_ret_t at_wrgpio(const wrgpio_args_t *args)
{
#ifdef CONFIG_DRIVER_SUPPORT_GPIO
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_gpiowt_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

#ifdef CONFIG_DRIVER_SUPPORT_GPIO
static td_u32 at_setup_gpiord_cmd(td_s32 argc, const rdgpio_args_t *args)
{
    pin_t io_num;
    gpio_level_t io_level;
    pin_mode_t io_mode;
    gpio_direction_t io_dir;

    if (argc != 1) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_num = (pin_t)(args->para1);
    if (io_num > PIN_NONE) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_mode = uapi_pin_get_mode(io_num);
    if (io_mode >= PIN_MODE_MAX) {
        return AT_RET_SYNTAX_ERROR;
    }

    io_dir = uapi_gpio_get_dir(io_num);

    io_level = uapi_gpio_get_val((pin_t)io_num);

    osal_printk("+RDGPIO:%d,%d,%d\r\n", io_num, io_dir, io_level);

    return EXT_ERR_SUCCESS;
}
#endif

at_ret_t at_rdgpio(const rdgpio_args_t *args)
{
#ifdef CONFIG_DRIVER_SUPPORT_GPIO
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    if (at_setup_gpiord_cmd(argc, args) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif

    return AT_RET_OK;
}

at_ret_t cmd_set_pm(const pm_args_t *args)
{
    unused(args);
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    bool open_pm = (args->para1 == 0) ? false : true;
    idle_set_open_pm(open_pm);
#endif
    return AT_RET_OK;
}

// at help dump cmd lists
at_ret_t at_help(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_u32 i;
    td_u32 cnt = at_cmd_get_entry_total();
    td_u32 total = 0;
    at_cmd_entry_t *cmd_entry = NULL;
    at_cmd_entry_t **cmd_tbl = (at_cmd_entry_t **)malloc(sizeof(at_cmd_entry_t *) * cnt);

    if (cmd_tbl == TD_NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    if (at_cmd_get_all_entrys((const at_cmd_entry_t **)cmd_tbl, cnt) != cnt) {
        free(cmd_tbl);
        return AT_RET_MEM_API_ERROR;
    }
    osal_printk("+HELP:cmd cnt:%d\r\n", cnt);
    for (i = 0; i < cnt; ++i) {
        cmd_entry = (at_cmd_entry_t *)cmd_tbl[i];
        osal_printk("AT+%-28s ", cmd_entry->name);
        total++;
        if (total % 3 == 0) {  /* 3 entrys per newline */
            osal_printk("\r\n");
        }
    }
    free(cmd_tbl);
#endif
    return AT_RET_OK;
}

// dump last system crash info
at_ret_t at_get_dump(void)
{
#ifdef CONFIG_SUPPORT_CRASHINFO_SAVE_TO_FLASH
    bool hascrashinfo = crashinfo_status_get();
    if (hascrashinfo == false) {
        osal_printk("No crash dump found!\r\n");
        return AT_RET_OK;
    }
    crashinfo_dump();
#else
    osal_printk("No crash dump found!\r\n");
#endif
    return AT_RET_OK;
}
// reboot system
at_ret_t at_exe_reset_cmd(void)
{
    uapi_watchdog_disable();
    /* Wait for 3000 us until the AT print is complete. */
    // call LOS_TaskDelay to wait
    plt_reboot();
    return AT_RET_OK;
}

static void at_copy_tm(struct tm *dest_tm, const struct tm *src_tm)
{
    if (src_tm == NULL) {
        (void)memset_s(dest_tm, sizeof(struct tm), 0, sizeof(struct tm));
    } else {
        dest_tm->tm_sec = src_tm->tm_sec;
        dest_tm->tm_min = src_tm->tm_min;
        dest_tm->tm_hour = src_tm->tm_hour;
        dest_tm->tm_mday = src_tm->tm_mday;
        dest_tm->tm_mon = src_tm->tm_mon;
        dest_tm->tm_year = src_tm->tm_year;
        dest_tm->tm_wday = src_tm->tm_wday;
        dest_tm->tm_yday = src_tm->tm_yday;
        dest_tm->tm_isdst = src_tm->tm_isdst;
        dest_tm->tm_gmtoff = src_tm->tm_gmtoff;
        dest_tm->tm_zone = src_tm->tm_zone;
    }
}

static at_ret_t at_str_to_tm(const char *str, struct tm *tm)
{
    CHAR *ret = NULL;
    size_t para_len = strlen(str);
    if (para_len == 8) { /* 8:Time format string length, such as hh:mm:ss or yyyymmdd */
        if (str[2] == ':') { /* 2:Index of Eigenvalues */
            ret = strptime(str, "%H:%M:%S", tm);
        } else {
            ret = strptime(str, "%Y%m%d", tm);
        }
    } else if (para_len == 10) { /* 10:Time format string length,such as yyyy/mm/dd  */
        ret = strptime(str, "%Y/%m/%d", tm);
    } else if (para_len == 5) { /* 5:Time format string length,such as hh:mm or mm/dd */
        if (str[2] == ':') { /* 2:Index of Eigenvalues */
            ret = strptime(str, "%H:%M", tm);
        } else if (str[2] == '/') { /* 2:Index of Eigenvalues */
            ret = strptime(str, "%m/%d", tm);
        }
    } else if (para_len == 7) { /* 7:Time format string length,such as yyyy/mm */
        if (str[4] == '/') { /* 4:Index of Eigenvalues */
            ret = strptime(str, "%Y/%m", tm);
        }
    }

    if (tm->tm_year < 70) { /* 70:the year is starting in 1970,tm_year must be greater than 70 */
        osal_printk("\nUsage: date -s set system time range from 1970.\n");
        return AT_RET_SYNTAX_ERROR;
    }

    if (tm->tm_mday > g_mon_lengths[(INT32)leap_year(tm->tm_year + DATE_BASE_YEAR)][tm->tm_mon]) {
        return AT_RET_SYNTAX_ERROR;
    }

    if ((tm->tm_sec < 0) || (tm->tm_sec > 59)) { /* Seconds (0-59), leap seconds shall not be used when set time. */
        return AT_RET_SYNTAX_ERROR;
    }
    return (ret == NULL) ? AT_RET_SYNTAX_ERROR : AT_RET_OK;
}

at_ret_t at_date_cmd(void)
{
    struct timeval64 now_time = {0};

    if (gettimeofday64(&now_time, NULL) != 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("%s\n", ctime64(&now_time.tv_sec));

    return AT_RET_OK;
}

at_ret_t at_date_set_cmd(const date_args_t *args)
{
    struct tm tm = {0};
    struct timeval64 now_time = {0};
    struct timeval64 set_time = {0};

    if (gettimeofday64(&now_time, NULL) != 0) {
        osal_printk("set_time failed...\n");
        return AT_RET_SYNTAX_ERROR;
    }

    set_time.tv_usec = now_time.tv_usec;
    at_copy_tm(&tm, localtime64(&now_time.tv_sec));

    if (at_str_to_tm(args->para1, &tm) != AT_RET_OK) {
        osal_printk("at_str_to_tm failed...\n");
        return AT_RET_SYNTAX_ERROR;
    }

    set_time.tv_sec = mktime64(&tm);
    if (set_time.tv_sec == -1) {
        osal_printk("mktime failed...\n");
        return AT_RET_SYNTAX_ERROR;
    }

    if (settimeofday64(&set_time, NULL) != 0) {
        osal_printk("settime failed...\n");
        return AT_RET_SYNTAX_ERROR;
    }

    at_date_cmd();

    return AT_RET_OK;
}

#define EFUSE_MFG_FLAG_ID_1 161
#define EFUSE_MFG_FLAG_ID_2 179
#define EFUSE_MFG_FLAG_ID_3 197
#define EFUSE_MFG_FLAG_BIT_POS 7
#define EFUSE_GROUP_MAX 3
at_ret_t cmd_write_mfg_flag(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_u8 id[EFUSE_GROUP_MAX] = {EFUSE_MFG_FLAG_ID_1, EFUSE_MFG_FLAG_ID_2, EFUSE_MFG_FLAG_ID_3};
    td_u8 index;
    td_u8 value = 0;
    bool writable = false;

    for (index = 0; index < EFUSE_GROUP_MAX; ++index) {
        if (uapi_efuse_read_bit(&value, id[index], EFUSE_MFG_FLAG_BIT_POS) != AT_RET_OK) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (value != 0) {
            continue;
        }
        if (uapi_efuse_write_bit(id[index], EFUSE_MFG_FLAG_BIT_POS) != AT_RET_OK) {
            return AT_RET_SYNTAX_ERROR;
        }
        writable = true;
        break;
    }
    if (writable == false) {
        osal_printk("mfg flag no remain left\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    return AT_RET_OK;
}
#define DIE_ID_LENGTH 21
at_ret_t cmd_get_dieid(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_u32 index;
    td_u8 die_id[DIE_ID_LENGTH] = {0};

    if (uapi_efuse_read_buffer(die_id, 0, sizeof(die_id)) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("OK\r\n");
    osal_printk("CHIP_ID: 0x%02x\r\n", die_id[0]);
    osal_printk("DIE_ID: 0x: ", die_id[0]);
    for (index = 1; index < sizeof(die_id); ++index) {
        osal_printk("%02x", die_id[index]);
    }
    osal_printk("OK\r\n");
#endif
    return AT_RET_OK;
}

at_ret_t cmd_set_customer_rsvd_efuse(const customer_rsvd_efuse_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    td_s32 argc = at_plt_convert_bin_to_dec((td_s32)args->para_map);
    errcode_t ret;
    td_u8 key[CUSTOM_RESVED_EFUSE_BYTE_LEN];
    td_u32 len = 0;
    td_u8 tmp_data;

    if (argc != 1) {
        return AT_RET_SYNTAX_ERROR;
    }
    if ((args->para1[0] != '0') && (args->para1[1] != 'x')) {
        return AT_RET_CMD_PARA_ERROR;
    }
 
    len = strlen((char *)(args->para1 + 2)); /* 2:偏移2个字符 */
    if (len != CUSTOM_RESVED_EFUSE_BYTE_LEN * 2) { /* 2:乘2 */
        return AT_RET_CMD_PARA_ERROR;
    }
    memset_s(key, CUSTOM_RESVED_EFUSE_BYTE_LEN, 0, CUSTOM_RESVED_EFUSE_BYTE_LEN);
    at_str_to_hex((char *)(args->para1 + 2), len, key); /* 2:偏移2个字符 */
    for (td_u8 index = 0; index < (CUSTOM_RESVED_EFUSE_BYTE_LEN >> 1); index++) {
        tmp_data = key[index];
        key[index] = key[CUSTOM_RESVED_EFUSE_BYTE_LEN - 1 - index];
        key[CUSTOM_RESVED_EFUSE_BYTE_LEN - 1 - index] = tmp_data;
    }
    ret = efuse_write_customer_rsvd_efuse(key, CUSTOM_RESVED_EFUSE_BYTE_LEN);
    if (ret != ERRCODE_SUCC) {
        osal_printk("SET CUSTOMER RSVD EFUSE ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_CMD_PARA_ERROR;
    }
    osal_printk("OK\r\n");
#else
    unused(args);
#endif
    return AT_RET_OK;
}

at_ret_t cmd_get_customer_rsvd_efuse(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u8 key[CUSTOM_RESVED_EFUSE_BYTE_LEN];
    td_u32 index;
 
    memset_s(key, CUSTOM_RESVED_EFUSE_BYTE_LEN, 0, CUSTOM_RESVED_EFUSE_BYTE_LEN);
    ret = efuse_read_item(EFUSE_CUSTOM_RESVED_ID, key, sizeof(key));
    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("READ EFUSE CUSTOM RESVED ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("OK\r\n");
    osal_printk("RESERVED EFUSE:0x");
    for (index = sizeof(key); index > 0; index--) {
        osal_printk("%02x", key[index - 1]);
    }
    osal_printk("\r\n");
#endif
    return AT_RET_OK;
}

at_ret_t cmd_disable_ssi_jtag(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;

    ret = efuse_write_jtag_ssi();
    if (ret != ERRCODE_SUCC) {
        osal_printk("SET EFUSE SSI JTAG ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_CMD_PARA_ERROR;
    }
    osal_printk("OK\r\n");
#endif
    return AT_RET_OK;
}

at_ret_t cmd_get_ssi_jtag_status(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u8 ssi_jtag = 0;

    ret = efuse_read_jtag_ssi(&ssi_jtag, sizeof(ssi_jtag));
    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("READ EFUSE SSI JTAG ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("OK\r\n");
    osal_printk("SSI JTAG: %d\r\n", ssi_jtag);
#endif
    return AT_RET_OK;
}

at_ret_t cmd_set_hash_root_public_key(const pubkey_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u8 key[HASH_ROOT_PUBLIC_KEY_LEN];
    td_u32 len = 0;

    if ((args->para1[0] != '0') && (args->para1[1] != 'x')) {
        return AT_RET_CMD_PARA_ERROR;
    }

    len = strlen((char *)(args->para1 + 2)); /* 2:偏移2个字符 */
    if (len != HASH_ROOT_PUBLIC_KEY_LEN * 2) { /* 2:乘2 */
        return AT_RET_CMD_PARA_ERROR;
    }
    memset_s(key, sizeof(key), 0, sizeof(key));
    at_str_to_hex((char *)(args->para1 + 2), len, key); /* 2:偏移2个字符 */
    ret = efuse_write_hash_root_public_key(key, sizeof(key));
    if (ret != ERRCODE_SUCC) {
        osal_printk("SET EFUSE KEY ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_CMD_PARA_ERROR;
    }
    osal_printk("OK\r\n");
#else
    unused(args);
#endif
    return AT_RET_OK;
}

at_ret_t cmd_get_hash_root_public_key(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u8 key[HASH_ROOT_PUBLIC_KEY_LEN];
    td_u32 index;

    memset_s(key, sizeof(key), 0, sizeof(key));
    ret = efuse_read_hash_root_public_key(key, sizeof(key));
    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("READ EFUSE HASH ROOT PUBLIC KEY ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("OK\r\n");
    osal_printk("KEY:");
    for (index = 0; index < sizeof(key); index++) {
        osal_printk("%02x", key[index]);
    }
    osal_printk("\r\n");
#endif
    return AT_RET_OK;
}

at_ret_t cmd_sec_verify_enable(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;

    ret = efuse_write_sec_verify();
    if (ret != ERRCODE_SUCC) {
        osal_printk("SET EFUSE SEC VERIFY ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_CMD_PARA_ERROR;
    }
    osal_printk("OK\r\n");
#endif
    return AT_RET_OK;
}

at_ret_t cmd_get_sec_verify_status(void)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u8 sec_verify = 0;

    ret = efuse_read_sec_verify(&sec_verify, sizeof(sec_verify));
    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("READ EFUSE SEC VERIFY ERROR, ret : 0x%x\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("OK\r\n");
    osal_printk("SEC VERIFY: %d\r\n", sec_verify);
#endif
    return AT_RET_OK;
}

at_ret_t plt_flash_read(const flashread_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u8 *data = TD_NULL;
    td_u32 index;

    data = (td_u8 *)malloc(args->length);
    if (data == TD_NULL) {
        free(data);
        return AT_RET_SYNTAX_ERROR;
    }
    ret = plt_flash_read_data(args->addr, args->length, data);
    if (ret != EXT_ERR_SUCCESS) {
        free(data);
        osal_printk("plt_flash_read_data error, ret: 0x%x\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    for (index = 0; index < (td_u32)args->length; ++index) {
        print_str("%x ", data[index]);
    }
    print_str("\r\n", data[index]);
    free(data);
#else
    unused(args);
#endif
    return AT_RET_OK;
}

#define FLASH_DATA_MAX_LENGTH 3
at_ret_t plt_flash_write(const flashwrite_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    errcode_t ret;
    td_u32 len = 0;
    td_u32 left = 0;
    td_u8 *data = TD_NULL;

    len = strlen((char *)(args->data));
    if (len != (td_u32)(args->length * 2)) { /* 2:乘2 */
        return AT_RET_CMD_PARA_ERROR;
    }
    data = (td_u8 *)malloc(args->length);
    if (data == TD_NULL) {
        free(data);
        return AT_RET_SYNTAX_ERROR;
    }
    at_str_to_hex((char *)(args->data), len, data);
    ret = plt_flash_write_data(args->addr, args->length, data, &left);
    if (ret != EXT_ERR_SUCCESS) {
        free(data);
        osal_printk("plt_flash_write error, ret: 0x%x\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("plt_flash_write left:%d\r\n", left);
    free(data);
#else
    unused(args);
#endif
    return AT_RET_OK;
}

at_ret_t save_license(const license_args_t *args)
{
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    uint32_t value_length = 0;
    char *tmp = at_parse_string((char *)(args->license), &value_length);
    if (tmp == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_CUSTOMER_FACTORY, &info);
    if (ret_val != ERRCODE_SUCC || info.part_info.addr_info.size == 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    ret_val = uapi_sfc_reg_erase(info.part_info.addr_info.addr, info.part_info.addr_info.size);
    if (ret_val != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    ret_val = uapi_sfc_reg_write(info.part_info.addr_info.addr, (uint8_t *)tmp, value_length);
    if (ret_val != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(args);
#endif
    return AT_RET_OK;
}
