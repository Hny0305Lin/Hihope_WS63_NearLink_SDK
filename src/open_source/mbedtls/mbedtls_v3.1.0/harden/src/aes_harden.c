/*
 *  FIPS-197 compliant AES implementation. And adapt to the harden API.
 *
 *  Copyright The Mbed TLS Contributors
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
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
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "common.h"
#include <string.h>
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "aes_harden.h"
#include "cipher_common.h"
#include "dfx.h"

#if defined(MBEDTLS_AES_ALT)

#define AES_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA )
#define AES_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )


static aes_harden_context g_aes_harden_context[MAX_AES_HARD_CTX_NUM] = {0};

static aes_harden_context * get_aes_harden_ctx( mbedtls_aes_context *ctx )
{
    unsigned int i = 0;
    for( i = 0; i < MAX_AES_HARD_CTX_NUM; ++i )
    {
        if( g_aes_harden_context[i].ctx == ctx )
        {
            return &g_aes_harden_context[i];
        }
    }
    return NULL;
}

static void mbedtls_cipher_sync_aes_ctx( aes_harden_context * harden_ctx, unsigned char *iv, sync_direction direction )
{
    if( direction == SOFT_TO_HARD && iv != NULL && harden_ctx != NULL )
    {
        memcpy(harden_ctx->symc_ctrl.iv, iv, 16);
    } else if( direction == HARD_TO_SOFT && iv != NULL && harden_ctx != NULL) {
        memcpy(iv, harden_ctx->symc_ctrl.iv, 16);
    }
}

void mbedtls_aes_init_harden( mbedtls_aes_context *ctx )
{
    /* init hard context */
    int ret = 0;
    int i = 0;
    unsigned int symc_handle;
    unsigned int ks_handle;
    crypto_symc_attr symc_attr = { 
        .symc_type = CRYPTO_SYMC_TYPE_NORMAL,
        .is_long_term = 0
    };
    if( IS_SYMC_FUNC_REGISTERED() != TD_TRUE )
        return;
    ret = CIPHER_SYMC_INIT();
    if( ret != 0 )
        return;
    ret = KM_INIT();
    if( ret != 0 )
        goto cipher_deinit;
    ret = CIPHER_SYMC_CREATE( &symc_handle, &symc_attr );
    if( ret != 0 )
        goto km_deinit;
    ret = CIPHER_CREATE_KEYSLOT( &ks_handle, KM_KEYSLOT_ENGINE_AES );
    if( ret != 0 )
        goto symc_destroy;
    ret = CIPHER_SYMC_ATTACH( symc_handle, ks_handle );
    if( ret != 0 )
        goto keyslot_destroy;
    for( i = 0; i < MAX_AES_HARD_CTX_NUM; ++i )
    {
        if( g_aes_harden_context[i].is_used == 0 ) {
            g_aes_harden_context[i].ctx = ctx;
            g_aes_harden_context[i].is_used = 1;
            g_aes_harden_context[i].symc_handle = symc_handle;
            g_aes_harden_context[i].keyslot_handle = ks_handle;
            g_aes_harden_context[i].only_software_support = 0;
            break;
        }
    }
    return;

keyslot_destroy:
    CIPHER_DESTROY_KEYSLOT( ks_handle );
symc_destroy:
    ( void )CIPHER_SYMC_DESTROY( symc_handle );
km_deinit:
    ( void )KM_DEINIT();
cipher_deinit:
    ( void )CIPHER_SYMC_DEINIT();
}

void mbedtls_aes_free_harden( mbedtls_aes_context *ctx )
{
    aes_harden_context * harden_ctx;

    if( ctx == NULL )
        return;

    harden_ctx = get_aes_harden_ctx( ctx );
    if( harden_ctx != NULL )
    {
        ( void )CIPHER_DESTROY_KEYSLOT( harden_ctx->keyslot_handle );
        ( void )CIPHER_SYMC_DESTROY( harden_ctx->symc_handle );
        ( void )KM_DEINIT();
        ( void )CIPHER_SYMC_DEINIT();
        ( void )memset( harden_ctx, 0, sizeof( aes_harden_context ) );
    }
}

