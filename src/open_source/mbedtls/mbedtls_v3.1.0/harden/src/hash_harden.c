/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hash harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "common.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "hash_harden.h"
#include "cipher_adapt.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "hash_harden_common.h"
#include "dfx.h"
#include <string.h>

#if defined(MBEDTLS_SHA1_ALT) || defined(MBEDTLS_SHA256_ALT) || defined(MBEDTLS_SHA512_ALT) || defined(MBEDTLS_MD_ALT)
static hash_harden_context g_hash_harden_context[MAX_HASH_HARD_CTX_NUM] = { 0 };

static unsigned char g_is_hash_registered = 0;

static int get_idle_channel( mbedtls_md_context_t *ctx )
{
    int i;
    for ( i = 0; i < MAX_HASH_HARD_CTX_NUM; ++i )
    {
        if( g_hash_harden_context[i].ctx == ctx )
        {
            return i;
        }
    }
    for ( i = 0; i < MAX_HASH_HARD_CTX_NUM; ++i )
    {
        if ( g_hash_harden_context[i].ctx == NULL ) {
            return i;
        }
    }
    return MAX_HASH_HARD_CTX_NUM;
}

static hash_harden_context* get_hash_harden_ctx( const void *ctx )
{
    unsigned int i = 0;
    if( ctx == NULL )
        return NULL;
    for ( i = 0; i < MAX_HASH_HARD_CTX_NUM; ++i )
    {
        if ( g_hash_harden_context[i].ctx == ctx )
        {
            return &g_hash_harden_context[i];
        }
    }
    return NULL;
}

int hash_harden_can_do( crypto_hash_type hash_type )
{
    if( IS_HASH_FUNC_REGISTERED() != TD_TRUE )
        return ( 0 );

    /* check whether if support this type */
    switch (hash_type) {
    #if defined(MBEDTLS_SHA1_USE_HARDWARE)
        case CRYPTO_HASH_TYPE_SHA1: break;
    #endif
    #if defined(MBEDTLS_SHA224_USE_HARDWARE)
        case CRYPTO_HASH_TYPE_SHA224: break;
    #endif
    #if defined(MBEDTLS_SHA256_USE_HARDWARE)
        case CRYPTO_HASH_TYPE_SHA256: break;
    #endif
    #if defined(MBEDTLS_SHA384_USE_HARDWARE)
        case CRYPTO_HASH_TYPE_SHA384: break;
    #endif
    #if defined(MBEDTLS_SHA512_USE_HARDWARE)
        case CRYPTO_HASH_TYPE_SHA512: break;
    #endif
        default: return 0;
    }

    return( 1 );
}

int check_ctx_go_harden( const void *ctx )
{
    return( get_hash_harden_ctx( ctx ) != NULL );
}

