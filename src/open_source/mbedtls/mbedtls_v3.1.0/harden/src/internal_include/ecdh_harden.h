/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_ECDH_HARDEN_H
#define MBEDTLS_ECDH_HARDEN_H

#include "mbedtls/ecdh.h"

int ecdh_harden( mbedtls_ecp_group *grp, mbedtls_mpi *z,
                 const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng );

#endif /* MBEDTLS_ECDH_HARDEN_H */