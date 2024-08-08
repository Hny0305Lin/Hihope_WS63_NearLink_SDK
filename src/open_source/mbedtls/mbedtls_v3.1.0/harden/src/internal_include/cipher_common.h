/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_CIPHER_COMMON_STRUCT_H
#define MBEDTLS_CIPHER_COMMON_STRUCT_H

#include "cipher_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Set symc_ctrl
 */
int mbedtls_cipher_set_ctrl(crypto_symc_ctrl_t *symc_ctrl, const crypto_symc_alg symc_alg,
    const crypto_symc_work_mode work_mode, const unsigned int key_len, const crypto_symc_bit_width bit_width,
    const crypto_symc_iv_change_type iv_change_flag, const unsigned char *iv, const unsigned int iv_len);

/*
 * Set CCM & GCM configuration
 */
int mbedtls_cipher_set_gcm_ccm_config(crypto_symc_config_aes_ccm_gcm *gcm_config, const unsigned char *aad,
    const unsigned int aad_len, const unsigned int data_len, const unsigned int tag_len);

/*
 * Direction of synchronize hard_ctx with soft_ctx
 */
typedef enum {
    SOFT_TO_HARD = 0,
    HARD_TO_SOFT,
} sync_direction;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CIPHER_COMMON_STRUCT_H */