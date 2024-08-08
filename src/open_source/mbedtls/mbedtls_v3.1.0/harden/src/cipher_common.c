/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "cipher_common.h"
#include "aes_alt.h"
#include "securec.h"

#if defined(MBEDTLS_AES_ALT) || defined(MBEDTLS_CCM_ALT) || defined(MBEDTLS_GCM_ALT)
/*
 * Set symc_ctrl
 */
typedef struct key_len_pair {
    unsigned int key_len;
    crypto_symc_key_length key_len_input;
} key_len_pair;

#define KEY_LEN_MAP_SIZE    3

key_len_pair key_len_map[] = {
    {.key_len = 16, .key_len_input = CRYPTO_SYMC_KEY_128BIT},
    {.key_len = 24, .key_len_input = CRYPTO_SYMC_KEY_192BIT},
    {.key_len = 32, .key_len_input = CRYPTO_SYMC_KEY_256BIT},
};

int mbedtls_cipher_set_ctrl(crypto_symc_ctrl_t *symc_ctrl, const crypto_symc_alg symc_alg,
    const crypto_symc_work_mode work_mode, const unsigned int key_len, const crypto_symc_bit_width bit_width,
    const crypto_symc_iv_change_type iv_change_flag, const unsigned char *iv, const unsigned int iv_len)
{
    int i = 0;
    symc_ctrl->symc_alg = symc_alg;
    symc_ctrl->work_mode = work_mode;
    symc_ctrl->symc_bit_width = bit_width;
    symc_ctrl->iv_change_flag = iv_change_flag;
    for (i = 0; i < KEY_LEN_MAP_SIZE; ++i) {
        if (key_len_map[i].key_len == key_len) {
            symc_ctrl->symc_key_length = key_len_map[i].key_len_input;
        }
    }
    (void)memcpy_s(symc_ctrl->iv, sizeof(symc_ctrl->iv), iv, iv_len);
    symc_ctrl->iv_length = iv_len;
    return 0;
}

int mbedtls_cipher_set_gcm_ccm_config(crypto_symc_config_aes_ccm_gcm *gcm_config, const unsigned char *aad,
    const unsigned int aad_len, const unsigned int data_len, const unsigned int tag_len)
{
    gcm_config->aad_buf.virt_addr = (void *)aad;
    gcm_config->aad_len = aad_len;
    gcm_config->data_len = data_len;
    gcm_config->tag_len = tag_len;
    return 0;
}
#endif