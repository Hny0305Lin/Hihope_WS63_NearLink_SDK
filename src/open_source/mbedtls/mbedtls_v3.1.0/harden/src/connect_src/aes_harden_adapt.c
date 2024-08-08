/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: aes harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "mbedtls_harden_adapt_api.h"
#include "dfx.h"

#include "securec.h"

static mbedtls_alt_aes_harden_func g_aes_func;

int mbedtls_alt_aes_encrypt(unsigned char *key, unsigned int key_len, const unsigned char src[16], unsigned char dst[16])
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.encrypt == NULL) {
        mbedtls_printf("Error: encrypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.encrypt(key, key_len, src, dst);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_aes_decrypt(unsigned char *key, unsigned int key_len, const unsigned char src[16], unsigned char dst[16])
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.decrypt == NULL) {
        mbedtls_printf("Error: decrypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.decrypt(key, key_len, src, dst);
    mbedtls_harden_log_func_exit();
    return ret;
}


int mbedtls_alt_aes_cbc_encrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.cbc_encrypt == NULL) {
        mbedtls_printf("Error: cbc encrypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.cbc_encrypt(key, key_len, iv, src, dst, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_aes_cbc_decrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.cbc_decrypt == NULL) {
        mbedtls_printf("Error: cbc decrypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.cbc_decrypt(key, key_len, iv, src, dst, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}


int mbedtls_alt_aes_cfb8_encrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.cbc_encrypt == NULL) {
        mbedtls_printf("Error: cfb8 encrypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.cfb8_encrypt(key, key_len, iv, src, dst, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_aes_cfb8_decrypt(unsigned char *key, unsigned key_len, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.cbc_encrypt == NULL) {
        mbedtls_printf("Error: cfb8 decrypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.cfb8_decrypt(key, key_len, iv, src, dst, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_aes_ofb_crypt(unsigned char *key, unsigned key_len, unsigned char *iv_off, unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.ofb_crypt == NULL) {
        mbedtls_printf("Error: ofb crypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.ofb_crypt(key, key_len, iv_off, iv, src, dst, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_aes_ctr_crypt(unsigned char *key, unsigned key_len, unsigned char *nc_off, unsigned char nonce_counter[16], unsigned char stream_block[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_aes_func.ctr_crypt == NULL) {
        mbedtls_printf("Error: ctr crypt unregister!\n");
        return -1;
    }
    ret = g_aes_func.ctr_crypt(key, key_len, nc_off, nonce_counter, stream_block, src, dst, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

void mbedtls_alt_aes_register(const mbedtls_alt_aes_harden_func *aes_func)
{
    if (aes_func == NULL) {
        return;
    }
    (void)memcpy_s(&g_aes_func, sizeof(mbedtls_alt_aes_harden_func), aes_func, sizeof(mbedtls_alt_aes_harden_func));
}