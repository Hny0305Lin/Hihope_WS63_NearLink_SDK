/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_ECDSA_HARDEN_H
#define MBEDTLS_ECDSA_HARDEN_H

#include "mbedtls/ecdsa.h"

int check_ecdsa_sign_harden_can_do( mbedtls_ecp_group *grp,
    int (*f_rng)(void *, unsigned char *, size_t), size_t blen );

int ecdsa_sign_harden( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

int check_ecdsa_verify_harden_can_do( mbedtls_ecp_group *grp, const unsigned char *buf, size_t blen );

int ecdsa_verify_harden( mbedtls_ecp_group *grp,
        const unsigned char *buf, size_t blen,
        const mbedtls_ecp_point *Q,
        const mbedtls_mpi *r,
        const mbedtls_mpi *s);

#endif /* MBEDTLS_ECDSA_HARDEN_H */