int mbedtls_aes_setkey_harden( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    int ret = -1;
    aes_harden_context * harden_ctx;
    switch( keybits )
    {
        case 128:
        case 192:
        case 256: break;
        default : return -1;
    }

    harden_ctx = get_aes_harden_ctx(ctx);
    if( harden_ctx != NULL )
    {
        memcpy(harden_ctx->key, key, keybits / 8);
        harden_ctx->key_length = keybits / 8;
        harden_ctx->only_software_support = 0;
#if !defined(MBEDTLS_AES_192_HARD_KEY_SUPPORT)
        if( keybits == 192 )
            harden_ctx->only_software_support = 1;
#endif
        if( harden_ctx->only_software_support == 0 )
        {
            ret = CIPHER_SET_CLEAR_KEY(harden_ctx->keyslot_handle, harden_ctx->key,
                harden_ctx->key_length, KM_KEYSLOT_ENGINE_AES);
            if( ret != 0 )
                harden_ctx->only_software_support = 1;
        }
    }
    return ret;
}

static int internal_aes_hardware_crypt( aes_harden_context *harden_ctx,
                                        int mode,
                                        size_t length,
                                        const unsigned char *input,
                                        unsigned char *output,
                                        unsigned long in_phys_addr,
                                        unsigned long out_phys_addr )
{
    int ret = 0;
    crypto_buf_attr in_buf;
    crypto_buf_attr out_buf;

    if( length == 0 )
        return ( 0 );
    ret = CIPHER_SYMC_SET_CONFIG( harden_ctx->symc_handle, &harden_ctx->symc_ctrl );
    if( ret != 0 )
    {
        mbedtls_harden_log_err( "CIPHER_SYMC_SET_CONFIG faild!\n" );
        return ret;
    }

    in_buf.virt_addr = (void *)input;
    in_buf.phys_addr = in_phys_addr;
    out_buf.virt_addr = output;
    out_buf.phys_addr = out_phys_addr;
    if( mode == MBEDTLS_AES_ENCRYPT ) {
        ret = CIPHER_SYMC_ENCRYPT(harden_ctx->symc_handle, &in_buf, &out_buf, length);
    } else {
        ret = CIPHER_SYMC_DECRYPT(harden_ctx->symc_handle, &in_buf, &out_buf, length);
    }

    ret = CIPHER_SYMC_GET_CONFIG( harden_ctx->symc_handle, &harden_ctx->symc_ctrl );
    if( ret != 0 ) {
        mbedtls_harden_log_err( "CIPHER_SYMC_GET_CONFIG faild!\n" );
        return ret;
    }

    return ( 0 );
}

static unsigned char check_offset_valid( size_t offset )
{
    if( offset == 0 )
        return 1;
    return ( 0 );
}

static unsigned char check_basic_cfg_can_use_hardware( mbedtls_aes_context *ctx,
                                const unsigned char *input,
                                const unsigned char *output,
                                unsigned long *in_phys_addr,
                                unsigned long *out_phys_addr,
                                aes_harden_context **harden_ctx )
{
    *harden_ctx = get_aes_harden_ctx( ctx );
    if( *harden_ctx == NULL || (*harden_ctx)->only_software_support == 1 )
        return ( 0 );
    if( CIPHER_GET_PHYS_MEM((void *)input, in_phys_addr) != 0 ||
        CIPHER_GET_PHYS_MEM((void *)output, out_phys_addr) != 0 )
        return ( 0 );
    return 1;
}

unsigned char check_if_phys_mem( const unsigned char *input )
{
    unsigned long in_phys_addr;
    if( CIPHER_GET_PHYS_MEM((void *)input, &in_phys_addr) != 0 )
        return 0;
    return 1;
}

