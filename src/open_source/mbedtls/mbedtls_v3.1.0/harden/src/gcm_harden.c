/*
 *  NIST SP800-38D compliant GCM implementation. And adapt to the gcm harden API.
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
 * http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * See also:
 * [MGV] http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf
 *
 * We use the algorithm described as Shoup's method with 4-bit tables in
 * [MGV] 4.1, pp. 12-13, to enhance speed without using too much memory.
 */

#include "common.h"
#include "mbedtls/gcm.h"
#include "gcm_harden.h"
#include "cipher_common.h"
#include "dfx.h"

#if defined(MBEDTLS_GCM_ALT)

#define GCM_VALIDATE_RET( cond ) \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_GCM_BAD_INPUT )
#define GCM_VALIDATE( cond ) \
    MBEDTLS_INTERNAL_VALIDATE( cond )

static gcm_harden_context g_gcm_harden_context[MAX_GCM_HARD_CTX_NUM] = {0};

static gcm_harden_context * get_gcm_harden_ctx( mbedtls_gcm_context *ctx )
{
    unsigned int i = 0;
    for( i = 0; i < MAX_GCM_HARD_CTX_NUM; ++i )
    {
        if( g_gcm_harden_context[i].ctx == ctx )
        {
            return &g_gcm_harden_context[i];
        }
    }
    return NULL;
}

static void internal_release_hard_resource( gcm_harden_context *harden_ctx )
{
    if( harden_ctx->symc_handle != 0 )
    {
        ( void )CIPHER_SYMC_DESTROY( harden_ctx->symc_handle );
        harden_ctx->symc_handle = 0;
    }
    if( harden_ctx->keyslot_handle != 0 )
    {
        ( void )CIPHER_DESTROY_KEYSLOT( harden_ctx->keyslot_handle );
        harden_ctx->keyslot_handle = 0;
    }
}

void mbedtls_gcm_init_harden( mbedtls_gcm_context *ctx )
{
    int i = 0;
    int ret = 0;

    if( IS_SYMC_FUNC_REGISTERED() != TD_TRUE )
        return;

    ret = CIPHER_SYMC_INIT();
    if( ret != 0 ) {
        mbedtls_harden_log_err( "CIPHER_SYMC_INIT faild!\n" );
        return;
    }

    ret = KM_INIT();
    if( ret != 0 ) {
        CIPHER_SYMC_DEINIT();
        mbedtls_harden_log_err( "KM_INIT faild!\n" );
        return;
    }

    for( i = 0; i < MAX_GCM_HARD_CTX_NUM; ++i )
    {
        if( g_gcm_harden_context[i].is_used == 0 ) {
            g_gcm_harden_context[i].ctx = ctx;
            g_gcm_harden_context[i].is_used = 1;
            g_gcm_harden_context[i].only_software_support = 0;
            g_gcm_harden_context[i].is_set_config = 0;
            g_gcm_harden_context[i].is_software_update_ad = 0;
            break;
        }
    }
}

int mbedtls_gcm_setkey_harden( mbedtls_gcm_context *ctx,
                                mbedtls_cipher_id_t cipher,
                                const unsigned char *key,
                                unsigned int keybits )
{
    gcm_harden_context *harden_ctx;
    harden_ctx = get_gcm_harden_ctx( ctx );
    if( harden_ctx == NULL )
        return -1;
    harden_ctx->key_length = keybits / 8;
    memcpy( harden_ctx->key, key, keybits / 8 );
    harden_ctx->only_software_support = 0;
    if( cipher == MBEDTLS_CIPHER_ID_CAMELLIA )
        harden_ctx->only_software_support = 1;
#if !defined( MBEDTLS_AES_192_HARD_KEY_SUPPORT )
        if( keybits == 192 )
            harden_ctx->only_software_support = 1;
#endif
    return 0;
}

/*
 * GCM hardware start
 */
