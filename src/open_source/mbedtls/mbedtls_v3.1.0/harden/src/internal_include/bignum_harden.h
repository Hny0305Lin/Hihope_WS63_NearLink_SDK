/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_BIGNUM_HARDEN_H
#define MBEDTLS_BIGNUM_HARDEN_H

#include "mbedtls/bignum.h"

int check_exp_mod_harden_can_do( const mbedtls_mpi *A,
        const mbedtls_mpi *E, const mbedtls_mpi *N );

int exp_mod_harden( mbedtls_mpi *X, const mbedtls_mpi *A,
        const mbedtls_mpi *E, const mbedtls_mpi *N);

int check_mod_harden_can_do( const mbedtls_mpi *A, const mbedtls_mpi *B );

int mod_harden( mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B );

#endif /* MBEDTLS_BIGNUM_HARDEN_H */