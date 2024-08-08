/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_CCM_HARDEN_H
#define MBEDTLS_CCM_HARDEN_H

#include "cipher_adapt.h"
#include "mbedtls/ccm.h"
#include "string.h"

#ifdef MBEDTLS_CCM_ALT

typedef struct ccm_harden_context{
    mbedtls_ccm_context *ctx;
    unsigned int symc_handle;
    unsigned int keyslot_handle;
    unsigned char key[32];
    unsigned int key_length;
    unsigned char iv[13];
    size_t iv_len;
    int mode;
    unsigned char aad[MBEDTLS_CIPHER_MAX_ADD_LEN];
    size_t cur_aad_len;
    size_t aad_len;
    size_t input_len;
    size_t tag_len;
    unsigned char is_used;
    unsigned char is_set_config;
    unsigned char only_software_support;
} ccm_harden_context;

void mbedtls_ccm_init_harden( mbedtls_ccm_context *ctx );

int mbedtls_ccm_setkey_harden(mbedtls_ccm_context *ctx,
                                mbedtls_cipher_id_t cipher,
                                const unsigned char *key,
                                unsigned int keybits);

void mbedtls_ccm_free_harden( mbedtls_ccm_context *ctx );

int mbedtls_ccm_starts_harden( mbedtls_ccm_context *ctx,
                                int mode,
                                const unsigned char *iv,
                                size_t iv_len );

int mbedtls_ccm_set_lengths_harden( mbedtls_ccm_context *ctx,
                                        size_t total_ad_len,
                                        size_t plaintext_len,
                                        size_t tag_len );

int mbedtls_ccm_update_ad_harden( mbedtls_ccm_context *ctx,
                                const unsigned char *add,
                                size_t add_len );

int mbedtls_ccm_update_harden( mbedtls_ccm_context *ctx,
                                const unsigned char *input, size_t input_len,
                                unsigned char *output, size_t output_size,
                                size_t *output_len );

int mbedtls_ccm_finish_harden( mbedtls_ccm_context *ctx,
                                unsigned char *tag, size_t tag_len );

#endif

#endif