void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
    AES_VALIDATE( ctx != NULL );
    mbedtls_aes_init_harden( ctx );
}

void mbedtls_aes_free( mbedtls_aes_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_aes_free_harden( ctx );
}

int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    return mbedtls_aes_setkey_harden( ctx, key, keybits );
}

int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    return mbedtls_aes_setkey_harden( ctx, key, keybits );
}


int ecb_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                int mode,
                                const unsigned char *input,
                                unsigned char *output,
                                size_t length )
{
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    aes_harden_context *harden_ctx;
    if( check_basic_cfg_can_use_hardware( ctx, input, output, &in_phys_addr, &out_phys_addr, &harden_ctx ) == 0 )
        return ( -1 );
    ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_ECB,
        harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT, CRYPTO_SYMC_IV_DO_NOT_CHANGE, NULL, 0 );
    if( ret != 0 )
        return ret;
    ret = internal_aes_hardware_crypt( harden_ctx, mode, length, input, output, in_phys_addr, out_phys_addr );
    if( ret != 0 )
        return ret;
    return ( 0 );
}

int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                           int mode,
                           const unsigned char input[16],
                           unsigned char output[16] )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );

    if( ecb_cfg_and_crypt_harden( ctx, mode, input, output, 16 ) == 0 )
        return( 0 );
    return( -1 );
}

int mbedtls_internal_aes_encrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, input, output);
}

int mbedtls_internal_aes_decrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_DECRYPT, input, output);
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void mbedtls_aes_xts_init( mbedtls_aes_xts_context *ctx )
{
    AES_VALIDATE( ctx != NULL );

    mbedtls_aes_init( &ctx->crypt );
    mbedtls_aes_init( &ctx->tweak );
}

void mbedtls_aes_xts_free( mbedtls_aes_xts_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_aes_free( &ctx->crypt );
    mbedtls_aes_free( &ctx->tweak );
}

