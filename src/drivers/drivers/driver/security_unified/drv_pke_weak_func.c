/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: driver weak functions for unified Framework.
 *
 * Create: 2023-05-31
*/

#include "drv_common_pke.h"
#include "ecc_ec_fp.h"
#include "curve_ec_fp.h"
#include "ed_ec_fp.h"

/*
 * pke operators
 */
td_s32 __attribute__((weak)) ecc_ecfp_mul_naf(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, k, p, r);
    crypto_unused(ecc);
    crypto_unused(k);
    crypto_unused(p);
    crypto_unused(r);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) curve_ecfp_mul_dot(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, k, p, r);
    crypto_unused(ecc);
    crypto_unused(k);
    crypto_unused(p);
    crypto_unused(r);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) ed_ecfp_mul_naf(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, k, p, r);
    crypto_unused(ecc);
    crypto_unused(k);
    crypto_unused(p);
    crypto_unused(r);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) ed_secret_to_public(const drv_pke_ecc_curve *ecc, const drv_pke_data *priv_key,
    const drv_pke_ecc_point *pub_key)
{
    crypto_unused(ecc);
    crypto_unused(priv_key);
    crypto_unused(pub_key);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

/*
 * pke algorithms
 */
td_s32 __attribute__((weak)) crypto_drv_pke_common_ecc_sign(drv_pke_ecc_curve_type curve_type,
    const drv_pke_data *priv_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    crypto_unused(curve_type);
    crypto_unused(priv_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) crypto_drv_pke_common_ecc_verify(drv_pke_ecc_curve_type curve_type,
    const drv_pke_ecc_point *pub_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    crypto_unused(curve_type);
    crypto_unused(pub_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) crypto_drv_pke_common_sm2_sign(const drv_pke_data *priv_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig)
{
    crypto_unused(priv_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) crypto_drv_pke_common_sm2_verify(const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    crypto_unused(pub_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) crypto_drv_pke_common_sm2_public_encrypt(const drv_pke_ecc_point *pub_key,
    const drv_pke_data *plain_text, const drv_pke_data *cipher_text)
{
    crypto_unused(pub_key);
    crypto_unused(plain_text);
    crypto_unused(cipher_text);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) crypto_drv_pke_common_sm2_private_decrypt(const drv_pke_data *priv_key,
    const drv_pke_data *cipher_text, const drv_pke_data *plain_text)
{
    crypto_unused(priv_key);
    crypto_unused(cipher_text);
    crypto_unused(plain_text);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

td_s32 __attribute__((weak)) crypto_drv_pke_common_sm2_dsa_hash(const drv_pke_data *sm2_id,
    const drv_pke_ecc_point *pub_key, const drv_pke_msg *msg, const drv_pke_data *hash)
{
    crypto_unused(sm2_id);
    crypto_unused(pub_key);
    crypto_unused(msg);
    crypto_unused(hash);
    return PKE_COMPAT_ERRNO(ERROR_UNSUPPORT);
}

