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

#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PLATFORM_HARDWARE_CONFIG_H */