static int mbedtls_aes_xts_decode_keys( const unsigned char *key,
                                        unsigned int keybits,
                                        const unsigned char **key1,
                                        unsigned int *key1bits,
                                        const unsigned char **key2,
                                        unsigned int *key2bits )
{
    const unsigned int half_keybits = keybits / 2;
    const unsigned int half_keybytes = half_keybits / 8;

    switch( keybits )
    {
        case 256: break;
        case 512: break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    *key1bits = half_keybits;
    *key2bits = half_keybits;
    *key1 = &key[0];
    *key2 = &key[half_keybytes];

    return 0;
}

int mbedtls_aes_xts_setkey_enc( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ret = mbedtls_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for the encryption mode. */
    ret = mbedtls_aes_setkey_enc( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for encryption. */
    return mbedtls_aes_setkey_enc( &ctx->crypt, key1, key1bits );
}

int mbedtls_aes_xts_setkey_dec( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ret = mbedtls_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for encryption. */
    ret = mbedtls_aes_setkey_enc( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for decryption. */
    return mbedtls_aes_setkey_dec( &ctx->crypt, key1, key1bits );
}

typedef unsigned char mbedtls_be128[16];

static void mbedtls_gf128mul_x_ble( unsigned char r[16],
                                    const unsigned char x[16] )
{
    uint64_t a, b, ra, rb;

    a = MBEDTLS_GET_UINT64_LE( x, 0 );
    b = MBEDTLS_GET_UINT64_LE( x, 8 );

    ra = ( a << 1 )  ^ 0x0087 >> ( 8 - ( ( b >> 63 ) << 3 ) );
    rb = ( a >> 63 ) | ( b << 1 );

    MBEDTLS_PUT_UINT64_LE( ra, r, 0 );
    MBEDTLS_PUT_UINT64_LE( rb, r, 8 );
}

int mbedtls_aex_crypt_xts_harden( mbedtls_aes_xts_context *ctx,
                                int mode,
                                size_t length,
                                const unsigned char data_unit[16],
                                const unsigned char *input,
                                unsigned char *output,
                                unsigned char *tweak,
                                unsigned char *prev_tweak )
{
    size_t j;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char *tweak_list = NULL;
    size_t blocks = length / 16;
    size_t processed_blocks = 0;
    size_t processing_blocks = 0;
    size_t left_blocks = blocks;
    size_t max_blocks = MAX_AES_XTS_TWEAK_SIZE / 16;
    size_t leftover = length % 16;
    size_t malloc_size = max_blocks * 16 > blocks * 16 ? blocks * 16 : max_blocks * 16;
    if( check_if_phys_mem( output ) == 0 )
        return -1;
    /* Compute the tweak. */
    tweak_list = ( unsigned char * )malloc( malloc_size );
    ret = mbedtls_aes_crypt_ecb( &ctx->tweak, MBEDTLS_AES_ENCRYPT,
                                 data_unit, tweak );
    if( ret != 0 )
        goto exit_free;

    while ( processed_blocks < blocks )
    {
        size_t count;
        size_t pos;
        size_t process_len;
        processing_blocks = max_blocks > left_blocks ? left_blocks : max_blocks;
        count = processing_blocks;
        pos = processed_blocks * 16;
        process_len = processing_blocks * 16;
        while( count-- )
        {
            size_t i;
            size_t start_pos = ( processing_blocks - count - 1 ) * 16;

            if( leftover && ( mode == MBEDTLS_AES_DECRYPT ) && count == 0 &&
                processed_blocks + processing_blocks == blocks )
            {
                /* We are on the last block in a decrypt operation that has
                 * leftover bytes, so we need to use the next tweak for this block,
                 * and this tweak for the lefover bytes. Save the current tweak for
                 * the leftovers and then update the current tweak for use on this,
                 * the last full block. */
                memcpy( prev_tweak, tweak, 16 );
                mbedtls_gf128mul_x_ble( tweak, tweak );
            }

            for( i = 0; i < 16; i++ )
                tweak_list[start_pos + i] = tweak[i];

            /* Update the tweak for the next block. */
            mbedtls_gf128mul_x_ble( tweak, tweak );
        }
        for( j = 0; j < process_len; j++ )
            output[pos + j] = input[pos + j] ^ tweak_list[j];
        ret = ecb_cfg_and_crypt_harden( &ctx->crypt, mode, output + pos, output + pos, process_len );
        if( ret != 0 )
            goto exit_free;
        for( j = 0; j < process_len; j++ )
            output[pos + j] ^= tweak_list[j];
        left_blocks -= processing_blocks;
        processed_blocks += processing_blocks;
    }

exit_free:
    free(tweak_list);
    return ret;
}

int mbedtls_aes_crypt_xts( mbedtls_aes_xts_context *ctx,
                           int mode,
                           size_t length,
                           const unsigned char data_unit[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t leftover = length % 16;
    unsigned char tweak[16];
    unsigned char prev_tweak[16];
    unsigned char tmp[16];

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( data_unit != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    /* Data units must be at least 16 bytes long. */
    if( length < 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* NIST SP 800-38E disallows data units larger than 2**20 blocks. */
    if( length > ( 1 << 20 ) * 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    ret = mbedtls_aex_crypt_xts_harden( ctx, mode, length, data_unit, input, output, tweak, prev_tweak );
    if( ret == 0 )
    {
        output += length - ( length % 16 );
        input += length - (length % 16);
    } else
    {
        return( -1 );
    }

    if( leftover )
    {
        /* If we are on the leftover bytes in a decrypt operation, we need to
         * use the previous tweak for these bytes (as saved in prev_tweak). */
        unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? prev_tweak : tweak;

        /* We are now on the final part of the data unit, which doesn't divide
         * evenly by 16. It's time for ciphertext stealing. */
        size_t i;
        unsigned char *prev_output = output - 16;

        /* Copy ciphertext bytes from the previous block to our output for each
         * byte of cyphertext we won't steal. At the same time, copy the
         * remainder of the input for this final round (since the loop bounds
         * are the same). */
        for( i = 0; i < leftover; i++ )
        {
            output[i] = prev_output[i];
            tmp[i] = input[i] ^ t[i];
        }

        /* Copy ciphertext bytes from the previous block for input in this
         * round. */
        for( ; i < 16; i++ )
            tmp[i] = prev_output[i] ^ t[i];

        ret = mbedtls_aes_crypt_ecb( &ctx->MBEDTLS_PRIVATE(crypt), mode, tmp, tmp );
        if( ret != 0 )
            return ret;

        /* Write the result back to the previous block, overriding the previous
         * output we copied. */
        for( i = 0; i < 16; i++ )
            prev_output[i] = tmp[i] ^ t[i];
    }

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
int cbc_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                int mode,
                                size_t length,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output )
{
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    aes_harden_context *harden_ctx;
    if( check_basic_cfg_can_use_hardware( ctx, input, output, &in_phys_addr, &out_phys_addr, &harden_ctx ) == 0 )
        return ( -1 );
    ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_CBC,
            harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT, CRYPTO_SYMC_IV_DO_NOT_CHANGE, iv, 16 );
    if( ret != 0 )
        return ret;
    ret = internal_aes_hardware_crypt( harden_ctx, mode, length, input, output, in_phys_addr, out_phys_addr );
    if( ret != 0 )
        return ret;
    mbedtls_cipher_sync_aes_ctx( harden_ctx, iv, HARD_TO_SOFT );
    return ( 0 );
}

int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    if( length % 16 )
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

    if( cbc_cfg_and_crypt_harden( ctx, mode, length, iv, input, output ) == 0 )
        return( 0 );
    return( -1 );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
int cfb128_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16], // iv is not more than 16
                       const unsigned char *input,
                       unsigned char *output )
{
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    aes_harden_context *harden_ctx;
    if( check_basic_cfg_can_use_hardware( ctx, input, output, &in_phys_addr, &out_phys_addr, &harden_ctx ) == 0 )
        return ( -1 );
    if( check_offset_valid( *iv_off ) == 0 || length % 16 != 0 )
        return ( -1 );
    ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_CFB,
            harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT, CRYPTO_SYMC_IV_DO_NOT_CHANGE, iv, 16 );
    if( ret != 0 )
        return ret;
    ret = internal_aes_hardware_crypt( harden_ctx, mode, length, input, output, in_phys_addr, out_phys_addr );
    if( ret != 0 )
        return ret;
    *iv_off = length % 16; // length after 16 byte crypt
    mbedtls_cipher_sync_aes_ctx( harden_ctx, iv, HARD_TO_SOFT );
    return ( 0 );
}

int cfb8_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                int mode,
                                size_t length,
                                unsigned char iv[16], // iv is not more than 16
                                const unsigned char *input,
                                unsigned char *output )
{
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    aes_harden_context *harden_ctx;
    if( check_basic_cfg_can_use_hardware( ctx, input, output, &in_phys_addr, &out_phys_addr, &harden_ctx ) == 0 )
        return ( -1 );
    ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_CFB,
            harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_8BIT, CRYPTO_SYMC_IV_DO_NOT_CHANGE, iv, 16 );
    if( ret != 0 )
        return ret;
    ret = internal_aes_hardware_crypt( harden_ctx, mode, length, input, output, in_phys_addr, out_phys_addr );
    if( ret != 0 )
        return ret;
    mbedtls_cipher_sync_aes_ctx( harden_ctx, iv, HARD_TO_SOFT );
    return ( 0 );
}

int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16], // iv is not more than 16
                       const unsigned char *input,
                       unsigned char *output )
{
    size_t n;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    return cfb128_cfg_and_crypt_harden( ctx, mode, length, iv_off, iv, input, output );
}

