/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver fapc. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "drv_fapc.h"
#include "crypto_drv_common.h"
#include "crypto_common_macro.h"

td_s32 drv_fapc_set_iv(crypto_fapc_iv_type type, const td_u8 *iv, td_u32 length)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    ret = hal_fapc_set_region_iv(type, iv, length);

    crypto_drv_func_exit();
    return ret;
}

static td_s32 inner_fapc_config_check(const crypto_fapc_config *config)
{
    crypto_chk_return(config == TD_NULL, TD_FAILURE, "config is NULL\n");

    crypto_unused(config);
    return TD_SUCCESS;
}

td_s32 drv_fapc_set_config(td_u32 region, const crypto_fapc_config *config)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_fapc_region_mode mode = CRYPTO_FAPC_REGION_MODE_INVALID;
    crypto_drv_func_enter();

    ret = inner_fapc_config_check(config);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_fapc_config_check failed\n");

    mode = config->mode;
    /* set start/end address. */
    ret = hal_fapc_set_region_addr(region, config->start_addr, config->end_addr);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_set_region_addr failed\n");

    /* set read/write permission. */
    ret = hal_fapc_set_region_permission(region, config->read_mid, config->write_mid);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_set_region_permission failed\n");

    if (mode == CRYPTO_FAPC_REGION_MODE_DEC_AUTH) {
        /* set region mac_addr. */
        ret = hal_fapc_set_region_mac_addr(region, config->mac_addr);
        crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_set_region_mac_addr failed\n");
    }
    /* set region mode. */
    ret = hal_fapc_set_region_mode(region, mode);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_set_region_permission failed\n");

    /* fapc enable. */
    ret = hal_fapc_region_enable(region, config->enable);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_region_enable failed\n");

    /* fapc set iv_start_addr. */
    ret = hal_fapc_set_region_iv_start_addr(region, config->iv_start_addr);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_set_region_iv_start_addr failed\n");

    /* fapc lock. */
    if (config->lock) {
        ret = hal_fapc_region_lock(region);
        crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_region_lock failed\n");
    }

    crypto_drv_func_exit();
    return ret;
}

td_s32 drv_fapc_set_bypass_config(td_u32 region, td_u32 start_addr, td_u32 end_addr, td_bool lock)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    /* set start/end address. */
    ret = hal_fapc_set_region_addr(region, start_addr, end_addr);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_set_region_addr failed\n");

    /* fapc lock. */
    if (lock) {
        ret = hal_fapc_region_lock(region);
        crypto_chk_return(ret != TD_SUCCESS, ret, "hal_fapc_region_lock failed\n");
    }

    crypto_drv_func_exit();
    return ret;
}

td_s32 drv_fapc_get_config(td_u32 region, const crypto_fapc_config *config)
{
    crypto_unused(region);
    crypto_unused(config);
    return TD_SUCCESS;
}