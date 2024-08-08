/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_PBKDF2_HMAC_HARDEN_H
#define MBEDTLS_PBKDF2_HMAC_HARDEN_H

#include "mbedtls/pkcs5.h"

int pbkdf2_hmac_harden_can_do( mbedtls_md_context_t *ctx, unsigned int iteration_count );

int pbkdf2_hmac_harden( mbedtls_md_context_t *ctx,
        const unsigned char *password,
        size_t plen, const unsigned char *salt, size_t slen,
        unsigned int iteration_count,
        uint32_t key_length, unsigned char *output );

#endif /* MBEDTLS_PBKDF2_HMAC_HARDEN_H */