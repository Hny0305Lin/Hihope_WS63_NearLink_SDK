/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: harden adapt register functions.
 *
 * Create: 2024-07-18
*/

#include "cipher_adapt.h"
#include "dfx.h"

mbedtls_harden_cipher_func g_harden_cipher_func = { 0 };

mbedtls_harden_km_func g_harden_km_func = { 0 };

mbedtls_harden_mem_func g_harden_mem_func = { 0 };

td_void mbedtls_cipher_adapt_register_func( mbedtls_harden_cipher_func *harden_cipher_func )
{
    g_harden_cipher_func.harden_hash_init = harden_cipher_func->harden_hash_init;
    g_harden_cipher_func.harden_hash_deinit = harden_cipher_func->harden_hash_deinit;
    g_harden_cipher_func.harden_hash_start = harden_cipher_func->harden_hash_start;
    g_harden_cipher_func.harden_hash_update = harden_cipher_func->harden_hash_update;
    g_harden_cipher_func.harden_hash_get = harden_cipher_func->harden_hash_get;
    g_harden_cipher_func.harden_hash_set = harden_cipher_func->harden_hash_set;
    g_harden_cipher_func.harden_hash_destroy = harden_cipher_func->harden_hash_destroy;
    g_harden_cipher_func.harden_hash_finish = harden_cipher_func->harden_hash_finish;
    g_harden_cipher_func.harden_hkdf = harden_cipher_func->harden_hkdf;
    g_harden_cipher_func.harden_hkdf_extract = harden_cipher_func->harden_hkdf_extract;
    g_harden_cipher_func.harden_hkdf_expand = harden_cipher_func->harden_hkdf_expand;
    g_harden_cipher_func.harden_trng_get_random = harden_cipher_func->harden_trng_get_random;
    g_harden_cipher_func.harden_trng_get_multi_random = harden_cipher_func->harden_trng_get_multi_random;
    g_harden_cipher_func.harden_pbkdf2 = harden_cipher_func->harden_pbkdf2;
    g_harden_cipher_func.harden_symc_init = harden_cipher_func->harden_symc_init;
    g_harden_cipher_func.harden_symc_deinit = harden_cipher_func->harden_symc_deinit;
    g_harden_cipher_func.harden_symc_create = harden_cipher_func->harden_symc_create;
    g_harden_cipher_func.harden_symc_destroy = harden_cipher_func->harden_symc_destroy;
    g_harden_cipher_func.harden_symc_set_config = harden_cipher_func->harden_symc_set_config;
    g_harden_cipher_func.harden_symc_get_config = harden_cipher_func->harden_symc_get_config;
    g_harden_cipher_func.harden_symc_attach = harden_cipher_func->harden_symc_attach;
    g_harden_cipher_func.harden_symc_encrypt = harden_cipher_func->harden_symc_encrypt;
    g_harden_cipher_func.harden_symc_decrypt = harden_cipher_func->harden_symc_decrypt;
    g_harden_cipher_func.harden_symc_get_tag= harden_cipher_func->harden_symc_get_tag;
    g_harden_cipher_func.harden_symc_mac_start = harden_cipher_func->harden_symc_mac_start;
    g_harden_cipher_func.harden_symc_mac_update = harden_cipher_func->harden_symc_mac_update;
    g_harden_cipher_func.harden_symc_mac_finish = harden_cipher_func->harden_symc_mac_finish;
    g_harden_cipher_func.harden_pke_init = harden_cipher_func->harden_pke_init;
    g_harden_cipher_func.harden_pke_deinit = harden_cipher_func->harden_pke_deinit;
    g_harden_cipher_func.harden_pke_mod = harden_cipher_func->harden_pke_mod;
    g_harden_cipher_func.harden_pke_exp_mod = harden_cipher_func->harden_pke_exp_mod;
    g_harden_cipher_func.harden_pke_ecc_gen_key = harden_cipher_func->harden_pke_ecc_gen_key;
    g_harden_cipher_func.harden_pke_ecdsa_sign = harden_cipher_func->harden_pke_ecdsa_sign;
    g_harden_cipher_func.harden_pke_ecdsa_verify = harden_cipher_func->harden_pke_ecdsa_verify;
    g_harden_cipher_func.harden_pke_eddsa_sign = harden_cipher_func->harden_pke_eddsa_sign;
    g_harden_cipher_func.harden_pke_eddsa_verify = harden_cipher_func->harden_pke_eddsa_verify;
    g_harden_cipher_func.harden_pke_gen_ecdh_key = harden_cipher_func->harden_pke_gen_ecdh_key;
    g_harden_cipher_func.harden_pke_check_dot_on_curve = harden_cipher_func->harden_pke_check_dot_on_curve;
    g_harden_cipher_func.harden_pke_rsa_sign = harden_cipher_func->harden_pke_rsa_sign;
    g_harden_cipher_func.harden_pke_rsa_verify = harden_cipher_func->harden_pke_rsa_verify;
    g_harden_cipher_func.harden_pke_rsa_public_encrypt = harden_cipher_func->harden_pke_rsa_public_encrypt;
    g_harden_cipher_func.harden_pke_rsa_private_decrypt = harden_cipher_func->harden_pke_rsa_private_decrypt;
}

