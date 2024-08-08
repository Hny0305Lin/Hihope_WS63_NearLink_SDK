/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: harden adapt functions header file.
 *
 * Create: 2024-07-18
*/

#ifndef MBEDTLS_HARDEN_ADAPT_API_H
#define MBEDTLS_HARDEN_ADAPT_API_H

#include "mbedtls_harden_struct.h"
#include "rsa_alt.h"
#include "crypto_cipher_common_struct.h"

/* Hash. */
typedef int (*mbedtls_alt_hash_start_func)(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type);
typedef int (*mbedtls_alt_hash_update_func)(mbedtls_alt_hash_clone_ctx *clone_ctx, const unsigned char *data, unsigned int data_len);
typedef int (*mbedtls_alt_hash_finish_func)(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len);

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
typedef int (*mbedtls_alt_aes_crypt_func)(unsigned char *key, unsigned int key_len, const unsigned char src[16], unsigned char dst[16]);
typedef int (*mbedtls_alt_aes_cbc_crypt_func)(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_cfb8_crypt_func)(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_ofb_crypt_func)(unsigned char *key, unsigned key_len, unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);
typedef int (*mbedtls_alt_aes_ctr_crypt_func)(unsigned char *key, unsigned key_len, unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

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

int mbedtls_alt_aes_encrypt(unsigned char *key, unsigned int key_len, const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_decrypt(unsigned char *key, unsigned int key_len, const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_cbc_encrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cbc_decrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cfb8_encrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cfb8_decrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ofb_crypt(unsigned char *key, unsigned key_len, unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ctr_crypt(unsigned char *key, unsigned key_len, unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

void mbedtls_alt_aes_register(const mbedtls_alt_aes_harden_func *aes_func);

int mbedtls_alt_rsa_public_encrypt(const mbedtls_rsa_context *ctx, const unsigned char *label,
    unsigned int label_len, const unsigned char *input, unsigned int ilen, unsigned char *output, unsigned int olen);

int mbedtls_alt_rsa_private_decrypt(const mbedtls_rsa_context *ctx, const unsigned char *label,
    unsigned int label_len, const unsigned char *input, unsigned int ilen, unsigned char *output, unsigned int olen_max,
    unsigned int *olen);

int mbedtls_alt_rsa_private_sign(const mbedtls_rsa_context *ctx, const unsigned char *hash, unsigned int hashlen,
    unsigned char *sig, unsigned int olen);

int mbedtls_alt_rsa_public_verify(const mbedtls_rsa_context *ctx, const unsigned char *hash, unsigned int hashlen,
    const unsigned char *sig, unsigned int olen);

/* ECP. */
typedef int (*mbedtls_alt_ecp_mul_func)(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);
typedef int (*mbedtls_alt_ecdsa_verify_func)(mbedtls_alt_ecp_curve_type curve_type,
    const unsigned char *hash, unsigned int hash_len,
    const unsigned char *pub_x, const unsigned char *pub_y,
    const unsigned char *sig_r, const unsigned char *sig_s, unsigned klen);

typedef struct {
    mbedtls_alt_ecp_mul_func ecp_mul;
    mbedtls_alt_ecdsa_verify_func ecdsa_verify;
} mbedtls_alt_ecp_harden_func;

int mbedtls_alt_ecp_mul(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);

int mbedtls_alt_ecdsa_verify(mbedtls_alt_ecp_curve_type curve_type,
    const unsigned char *hash, unsigned int hash_len,
    const unsigned char *pub_x, const unsigned char *pub_y,
    const unsigned char *sig_r, const unsigned char *sig_s, unsigned klen);

void mbedtls_alt_ecp_register(const mbedtls_alt_ecp_harden_func *aes_func);

#endif