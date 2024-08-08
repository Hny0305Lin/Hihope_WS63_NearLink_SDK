/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc calculate algorithm function implementation in kernel.
 *
 * Create: 2023-05-29
*/

#include "crypto_osal_adapt.h"
#include "drv_pke_cal.h"
#include "kapi_pke_cal.h"
#include "drv_common_pke.h"

td_s32 kapi_pke_check_dot_on_curve(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve)
{
    td_s32 ret = drv_cipher_pke_check_dot_on_curve(curve_type, pub_key, is_on_curve);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_pke_check_don_curve failed, ret is 0x%x\n", ret);
    return ret;
}

td_s32 kapi_pke_mul_dot(drv_pke_ecc_curve_type curve_type, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r)
{
    td_s32 ret;
    const drv_pke_ecc_curve *ecc = NULL;

    ecc = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ecc == NULL, TD_FAILURE, "crypto_drv_pke_common_get_ecc_curve failed\n");

    ret = drv_cipher_pke_mul_dot(ecc, k, p, r);
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_cipher_pke_mul_dot failed, ret is 0x%x\n", ret);

    return ret;
}