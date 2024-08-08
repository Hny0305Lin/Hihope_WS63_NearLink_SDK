/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls ecp harden api.
 *
 * Create: 2024-3-13
*/

#ifndef ECP_HARDEN_IMPL_H
#define ECP_HARDEN_IMPL_H

#include "mbedtls_harden_adapt_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int mbedtls_alt_ecp_mul_impl(mbedtls_alt_ecp_curve_type curve_type, const mbedtls_alt_ecp_data *k,
    const mbedtls_alt_ecp_point *p, const mbedtls_alt_ecp_point *r);

int mbedtls_alt_ecdsa_verify_impl(mbedtls_alt_ecp_curve_type curve_type,
    const unsigned char *hash, unsigned int hash_len,
    const unsigned char *pub_x, const unsigned char *pub_y,
    const unsigned char *sig_r, const unsigned char *sig_s, unsigned klen);

int mbedtls_alt_ecp_init(void);

int mbedtls_alt_ecp_deinit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif