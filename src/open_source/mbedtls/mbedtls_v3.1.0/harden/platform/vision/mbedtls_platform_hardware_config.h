/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_PLATFORM_HARDWARE_CONFIG_H
#define MBEDTLS_PLATFORM_HARDWARE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MBEDTLS_HARDEN_SUCCESS           TD_SUCCESS
#define MBEDTLS_HARDEN_TRUE              TD_TRUE
#define MBEDTLS_HARDEN_FALSE             TD_FALSE

#define MAX_AES_HARD_CTX_NUM  4

#define MAX_AES_XTS_TWEAK_SIZE   256

#define MAX_GCM_HARD_CTX_NUM   2

#define MBEDTLS_CIPHER_MAX_ADD_LEN   128

#define MAX_CCM_HARD_CTX_NUM   2

#define MAX_CMAC_HARD_CTX_NUM   4

#define HARDWARE_521_ALIGN_576_LEN

#define MAX_HASH_HARD_CTX_NUM   4

/*******************************************alternative macro*********************************************/

#define MBEDTLS_ENTROPY_HARDWARE_ALT

// #define MBEDTLS_AES_ALT

// #define MBEDTLS_CMAC_ALT

#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT

#define MBEDTLS_ECDSA_VERIFY_ALT

#define MBEDTLS_ECDSA_SIGN_ALT

#define MBEDTLS_ECC_GEN_KEY_ALT

#define MBEDTLS_ECDSA_GENKEY_ALT

#define MBEDTLS_SHA256_ALT

#define MBEDTLS_SHA512_ALT

#define MBEDTLS_PBKDF2_HMAC_ALT

/*******************************************alternative macro*********************************************/

/*******************************************algorithm cfg macro*******************************************/

#define MBEDTLS_BP256R1_USE_HARDWARE
#define MBEDTLS_BP384R1_USE_HARDWARE
#define MBEDTLS_BP512R1_USE_HARDWARE
#define MBEDTLS_CURVE25519_USE_HARDWARE

#define MBEDTLS_SHA256_USE_HARDWARE
#define MBEDTLS_SHA384_USE_HARDWARE
#define MBEDTLS_SHA512_USE_HARDWARE

#define MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE
#define MBEDTLS_BIGNUM_MOD_USE_HARDWARE
#define MBEDTLS_NO_PLATFORM_ENTROPY
/*******************************************algorithm cfg macro*******************************************/

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PLATFORM_USER_CONFIG_H */