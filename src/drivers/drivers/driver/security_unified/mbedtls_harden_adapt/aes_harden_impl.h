/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls aes harden api.
 *
 * Create: 2023-12-23
*/

#ifndef AES_HARDEN_IMPL_H
#define AES_HARDEN_IMPL_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int mbedtls_alt_aes_encrypt_impl(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_decrypt_impl(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16]);

int mbedtls_alt_aes_cbc_encrypt_impl(const unsigned char *key, unsigned key_len, unsigned char iv[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_cbc_decrypt_impl(const unsigned char *key, unsigned key_len, unsigned char iv[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ofb_crypt_impl(const unsigned char *key, unsigned key_len, unsigned char *iv_off,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_ctr_crypt_impl(const unsigned char *key, unsigned key_len, unsigned char *nc_off,
    unsigned char nonce_counter[16], unsigned char stream_block[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len);

int mbedtls_alt_aes_init(void);

int mbedtls_alt_aes_deinit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif