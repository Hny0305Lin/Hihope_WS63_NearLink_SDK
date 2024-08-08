/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides pke driver source \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "unified_cipher_pke.h"
#include "kapi_pke.h"
#include "kapi_pke_cal.h"

#include "sl_common.h"
#include "crypto_common_macro.h"

#ifdef CONFIG_PKE_SUPPORT_ECC_GEN_KEY
errcode_t uapi_drv_cipher_pke_ecc_gen_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key,
    const uapi_drv_cipher_pke_ecc_point_t *output_pub_key)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_ecc_gen_key(curve_type, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)output_priv_key, (const drv_pke_ecc_point *)output_pub_key);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

errcode_t uapi_drv_cipher_pke_ecdsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_ecdsa_sign(curve_type, (const drv_pke_data *)priv_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_ecdsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_ecdsa_verify(curve_type, (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

#ifdef CONFIG_PKE_SUPPORT_EDWARD
errcode_t uapi_drv_cipher_pke_eddsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_eddsa_sign(curve_type, (const drv_pke_data *)priv_key,
        (const drv_pke_msg *)msg, (const drv_pke_ecc_sig *)sig);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_eddsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_eddsa_verify(curve_type, (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_msg *)msg, (const drv_pke_ecc_sig *)sig);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

#ifdef CONFIG_PKE_SUPPORT_ECC_ECDH
errcode_t uapi_drv_cipher_pke_ecc_gen_ecdh_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *input_pub_key,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_shared_key)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_ecc_gen_ecdh_key(curve_type, (const drv_pke_ecc_point *)input_pub_key,
        (const drv_pke_data *)input_priv_key, (const drv_pke_data *)output_shared_key);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

#ifdef CONFIG_PKE_SUPPORT_ECC_CAL
errcode_t uapi_drv_cipher_pke_check_dot_on_curve(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key, bool *is_on_curve)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_check_dot_on_curve(curve_type, (const drv_pke_ecc_point *)pub_key, (uint8_t *)is_on_curve);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

#if defined(CONFIG_PKE_SUPPORT_SM2_SIGN) || defined(CONFIG_PKE_SUPPORT_SM2_VERIFY)
errcode_t uapi_drv_cipher_pke_sm2_dsa_hash(const uapi_drv_cipher_pke_data_t *sm2_id,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    uapi_drv_cipher_pke_data_t *hash)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_sm2_dsa_hash((const drv_pke_data *)sm2_id,
        (const drv_pke_ecc_point *)pub_key, (const drv_pke_msg *)msg, (drv_pke_data *)hash);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

#ifdef CONFIG_PKE_SUPPORT_SM2_CRYPTO
errcode_t uapi_drv_cipher_pke_sm2_public_encrypt(const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *plain_text,
    const uapi_drv_cipher_pke_data_t *cipher_text)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_sm2_public_encrypt((const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)plain_text, (const drv_pke_data *)cipher_text);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_sm2_private_decrypt(const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *cipher_text,
    const uapi_drv_cipher_pke_data_t *plain_text)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_sm2_private_decrypt((const drv_pke_data *)priv_key,
        (const drv_pke_data *)cipher_text, (const drv_pke_data *)plain_text);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

#ifdef CONFIG_PKE_SUPPORT_RSA
errcode_t uapi_drv_cipher_pke_rsa_sign(const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_data_t *input_hash,
    uapi_drv_cipher_pke_data_t *sign)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_rsa_sign((const drv_pke_rsa_priv_key *)priv_key, scheme, hash_type,
        (const drv_pke_data *)input_hash, (drv_pke_data *)sign);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_rsa_verify(const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    uapi_drv_cipher_pke_data_t *input_hash,
    const uapi_drv_cipher_pke_data_t *sig)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_rsa_verify((const drv_pke_rsa_pub_key *)pub_key, scheme, hash_type,
        (drv_pke_data *)input_hash, (const drv_pke_data *)sig);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_rsa_public_encrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    uapi_drv_cipher_pke_data_t *output)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_rsa_public_encrypt(scheme, hash_type, (const drv_pke_rsa_pub_key *)pub_key,
        (const drv_pke_data *)input, (const drv_pke_data *)label, (drv_pke_data *)output);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_rsa_private_decrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    const uapi_drv_cipher_pke_data_t *output)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_rsa_private_decrypt(scheme, hash_type, (const drv_pke_rsa_priv_key *)priv_key,
        (const drv_pke_data *)input, (const drv_pke_data *)label, (drv_pke_data *)output);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_dh_gen_key(const uapi_drv_cipher_pke_data_t *g_data,
    const uapi_drv_cipher_pke_data_t *mod_n, const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_data_t *output_pub_key)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_dh_gen_key((const drv_pke_data *)g_data, (const drv_pke_data *)mod_n,
        (const drv_pke_data *)input_priv_key, (const drv_pke_data *)output_priv_key,
        (const drv_pke_data *)output_pub_key);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_dh_compute_key(const uapi_drv_cipher_pke_data_t *mod_n,
    const uapi_drv_cipher_pke_data_t  *input_priv_key, const uapi_drv_cipher_pke_data_t  *input_pub_key,
    const uapi_drv_cipher_pke_data_t  *output_shared_key)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_dh_compute_key((const drv_pke_data *)mod_n, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)input_pub_key, (const drv_pke_data *)output_shared_key);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif

#ifdef CONFIG_PKE_SUPPORT_BIG_NUMBER
errcode_t uapi_drv_cipher_pke_add_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_add_mod((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)p,
        (const drv_pke_data *)c);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_sub_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_sub_mod((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)p,
        (const drv_pke_data *)c);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mul_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_mul_mod((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)p,
        (const drv_pke_data *)c);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_inv_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p,
    const uapi_drv_cipher_pke_data_t *c)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_inv_mod((const drv_pke_data *)a, (const drv_pke_data *)p, (const drv_pke_data *)c);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p,
    const uapi_drv_cipher_pke_data_t *c)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_mod((const drv_pke_data *)a, (const drv_pke_data *)p, (const drv_pke_data *)c);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mul(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *c)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_mul((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)c);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_exp_mod(const uapi_drv_cipher_pke_data_t *n, const uapi_drv_cipher_pke_data_t *k,
    const uapi_drv_cipher_pke_data_t *in, const uapi_drv_cipher_pke_data_t *out)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_pke_exp_mod((const drv_pke_data *)n, (const drv_pke_data *)k, (const drv_pke_data *)in,
        (const drv_pke_data *)out);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}
#endif