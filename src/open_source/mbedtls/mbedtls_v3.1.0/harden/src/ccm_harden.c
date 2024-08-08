/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "ccm_harden.h"
#include "cipher_common.h"
#include "dfx.h"

#if defined(MBEDTLS_CCM_ALT)

static ccm_harden_context g_ccm_harden_context[MAX_CCM_HARD_CTX_NUM] = {0};

static ccm_harden_context * get_ccm_harden_ctx( mbedtls_ccm_context *ctx )
{
    unsigned int i = 0;

    if ( ctx == NULL )
        return NULL;

    for( i = 0; i < MAX_CCM_HARD_CTX_NUM; ++i )
    {
        if( g_ccm_harden_context[i].ctx == ctx )
        {
            return &g_ccm_harden_context[i];
        }
    }
    return NULL;
}

static void internal_release_hard_resource( ccm_harden_context *harden_ctx )
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

void mbedtls_ccm_init_harden( mbedtls_ccm_context *ctx )
{
    int i = 0;
    int ret = 0;
    if( ctx == NULL || IS_SYMC_FUNC_REGISTERED() != TD_TRUE )
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

    for( i = 0; i < MAX_CCM_HARD_CTX_NUM; ++i )
    {
        if( g_ccm_harden_context[i].is_used == 0 )
        {
            memset( &g_ccm_harden_context[i], 0, sizeof( ccm_harden_context ) );
            memset( g_ccm_harden_context[i].key, 0, sizeof( g_ccm_harden_context[i].key ) );
            g_ccm_harden_context[i].ctx = ctx;
            g_ccm_harden_context[i].is_used = 1;
            break;
        }
    }
}

int mbedtls_ccm_setkey_harden(mbedtls_ccm_context *ctx,
                                mbedtls_cipher_id_t cipher,
                                const unsigned char *key,
                                unsigned int keybits)
{
    ccm_harden_context *harden_ctx;

    if( ctx == NULL || key == NULL )
        return -1;
    if( keybits != 128 && keybits != 192 && keybits != 256 )
        return -1;

    harden_ctx = get_ccm_harden_ctx( ctx );
    if( harden_ctx == NULL )
        return -1;

    harden_ctx->key_length = keybits / 8;
    memcpy(harden_ctx->key, key, harden_ctx->key_length);

    if( cipher == MBEDTLS_CIPHER_ID_CAMELLIA ) {
        harden_ctx->only_software_support = 1;
        return -1;
    }
#if !defined(MBEDTLS_AES_192_HARD_KEY_SUPPORT)
    if( keybits == 192 ) {
        harden_ctx->only_software_support = 1;
        return -1;
    }
#endif
    return 0;
}

void mbedtls_ccm_free_harden( mbedtls_ccm_context *ctx )
{
    ccm_harden_context * harden_ctx;
    harden_ctx = get_ccm_harden_ctx( ctx );
    if( harden_ctx != NULL )
    {
        internal_release_hard_resource( harden_ctx );
        KM_DEINIT();
        CIPHER_SYMC_DEINIT();
        memset( harden_ctx, 0, sizeof( ccm_harden_context ) );
    }
}

static void internal_ccm_hardware_config( ccm_harden_context *harden_ctx )
{
    int ret = 0;
    crypto_symc_ctrl_t symc_ctrl = {0};
    crypto_symc_config_aes_ccm_gcm ccm_config = {0};

    ret = mbedtls_cipher_set_ctrl( &symc_ctrl, CRYPTO_SYMC_ALG_AES,
        CRYPTO_SYMC_WORK_MODE_CCM, harden_ctx->key_length, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        CRYPTO_SYMC_CCM_IV_CHANGE_START, harden_ctx->iv, harden_ctx->iv_len );
    if( ret != 0 )
    {
        mbedtls_harden_log_err( "mbedtls_cipher_set_ctrl faild!\n" );
        goto exit_fail;
    }
    ret = mbedtls_cipher_set_gcm_ccm_config( &ccm_config, harden_ctx->aad,
        harden_ctx->aad_len, harden_ctx->input_len, harden_ctx->tag_len );
    if( ret != 0 )
    {
        mbedtls_harden_log_err( "mbedtls_cipher_set_gcm_ccm_config faild!\n" );
        goto exit_fail;
    }
    symc_ctrl.param = ( void * )&ccm_config;
    ret = CIPHER_SYMC_SET_CONFIG( harden_ctx->symc_handle, &symc_ctrl );
    if( ret != 0 )
    {
        mbedtls_harden_log_err( "CIPHER_SYMC_SET_CONFIG faild!\n" );
        goto exit_fail;
    }
    harden_ctx->is_set_config = 1;
    return;

exit_fail:
    harden_ctx->only_software_support = 1;
    mbedtls_harden_log_err( "internal_ccm_hardware_config faild, ret = 0x%x!\n", ret );
}