int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                            int mode,
                            size_t length,
                            unsigned char iv[16], // iv is not more than 16
                            const unsigned char *input,
                            unsigned char *output )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );


    return cfb8_cfg_and_crypt_harden( ctx, mode, length, iv, input, output );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
int ofb_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                size_t length,
                                size_t *iv_off,
                                unsigned char iv[16], // 16-byte iv length
                                const unsigned char *input,
                                unsigned char *output )
{
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    aes_harden_context *harden_ctx;
    if( check_basic_cfg_can_use_hardware( ctx, input, output, &in_phys_addr, &out_phys_addr, &harden_ctx ) == 0 )
        return ( -1 );
    if( check_offset_valid( *iv_off ) == 0 )
        return ( -1 );
    ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_OFB,
            harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT, CRYPTO_SYMC_IV_DO_NOT_CHANGE, iv, 16 ); // 16 iv len
    if( ret != 0 )
        return ret;
    ret = internal_aes_hardware_crypt( harden_ctx, MBEDTLS_AES_ENCRYPT, length, input, output,
        in_phys_addr, out_phys_addr );
    if( ret != 0 )
        return ret;
    *iv_off = length % 16; // length after 16 byte crypt
    mbedtls_cipher_sync_aes_ctx( harden_ctx, iv, HARD_TO_SOFT );
    return ( 0 );
}

