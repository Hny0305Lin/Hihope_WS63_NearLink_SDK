/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden api.
 *
 * Create: 2023-12-23
*/

#ifndef MBEDTLS_HARDEN_ADAPT_API_H
#define MBEDTLS_HARDEN_ADAPT_API_H

#include "mbedtls_harden_struct.h"

/* Hash. */
typedef int (*mbedtls_alt_hash_start_func)(mbedtls_alt_hash_clone_ctx *clone_ctx,
    mbedtls_alt_hash_type hash_type);
typedef int (*mbedtls_alt_hash_update_func)(mbedtls_alt_hash_clone_ctx *clone_ctx,
    const unsigned char *data, unsigned int data_len);
typedef int (*mbedtls_alt_hash_finish_func)(mbedtls_alt_hash_clone_ctx *clone_ctx,
    unsigned char *out, unsigned int out_len);

typedef struct {
    mbedtls_alt_hash_start_func   start;
    mbedtls_alt_hash_update_func  update;
    mbedtls_alt_hash_finish_func  finish;
} mbedtls_alt_hash_harden_func;

int mbedtls_alt_hash_start(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type);

int mbedtls_alt_hash_update(mbedtls_alt_hash_clone_ctx *clone_ctx, const unsigned char *data, unsigned int data_len);

int mbedtls_alt_hash_finish(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len);

void mbedtls_alt_hash_register(const mbedtls_alt_hash_harden_func *hash_func);

/* AES. */
typedef int (*mbedtls_alt_aes_crypt_func)(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);
typedef int (*mbedtls_alt_aes_cbc_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_cfb8_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_ofb_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_ctr_crypt_func)(const unsigned char *key, unsigned key_len,
    unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

typedef struct {
    mbedtls_alt_aes_crypt_func        encrypt;
    mbedtls_alt_aes_crypt_func        decrypt;
    mbedtls_alt_aes_cbc_crypt_func    cbc_encrypt;
    mbedtls_alt_aes_cbc_crypt_func    cbc_decrypt;
    mbedtls_alt_aes_cfb8_crypt_func   cfb8_encrypt;
    mbedtls_alt_aes_cfb8_crypt_func   cfb8_decrypt;
    mbedtls_alt_aes_ofb_crypt_func    ofb_crypt;
    mbedtls_alt_aes_ctr_crypt_func    ctr_crypt;
} mbedtls_alt_aes_harden_func;

int mbedtls_alt_aes_encrypt(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_decrypt(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_cbc_encrypt(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cbc_decrypt(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cfb8_encrypt(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cfb8_decrypt(const unsigned char *key, unsigned key_len,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ofb_crypt(const unsigned char *key, unsigned key_len,
    unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ctr_crypt(const unsigned char *key, unsigned key_len,
    unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

void mbedtls_alt_aes_register(const mbedtls_alt_aes_harden_func *aes_func);

/* ECP. */
typedef int (*mbedtls_alt_ecp_mul_func)(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);
typedef int (*mbedtls_alt_ecdsa_verify_func)(mbedtls_alt_ecp_curve_type curve_type,
    const unsigned char *hash, unsigned int hahs_len,
    const unsigned char *pub_x, const unsigned char *pub_y,
    const unsigned char *sig_r, const unsigned char *sig_s, unsigned klen);

typedef struct {
    mbedtls_alt_ecp_mul_func ecp_mul;
    mbedtls_alt_ecdsa_verify_func ecdsa_verify;
} mbedtls_alt_ecp_harden_func;

int mbedtls_alt_ecp_mul(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);

void mbedtls_alt_ecp_register(const mbedtls_alt_ecp_harden_func *ecp_func);

#endif