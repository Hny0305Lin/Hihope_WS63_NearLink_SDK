/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/
#ifndef MBEDTLS_ECC_COMMON_H
#define MBEDTLS_ECC_COMMON_H

#include "cipher_adapt.h"
#include "mbedtls/ecp.h"

void get_curve_type( mbedtls_ecp_group_id grp_id, drv_pke_ecc_curve_type *curve_type, unsigned int *klen );

int check_ecc_harden_can_do( mbedtls_ecp_group *grp, int (*f_rng)(void *, unsigned char *, size_t) );

int ecp_priv_key_alloc( const unsigned int klen, drv_pke_data *priv_key );

void ecp_priv_key_free( drv_pke_data *priv_key );

int ecp_pub_key_alloc( const unsigned int klen, drv_pke_ecc_point *pub_key );

void ecp_pub_key_free( drv_pke_ecc_point *pub_key );

int ecp_priv_key_create( drv_pke_ecc_curve_type ecc_type, unsigned int klen, const mbedtls_mpi *d,
                         drv_pke_data *priv_key );

int ecp_pub_key_create( drv_pke_ecc_curve_type ecc_type, unsigned int klen, const mbedtls_ecp_point *pub,
                        drv_pke_ecc_point *pub_key );

#endif /* MBEDTLS_ECC_COMMON_H */