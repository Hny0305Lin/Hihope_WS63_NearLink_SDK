/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_AES_HARDEN_H
#define MBEDTLS_AES_HARDEN_H

#include "cipher_adapt.h"
#include "mbedtls/aes.h"
#include "string.h"

#ifdef MBEDTLS_AES_ALT

typedef struct aes_harden_context{
    unsigned int symc_handle;
    unsigned int keyslot_handle;
    mbedtls_aes_context *ctx;
    crypto_symc_ctrl_t symc_ctrl;
    unsigned char key[32];
    unsigned int key_length;
    unsigned char is_used;
    unsigned char only_software_support;
} aes_harden_context;

unsigned char check_if_phys_mem( const unsigned char *input );

void mbedtls_aes_init_harden( mbedtls_aes_context *ctx );

void mbedtls_aes_free_harden( mbedtls_aes_context *ctx );

int mbedtls_aes_setkey_harden( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits );

void mbedtls_sync_aes_software_ctx( mbedtls_aes_context *ctx, unsigned char *iv );

#ifdef MBEDTLS_ECB_MODE_USE_HARDWARE
int ecb_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                int mode,
                                const unsigned char *input,
                                unsigned char *output,
                                size_t length );
#endif

#ifdef MBEDTLS_CBC_MODE_USE_HARDWARE
int cbc_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                int mode,
                                size_t length,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output );
#endif

#ifdef MBEDTLS_CFB_MODE_USE_HARDWARE
int cfb128_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output );

int cfb8_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                int mode,
                                size_t length,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output );
#endif

#ifdef MBEDTLS_OFB_MODE_USE_HARDWARE
int ofb_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                size_t length,
                                size_t *iv_off,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output );
#endif

#ifdef MBEDTLS_CTR_MODE_USE_HARDWARE
int ctr_cfg_and_crypt_harden( mbedtls_aes_context *ctx,
                                size_t length,
                                size_t *nc_off,
                                unsigned char nonce_counter[16],
                                unsigned char stream_block[16],
                                const unsigned char *input,
                                unsigned char *output );
#endif

#endif

#endif