/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_HASH_HARDEN_H
#define MBEDTLS_HASH_HARDEN_H

#include "hash_harden_common.h"

typedef struct {
    void *ctx;
    unsigned int hash_handle;
    crypto_buf_attr src_buf;
    crypto_hash_attr hash_attr;
    int is_smaller;
    unsigned char is_used;
} hash_harden_context;

int hash_harden_can_do( crypto_hash_type hash_type );

int check_ctx_go_harden( const void *ctx );

int hash_init_harden( void );

void hash_free_harden( void *ctx );

int hash_starts_harden( void *ctx, crypto_hash_type hash_type, int is_smaller );

int hash_update_harden( void *ctx, const unsigned char *input, size_t ilen );

int hash_finish_harden( void *ctx, unsigned char *output );

void hash_clone_harden( void *dst, const void *src, crypto_hash_type bigger, crypto_hash_type smaller);

void hmac_free_harden( void *ctx );

int hmac_harden_can_do( const mbedtls_md_info_t *md_info, int hmac );

int hmac_setup_harden( mbedtls_md_context_t *ctx, const mbedtls_md_info_t *md_info );

int hmac_starts_harden( void *ctx, const unsigned char *key, size_t keylen);

int hmac_update_harden( void *ctx, const unsigned char *input, size_t ilen );

int hmac_finish_harden( mbedtls_md_context_t *ctx, unsigned char *output );

int hmac_reset_harden( mbedtls_md_context_t *ctx );

#endif /* MBEDTLS_HASH_HARDEN_H */