td_void mbedtls_km_adapt_register_func( mbedtls_harden_km_func *harden_km_func )
{
    g_harden_km_func.harden_km_init = harden_km_func->harden_km_init;
    g_harden_km_func.harden_km_deinit = harden_km_func->harden_km_deinit;
    g_harden_km_func.harden_km_create_keyslot = harden_km_func->harden_km_create_keyslot;
    g_harden_km_func.harden_km_destroy_keyslot = harden_km_func->harden_km_destroy_keyslot;
    g_harden_km_func.harden_km_set_clear_key = harden_km_func->harden_km_set_clear_key;
}

td_void mbedtls_mem_adapt_register_func( mbedtls_harden_mem_func *harden_mem_func )
{
    g_harden_mem_func.harden_alloc_phys_buf = harden_mem_func->harden_alloc_phys_buf;
    g_harden_mem_func.harden_free_phys_buf = harden_mem_func->harden_free_phys_buf;
    g_harden_mem_func.harden_get_phys_addr = harden_mem_func->harden_get_phys_addr;
}

/* HASH */
td_s32 CIPHER_HASH_INIT( td_void )
{
    if( g_harden_cipher_func.harden_hash_init == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_init func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_init();
}

td_s32 CIPHER_HASH_DEINIT( td_void )
{
    if( g_harden_cipher_func.harden_hash_deinit == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_deinit func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_deinit();
}

td_s32 CIPHER_HASH_START( td_handle *uapi_hash_handle, const crypto_hash_attr *hash_attr )
{
    if( g_harden_cipher_func.harden_hash_start == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_start func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_start( uapi_hash_handle, hash_attr );
}

td_s32 CIPHER_HASH_UPDATE( td_handle uapi_hash_handle, const crypto_buf_attr *src_buf, const td_u32 len )
{
    if( g_harden_cipher_func.harden_hash_update == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_update func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_update( uapi_hash_handle, src_buf, len );
}

td_s32 CIPHER_HASH_GET( td_handle uapi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx )
{
    if( g_harden_cipher_func.harden_hash_get == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_get func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_get( uapi_hash_handle, hash_clone_ctx );
}

td_s32 CIPHER_HASH_SET( td_handle uapi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx )
{
    if( g_harden_cipher_func.harden_hash_set == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_set func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_set( uapi_hash_handle, hash_clone_ctx );
}

td_s32 CIPHER_HASH_DESTROY( td_handle uapi_hash_handle )
{
    if( g_harden_cipher_func.harden_hash_destroy == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_destroy func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_destroy( uapi_hash_handle );
}

td_s32 CIPHER_HASH_FINISH( td_handle uapi_hash_handle, td_u8 *out, td_u32 *out_len )
{
    if( g_harden_cipher_func.harden_hash_finish == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hash_finish func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hash_finish( uapi_hash_handle, out, out_len );
}

/* HKDF */
td_s32 CIPHER_HKDF( crypto_hkdf_t *hkdf_param, td_u8 *okm, td_u32 okm_length )
{
    if( g_harden_cipher_func.harden_hkdf == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hkdf func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hkdf( hkdf_param, okm, okm_length );
}

td_s32 CIPHER_HKDF_EXTRACT( crypto_hkdf_extract_t *extract_param, td_u8 *prk, td_u32 *prk_length )
{
    if( g_harden_cipher_func.harden_hkdf_extract == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hkdf_extract func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hkdf_extract( extract_param, prk, prk_length );
}

td_s32 CIPHER_HKDF_EXPAND( const crypto_hkdf_expand_t *expand_param, td_u8 *okm, td_u32 okm_length )
{
    if( g_harden_cipher_func.harden_hkdf_expand == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_hkdf_expand func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_hkdf_expand( expand_param, okm, okm_length );
}

/* TRNG */
td_s32 CIPHER_TRNG_GET_RANDOM( td_u32 *randnum )
{
    if( g_harden_cipher_func.harden_trng_get_random == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_trng_get_random func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_trng_get_random( randnum );
}

td_s32 CIPHER_TRNG_GET_MULTI_RANDOM( td_u32 size, td_u8 *randnum )
{
    if( g_harden_cipher_func.harden_trng_get_multi_random == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_trng_get_multi_random func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_trng_get_multi_random(size, randnum);
}

/* PBKDF2 */
td_s32 CIPHER_PBKDF2( const crypto_kdf_pbkdf2_param *param, td_u8 *out, const td_u32 out_len )
{
    if( g_harden_cipher_func.harden_pbkdf2 == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pbkdf2 func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pbkdf2( param, out, out_len );
}

/* SYMC */
td_s32 CIPHER_SYMC_INIT( td_void )
{
    if( g_harden_cipher_func.harden_symc_init == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_init func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_init();
}

td_s32 CIPHER_SYMC_DEINIT( td_void )
{
    if( g_harden_cipher_func.harden_symc_deinit == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_deinit func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_deinit();
}

td_s32 CIPHER_SYMC_CREATE( td_handle *symc_handle, const crypto_symc_attr *symc_attr )
{
    if( g_harden_cipher_func.harden_symc_create == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_create func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_create( symc_handle, symc_attr );
}

td_s32 CIPHER_SYMC_DESTROY( td_handle symc_handle )
{
    if( g_harden_cipher_func.harden_symc_destroy == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_destroy func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_destroy( symc_handle );
}

td_s32 CIPHER_SYMC_SET_CONFIG( td_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl )
{
    if( g_harden_cipher_func.harden_symc_set_config == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_set_config func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_set_config( symc_handle, symc_ctrl );
}

td_s32 CIPHER_SYMC_GET_CONFIG( td_handle symc_handle, crypto_symc_ctrl_t *symc_ctrl )
{
    if( g_harden_cipher_func.harden_symc_get_config == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_get_config func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_get_config( symc_handle, symc_ctrl );
}

td_s32 CIPHER_SYMC_ATTACH( td_handle symc_handle, td_handle keyslot_handle )
{
    if( g_harden_cipher_func.harden_symc_attach == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_attach func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_attach( symc_handle, keyslot_handle );
}

td_s32 CIPHER_SYMC_ENCRYPT( td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length )
{
    if( g_harden_cipher_func.harden_symc_encrypt == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_encrypt func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_encrypt( symc_handle, src_buf, dst_buf, length );
}

td_s32 CIPHER_SYMC_DECRYPT( td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length )
{
    if( g_harden_cipher_func.harden_symc_decrypt == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_decrypt func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_decrypt( symc_handle, src_buf, dst_buf, length );
}

td_s32 CIPHER_SYMC_GET_TAG( td_handle symc_handle, td_u8 *tag, td_u32 tag_length )
{
    if( g_harden_cipher_func.harden_symc_get_tag == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_get_tag func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_get_tag( symc_handle, tag, tag_length );
}

td_s32 CIPHER_SYMC_MAC_START( td_handle *symc_handle, const crypto_symc_mac_attr *mac_attr )
{
    if( g_harden_cipher_func.harden_symc_mac_start == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_mac_start func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_mac_start( symc_handle, mac_attr );
}

td_s32 CIPHER_SYMC_MAC_UPDATE( td_handle symc_handle, const crypto_buf_attr *src_buf, td_u32 length )
{
    if( g_harden_cipher_func.harden_symc_mac_update == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_mac_update func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_mac_update( symc_handle, src_buf, length );
}

td_s32 CIPHER_SYMC_MAC_FINISH( td_handle symc_handle, td_u8 *mac, td_u32 *mac_length )
{
    if( g_harden_cipher_func.harden_symc_mac_finish == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_symc_mac_finish func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_symc_mac_finish( symc_handle, mac, mac_length );
}

/* PKE */
td_s32 CIPHER_PKE_INIT( td_void )
{
    if( g_harden_cipher_func.harden_pke_init == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_init func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_init();
}

td_s32 CIPHER_PKE_DEINIT( td_void )
{
    if( g_harden_cipher_func.harden_pke_deinit == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_deinit func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_deinit();
}

td_s32 CIPHER_PKE_MOD( const drv_pke_data *a, const drv_pke_data *p, drv_pke_data *c )
{
    if( g_harden_cipher_func.harden_pke_mod == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_mod func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_mod( a, p, c );
}

td_s32 CIPHER_PKE_EXP_MOD( const drv_pke_data *n, const drv_pke_data *k,
    const drv_pke_data *in, const drv_pke_data *out )
{
    if( g_harden_cipher_func.harden_pke_exp_mod == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_exp_mod func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_exp_mod( n, k, in, out );
}

td_s32 CIPHER_PKE_ECC_GEN_KEY( drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key )
{
    if( g_harden_cipher_func.harden_pke_ecc_gen_key == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_ecc_gen_key func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_ecc_gen_key( curve_type, input_priv_key, output_priv_key, output_pub_key );
}

td_s32 CIPHER_PKE_ECDSA_SIGN( drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig )
{
    if( g_harden_cipher_func.harden_pke_ecdsa_sign == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_ecdsa_sign func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_ecdsa_sign( curve_type, priv_key, hash, sig );
}
td_s32 CIPHER_PKE_ECDSA_VERIFY( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig )
{
    if( g_harden_cipher_func.harden_pke_ecdsa_verify == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_ecdsa_verify func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_ecdsa_verify( curve_type, pub_key, hash, sig );
}

td_s32 CIPHER_PKE_EDDSA_SIGN( drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig )
{
    if( g_harden_cipher_func.harden_pke_eddsa_sign == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_eddsa_sign func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_eddsa_sign( curve_type, priv_key, msg, sig );
}
td_s32 CIPHER_PKE_EDDSA_VERIFY( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig )
{
    if( g_harden_cipher_func.harden_pke_eddsa_verify == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_eddsa_verify func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_eddsa_verify( curve_type, pub_key, msg, sig );
}

td_s32 CIPHER_PKE_ECC_GEN_ECDH_KEY( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key )
{
    if( g_harden_cipher_func.harden_pke_gen_ecdh_key == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_gen_ecdh_key func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_gen_ecdh_key( curve_type, input_pub_key, input_priv_key, output_shared_key );
}

td_s32 CIPHER_PKE_CHECK_DOT_ON_CURVE( drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve )
{
    if( g_harden_cipher_func.harden_pke_check_dot_on_curve == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_check_dot_on_curve func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_check_dot_on_curve( curve_type, pub_key, is_on_curve );
}

td_s32 CIPHER_PKE_RSA_SIGN( const drv_pke_rsa_priv_key *priv_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, const drv_pke_data *input_hash,
    drv_pke_data *sign )
{
    if( g_harden_cipher_func.harden_pke_rsa_sign == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_rsa_sign func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_rsa_sign( priv_key, scheme, hash_type, input_hash, sign );
}

td_s32 CIPHER_PKE_RSA_VERIFY( const drv_pke_rsa_pub_key *pub_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, drv_pke_data *input_hash, const drv_pke_data *sig )
{
    if( g_harden_cipher_func.harden_pke_rsa_verify == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_rsa_verify func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_rsa_verify( pub_key, scheme, hash_type, input_hash, sig );
}

td_s32 CIPHER_PKE_RSA_PUBLIC_ENCRYPT( drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output )
{
    if( g_harden_cipher_func.harden_pke_rsa_public_encrypt == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_rsa_public_encrypt func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_rsa_public_encrypt( scheme, hash_type, pub_key, input, label, output );
}

td_s32 CIPHER_PKE_RSA_PRIVATE_DECRYPT( drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output )
{
    if( g_harden_cipher_func.harden_pke_rsa_private_decrypt == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_pke_rsa_private_decrypt func\n");
        return TD_FAILURE;
    }
    return g_harden_cipher_func.harden_pke_rsa_private_decrypt( scheme, hash_type, priv_key, input, label, output );
}

/* KM */
td_s32 KM_INIT( td_void )
{
    if( g_harden_km_func.harden_km_init == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_km_init func\n");
        return TD_FAILURE;
    }
    return g_harden_km_func.harden_km_init();
}

td_s32 KM_DEINIT( td_void )
{
    if( g_harden_km_func.harden_km_deinit == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_km_deinit func\n");
        return TD_FAILURE;
    }
    return g_harden_km_func.harden_km_deinit();
}

td_s32 CIPHER_CREATE_KEYSLOT( td_handle *keyslot_handle, km_keyslot_engine key_engine )
{
    if( g_harden_km_func.harden_km_create_keyslot == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_km_create_keyslot func\n");
        return TD_FAILURE;
    }
    return g_harden_km_func.harden_km_create_keyslot( keyslot_handle, key_engine );
}

td_void CIPHER_DESTROY_KEYSLOT( td_handle keyslot_handle )
{
    if( g_harden_km_func.harden_km_destroy_keyslot == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_km_destroy_keyslot func\n");
        return;
    }
    g_harden_km_func.harden_km_destroy_keyslot( keyslot_handle );
}

td_s32 CIPHER_SET_CLEAR_KEY( td_handle keyslot_handle, td_u8 *key, td_u32 keylen,
    km_keyslot_engine key_engine )
{
    if( g_harden_km_func.harden_km_set_clear_key == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_km_set_clear_key func\n");
        return TD_FAILURE;
    }
    return g_harden_km_func.harden_km_set_clear_key( keyslot_handle, key, keylen, key_engine );
}

/* MEM */
td_s32 CIPHER_ALLOC_BUF_ATTR( crypto_buf_attr *buf_attr, void **virt_addr, unsigned int size )
{
    if( g_harden_mem_func.harden_alloc_phys_buf == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_alloc_phys_buf func\n");
        return TD_FAILURE;
    }
    return g_harden_mem_func.harden_alloc_phys_buf( buf_attr, virt_addr, size );
}

td_void CIPHER_FREE_BUF_ATTR( crypto_buf_attr *buf_attr, void *virt_addr, unsigned int size )
{
    if( g_harden_mem_func.harden_free_phys_buf == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_free_phys_buf func\n");
        return;
    }
    g_harden_mem_func.harden_free_phys_buf( buf_attr, virt_addr, size );
}

td_s32 CIPHER_GET_PHYS_MEM( void *virt_addr, unsigned long *phys_addr )
{
    if( g_harden_mem_func.harden_get_phys_addr == TD_NULL )
    {
        mbedtls_harden_log_err("Unexpected error, please register the harden_get_phys_addr func\n");
        return TD_FAILURE;
    }
    return g_harden_mem_func.harden_get_phys_addr( virt_addr, phys_addr );
}

td_bool IS_KM_FUNC_REGISTERED()
{
    if ( g_harden_km_func.harden_km_init != TD_NULL &&
            g_harden_km_func.harden_km_deinit != TD_NULL &&
            g_harden_km_func.harden_km_create_keyslot != TD_NULL &&
            g_harden_km_func.harden_km_destroy_keyslot != TD_NULL &&
            g_harden_km_func.harden_km_set_clear_key != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_MEM_FUNC_REGISTERED()
{
    if ( g_harden_mem_func.harden_alloc_phys_buf != TD_NULL &&
            g_harden_mem_func.harden_free_phys_buf != TD_NULL &&
            g_harden_mem_func.harden_get_phys_addr != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_SYMC_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_symc_init != TD_NULL &&
            g_harden_cipher_func.harden_symc_deinit != TD_NULL &&
            g_harden_cipher_func.harden_symc_create != TD_NULL &&
            g_harden_cipher_func.harden_symc_destroy != TD_NULL &&
            g_harden_cipher_func.harden_symc_set_config != TD_NULL &&
            g_harden_cipher_func.harden_symc_get_config != TD_NULL &&
            g_harden_cipher_func.harden_symc_attach != TD_NULL &&
            g_harden_cipher_func.harden_symc_encrypt != TD_NULL &&
            g_harden_cipher_func.harden_symc_decrypt != TD_NULL &&
            g_harden_cipher_func.harden_symc_get_tag != TD_NULL &&
            g_harden_cipher_func.harden_symc_mac_start != TD_NULL &&
            g_harden_cipher_func.harden_symc_mac_update != TD_NULL &&
            g_harden_cipher_func.harden_symc_mac_finish != TD_NULL &&
            IS_KM_FUNC_REGISTERED() &&
            IS_MEM_FUNC_REGISTERED() )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_HASH_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_hash_init  != TD_NULL &&
            g_harden_cipher_func.harden_hash_deinit != TD_NULL &&
            g_harden_cipher_func.harden_hash_start != TD_NULL &&
            g_harden_cipher_func.harden_hash_update != TD_NULL &&
            g_harden_cipher_func.harden_hash_get != TD_NULL &&
            g_harden_cipher_func.harden_hash_set != TD_NULL &&
            g_harden_cipher_func.harden_hash_destroy != TD_NULL &&
            g_harden_cipher_func.harden_hash_finish != TD_NULL &&
            IS_KM_FUNC_REGISTERED() &&
            IS_MEM_FUNC_REGISTERED() )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_HKDF_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_hash_init  != TD_NULL &&
            g_harden_cipher_func.harden_hash_deinit != TD_NULL &&
            g_harden_cipher_func.harden_hkdf != TD_NULL &&
            g_harden_cipher_func.harden_hkdf_extract != TD_NULL &&
            g_harden_cipher_func.harden_hkdf_expand != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PBKDF2_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pbkdf2 != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_TRNG_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_trng_get_random != TD_NULL &&
            g_harden_cipher_func.harden_trng_get_multi_random != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_MOD_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_mod != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_EXP_MOD_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_exp_mod != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_EDDSA_SIGN_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_eddsa_sign != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_EDDSA_VERIFY_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_eddsa_verify != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_ECC_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_gen_ecdh_key != TD_NULL &&
            g_harden_cipher_func.harden_pke_ecc_gen_key != TD_NULL &&
            g_harden_cipher_func.harden_pke_ecdsa_sign != TD_NULL &&
            g_harden_cipher_func.harden_pke_ecdsa_verify != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_CHECK_DOT_ON_CURVE_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_check_dot_on_curve != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_RSA_SIGN_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_rsa_sign != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_RSA_VERIFY_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_rsa_verify != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_RSA_PUBLIC_ENCRYPT_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_rsa_public_encrypt != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_bool IS_PKE_RSA_PRIVATE_DECRYPT_FUNC_REGISTERED()
{
    if ( g_harden_cipher_func.harden_pke_init != TD_NULL &&
            g_harden_cipher_func.harden_pke_deinit != TD_NULL &&
            g_harden_cipher_func.harden_pke_rsa_private_decrypt != TD_NULL )
    {
        return TD_TRUE;
    }
    return TD_FALSE;
}