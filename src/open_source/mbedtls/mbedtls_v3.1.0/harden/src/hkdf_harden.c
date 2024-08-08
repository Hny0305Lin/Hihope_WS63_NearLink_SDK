/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "hkdf_harden.h"
#include "cipher_adapt.h"
#include "mbedtls/error.h"
#include "dfx.h"
#include "hash_harden_common.h"
#include "mbedtls/platform_util.h"

#if defined(MBEDTLS_HKDF_ALT)
int hkdf_harden_can_do( const mbedtls_md_info_t *md )
{
    crypto_hash_type hmac_type = CRYPTO_HASH_TYPE_INVALID;

    if( IS_HKDF_FUNC_REGISTERED() != TD_TRUE || md == NULL )
        return ( 0 );

    get_hash_info( md->type, &hmac_type, NULL, NULL );

    return( hmac_type != CRYPTO_HASH_TYPE_INVALID );
}

int hkdf_extract_harden( const mbedtls_md_info_t *md,
        const unsigned char *salt, size_t salt_len,
        const unsigned char *ikm, size_t ikm_len,
        unsigned char *prk )
{
    int ret;
    crypto_hkdf_extract_t extract_param;
    unsigned int prk_len = 0;
    crypto_hash_type hmac_type = CRYPTO_HASH_TYPE_INVALID;

    mbedtls_harden_log_func_enter();

    get_hash_info( md->type, &hmac_type, NULL, NULL );

    extract_param.hmac_type = hmac_type;
    extract_param.salt = (unsigned char *)salt;
    extract_param.salt_length = salt_len;
    extract_param.ikm = (unsigned char *)ikm;
    extract_param.ikm_length = ikm_len;

    ret = CIPHER_HASH_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_INIT failed, ret = 0x%x!\n", ret );
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    ret = CIPHER_HKDF_EXTRACT( &extract_param, prk, &prk_len );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HKDF_EXTRACT failed, ret = 0x%x!\n", ret );

        CIPHER_HASH_DEINIT();

        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    CIPHER_HASH_DEINIT();

    mbedtls_harden_log_func_exit();

    return( 0 );
}

int hkdf_expand_harden( const mbedtls_md_info_t *md, const unsigned char *prk,
                         size_t prk_len, const unsigned char *info,
                         size_t info_len, unsigned char *okm, size_t okm_len )
{
    int ret;
    crypto_hkdf_expand_t expand_param;
    crypto_hash_type hmac_type = CRYPTO_HASH_TYPE_INVALID;

    mbedtls_harden_log_func_enter();

    get_hash_info( md->type, &hmac_type, NULL, NULL );

    expand_param.hmac_type = hmac_type;
    expand_param.prk = (unsigned char *)prk;
    expand_param.prk_length = prk_len;
    expand_param.info = (unsigned char *)info;
    expand_param.info_length = info_len;

    ret = CIPHER_HASH_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_INIT failed, ret = 0x%x!\n", ret );
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    ret = CIPHER_HKDF_EXPAND( &expand_param, okm, okm_len );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HKDF_EXPAND failed, ret = 0x%x!\n", ret );

        CIPHER_HASH_DEINIT();

        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    CIPHER_HASH_DEINIT();

    mbedtls_harden_log_func_exit();

    return( 0 );
}

int hkdf_harden( const mbedtls_md_info_t *md, const unsigned char *salt,
                  size_t salt_len, const unsigned char *ikm, size_t ikm_len,
                  const unsigned char *info, size_t info_len,
                  unsigned char *okm, size_t okm_len )
{
    int ret;
    crypto_hkdf_t hkdf_param;
    crypto_hash_type hmac_type = CRYPTO_HASH_TYPE_INVALID;

    mbedtls_harden_log_func_enter();

    get_hash_info( md->type, &hmac_type, NULL, NULL );

    hkdf_param.hmac_type = hmac_type;
    hkdf_param.salt = (unsigned char *)salt;
    hkdf_param.salt_length = salt_len;
    hkdf_param.ikm = (unsigned char *)ikm;
    hkdf_param.ikm_length = ikm_len;
    hkdf_param.info = (unsigned char *)info;
    hkdf_param.info_length = info_len;

    ret = CIPHER_HASH_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_INIT failed, ret = 0x%x!\n", ret );
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    ret = CIPHER_HKDF( &hkdf_param, okm, okm_len );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HKDF failed, ret = 0x%x!\n", ret );

        CIPHER_HASH_DEINIT();

        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );
    }

    CIPHER_HASH_DEINIT();

    mbedtls_harden_log_func_exit();

    return( 0 );
}

int mbedtls_hkdf( const mbedtls_md_info_t *md, const unsigned char *salt,
                  size_t salt_len, const unsigned char *ikm, size_t ikm_len,
                  const unsigned char *info, size_t info_len,
                  unsigned char *okm, size_t okm_len )
{
    if( md == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    if( hkdf_harden_can_do( md ) == 1 )
    {
        return hkdf_harden( md, salt, salt_len, ikm, ikm_len, info, info_len, okm, okm_len );
    }

    return( -1 );
}

int mbedtls_hkdf_extract( const mbedtls_md_info_t *md,
                          const unsigned char *salt, size_t salt_len,
                          const unsigned char *ikm, size_t ikm_len,
                          unsigned char *prk )
{
    size_t hash_len;
    if( salt == NULL )
    {
        if( salt_len != 0 )
        {
            return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
        }

        hash_len = mbedtls_md_get_size( md );

        if( hash_len == 0 )
        {
            return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
        }
    }

    if( hkdf_harden_can_do( md ) == 1 )
    {
        return hkdf_extract_harden( md, salt, salt_len, ikm, ikm_len, prk );
    }

    return( -1 );
}

int mbedtls_hkdf_expand( const mbedtls_md_info_t *md, const unsigned char *prk,
                         size_t prk_len, const unsigned char *info,
                         size_t info_len, unsigned char *okm, size_t okm_len )
{
    if( okm == NULL )
    {
        return( MBEDTLS_ERR_HKDF_BAD_INPUT_DATA );
    }

    if( hkdf_harden_can_do( md ) == 1 )
    {
        return hkdf_expand_harden( md, prk, prk_len, info, info_len, okm, okm_len );
    }

    return( -1 );
}
#endif /* MBEDTLS_HKDF_ALT */