int mbedtls_aes_crypt_ofb( mbedtls_aes_context *ctx,
                           size_t length,
                           size_t *iv_off,
                           unsigned char iv[16], // iv is not more than 16
                           const unsigned char *input,
                           unsigned char *output )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    if( *iv_off > 15 ) // iv offset should be in entent of iv array
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    return ofb_cfg_and_crypt_harden( ctx, length, iv_off, iv, input, output );
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
int ctr_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                size_t length,
                                size_t *nc_off,
                                unsigned char nonce_counter[16],
                                unsigned char stream_block[16],
                                const unsigned char *input,
                                unsigned char *output )
{
    int i;
    int ret = 0;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    aes_harden_context *harden_ctx;
    if( check_basic_cfg_can_use_hardware( ctx, input, output, &in_phys_addr, &out_phys_addr, &harden_ctx ) == 0 )
        return ( -1 );
    if( check_offset_valid( *nc_off ) == 0 )
        return ( -1 );
    ret = mbedtls_cipher_set_ctrl( &harden_ctx->symc_ctrl, CRYPTO_SYMC_ALG_AES, CRYPTO_SYMC_WORK_MODE_CTR,
            harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT, CRYPTO_SYMC_IV_DO_NOT_CHANGE, nonce_counter,
            16 ); // iv length 16
    if( ret != 0 )
        return ret;
    ret = internal_aes_hardware_crypt( harden_ctx, MBEDTLS_AES_ENCRYPT, length, input, output,
        in_phys_addr, out_phys_addr );
    if( ret != 0 )
        return ret;
    *nc_off = length % 16; // left non 16 align length
    mbedtls_cipher_sync_aes_ctx( harden_ctx, nonce_counter, HARD_TO_SOFT );
    memcpy( stream_block, nonce_counter, 16 ); // 16-byte iv array
    for( i = 16; i > 0; i-- )
            if( --stream_block[i - 1] != 0xff )
                break;
    return mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, stream_block, stream_block );
}

int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16], // 16-byte array to store nonce and counters.
                       unsigned char stream_block[16], // 16-byte array used to store stream cipher blocks.
                       const unsigned char *input,
                       unsigned char *output )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( nc_off != NULL );
    AES_VALIDATE_RET( nonce_counter != NULL );
    AES_VALIDATE_RET( stream_block != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    if ( *nc_off > 0x0F ) // iv offset should be in entent of iv array
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    return ctr_cfg_and_crypt_harden( ctx, length, nc_off, nonce_counter, stream_block, input, output );
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_ALT */

