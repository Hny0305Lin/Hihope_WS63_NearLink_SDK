/**
 * \brief NIST SP800-38B compliant CMAC implementation for AES and 3DES. And adapt to cmac harden API.
 *
 *  Copyright The Mbed TLS Contributors
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * References:
 *
 * - NIST SP 800-38B Recommendation for Block Cipher Modes of Operation: The
 *      CMAC Mode for Authentication
 *   http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-38b.pdf
 *
 * - RFC 4493 - The AES-CMAC Algorithm
 *   https://tools.ietf.org/html/rfc4493
 *
 * - RFC 4615 - The Advanced Encryption Standard-Cipher-based Message
 *      Authentication Code-Pseudo-Random Function-128 (AES-CMAC-PRF-128)
 *      Algorithm for the Internet Key Exchange Protocol (IKE)
 *   https://tools.ietf.org/html/rfc4615
 *
 *   Additional test vectors: ISO/IEC 9797-1
 *
 */

#include "common.h"
#include "mbedtls/error.h"
#include "cmac_harden.h"
#include "cipher_common.h"
#include "dfx.h"

#if defined(MBEDTLS_CMAC_ALT)

static cmac_harden_context g_cmac_harden_context[MAX_CMAC_HARD_CTX_NUM] = {0};

static cmac_harden_context * get_cmac_harden_ctx( mbedtls_cipher_context_t *ctx )
{
    unsigned int i = 0;
    for( i = 0; i < MAX_CMAC_HARD_CTX_NUM; ++i )
    {
        if( g_cmac_harden_context[i].ctx == ctx )
        {
            return &g_cmac_harden_context[i];
        }
    }
    return NULL;
}

static int internal_cmac_starts_harden( cmac_harden_context *harden_ctx )
{
    int ret = 0;
    crypto_symc_mac_attr mac_attr = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_CMAC,
        .is_long_term = 1
    };

    ret = CIPHER_SYMC_INIT();
    if(ret != 0)
        return ret;
    ret = KM_INIT();
    if(ret != 0)
        goto cipher_deinit;
    ret = CIPHER_CREATE_KEYSLOT(&harden_ctx->keyslot_handle, KM_KEYSLOT_ENGINE_AES);
    if(ret != 0)
        goto km_deinit;
    
    ret = CIPHER_SET_CLEAR_KEY(harden_ctx->keyslot_handle, harden_ctx->key, harden_ctx->key_length,
        KM_KEYSLOT_ENGINE_AES);
    if( ret != 0 )
        goto keyslot_destroy;
    mac_attr.keyslot_chn = harden_ctx->keyslot_handle;
    switch( harden_ctx->key_length )
    {
        case 16:
            mac_attr.symc_key_length = CRYPTO_SYMC_KEY_128BIT;
            break;
        case 24:
            mac_attr.symc_key_length = CRYPTO_SYMC_KEY_192BIT;
            break;
        case 32:
            mac_attr.symc_key_length = CRYPTO_SYMC_KEY_256BIT;
            break;
    }
    ret = CIPHER_SYMC_MAC_START(&harden_ctx->symc_handle, &mac_attr);
    if(ret != 0)
        goto keyslot_destroy;
    return ret;

keyslot_destroy:
    CIPHER_DESTROY_KEYSLOT(harden_ctx->keyslot_handle);
km_deinit:
    (void)KM_DEINIT();
cipher_deinit:
    (void)CIPHER_SYMC_DEINIT();
    return ret;
}

static void internal_check_and_free_harden_ctx( mbedtls_cipher_context_t *ctx )
{
    int i = 0;
    for( i = 0; i < MAX_CMAC_HARD_CTX_NUM; ++i )
    {
        if( g_cmac_harden_context[i].ctx == ctx ) {
            if( g_cmac_harden_context[i].symc_handle != 0 )
            {
                ( void )CIPHER_SYMC_DESTROY( g_cmac_harden_context[i].symc_handle );
                ( void )CIPHER_SYMC_DEINIT();
            }
            if( g_cmac_harden_context[i].keyslot_handle != 0 )
            {
                ( void )CIPHER_DESTROY_KEYSLOT( g_cmac_harden_context[i].keyslot_handle );
                ( void )KM_DEINIT();
            }
            memset( &g_cmac_harden_context[i], 0, sizeof( cmac_harden_context ) );
        }
    }
}