int mbedtls_gcm_start_harden( mbedtls_gcm_context *ctx,
                                int mode,
                                const unsigned char *iv,
                                size_t iv_len )
{
    int ret = 0;
    gcm_harden_context *harden_ctx;
    crypto_symc_attr symc_attr = { 
        .symc_type = CRYPTO_SYMC_TYPE_NORMAL,
        .is_long_term = 0
    };

    harden_ctx = get_gcm_harden_ctx( ctx );
    if( harden_ctx == NULL || harden_ctx->only_software_support == 1 )
        return ( -1 );
    if( iv_len > 16 )
    {
        ret = -1;
        goto exit_set_ctx;
    }
    harden_ctx->iv_len = iv_len;
    harden_ctx->mode = mode;
    memcpy( harden_ctx->iv, iv, iv_len );

    internal_release_hard_resource( harden_ctx );

    ret = CIPHER_SYMC_CREATE( &harden_ctx->symc_handle, &symc_attr );
    if( ret != 0 ) {
        mbedtls_harden_log_err( "CIPHER_SYMC_CREATE faild!\n" );
        goto exit_set_ctx;
    }
    ret = CIPHRE_CREATE_KEYSLOT( &harden_ctx->keyslot_handle, KM_KEYSLOT_ENGINE_AES );
    if( ret != 0 ) {
        mbedtls_harden_log_err( "CIPHRE_CREATE_KEYSLOT faild!\n" );
        goto exit_cipher_destroy;
    }
    ret = CIPHER_SET_CLEAR_KEY( harden_ctx->keyslot_handle, harden_ctx->key, harden_ctx->key_length,
        KM_KEYSLOT_ENGINE_AES );
    if( ret != 0 ) {
        mbedtls_harden_log_err( "CIPHER_SET_CLEAR_KEY faild!\n" );
        goto exit_destroy_keyslot;
    }
    ret = CIPHER_SYMC_ATTACH( harden_ctx->symc_handle, harden_ctx->keyslot_handle );
    if( ret != 0 ) {
        mbedtls_harden_log_err( "CIPHER_SYMC_ATTACH faild!\n" );
        goto exit_destroy_keyslot;
    }
    return ( 0 );

exit_destroy_keyslot:
    CIPHER_DESTROY_KEYSLOT( harden_ctx->keyslot_handle );
exit_cipher_destroy:
    CIPHER_SYMC_DESTROY( harden_ctx->symc_handle );
exit_set_ctx:
    harden_ctx->only_software_support = 1;
    return ret;
}

/*
 * GCM hardware cache add
 */
int mbedtls_gcm_update_ad_harden( mbedtls_gcm_context *ctx,
                                const unsigned char *add,
                                size_t add_len )
{
    int ret = 0;
    gcm_harden_context *harden_ctx;
    harden_ctx = get_gcm_harden_ctx( ctx );
    if( harden_ctx == NULL || harden_ctx->only_software_support == 1 )
        return ( -1 );
    if( harden_ctx->add_len + add_len > MBEDTLS_CIPHER_MAX_ADD_LEN )
    {
        harden_ctx->only_software_support = 1;
        return ( -1 );
    }
    memcpy( harden_ctx->add + harden_ctx->add_len, add, add_len );
    harden_ctx->add_len += add_len;
    return ( 0 );
}

/*
 * GCM hardware update
 */
int mbedtls_gcm_update_harden( mbedtls_gcm_context *ctx,
                                const unsigned char *input, size_t input_length,
                                unsigned char *output )
{
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    crypto_buf_attr in_buf;
    crypto_buf_attr out_buf;
    gcm_harden_context *harden_ctx;

    harden_ctx = get_gcm_harden_ctx( ctx );
    if( harden_ctx == NULL || harden_ctx->only_software_support == 1 )
        return ( -1 );
    if( CIPHER_GET_PHYS_MEM( ( void * )input, &in_phys_addr ) != 0 ||
        CIPHER_GET_PHYS_MEM( output, &out_phys_addr ) != 0 )
    {
        harden_ctx->only_software_support = 1;
        return ( -1 );
    }
    if( harden_ctx->is_set_config == 0 )
    {
        ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_GCM,
            harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT, CRYPTO_SYMC_GCM_IV_CHANGE_START,
            harden_ctx->iv, harden_ctx->iv_len );
        if( ret != 0 )
            return ret;
        ret = mbedtls_cipher_set_gcm_ccm_config( &harden_ctx->gcm_config, harden_ctx->add, harden_ctx->add_len,
            0, 16 );
        if( ret != 0 )
            return ret;
        harden_ctx->symc_ctrl.param = ( void * )&harden_ctx->gcm_config;
        ret = CIPHER_SYMC_SET_CONFIG( harden_ctx->symc_handle, &harden_ctx->symc_ctrl );
        if( ret != 0 )
        {
            mbedtls_harden_log_err( "CIPHER_SYMC_SET_CONFIG faild!\n" );
            return ret;
        }
        harden_ctx->is_set_config = 1;
    }
    in_buf.virt_addr = (void *)input;
    in_buf.phys_addr = in_phys_addr;
    out_buf.virt_addr = output;
    out_buf.phys_addr = out_phys_addr;
    if( harden_ctx->mode == MBEDTLS_GCM_ENCRYPT )
    {
        ret = CIPHER_SYMC_ENCRYPT( harden_ctx->symc_handle, &in_buf, &out_buf, input_length );
    } else {
        ret = CIPHER_SYMC_DECRYPT( harden_ctx->symc_handle, &in_buf, &out_buf, input_length );
    }
    if( ret != 0 )
        mbedtls_harden_log_err( "CIPHER_SYMC_CRYPTO faild!\n" );
    return ret;
}

/*
 * GCM hardware finish
 */