int hash_init_harden( void )
{
    int ret;

    mbedtls_harden_log_func_enter();

    if( IS_HASH_FUNC_REGISTERED() != TD_TRUE )
    {
        g_is_hash_registered = 0;
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    ret = CIPHER_HASH_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_INIT failed, ret = 0x%x!\n", ret);
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    g_is_hash_registered = 1;

    mbedtls_harden_log_func_exit();

    return( 0 );
}

void hash_free_harden( void *ctx )
{
    hash_harden_context *harden_context = NULL;

    mbedtls_harden_log_func_enter();

    if( IS_HASH_FUNC_REGISTERED() != TD_TRUE )
        return;

    harden_context = get_hash_harden_ctx( ctx );
    if ( harden_context != NULL )
    {
        if (harden_context->is_used != 0)
            CIPHER_HASH_DESTROY(harden_context->hash_handle);
        memset(harden_context, 0, sizeof(hash_harden_context));
    }

    CIPHER_HASH_DEINIT();

    mbedtls_harden_log_func_exit();
}

static int channel_is_using(mbedtls_md_context_t *ctx)
{
    int i;
    for ( i = 0; i < MAX_HASH_HARD_CTX_NUM; ++i )
    {
        if( g_hash_harden_context[i].ctx == ctx && g_hash_harden_context[i].is_used == 1 )
        {
            return 1;
        }
    }
    return 0;
}

int hash_starts_harden( void *ctx, crypto_hash_type hash_type, int is_smaller )
{
    int ret;
    crypto_hash_attr hash_attr = {0};
    unsigned int hash_handle;
    int idle_chn_idx = 0;
    hash_harden_context *harden_context = NULL;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    hash_attr.is_long_term = MBEDTLS_HARDEN_FALSE;
    hash_attr.hash_type = hash_type;

    /* if this ctx is using then destroy it first before start a new channel */
    if (channel_is_using( (mbedtls_md_context_t *)ctx ) == 1)
    {
        harden_context = get_hash_harden_ctx( ctx );
        if ( harden_context == NULL )
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;

        CIPHER_HASH_DESTROY( harden_context->hash_handle );
    }

    if ( ( idle_chn_idx = get_idle_channel( (mbedtls_md_context_t *)ctx ) ) == MAX_HASH_HARD_CTX_NUM )
    {
        mbedtls_harden_log_err("hash channel over flow!\n");
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    ret = CIPHER_HASH_START( &hash_handle, &hash_attr );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_START failed! ret = 0x%x\n", ret);
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    g_hash_harden_context[idle_chn_idx].ctx = ctx;
    g_hash_harden_context[idle_chn_idx].hash_handle = hash_handle;
    g_hash_harden_context[idle_chn_idx].is_smaller = is_smaller;
    g_hash_harden_context[idle_chn_idx].is_used = 1;

    mbedtls_harden_log_func_exit();

    return( 0 );
}

int hash_update_harden( void *ctx, const unsigned char *input, size_t ilen )
{
    int ret;
    hash_harden_context *harden_context = NULL;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    harden_context = get_hash_harden_ctx( ctx );
    harden_context->src_buf.virt_addr = (unsigned char *)input;

    ret = CIPHER_HASH_UPDATE( harden_context->hash_handle, &harden_context->src_buf, ilen );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_UPDATE failed! ret = 0x%x\n", ret);
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    mbedtls_harden_log_func_exit();
    return ( 0 );
}

int hash_finish_harden( void *ctx, unsigned char *output )
{
    int ret;
    unsigned int buffer_len;
    hash_harden_context *harden_context = NULL;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    harden_context = get_hash_harden_ctx( ctx );
    buffer_len = 64; // 64: max_hash_length

    /* This line of code is placed in the front because the hash channel will be released regardless of
       whether the CIPHER_HASH_FINISH succeeds or fails. */
    harden_context->is_used = 0;

    ret = CIPHER_HASH_FINISH( harden_context->hash_handle, output, &buffer_len );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_FINISH failed! ret = 0x%x\n", ret);
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    mbedtls_harden_log_func_exit();
    return( 0 );
}

void hash_clone_harden( void *dst, const void *src, crypto_hash_type bigger, crypto_hash_type smaller)
{
    int ret;
    hash_harden_context *src_context;
    hash_harden_context *dst_context;
    unsigned int dst_hash_handle;
    crypto_hash_attr hash_attr;
    crypto_hash_clone_ctx clone_ctx = {0};

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return;

    src_context = get_hash_harden_ctx( src );
    if( src_context == NULL )
    {
        return;
    }

    dst_context = get_hash_harden_ctx( dst );
    if( dst_context == NULL )
    {
        hash_attr.is_long_term = MBEDTLS_HARDEN_FALSE;
        hash_attr.hash_type = src_context->is_smaller ? smaller : bigger;

        ret = CIPHER_HASH_START(&dst_hash_handle, &hash_attr);
        if (ret != MBEDTLS_HARDEN_SUCCESS)
        {
            mbedtls_harden_log_err("CIPHER_HASH_START failed! ret = 0x%x\n", ret);
            return;
        }

        for(int i = 0; i < MAX_HASH_HARD_CTX_NUM; ++i )
        {
            if( g_hash_harden_context[i].is_used == 0 )
            {
                g_hash_harden_context[i].ctx = dst;
                g_hash_harden_context[i].is_used = 1;
                g_hash_harden_context[i].hash_handle = dst_hash_handle;
                g_hash_harden_context[i].is_smaller = src_context->is_smaller;
                break;
            }
        }
    }
    else
    {
        dst_hash_handle = dst_context->hash_handle;
    }

    ret = CIPHER_HASH_GET( src_context->hash_handle, &clone_ctx );
    if ( ret != MBEDTLS_HARDEN_SUCCESS ) {
        mbedtls_harden_log_err("CIPHER_HASH_GET failed! ret = 0x%x\n", ret);
        return;
    }
    ret = CIPHER_HASH_SET( dst_hash_handle, &clone_ctx );
    if ( ret != MBEDTLS_HARDEN_SUCCESS ) {
        mbedtls_harden_log_err("CIPHER_HASH_SET failed! ret = 0x%x\n", ret);
        return;
    }
    mbedtls_harden_log_func_exit();
    return;
}

void hmac_free_harden( void *ctx )
{
    hash_harden_context *harden_context;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return;

    harden_context = get_hash_harden_ctx( ctx );
    if ( harden_context != NULL )
    {
        if( harden_context->is_used != 0 )
        {
            CIPHER_HASH_DESTROY( harden_context->hash_handle );
            CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );
        }
        if( harden_context->hash_attr.key != NULL )
        {
            memset( harden_context->hash_attr.key, 0, harden_context->hash_attr.key_len );
            mbedtls_free( harden_context->hash_attr.key );
            harden_context->hash_attr.key = NULL;
        }

        memset( harden_context, 0, sizeof(hash_harden_context) );

        KM_DEINIT();
        CIPHER_HASH_DEINIT();
    }

    mbedtls_harden_log_func_exit();
}

int hmac_harden_can_do( const mbedtls_md_info_t *md_info, int hmac )
{
    crypto_hash_type hash_type = CRYPTO_HASH_TYPE_INVALID;

    if( IS_HASH_FUNC_REGISTERED() != TD_TRUE )
        return ( 0 );

    if( hmac != 1 )
        return 0;

    get_hash_info( md_info->type, &hash_type, NULL, NULL );

    return hash_harden_can_do( hash_type );
}

int hmac_setup_harden( mbedtls_md_context_t *ctx, const mbedtls_md_info_t *md_info )
{
    int ret;
    crypto_hash_type hash_type = CRYPTO_HASH_TYPE_INVALID;
    int idle_chn_idx = 0;

    mbedtls_harden_log_func_enter();

    if( IS_HASH_FUNC_REGISTERED() != TD_TRUE )
    {
        g_is_hash_registered = 0;
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    if ( ( idle_chn_idx = get_idle_channel( (mbedtls_md_context_t *)ctx ) ) == MAX_HASH_HARD_CTX_NUM )
    {
        mbedtls_harden_log_err("hash channel over flow!\n");
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    /*  The reason why this function must be invoked here is that in the invoking process of the hmac_drbg algorithm,
        mbedtls_md_setup is directly invoked without invoking mbedtls_md_init. */
    ret = CIPHER_HASH_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_INIT failed, ret = 0x%x!\n", ret);
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    ret = KM_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        /*  Even if the operation fails, you do NOT need to perform CIPHER_HASH_DEINIT,
            because resources are released in hmac_free_harden which will always be invoked. */
        mbedtls_harden_log_err("KM_INIT failed, ret = 0x%x!\n", ret);
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    // hash_type has been checked in function hmac_harden_can_do
    get_hash_info( md_info->type, &hash_type, NULL, NULL );
    
    g_hash_harden_context[idle_chn_idx].ctx = ctx;
    g_hash_harden_context[idle_chn_idx].hash_attr.hash_type = hash_type;
    g_is_hash_registered = 1;

    mbedtls_harden_log_func_exit();

    return( 0 );
}

int hmac_starts_harden( void *ctx, const unsigned char *key, size_t keylen)
{
    int ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    unsigned int hash_handle;
    km_keyslot_engine key_engine = KM_KEYSLOT_ENGINE_AES;
    hash_harden_context *harden_context;
    size_t used_key_len = keylen;
    unsigned int block_size;
    unsigned int olen;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 || key == NULL )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    harden_context = get_hash_harden_ctx( ctx );
    if( harden_context == NULL )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    get_hash_info( ((mbedtls_md_context_t *)ctx)->MBEDTLS_PRIVATE(md_info)->type, NULL, &olen, &block_size );
    if( block_size < keylen )
        used_key_len = olen;

    if( harden_context->hash_attr.key == NULL )
    {
        harden_context->hash_attr.key = (unsigned char *)mbedtls_calloc(1, used_key_len);
    }
    else if( harden_context->hash_attr.key_len < used_key_len )
    {
        memset(harden_context->hash_attr.key, 0, harden_context->hash_attr.key_len);
        mbedtls_free(harden_context->hash_attr.key);
        harden_context->hash_attr.key = (unsigned char *)mbedtls_calloc(1, used_key_len);
    }

    if( harden_context->hash_attr.key == NULL )
    {
        mbedtls_harden_log_err( "mbedtls_calloc failed!\n" );
        return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }
    harden_context->hash_attr.key_len = used_key_len;
    harden_context->hash_attr.is_long_term = MBEDTLS_HARDEN_TRUE;
    harden_context->hash_attr.is_keyslot = MBEDTLS_HARDEN_TRUE;
    if( used_key_len == keylen )
    {
        memcpy( harden_context->hash_attr.key, key, used_key_len );
    } else {
        ret = get_hmac_hashed_key( &(harden_context->hash_attr), (unsigned char*)key, keylen );
        // don't worry about resource free, which will always be called in mbedtls_md_free
        if( ret != 0 )
        {
            mbedtls_harden_log_err( "get_hmac_hashed_key faild!\n" );
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        }
    }

    get_km_keyslot_engine( harden_context->hash_attr.hash_type, &key_engine );
    if (key_engine == KM_KEYSLOT_ENGINE_AES) {
        mbedtls_harden_log_err("ALL HMAC TYPE NOT SUPPORT!\n");
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    // don't worry about resource free, which will always be called in mbedtls_md_free
    ret = CIPHER_CREATE_KEYSLOT( &harden_context->hash_attr.drv_keyslot_handle, key_engine );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("CIPHRE_CREATE_KEYSLOT faild!\n");
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    ret = CIPHER_SET_CLEAR_KEY( harden_context->hash_attr.drv_keyslot_handle,
                                (unsigned char *)harden_context->hash_attr.key, used_key_len, key_engine );
    if( ret != 0 )
    {
        CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );
        mbedtls_harden_log_err( "CIPHER_SET_CLEAR_KEY faild!\n" );
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    // don't worry about resource free, which will always be called in mbedtls_md_free
    ret = CIPHER_HASH_START( &hash_handle, &harden_context->hash_attr );
    if ( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );
        mbedtls_harden_log_err( "CIPHER_HASH_START failed, ret = 0x%x!\n", ret );
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    harden_context->hash_handle = hash_handle;
    harden_context->is_used = 1;
    mbedtls_harden_log_func_exit();

    return( 0 );
}

int hmac_update_harden( void *ctx, const unsigned char *input, size_t ilen )
{
    int ret;
    hash_harden_context *harden_context = NULL;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    if( ilen == 0 )
        return( 0 );

    harden_context = get_hash_harden_ctx( ctx );
    harden_context->src_buf.virt_addr = (unsigned char *)input;

    ret = CIPHER_HASH_UPDATE( harden_context->hash_handle, &harden_context->src_buf, ilen );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err( "CIPHER_HASH_UPDATE failed! ret = 0x%x\n", ret );
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    mbedtls_harden_log_func_exit();
    return ( 0 );
}

int hmac_finish_harden( mbedtls_md_context_t *ctx, unsigned char *output )
{
    int ret;
    unsigned int buffer_len;
    hash_harden_context *harden_context = NULL;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    harden_context = get_hash_harden_ctx( ctx );
    buffer_len = 64; // 64: max_hash_length

    /* This line of code is placed in the front because the hash channel will be released regardless of
       whether the CIPHER_HASH_FINISH succeeds or fails. */
    harden_context->is_used = 0;

    ret = CIPHER_HASH_FINISH( harden_context->hash_handle, output, &buffer_len );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_HASH_FINISH failed! ret = 0x%x\n", ret);
        CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );

    mbedtls_harden_log_func_exit();

    return( 0 );
}

int hmac_reset_harden( mbedtls_md_context_t *ctx )
{
    int ret;
    hash_harden_context *harden_context = NULL;
    km_keyslot_engine key_engine = KM_KEYSLOT_ENGINE_AES;

    mbedtls_harden_log_func_enter();

    if( g_is_hash_registered == 0 )
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

    harden_context = get_hash_harden_ctx( ctx );
    if( harden_context != NULL )
    {
        if( harden_context->is_used == 0 )
        {
            get_km_keyslot_engine( harden_context->hash_attr.hash_type, &key_engine );

            // don't worry about resource free, which will always be called in mbedtls_md_free
            ret = CIPHER_CREATE_KEYSLOT( &harden_context->hash_attr.drv_keyslot_handle, key_engine );
            if( ret != MBEDTLS_HARDEN_SUCCESS )
            {
                mbedtls_harden_log_err("CIPHRE_CREATE_KEYSLOT faild!\n");
                return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
            }
            ret = CIPHER_SET_CLEAR_KEY( harden_context->hash_attr.drv_keyslot_handle,
                                        harden_context->hash_attr.key,
                                        harden_context->hash_attr.key_len, key_engine );
            if( ret != MBEDTLS_HARDEN_SUCCESS )
            {
                CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );
                mbedtls_harden_log_err("CIPHER_SET_CLEAR_KEY faild!\n");
                return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
            }

            // don't worry about resource free, which will always be called in mbedtls_md_free
            ret = CIPHER_HASH_START( &harden_context->hash_handle, &harden_context->hash_attr );
            if( ret != MBEDTLS_HARDEN_SUCCESS )
            {
                CIPHER_DESTROY_KEYSLOT( harden_context->hash_attr.drv_keyslot_handle );
                mbedtls_harden_log_err( "CIPHER HASH START failed! ret = 0x%x!\n", ret );
                return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
            }

            harden_context->is_used = 1;
        }
    }

    mbedtls_harden_log_func_exit();

    return( 0 );
}
#endif /* MBEDTLS_SHA1_ALT || MBEDTLS_SHA256_ALT || MBEDTLS_SHA512_ALT */

#if defined(MBEDTLS_SHA1_ALT)

#define SHA1_VALIDATE_RET(cond)                             \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA1_BAD_INPUT_DATA )
#define SHA1_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    SHA1_VALIDATE( ctx != NULL );

    ( void )hash_init_harden();
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if( ctx == NULL )
        return;

    hash_free_harden( ctx );
}

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    SHA1_VALIDATE( dst != NULL );
    SHA1_VALIDATE( src != NULL );

    if( check_ctx_go_harden( src ) == 1 )
    {
        return hash_clone_harden( dst, src, CRYPTO_HASH_TYPE_SHA1,  CRYPTO_HASH_TYPE_SHA1 );
    }
}

int mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    SHA1_VALIDATE_RET( ctx != NULL );

    if( hash_harden_can_do( CRYPTO_HASH_TYPE_SHA1 ) == 1 )
    {
        return hash_starts_harden( ctx, CRYPTO_HASH_TYPE_SHA1, 0 );
    }
    return( -1 );
}

int mbedtls_sha1_update( mbedtls_sha1_context *ctx,
                             const unsigned char *input,
                             size_t ilen )
{
    SHA1_VALIDATE_RET( ctx != NULL );
    SHA1_VALIDATE_RET( ilen == 0 || input != NULL );

    if( check_ctx_go_harden( ctx ) == 1 )
    {
        return hash_update_harden( ctx, input, ilen );
    }
    return( -1 );
}

int mbedtls_sha1_finish( mbedtls_sha1_context *ctx,
                             unsigned char output[20] )
{
    SHA1_VALIDATE_RET( ctx != NULL );
    SHA1_VALIDATE_RET( (unsigned char *)output != NULL );

    if( check_ctx_go_harden( ctx ) == 1 )
    {
        return hash_finish_harden( ctx, output );
    }
    return( -1 );
}
#endif /* MBEDTLS_SHA1_ALT */

#if defined(MBEDTLS_SHA256_ALT)

#define SHA256_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA )
#define SHA256_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    SHA256_VALIDATE( ctx != NULL );

    ( void )hash_init_harden();
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL )
        return;

    hash_free_harden( ctx );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    SHA256_VALIDATE( dst != NULL );
    SHA256_VALIDATE( src != NULL );

    if( check_ctx_go_harden( src ) == 1 )
    {
        return hash_clone_harden( dst, src, CRYPTO_HASH_TYPE_SHA256,  CRYPTO_HASH_TYPE_SHA224 );
    }
}

int mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 )
{
    crypto_hash_type hash_type = CRYPTO_HASH_TYPE_INVALID;
    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( is224 == 0 || is224 == 1 );

    if( is224 == 1 )
    {
        hash_type = CRYPTO_HASH_TYPE_SHA224;
    }

    if( is224 == 0 )
    {
        hash_type = CRYPTO_HASH_TYPE_SHA256;
    }

    if( hash_harden_can_do( hash_type ) == 1 )
    {
        return hash_starts_harden( ctx, hash_type, is224 );
    }
    return( -1 );
}

int mbedtls_sha256_update( mbedtls_sha256_context *ctx,
                               const unsigned char *input,
                               size_t ilen )
{
    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    if( check_ctx_go_harden( ctx ) == 1 )
    {
        return hash_update_harden( ctx, input, ilen );
    }

    return( -1 );
}

int mbedtls_sha256_finish( mbedtls_sha256_context *ctx,
                               unsigned char *output )
{
    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( (unsigned char *)output != NULL );

    if( check_ctx_go_harden( ctx ) == 1 )
    {
        return hash_finish_harden( ctx, output );
    }

    return( -1 );
}
#endif /* MBEDTLS_SHA256_ALT */

#if defined(MBEDTLS_SHA512_ALT)

#define SHA512_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA512_BAD_INPUT_DATA )
#define SHA512_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

