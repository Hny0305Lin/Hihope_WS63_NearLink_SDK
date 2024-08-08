/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: driver of APC module.
 *
 * Create: 2021-01-13
 */
#include "hal_fapc.h"
#include "hal_fapc_reg.h"
#include "crypto_drv_common.h"

#define clr_region_val(reg, region)                   (reg) &= ~(0x03U << ((region) * 2U))
#define set_region_val(reg, region, val)              (reg) |= ((val) & 0x03U) << ((region) * 2U)

td_s32 hal_fapc_set_region_addr(td_u32 region, td_u32 start_addr, td_u32 end_addr)
{
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");
    /* need to check address */
    fapc_reg_write(FAPC_CFG_START_ADDR(region), start_addr);
    fapc_reg_write(FAPC_CFG_END_ADDR(region), end_addr);
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_permission(td_u32 region, td_u32 read_mid, td_u32 write_mid)
{
    crypto_unused(region);
    crypto_unused(read_mid);
    crypto_unused(write_mid);
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_mac_addr(td_u32 region, td_u32 mac_addr)
{
    crypto_unused(region);
    crypto_unused(mac_addr);
    return TD_SUCCESS;
}

#define FAPC_REGION_MODE_NORMAL_VAL     0
#define FAPC_REGION_MODE_DEV_VAL        2
td_s32 hal_fapc_set_region_mode(td_u32 region, crypto_fapc_region_mode region_mode)
{
    td_u32 region_mode_reg_value = 0;
    td_u32 mode_cfg = 0;
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    switch (region_mode) {
        case CRYPTO_FAPC_REGION_MODE_NORMAL:
            region_mode_reg_value = FAPC_REGION_MODE_NORMAL_VAL;
            break;
        case CRYPTO_FAPC_REGION_MODE_DEC:
            region_mode_reg_value = FAPC_REGION_MODE_DEV_VAL;
            break;
        default:
            crypto_log_err("Invalid region_mode\n");
            return TD_FAILURE;
    }

    mode_cfg = fapc_reg_read(FAPC_DEC_AUTH_CFG);
    clr_region_val(mode_cfg, region);
    set_region_val(mode_cfg, region, region_mode_reg_value);
    fapc_reg_write(FAPC_DEC_AUTH_CFG, mode_cfg);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_region_enable(td_u32 region, td_bool enable)
{
    crypto_unused(region);
    crypto_unused(enable);
    return TD_SUCCESS;
}

td_s32 hal_fapc_region_lock(td_u32 region)
{
    td_u32 lock_cfg = 0;
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    lock_cfg = fapc_reg_read(FAPC_ONE_WAY_LOCK);
    lock_cfg |= 1 << region;
    fapc_reg_write(FAPC_ONE_WAY_LOCK, lock_cfg);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_iv(crypto_fapc_iv_type iv_type, const td_u8 *iv, td_u32 iv_length)
{
    const td_u32 *iv_word = TD_NULL;
    td_u32 i;
    crypto_hal_func_enter();
    crypto_chk_return(iv_type != CRYPTO_FAPC_IV_TYPE_ACPU, TD_FAILURE, "unsupport iv_type\n");
    crypto_chk_return(iv == TD_NULL, TD_FAILURE, "iv is NULL\n");
    crypto_chk_return(iv_length != CRYPTO_IV_LEN_IN_BYTES, TD_FAILURE, "invalid iv_length\n");

    iv_word = (const td_u32 *)iv;
    for (i = 0; i < iv_length / CRYPTO_WORD_WIDTH; i++) {
        fapc_reg_write(FAPC_ACORE_IV_BASE_ADDR + i * CRYPTO_WORD_WIDTH, iv_word[i]);
    }
    /* config iv valid. */
    fapc_reg_write(FAPC_IV_VALID, 0x1);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_iv_start_addr(td_u32 region, td_u32 start_addr)
{
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    fapc_reg_write(FAPC_IV_ACPU_START_ADDR(region), start_addr);
    crypto_hal_func_exit();
    return TD_SUCCESS;
}