int mbedtls_gcm_finish_harden( mbedtls_gcm_context *ctx,
                                unsigned char *tag, size_t tag_len )
{
    int ret = 0;
    gcm_harden_context *harden_ctx;
    if( tag_len > 16 || tag_len < 4 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    harden_ctx = get_gcm_harden_ctx( ctx );
    if ( harden_ctx == NULL || harden_ctx->only_software_support == 1 )
        return ( -1 );
    if ( harden_ctx->is_set_config == 0 )
    {
        mbedtls_harden_log_err( "set config first!\n" );
        return ( -1 );
    }
    ret = CIPHER_SYMC_GET_TAG( harden_ctx->symc_handle, tag, tag_len );
    if( ret != 0 )
        mbedtls_harden_log_err( "CIPHER_SYMC_GET_TAG faild!\n" );
    internal_release_hard_resource( harden_ctx );
    harden_ctx->is_set_config = 0;
    return ret;
}

void mbedtls_gcm_free_harden( mbedtls_gcm_context *ctx )
{
    gcm_harden_context * harden_ctx;
    harden_ctx = get_gcm_harden_ctx( ctx );
    if ( harden_ctx != NULL )
    {
        internal_release_hard_resource( harden_ctx );
        KM_DEINIT();
        CIPHER_SYMC_DEINIT();
        ( void )memset( harden_ctx, 0, sizeof( gcm_harden_context ) );
    }
}

void mbedtls_gcm_init( mbedtls_gcm_context *ctx )
{
    GCM_VALIDATE( ctx != NULL );

    mbedtls_gcm_init_harden( ctx );
}

int mbedtls_gcm_setkey( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( key != NULL );
    GCM_VALIDATE_RET( keybits == 128 || keybits == 192 || keybits == 256 );

    return mbedtls_gcm_setkey_harden( ctx, cipher, key, keybits );
}

int mbedtls_gcm_starts( mbedtls_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv, size_t iv_len )
{
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );

    return mbedtls_gcm_start_harden( ctx, mode, iv, iv_len );
}

int mbedtls_gcm_update_ad( mbedtls_gcm_context *ctx,
                           const unsigned char *add, size_t add_len )
{
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );

    /* IV is limited to 2^64 bits, so 2^61 bytes */
    if( (uint64_t) add_len >> 61 != 0 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    return mbedtls_gcm_update_ad_harden( ctx, add, add_len );
}

int mbedtls_gcm_update( mbedtls_gcm_context *ctx,
                        const unsigned char *input, size_t input_length,
                        unsigned char *output, size_t output_size,
                        size_t *output_length )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    if( output_size < input_length )
        return( MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL );
    GCM_VALIDATE_RET( output_length != NULL );
    *output_length = input_length;

    /* Exit early if input_length==0 so that we don't do any pointer arithmetic
     * on a potentially null pointer.
     * Returning early also means that the last partial block of AD remains
     * untouched for mbedtls_gcm_finish */
    if( input_length == 0 )
        return( 0 );

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( input != NULL );
    GCM_VALIDATE_RET( output != NULL );

    if( output > input && (size_t) ( output - input ) < input_length )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
     * Also check for possible overflow */
    if( ctx->len + input_length < ctx->len ||
        (uint64_t) ctx->len + input_length > 0xFFFFFFFE0ull )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    ret = mbedtls_gcm_update_harden( ctx, input, input_length, output );
    if( ret == 0 )
    {
        ctx->len += input_length;
    }

    return ret;
}

int mbedtls_gcm_finish( mbedtls_gcm_context *ctx,
                        unsigned char *output, size_t output_size,
                        size_t *output_length,
                        unsigned char *tag, size_t tag_len )
{
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( tag != NULL );

    return mbedtls_gcm_finish_harden( ctx, tag, tag_len );
}

int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t olen;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );
    GCM_VALIDATE_RET( tag != NULL );

    if( ( ret = mbedtls_gcm_starts( ctx, mode, iv, iv_len ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_gcm_update_ad( ctx, add, add_len ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_gcm_update( ctx, input, length,
                                    output, length, &olen ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_gcm_finish( ctx, NULL, 0, &olen, tag, tag_len ) ) != 0 )
        return( ret );

    return( 0 );
}

VMP_TAG
int mbedtls_gcm_auth_decrypt( mbedtls_gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag,
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char check_tag[16];
    size_t i;
    int diff;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );
    GCM_VALIDATE_RET( tag != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );

    if( ( ret = mbedtls_gcm_crypt_and_tag( ctx, MBEDTLS_GCM_DECRYPT, length,
                                   iv, iv_len, add, add_len,
                                   input, output, tag_len, check_tag ) ) != 0 )
    {
        return( ret );
    }

    /* Check tag in "constant-time" */
    for( diff = 0, i = 0; i < tag_len; i++ )
        diff |= tag[i] ^ check_tag[i];

    if( diff != 0 )
    {
        mbedtls_platform_zeroize( output, length );
        return( MBEDTLS_ERR_GCM_AUTH_FAILED );
    }

    return( 0 );
}

void mbedtls_gcm_free( mbedtls_gcm_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_gcm_free_harden( ctx );

    mbedtls_cipher_free( &ctx->cipher_ctx );
}
#endif