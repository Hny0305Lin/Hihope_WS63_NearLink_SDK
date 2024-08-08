/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver trng. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "drv_trng.h"
#include "drv_inner.h"

#include "hal_cipher_trng.h"
#include "crypto_drv_common.h"
#include "crypto_common_macro.h"

#define TRNG_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_TRNG, err_code)
#define TRNG_ONCE_WIDTH_IN_BYTE         4

static td_bool g_drv_trng_init_flag = TD_FALSE;

td_s32 drv_cipher_trng_init(td_void)
{
    td_s32 ret;
    if (g_drv_trng_init_flag == TD_TRUE) {
        return TD_SUCCESS;
    }
    ret = hal_cipher_trng_init();
    g_drv_trng_init_flag = TD_TRUE;

    return ret;
}

td_s32 drv_cipher_trng_get_random(td_u32 *randnum)
{
    crypto_chk_return(g_drv_trng_init_flag == TD_FALSE, TRNG_COMPAT_ERRNO(ERROR_NOT_INIT), "call init first!\n");
    crypto_chk_return(randnum == TD_NULL, TRNG_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), "randnum is NULL\n");
    return hal_cipher_trng_get_random(randnum);
}

td_s32 drv_cipher_trng_get_multi_random(td_u32 size, td_u8 *randnum)
{
    td_s32 ret = TD_FAILURE;
    td_u32 cnt;
    td_u32 i;
    td_u32 randnum_once = 0;
    td_u32 tail = 0;
    crypto_chk_return(g_drv_trng_init_flag == TD_FALSE, TRNG_COMPAT_ERRNO(ERROR_NOT_INIT), "call init first!\n");
    crypto_chk_return(randnum == TD_NULL, TRNG_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), "randnum is NULL\n");
    cnt = size / TRNG_ONCE_WIDTH_IN_BYTE;
    for (i = 0; i < cnt; i++) {
        ret = hal_cipher_trng_get_random(&randnum_once);
        crypto_chk_return(ret != TD_SUCCESS, ret, "ERROR! hal_cipher_trng_get_random failed!\n");
        ret = memcpy_s(randnum + i * TRNG_ONCE_WIDTH_IN_BYTE, size - i * TRNG_ONCE_WIDTH_IN_BYTE,
            &randnum_once, TRNG_ONCE_WIDTH_IN_BYTE);
        crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed!\n");
    }
    /* less then 4 byte */
    tail = size - cnt * TRNG_ONCE_WIDTH_IN_BYTE;
    if (tail != 0) {
        ret = hal_cipher_trng_get_random(&randnum_once);
        crypto_chk_return(ret != TD_SUCCESS, ret, "ERROR! hal_cipher_trng_get_random failed!\n");
        ret = memcpy_s(randnum + cnt * TRNG_ONCE_WIDTH_IN_BYTE, tail, &randnum_once, tail);
        crypto_chk_return(ret != TD_SUCCESS, ret, "memcpy_s failed!\n");
    }
    return ret;
}

td_s32 drv_cipher_trng_deinit(td_void)
{
    if (g_drv_trng_init_flag == TD_FALSE) {
        return TD_SUCCESS;
    }
    g_drv_trng_init_flag = TD_FALSE;
    return hal_cipher_trng_deinit();
}