/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the implementation of ecdsa. \n
 * \n
 * Create: 2022-08-08
*/

#include "crypto_drv_common.h"
#include "hal_pke.h"
#include "drv_pke_inner.h"
#include "drv_common_pke.h"
#include "ecc_ec_fp.h"
#include "drv_pke.h"

td_s32 drv_cipher_pke_register_wait_func(crypto_wait_t *wait,
    crypto_wait_timeout_interruptible wait_func, td_u32 timeout_ms, uintptr_t check_word)
{
    uintptr_t chk_sum = 0;

    chk_sum = (uintptr_t)wait ^ (uintptr_t)wait_func ^ (uintptr_t)timeout_ms;
    crypto_chk_return((chk_sum != check_word), PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "Invalid input params!\n");

    hal_pke_set_wait_event(wait, wait_func, timeout_ms, TD_FALSE);

    return TD_SUCCESS;
}

td_void drv_cipher_pke_init_ecc_param(const pke_default_parameters *ecc_params, const td_u32 *ecc_num)
{
    g_ecc_params = (pke_default_parameters *)ecc_params;
    g_ecc_num = (td_u32 *)ecc_num;
}

td_s32 drv_cipher_pke_init(void)
{
    td_s32 ret = TD_FAILURE;

    ret = hal_pke_init();
    if (ret != TD_SUCCESS) {
        crypto_print_func_err(hal_pke_init, ret);
        hal_pke_deinit();
        return ret;
    }

    return ret;
}

td_s32 drv_cipher_pke_deinit(void)
{
    td_s32 ret = TD_FAILURE;

    ret = hal_pke_deinit();
    if (ret != TD_SUCCESS) {
        crypto_print_func_err(drv_pke_deinit, ret);
        return ret;
    }

    hal_pke_unlock();
    return ret;
}
/************************************************** initial API end************************************/
td_s32 drv_cipher_pke_ecdsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);

    if (curve_type == DRV_PKE_ECC_TYPE_SM2) {
        ret = crypto_drv_pke_common_sm2_sign(priv_key, hash, sig);
    } else {
        ret = crypto_drv_pke_common_ecc_sign(curve_type, priv_key, hash, sig);
    }

    crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_ecdsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);

    if (curve_type == DRV_PKE_ECC_TYPE_SM2) {
        ret = crypto_drv_pke_common_sm2_verify(pub_key, hash, sig);
    } else {
        ret = crypto_drv_pke_common_ecc_verify(curve_type, pub_key, hash, sig);
    }

    crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_sm2_dsa_hash(const drv_pke_data *sm2_id, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, drv_pke_data *hash)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();
    ret = crypto_drv_pke_common_sm2_dsa_hash(sm2_id, pub_key, msg, hash);
    crypto_drv_func_exit();
    return ret;
}

td_s32 drv_cipher_pke_sm2_public_encrypt(const drv_pke_ecc_point *pub_key, const drv_pke_data *plain_text,
    const drv_pke_data *cipher_text)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = crypto_drv_pke_common_sm2_public_encrypt(pub_key, plain_text, cipher_text);
    crypto_drv_pke_common_suspend();
    return ret;
}

td_s32 drv_cipher_pke_sm2_private_decrypt(const drv_pke_data *priv_key, const drv_pke_data *cipher_text,
    const drv_pke_data *plain_text)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = crypto_drv_pke_common_sm2_private_decrypt(priv_key, cipher_text, plain_text);
    crypto_drv_pke_common_suspend();
    return ret;
}