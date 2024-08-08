/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls aes harden implement long term.
 *
 * Create: 2023-12-23
*/

#include "aes_harden_impl.h"
#include "mbedtls_harden_adapt_api.h"

#include "drv_symc.h"
#include "drv_klad.h"
#include "drv_keyslot.h"

#include "crypto_drv_common.h"

static td_handle g_symc_handle;
static td_handle g_keyslot_handle;
static crypto_mutex g_mbedtls_aes_lock;

static crypto_symc_key_length inner_get_key_length(unsigned int key_len)
{
    switch (key_len) {
        case CRYPTO_128_KEY_LEN:
            return CRYPTO_SYMC_KEY_128BIT;
        case CRYPTO_192_KEY_LEN:
            return CRYPTO_SYMC_KEY_192BIT;
        case CRYPTO_256_KEY_LEN:
            return CRYPTO_SYMC_KEY_256BIT;
        default:
            return CRYPTO_SYMC_KEY_LENGTH_INVALID;
    }
}

static int inner_set_key(const unsigned char *key, unsigned int key_len)
{
    int ret;
    td_handle klad_handle;
    crypto_klad_attr klad_attr = {
        .key_cfg = {
            .engine = CRYPTO_KLAD_ENGINE_AES,
            .decrypt_support = TD_TRUE,
            .encrypt_support = TD_TRUE
        },
        .key_sec_cfg = {
            .key_sec = TD_FALSE,
            .master_only_enable = TD_FALSE,
            .dest_buf_sec_support = TD_FALSE,
            .dest_buf_non_sec_support = TD_TRUE,
            .src_buf_sec_support = TD_FALSE,
            .src_buf_non_sec_support = TD_TRUE,
        }
    };
    crypto_klad_clear_key clear_key = {
        .key_length = key_len,
        .key = (unsigned char *)key
    };

    ret = drv_klad_create(&klad_handle);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_klad_create failed\n");

    ret = drv_klad_attach(klad_handle, CRYPTO_KLAD_DEST_MCIPHER, g_keyslot_handle);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_klad_destroy, "drv_klad_attach failed\n");

    ret = drv_klad_set_attr(klad_handle, &klad_attr);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_klad_destroy, "drv_klad_set_attr failed\n");

    ret = drv_klad_set_clear_key(klad_handle, &clear_key);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_klad_destroy, "drv_klad_set_clear_key failed\n");

exit_klad_destroy:
    drv_klad_destroy(klad_handle);
    return ret;
}

static int mbedlts_adapt_aes_crypto(crypto_symc_ctrl_t *symc_ctrl, const unsigned char *key, unsigned int key_len,
    const unsigned char *src, unsigned char *dst, unsigned int data_len, unsigned int is_decrypt)
{
    int ret;
    crypto_buf_attr src_buf;
    crypto_buf_attr dst_buf;

    ret = crypto_mutex_lock(&g_mbedtls_aes_lock);
    crypto_chk_return(ret != 0, ret, "crypto_mutex_lock failed\n");

    ret = inner_set_key(key, key_len);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_unlock, "drv_cipher_symc_set_key failed\n");

    ret = drv_cipher_symc_set_config(g_symc_handle, symc_ctrl);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_unlock, "drv_cipher_symc_set_config failed\n");

    src_buf.phys_addr = (uint32_t)(uintptr_t)src;
    dst_buf.phys_addr = (uint32_t)(uintptr_t)dst;
    if (is_decrypt == 0) {
        ret = drv_cipher_symc_encrypt(g_symc_handle, &src_buf, &dst_buf, data_len);
    } else {
        ret = drv_cipher_symc_decrypt(g_symc_handle, &src_buf, &dst_buf, data_len);
    }
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_unlock, "drv_cipher_symc_crypto failed\n");

    ret = drv_cipher_symc_get_config(g_symc_handle, symc_ctrl);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_unlock, "drv_cipher_symc_get_config failed\n");

exit_unlock:
    crypto_mutex_unlock(&g_mbedtls_aes_lock);
    return ret;
}

