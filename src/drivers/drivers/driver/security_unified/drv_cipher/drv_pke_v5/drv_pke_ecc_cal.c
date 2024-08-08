/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc calculate algorithm function implement.
 *
 * Create: 2023-05-24
*/

#include "crypto_osal_adapt.h"
#include "drv_common.h"
#include "drv_common_pke.h"
#include "ecc_ec_fp.h"
#include "ed_ec_fp.h"
#include "curve_ec_fp.h"
#include "drv_pke.h"

td_s32 drv_cipher_pke_check_dot_on_curve(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = ecc_ecfp_point_valid_standard(sec_arg_add_cs(curve_type, pub_key, is_on_curve));
    crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_mul_dot(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);

    ret = ecc_ecfp_mul_naf(sec_arg_add_cs(ecc, k, p, r));

    crypto_drv_pke_common_suspend();
    return ret;
}
