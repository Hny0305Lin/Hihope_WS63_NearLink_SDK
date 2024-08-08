/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "hash_harden_common.h"
#include "dfx.h"
#include "mbedtls/error.h"
#include <string.h>

static const md_type_info g_md_type_info_table[] = {
#if defined(MBEDTLS_SHA1_USE_HARDWARE)
    { MBEDTLS_MD_SHA1,   CRYPTO_HASH_TYPE_HMAC_SHA1,   20,  64 },
#endif
#if defined(MBEDTLS_SHA224_USE_HARDWARE)
    { MBEDTLS_MD_SHA224, CRYPTO_HASH_TYPE_HMAC_SHA224, 28,  64 },
#endif
#if defined(MBEDTLS_SHA256_USE_HARDWARE)
    { MBEDTLS_MD_SHA256, CRYPTO_HASH_TYPE_HMAC_SHA256, 32,  64 },
#endif
#if defined(MBEDTLS_SHA384_USE_HARDWARE)
    { MBEDTLS_MD_SHA384, CRYPTO_HASH_TYPE_HMAC_SHA384, 48, 128 },
#endif
#if defined(MBEDTLS_SHA512_USE_HARDWARE)
    { MBEDTLS_MD_SHA512, CRYPTO_HASH_TYPE_HMAC_SHA512, 64, 128 },
#endif
    { MBEDTLS_MD_NONE,   CRYPTO_HASH_TYPE_INVALID,     0,   0  },
};

static const kslot_hmac_engine_map g_kslot_hmac_engine_map_table[] = {
#if defined(MBEDTLS_SHA1_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA1,     KM_KEYSLOT_ENGINE_HMAC_SHA1   },
#endif
#if defined(MBEDTLS_SHA224_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA224,   KM_KEYSLOT_ENGINE_HMAC_SHA224 },
#endif
#if defined(MBEDTLS_SHA256_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA256,   KM_KEYSLOT_ENGINE_HMAC_SHA256 },
#endif
#if defined(MBEDTLS_SHA384_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA384,   KM_KEYSLOT_ENGINE_HMAC_SHA384 },
#endif
#if defined(MBEDTLS_SHA512_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA512,   KM_KEYSLOT_ENGINE_HMAC_SHA512 },
#endif
    {  CRYPTO_HASH_TYPE_INVALID,      KM_KEYSLOT_ENGINE_AES,        },
};

static const hmac_hash_map g_hmac_hash_map_table[] = {
#if defined(MBEDTLS_SHA1_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA1,     CRYPTO_HASH_TYPE_SHA1    },
#endif
#if defined(MBEDTLS_SHA224_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA224,   CRYPTO_HASH_TYPE_SHA224  },
#endif
#if defined(MBEDTLS_SHA256_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA256,   CRYPTO_HASH_TYPE_SHA256  },
#endif
#if defined(MBEDTLS_SHA384_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA384,   CRYPTO_HASH_TYPE_SHA384  },
#endif
#if defined(MBEDTLS_SHA512_USE_HARDWARE)
    { CRYPTO_HASH_TYPE_HMAC_SHA512,   CRYPTO_HASH_TYPE_SHA512  },
#endif
    {  CRYPTO_HASH_TYPE_INVALID,      CRYPTO_HASH_TYPE_INVALID },
};

void get_km_keyslot_engine(crypto_hash_type hash_type, km_keyslot_engine *key_engine)
{
    unsigned int i = 0;
    for (i = 0; i < sizeof(g_md_type_info_table) / sizeof(g_md_type_info_table[0]); i++) {
        if (g_kslot_hmac_engine_map_table[i].hash_type == hash_type) {
            if (key_engine != NULL)
                *key_engine = g_kslot_hmac_engine_map_table[i].key_engine;
            return;
        }
    }
    return;
}

void get_hash_info(mbedtls_md_type_t md_type, crypto_hash_type *hash_type, unsigned int *olen, unsigned int *block_size)
{
    unsigned int i = 0;
    for (i = 0; i < sizeof(g_md_type_info_table) / sizeof(g_md_type_info_table[0]); i++) {
        if (g_md_type_info_table[i].md_type == md_type) {
            if (hash_type != NULL)
                *hash_type = g_md_type_info_table[i].hash_type;
            if (olen != NULL)
                *olen = g_md_type_info_table[i].olen;
            if (block_size != NULL)
                *block_size = g_md_type_info_table[i].block_size;
            return;
        }
    }
    return;
}

int get_hmac_hashed_key( crypto_hash_attr *hmac_attr, unsigned char *key, size_t keylen )
{
    unsigned int i = 0;
    int ret = MBEDTLS_HARDEN_SUCCESS;
    crypto_hash_attr hash_attr;
    unsigned int hash_handle;
    crypto_buf_attr src_buf = {0};
    unsigned int out_len = hmac_attr->key_len;

    hash_attr.is_long_term = 0;
    src_buf.virt_addr = key;
    for (i = 0; i < sizeof(g_hmac_hash_map_table) / sizeof(g_hmac_hash_map_table[0]); i++) {
        if (g_hmac_hash_map_table[i].hmac_type == hmac_attr->hash_type) {
            hash_attr.hash_type = g_hmac_hash_map_table[i].hash_type;
        }
    }

    ret = CIPHER_HASH_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
        return ( -1 );

    ret = CIPHER_HASH_START( &hash_handle, &hash_attr );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
        goto exit_deinit;

    ret = CIPHER_HASH_UPDATE( hash_handle, &src_buf, keylen );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
        goto exit_destroy;

    ret = CIPHER_HASH_FINISH( hash_handle, hmac_attr->key, &out_len );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
        goto exit_destroy;

    CIPHER_HASH_DEINIT();

    return ret;
exit_destroy:
    (void)CIPHER_HASH_DESTROY( hash_handle );
exit_deinit:
    CIPHER_HASH_DEINIT();
    return ( -1 );
}