void mbedtls_sha512_init( mbedtls_sha512_context *ctx )
{
    SHA512_VALIDATE( ctx != NULL );

    ( void )hash_init_harden();
}

void mbedtls_sha512_free( mbedtls_sha512_context *ctx )
{
    if( ctx == NULL )
        return;

    hash_free_harden( ctx );
}

void mbedtls_sha512_clone( mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src )
{
    SHA512_VALIDATE( dst != NULL );
    SHA512_VALIDATE( src != NULL );

    if( check_ctx_go_harden( src ) == 1 )
    {
        return hash_clone_harden( dst, src, CRYPTO_HASH_TYPE_SHA512,  CRYPTO_HASH_TYPE_SHA384 );
    }
}

int mbedtls_sha512_starts( mbedtls_sha512_context *ctx, int is384 )
{
    crypto_hash_type hash_type = CRYPTO_HASH_TYPE_INVALID;
    SHA512_VALIDATE_RET( ctx != NULL );
    SHA512_VALIDATE_RET( is384 == 0 || is384 == 1 );

    if( is384 == 1 )
    {
        hash_type = CRYPTO_HASH_TYPE_SHA384;
    }

    if( is384 == 0 )
    {
        hash_type = CRYPTO_HASH_TYPE_SHA512;
    }

    if( hash_harden_can_do( hash_type ) == 1 )
    {
        return hash_starts_harden( ctx, hash_type, is384 );
    }
    return( -1 );
}

int mbedtls_sha512_update( mbedtls_sha512_context *ctx,
                               const unsigned char *input,
                               size_t ilen )
{
    SHA512_VALIDATE_RET( ctx != NULL );
    SHA512_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    if( check_ctx_go_harden( ctx ) == 1 )
    {
        return hash_update_harden( ctx, input, ilen );
    }

    return( -1 );
}

int mbedtls_sha512_finish( mbedtls_sha512_context *ctx,
                               unsigned char *output )
{
    SHA512_VALIDATE_RET( ctx != NULL );
    SHA512_VALIDATE_RET( (unsigned char *)output != NULL );

    if( check_ctx_go_harden( ctx ) == 1 )
    {
        return hash_finish_harden( ctx, output );
    }

    return( -1 );
}
#endif /* MBEDTLS_SHA512_ALT */