int mbedtls_cipher_cmac_starts_harden( mbedtls_cipher_context_t *ctx,
                                const unsigned char *key, size_t keybits,
                                mbedtls_cipher_type_t type )
{
    int i = 0;
    int ret = 0;
    internal_check_and_free_harden_ctx( ctx );
    if( IS_SYMC_FUNC_REGISTERED() != TD_TRUE )
        return ( -1 );
    if( type == MBEDTLS_CIPHER_DES_EDE3_ECB )
        return ( -1 );
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
    if( type == MBEDTLS_CIPHER_AES_192_ECB )
        return ( -1 );
#endif
    for(i = 0; i < MAX_CMAC_HARD_CTX_NUM; ++i)
    {
        if( g_cmac_harden_context[i].is_used == 0 ) {
            g_cmac_harden_context[i].ctx = ctx;
            g_cmac_harden_context[i].is_used = 1;
            g_cmac_harden_context[i].key_length = keybits / 8;
            g_cmac_harden_context[i].is_update = 0;
            g_cmac_harden_context[i].is_finish = 0;
            memcpy( g_cmac_harden_context[i].key, key, g_cmac_harden_context[i].key_length );
            ret = internal_cmac_starts_harden( &g_cmac_harden_context[i] );
            if( ret != 0 )
                g_cmac_harden_context[i].use_software = 1;
            break;
        }
    }
    return ret;
}

int mbedtls_cipher_cmac_update_harden( mbedtls_cipher_context_t *ctx,
                                        const unsigned char *input, size_t ilen )
{
    int ret = 0;
    cmac_harden_context *harden_ctx;
    crypto_buf_attr src_buf = {0};

    harden_ctx = get_cmac_harden_ctx( ctx );
    if( harden_ctx == NULL || harden_ctx->use_software == 1 )
        return ( -1 );
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
    if( harden_ctx->key_length == 24 )
        return ( -1 );
#endif
    if( harden_ctx->is_finish == 1 )
    {
        ret = internal_cmac_starts_harden( harden_ctx );
        if( ret != 0 )
        {
            harden_ctx->is_finish = 0;
            harden_ctx->use_software = 1;
            return ( -1 );
        }
    }
    if( ilen == 0 )
        return ( 0 );
    harden_ctx->is_update = 1;
    src_buf.virt_addr = ( void * )input;
    ret = CIPHER_SYMC_MAC_UPDATE( harden_ctx->symc_handle, &src_buf, ilen );
    return ret;
}

static void reset_harden_ctx( cmac_harden_context *harden_ctx )
{
    harden_ctx->is_finish = 1;
    harden_ctx->is_update = 0;
    harden_ctx->use_software = 0;
    harden_ctx->symc_handle = 0;
    harden_ctx->keyslot_handle = 0;
}

static void release_hard_resource( cmac_harden_context *harden_ctx )
{
    ( void )CIPHER_DESTROY_KEYSLOT( harden_ctx->keyslot_handle );
    ( void )KM_DEINIT();
    ( void )CIPHER_SYMC_DEINIT();
}

int mbedtls_cipher_cmac_finish_harden( mbedtls_cipher_context_t *ctx,
                                        unsigned char *output )
{
    int ret = 0;
    cmac_harden_context *harden_ctx;
    unsigned int mac_len = MBEDTLS_CIPHER_BLKSIZE_MAX;
    harden_ctx = get_cmac_harden_ctx( ctx );
    if( harden_ctx == NULL )
        return ( -1 );
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
    if( harden_ctx->key_length == 24 )
        return ( -1 );
#endif
    if( harden_ctx->is_update == 0 )
    {
        ( void )CIPHER_SYMC_DESTROY( harden_ctx->symc_handle );
        release_hard_resource( harden_ctx );
        reset_harden_ctx( harden_ctx );
        return ( -1 );
    }
    ret = CIPHER_SYMC_MAC_FINISH( harden_ctx->symc_handle, output, &mac_len );
    release_hard_resource( harden_ctx );
    reset_harden_ctx( harden_ctx );
    return ret;
}