int mbedtls_ccm_starts_harden( mbedtls_ccm_context *ctx,
                                int mode,
                                const unsigned char *iv,
                                size_t iv_len )
{
    int ret = 0;
    ccm_harden_context *harden_ctx;
    crypto_symc_attr symc_attr = {
        .symc_type = CRYPTO_SYMC_TYPE_NORMAL,
        .is_long_term = 0
    };

    harden_ctx = get_ccm_harden_ctx( ctx );
    if( harden_ctx == NULL || harden_ctx->only_software_support == 1 )
        return ( -1 );
    if( mode == MBEDTLS_CCM_STAR_ENCRYPT || mode == MBEDTLS_CCM_STAR_DECRYPT )
    {
        harden_ctx->only_software_support = 1;
        return (-1);
    }
    harden_ctx->mode = mode;
    harden_ctx->iv_len = iv_len;
    memcpy(harden_ctx->iv, iv, iv_len);

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

int mbedtls_ccm_set_lengths_harden( mbedtls_ccm_context *ctx,
                                        size_t total_ad_len,
                                        size_t plaintext_len,
                                        size_t tag_len )
{
    ccm_harden_context *harden_ctx;
    harden_ctx = get_ccm_harden_ctx( ctx );
    if( harden_ctx != NULL )
    {
        if( total_ad_len > MBEDTLS_CIPHER_MAX_ADD_LEN || plaintext_len == 0 )
        {
            harden_ctx->only_software_support = 1;
            return ( -1 );
        } else {
            harden_ctx->aad_len = total_ad_len;
            harden_ctx->cur_aad_len = 0;
            harden_ctx->input_len = plaintext_len;
            harden_ctx->tag_len = tag_len;
            return ( 0 );
        }
    }
    return ( -1 );
}

int mbedtls_ccm_update_ad_harden( mbedtls_ccm_context *ctx,
                                const unsigned char *add,
                                size_t add_len )
{
    int ret = 0;
    size_t temp;
    ccm_harden_context *harden_ctx;
    harden_ctx = get_ccm_harden_ctx( ctx );
    if( harden_ctx != NULL && harden_ctx->only_software_support == 0 )
    {
        temp = add_len + harden_ctx->cur_aad_len;
        if( temp > harden_ctx->aad_len )
        {
            ret = mbedtls_ccm_update_ad_software( ctx, harden_ctx->aad, harden_ctx->aad_len );
            if(ret != 0)
                return ret;
            harden_ctx->only_software_support = 1;
            return ( -1 );
        }
        memcpy( harden_ctx->aad + harden_ctx->cur_aad_len, add, add_len );
        harden_ctx->cur_aad_len += add_len;
        return ( 0 );
    }
    return ( -1 );
}

int mbedtls_ccm_update_harden( mbedtls_ccm_context *ctx,
                                const unsigned char *input, size_t input_len,
                                unsigned char *output, size_t output_size,
                                size_t *output_len )
{
    int ret = 0;
    ccm_harden_context *harden_ctx;
    unsigned long in_phys_addr;
    unsigned long out_phys_addr;
    crypto_buf_attr in_buf = {0};
    crypto_buf_attr out_buf = {0};

    if( output_size < input_len )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    harden_ctx = get_ccm_harden_ctx( ctx );
    if( harden_ctx == NULL || harden_ctx->only_software_support == 1 )
        return ( -1 );
    if( CIPHER_GET_PHYS_MEM( ( void * )input, &in_phys_addr ) != 0 ||
        CIPHER_GET_PHYS_MEM( output, &out_phys_addr ) != 0 )
    {
        harden_ctx->only_software_support = 1;
        return ( -1 );
    }
    if( harden_ctx->is_set_config == 0 )
        internal_ccm_hardware_config( harden_ctx );
    if( harden_ctx->only_software_support == 1 )
        return ( -1 );
    else {
        if (input_len == 0) {
            return ( 0 );
        }
        in_buf.virt_addr = (void *)input;
        in_buf.phys_addr = in_phys_addr;
        out_buf.virt_addr = output;
        out_buf.phys_addr = out_phys_addr;
        if( harden_ctx->mode == MBEDTLS_CCM_ENCRYPT )
        {
            ret = CIPHER_SYMC_ENCRYPT( harden_ctx->symc_handle, &in_buf, &out_buf, input_len );
        } else {
            ret = CIPHER_SYMC_DECRYPT( harden_ctx->symc_handle, &in_buf, &out_buf, input_len );
        }
        if( ret != 0 )
            mbedtls_harden_log_err( "CIPHER_SYMC_CRYPTO faild!\n" );
    }
    *output_len = input_len;
    return ret;
}

static int internal_ccm_hardware_finish( ccm_harden_context *harden_ctx,
                                        unsigned char *tag,
                                        size_t tag_len)
{
    int ret = 0;
    ret = CIPHER_SYMC_GET_TAG(harden_ctx->symc_handle, tag, tag_len);
    if (ret != 0) {
        mbedtls_harden_log_err("CIPHER_SYMC_GET_TAG faild!\n");
    }
    internal_release_hard_resource( harden_ctx );
    return ret;
}

int mbedtls_ccm_finish_harden( mbedtls_ccm_context *ctx,
                                unsigned char *tag, size_t tag_len )
{
    int ret = 0;
    ccm_harden_context *harden_ctx;
    harden_ctx = get_ccm_harden_ctx( ctx );
    if ( harden_ctx == NULL || harden_ctx->only_software_support == 1 || harden_ctx->is_set_config == 0 )
    {
        return ( -1 );
    } else {
        ret = internal_ccm_hardware_finish(harden_ctx, tag, tag_len);
        harden_ctx->is_set_config = 0;
    }
    return ret;
}

void mbedtls_ccm_init( mbedtls_ccm_context *ctx )
{
    mbedtls_ccm_init_harden( ctx );
}

int mbedtls_ccm_setkey( mbedtls_ccm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    return mbedtls_ccm_setkey_harden( ctx, cipher, key, keybits );
}

void mbedtls_ccm_free( mbedtls_ccm_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_ccm_free_harden( ctx );
    mbedtls_cipher_free( &ctx->cipher_ctx );
}

int mbedtls_ccm_starts( mbedtls_ccm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len )
{
    if( iv_len < 7 || iv_len > 13 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );
    if ( ctx == NULL || iv == NULL )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    return mbedtls_ccm_starts_harden( ctx, mode, iv, iv_len );
}

int mbedtls_ccm_set_lengths( mbedtls_ccm_context *ctx,
                             size_t total_ad_len,
                             size_t plaintext_len,
                             size_t tag_len )
{
    if( tag_len == 2 || tag_len > 16 || tag_len % 2 != 0 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    if( total_ad_len >= 0xFF00 )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    return mbedtls_ccm_set_lengths_harden( ctx, total_ad_len, plaintext_len, tag_len );
}

int mbedtls_ccm_update_ad( mbedtls_ccm_context *ctx,
                           const unsigned char *add,
                           size_t add_len )
{
    if ( ctx == NULL || add == NULL )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    return mbedtls_ccm_update_ad_harden( ctx, add, add_len );
}

int mbedtls_ccm_update( mbedtls_ccm_context *ctx,
                        const unsigned char *input, size_t input_len,
                        unsigned char *output, size_t output_size,
                        size_t *output_len )
{
    if ( ctx == NULL || input == NULL || output == NULL )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    return mbedtls_ccm_update_harden( ctx, input, input_len, output, output_size, output_len );
}

int mbedtls_ccm_finish( mbedtls_ccm_context *ctx,
                        unsigned char *tag, size_t tag_len )
{
    if ( ctx == NULL || tag == NULL )
        return( MBEDTLS_ERR_CCM_BAD_INPUT );

    return mbedtls_ccm_finish_harden( ctx, tag, tag_len );
}

#endif