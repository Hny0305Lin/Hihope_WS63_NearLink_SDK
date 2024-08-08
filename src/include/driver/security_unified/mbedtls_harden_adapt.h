/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: The header file of the functions, which will be registered to mbedtls.
 *
 * Create: 2023-07-12
*/

#ifndef CRYPTO_MBEDTLS_HARDEN_ADAPT_H
#define CRYPTO_MBEDTLS_HARDEN_ADAPT_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup security_unified_mbedtls_harden_adapt mbedtls_harden_adapt
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief  mbedtls harden adapt functions register, called by system mbedtls harden.
 * @else
 * @brief  security_unified 模块对接第三方 mbedtls 接口需要注册的函数，若开启第三方对接宏 MBEDTLS_HARDEN_OPEN，
 * 该接口由对接适配层调用。
 * @endif
 */
int32_t mbedtls_adapt_register_func(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CRYPTO_MBEDTLS_HARDEN_ADAPT_H */