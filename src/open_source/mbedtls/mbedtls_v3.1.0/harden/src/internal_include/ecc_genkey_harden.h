/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_ECC_GENKEY_HARDEN_H
#define MBEDTLS_ECC_GENKEY_HARDEN_H

#include "mbedtls/ecp.h"

int ecc_genkey_harden( mbedtls_ecp_group *grp, const mbedtls_ecp_point *G,
        mbedtls_mpi *d, mbedtls_ecp_point *Q, int (*f_rng)(void *, unsigned char *, size_t),
        void *p_rng );

#endif /* MBEDTLS_ECC_GENKEY_HARDEN_H */