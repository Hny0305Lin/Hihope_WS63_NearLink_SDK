/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden alg alt internal source file.
 *
 * Create: 2023-05-10
*/
#ifndef CCM_ALT_H
#define CCM_ALT_H
/**
 * \brief    The CCM context-type alt definition. The CCM context is passed
 *           to the APIs called.
 */
typedef struct mbedtls_ccm_context
{
    mbedtls_cipher_context_t MBEDTLS_PRIVATE(cipher_ctx);    /*!< The cipher context used. */
}
mbedtls_ccm_context;

#endif