int mbedtls_alt_aes_encrypt_impl(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16])
{
    crypto_symc_ctrl_t symc_ctrl = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_ECB,
        .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT
    };
    symc_ctrl.symc_key_length = inner_get_key_length(key_len);
    return mbedlts_adapt_aes_crypto(&symc_ctrl, key, key_len, src, dst, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, 0);
}

int mbedtls_alt_aes_decrypt_impl(const unsigned char *key, unsigned int key_len,
    const unsigned char src[16], unsigned char dst[16])
{
    crypto_symc_ctrl_t symc_ctrl = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_ECB,
        .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT
    };
    symc_ctrl.symc_key_length = inner_get_key_length(key_len);
    return mbedlts_adapt_aes_crypto(&symc_ctrl, key, key_len, src, dst, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, 1);
}

int mbedtls_alt_aes_cbc_encrypt_impl(const unsigned char *key, unsigned key_len, unsigned char iv[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret;
    crypto_symc_ctrl_t symc_ctrl = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_CBC,
        .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT,
        .iv_length = CRYPTO_IV_LEN_IN_BYTES
    };
    (void)memcpy_s(symc_ctrl.iv, sizeof(symc_ctrl.iv), iv, sizeof(symc_ctrl.iv));

    symc_ctrl.symc_key_length = inner_get_key_length(key_len);
    ret = mbedlts_adapt_aes_crypto(&symc_ctrl, key, key_len, src, dst, data_len, 0);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "mbedlts_adapt_aes_crypto failed\n");

    (void)memcpy_s(iv, sizeof(symc_ctrl.iv), symc_ctrl.iv, sizeof(symc_ctrl.iv));
    return 0;
}

int mbedtls_alt_aes_cbc_decrypt_impl(const unsigned char *key, unsigned key_len, unsigned char iv[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret;
    crypto_symc_ctrl_t symc_ctrl = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_CBC,
        .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT,
        .iv_length = CRYPTO_IV_LEN_IN_BYTES
    };
    (void)memcpy_s(symc_ctrl.iv, sizeof(symc_ctrl.iv), iv, sizeof(symc_ctrl.iv));
    symc_ctrl.symc_key_length = inner_get_key_length(key_len);
    ret = mbedlts_adapt_aes_crypto(&symc_ctrl, key, key_len, src, dst, data_len, 1);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "mbedlts_adapt_aes_crypto failed\n");

    (void)memcpy_s(iv, sizeof(symc_ctrl.iv), symc_ctrl.iv, sizeof(symc_ctrl.iv));
    return 0;
}

int mbedtls_alt_aes_ofb_crypt_impl(const unsigned char *key, unsigned key_len, unsigned char *iv_off,
    unsigned char iv[16], const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret;
    unsigned int i;
    unsigned int processed_len = 0;
    unsigned int processing_len = 0;
    unsigned int left = data_len;
    crypto_symc_ctrl_t symc_ctrl = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_OFB,
        .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT,
        .iv_length = CRYPTO_IV_LEN_IN_BYTES
    };
    symc_ctrl.symc_key_length = inner_get_key_length(key_len);

    if (*iv_off != 0) {
        for (i = *iv_off; i < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && processed_len < data_len; i++) {
            dst[processed_len] = src[processed_len] ^ iv[i];
            processed_len++;
        }
    }
    left = data_len - processed_len;
    processing_len = left - left % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    if (processing_len != 0) {
        (void)memcpy_s(symc_ctrl.iv, sizeof(symc_ctrl.iv), iv, sizeof(symc_ctrl.iv));
        ret = mbedlts_adapt_aes_crypto(&symc_ctrl, key, key_len, src, dst, processing_len, 0);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "mbedlts_adapt_aes_crypto failed\n");
        left -= processing_len;
        processed_len += processing_len;
        (void)memcpy_s(iv, sizeof(symc_ctrl.iv), symc_ctrl.iv, sizeof(symc_ctrl.iv));
    }
    *iv_off = (unsigned char)left;
    ret = mbedtls_alt_aes_encrypt_impl(key, key_len, iv, iv);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "mbedlts_adapt_aes_crypto failed\n");
    for (i = 0; i < *iv_off; i++) {
        dst[processed_len] = src[processed_len] ^ iv[i];
        processed_len++;
    }
    return 0;
}

