/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_HASH_HARDEN_COMMON_H
#define MBEDTLS_HASH_HARDEN_COMMON_H

#include "cipher_adapt.h"
#include "mbedtls/platform.h"
#include "md_wrap.h"

typedef struct {
    mbedtls_md_type_t md_type;
    crypto_hash_type hash_type;
    size_t olen;
    size_t block_size;
} md_type_info;

typedef struct {
    crypto_hash_type hash_type;
    km_keyslot_engine key_engine;
} kslot_hmac_engine_map;

typedef struct {
    crypto_hash_type hmac_type;
    crypto_hash_type hash_type;
} hmac_hash_map;

void get_km_keyslot_engine(crypto_hash_type hash_type, km_keyslot_engine *key_engine);

void get_hash_info( mbedtls_md_type_t md_type, crypto_hash_type *hash_type, unsigned int *olen,
    unsigned int *block_size);

int get_hmac_hashed_key( crypto_hash_attr *hmac_attr, unsigned char *key, size_t keylen );

#endif /* MBEDTLS_HASH_HARDEN_COMMON_H */