/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_GCM_HARDEN_H
#define MBEDTLS_GCM_HARDEN_H

#include "cipher_adapt.h"
#include "mbedtls/gcm.h"
#include "string.h"

#ifdef MBEDTLS_GCM_ALT

typedef struct gcm_harden_context{
    mbedtls_gcm_context *ctx;
    crypto_symc_ctrl_t symc_ctrl;
    crypto_symc_config_aes_ccm_gcm gcm_config;
    int mode;
    unsigned int symc_handle;
    unsigned int keyslot_handle;
    unsigned char key[32];
    unsigned char add[MBEDTLS_CIPHER_MAX_ADD_LEN];
    unsigned char iv[16];
    size_t iv_len;
    size_t add_len;
    unsigned int key_length;
    unsigned char is_used;
    unsigned char is_set_config;
    unsigned char is_software_update_ad;
    unsigned char only_software_support;
} gcm_harden_context;

void mbedtls_gcm_init_harden( mbedtls_gcm_context *ctx );

void mbedtls_gcm_setkey_harden( mbedtls_gcm_context *ctx,
                                mbedtls_cipher_id_t cipher,
                                const unsigned char *key,
                                unsigned int keybits );

int mbedtls_gcm_start_harden( mbedtls_gcm_context *ctx,
                                int mode,
                                const unsigned char *iv,
                                size_t iv_len );

int mbedtls_gcm_update_ad_harden( mbedtls_gcm_context *ctx,
                                const unsigned char *add,
                                size_t add_len );

int mbedtls_gcm_update_harden( mbedtls_gcm_context *ctx,
                                const unsigned char *input, size_t input_length,
                                unsigned char *output );

int mbedtls_gcm_finish_harden( mbedtls_gcm_context *ctx,
                                unsigned char *tag, size_t tag_len );

void mbedtls_gcm_free_harden( mbedtls_gcm_context *ctx );

#endif

#endif