int mbedtls_alt_aes_ctr_crypt_impl(const unsigned char *key, unsigned key_len, unsigned char *nc_off,
    unsigned char nonce_counter[16], unsigned char stream_block[16],
    const unsigned char *src, unsigned char *dst, unsigned int data_len)
{
    int ret;
    unsigned int i;
    unsigned int processed_len = 0;
    unsigned int processing_len = 0;
    unsigned int left = data_len;
    crypto_symc_ctrl_t symc_ctrl = {
        .symc_alg = CRYPTO_SYMC_ALG_AES,
        .work_mode = CRYPTO_SYMC_WORK_MODE_CTR,
        .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT,
        .iv_length = CRYPTO_IV_LEN_IN_BYTES
    };
    symc_ctrl.symc_key_length = inner_get_key_length(key_len);

    if (*nc_off != 0) {
        for (i = *nc_off; i < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && processed_len < data_len; i++) {
            dst[processed_len] = src[processed_len] ^ stream_block[i];
            processed_len++;
        }
    }
    left = data_len - processed_len;
    processing_len = left - left % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    if (processing_len != 0) {
        (void)memcpy_s(symc_ctrl.iv, sizeof(symc_ctrl.iv), nonce_counter, sizeof(symc_ctrl.iv));
        ret = mbedlts_adapt_aes_crypto(&symc_ctrl, key, key_len, src, dst, processing_len, 0);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "mbedlts_adapt_aes_crypto failed\n");
        left -= processing_len;
        processed_len += processing_len;
        (void)memcpy_s(nonce_counter, sizeof(symc_ctrl.iv), symc_ctrl.iv, sizeof(symc_ctrl.iv));
    }
    *nc_off = (unsigned char)left;
    ret = mbedtls_alt_aes_encrypt_impl(key, key_len, nonce_counter, stream_block);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "mbedlts_adapt_aes_crypto failed\n");
    for (i = 0; i < *nc_off; i++) {
        dst[processed_len] = src[processed_len] ^ stream_block[i];
        processed_len++;
    }
    return 0;
}

void __attribute__((weak)) mbedtls_alt_aes_register(const mbedtls_alt_aes_harden_func *aes_func)
{
    crypto_unused(aes_func);
    return;
}

int mbedtls_alt_aes_init(void)
{
    crypto_symc_attr symc_attr = {0};
    mbedtls_alt_aes_harden_func aes_func = {
        .encrypt        = mbedtls_alt_aes_encrypt_impl,
        .decrypt        = mbedtls_alt_aes_decrypt_impl,
        .cbc_encrypt    = mbedtls_alt_aes_cbc_encrypt_impl,
        .cbc_decrypt    = mbedtls_alt_aes_cbc_decrypt_impl,
        .ofb_crypt      = mbedtls_alt_aes_ofb_crypt_impl,
        .ctr_crypt      = mbedtls_alt_aes_ctr_crypt_impl
    };
    (void)drv_cipher_symc_create(&g_symc_handle, &symc_attr);
    (void)drv_keyslot_create(&g_keyslot_handle, CRYPTO_KEYSLOT_TYPE_MCIPHER);
    (void)drv_cipher_symc_attach(g_symc_handle, g_keyslot_handle);
    mbedtls_alt_aes_register(&aes_func);
    (void)crypto_mutex_init(&g_mbedtls_aes_lock);

    return 0;
}

int mbedtls_alt_aes_deinit(void)
{
    mbedtls_alt_aes_harden_func aes_func = { 0 };
    (void)drv_cipher_symc_destroy(g_symc_handle);
    (void)drv_keyslot_destroy(g_keyslot_handle);
    mbedtls_alt_aes_register(&aes_func);
    (void)crypto_mutex_destroy(&g_mbedtls_aes_lock);

    return 0;
}