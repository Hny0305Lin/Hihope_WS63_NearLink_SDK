/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_CMAC_HARDEN_H
#define MBEDTLS_CMAC_HARDEN_H

#include "cipher_adapt.h"
#include "mbedtls/cmac.h"
#include "string.h"

#ifdef MBEDTLS_CMAC_ALT

typedef struct cmac_harden_context{
    unsigned int symc_handle;
    unsigned int keyslot_handle;
    mbedtls_cipher_context_t *ctx;
    unsigned char key[32];
    unsigned int key_length;
    unsigned char is_update;
    unsigned char is_finish;
    unsigned char use_software;
    unsigned char is_used;
} cmac_harden_context;

int mbedtls_cipher_cmac_starts_harden( mbedtls_cipher_context_t *ctx,
                                const unsigned char *key, size_t keybits,
                                mbedtls_cipher_type_t type );

int mbedtls_cipher_cmac_update_harden( mbedtls_cipher_context_t *ctx,
                                        const unsigned char *input, size_t ilen );

int mbedtls_cipher_cmac_finish_harden( mbedtls_cipher_context_t *ctx,
                                        unsigned char *output );

int mbedtls_cipher_cmac_reset_harden( mbedtls_cipher_context_t *ctx );

#endif

#endif