int mbedtls_cipher_cmac_reset_harden( mbedtls_cipher_context_t *ctx )
{
    cmac_harden_context *harden_ctx;
    harden_ctx = get_cmac_harden_ctx( ctx );
    if( harden_ctx == NULL )
        return ( -1 );

    if(harden_ctx->symc_handle != 0)
    {
        (void)CIPHER_SYMC_DESTROY(harden_ctx->symc_handle);
        (void)CIPHER_SYMC_DEINIT();
    }
    if(harden_ctx->keyslot_handle != 0)
    {
        (void)CIPHER_DESTROY_KEYSLOT(harden_ctx->keyslot_handle);
        (void)KM_DEINIT();
    }
    reset_harden_ctx( harden_ctx );
    return 0;
}

int mbedtls_cipher_cmac_starts( mbedtls_cipher_context_t *ctx,
                                const unsigned char *key, size_t keybits )
{
    mbedtls_cipher_type_t type;

    if( ctx == NULL || ctx->cipher_info == NULL || key == NULL )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    type = ctx->cipher_info->type;

    switch( type )
    {
        case MBEDTLS_CIPHER_AES_128_ECB:
        case MBEDTLS_CIPHER_AES_192_ECB:
        case MBEDTLS_CIPHER_AES_256_ECB:
            break;
        default:
            return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    return mbedtls_cipher_cmac_starts_harden( ctx, key, keybits, type );
}

int mbedtls_cipher_cmac_update( mbedtls_cipher_context_t *ctx,
                                const unsigned char *input, size_t ilen )
{
    if( ctx == NULL || ctx->cipher_info == NULL || input == NULL)
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    return mbedtls_cipher_cmac_update_harden( ctx, input, ilen );
}

int mbedtls_cipher_cmac_finish( mbedtls_cipher_context_t *ctx,
                                unsigned char *output )
{
    if( ctx == NULL || ctx->cipher_info == NULL || output == NULL )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    return mbedtls_cipher_cmac_finish_harden( ctx, output );
}

int mbedtls_cipher_cmac_reset( mbedtls_cipher_context_t *ctx )
{

    if( ctx == NULL || ctx->cipher_info == NULL )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    return mbedtls_cipher_cmac_reset_harden( ctx );
}

int mbedtls_cipher_cmac( const mbedtls_cipher_info_t *cipher_info,
                         const unsigned char *key, size_t keylen,
                         const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    mbedtls_cipher_context_t ctx;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    if( cipher_info == NULL || key == NULL || input == NULL || output == NULL )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    mbedtls_cipher_init( &ctx );

    if( ( ret = mbedtls_cipher_setup( &ctx, cipher_info ) ) != 0 )
        goto exit;

    ret = mbedtls_cipher_cmac_starts( &ctx, key, keylen );
    if( ret != 0 )
       goto exit;

    ret = mbedtls_cipher_cmac_update( &ctx, input, ilen );
    if( ret != 0 )
        goto exit;

exit:
    ret = mbedtls_cipher_cmac_finish( &ctx, output );

    mbedtls_cipher_free( &ctx );

    return( ret );
}

int mbedtls_aes_cmac_prf_128( const unsigned char *key, size_t key_length,
                              const unsigned char *input, size_t in_len,
                              unsigned char output[16] )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const mbedtls_cipher_info_t *cipher_info;
    unsigned char zero_key[MBEDTLS_AES_BLOCK_SIZE];
    unsigned char int_key[MBEDTLS_AES_BLOCK_SIZE];

    if( key == NULL || input == NULL || output == NULL )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    cipher_info = mbedtls_cipher_info_from_type( MBEDTLS_CIPHER_AES_128_ECB );
    if( cipher_info == NULL )
    {
        /* Failing at this point must be due to a build issue */
        ret = MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
        goto exit;
    }

    if( key_length == MBEDTLS_AES_BLOCK_SIZE )
    {
        /* Use key as is */
        memcpy( int_key, key, MBEDTLS_AES_BLOCK_SIZE );
    }
    else
    {
        memset( zero_key, 0, MBEDTLS_AES_BLOCK_SIZE );

        ret = mbedtls_cipher_cmac( cipher_info, zero_key, 128, key,
                                   key_length, int_key );
        if( ret != 0 )
            goto exit;
    }

    ret = mbedtls_cipher_cmac( cipher_info, int_key, 128, input, in_len,
                               output );

exit:
    mbedtls_platform_zeroize( int_key, sizeof( int_key ) );

    return( ret );
}

#endif /* !MBEDTLS_CMAC_ALT */