/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "ecc_harden_common.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "common.h"
#include "mbedtls_platform_hardware_config.h"

typedef struct {
    mbedtls_ecp_group_id group_id;
    drv_pke_ecc_curve_type curve_type;
    unsigned int klen;
} curve_info;

static const curve_info g_curve_info_table[] = {
    { MBEDTLS_ECP_DP_NONE,       DRV_PKE_ECC_TYPE_INVALID,      0               },
#if defined(MBEDTLS_SECP192R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_SECP192R1,  DRV_PKE_ECC_TYPE_FIPS_P192R,   DRV_PKE_LEN_192 },
#endif
#if defined(MBEDTLS_SECP224R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_SECP224R1,  DRV_PKE_ECC_TYPE_FIPS_P224R,   DRV_PKE_LEN_224 },
#endif
#if defined(MBEDTLS_SECP256R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_SECP256R1,  DRV_PKE_ECC_TYPE_FIPS_P256R,   DRV_PKE_LEN_256 },
#endif
#if defined(MBEDTLS_SECP384R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_SECP384R1,  DRV_PKE_ECC_TYPE_FIPS_P384R,   DRV_PKE_LEN_384 },
#endif
#if defined(MBEDTLS_SECP521R1_USE_HARDWARE)
#if defined(HARDWARE_521_ALIGN_576_LEN)
    { MBEDTLS_ECP_DP_SECP521R1,  DRV_PKE_ECC_TYPE_FIPS_P521R,   DRV_PKE_LEN_576 },
#else
    { MBEDTLS_ECP_DP_SECP521R1,  DRV_PKE_ECC_TYPE_FIPS_P521R,   DRV_PKE_LEN_521 },
#endif
#endif
#if defined(MBEDTLS_BP256R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_BP256R1,    DRV_PKE_ECC_TYPE_RFC5639_P256, DRV_PKE_LEN_256 },
#endif
#if defined(MBEDTLS_BP384R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_BP384R1,    DRV_PKE_ECC_TYPE_RFC5639_P384, DRV_PKE_LEN_384 },
#endif
#if defined(MBEDTLS_BP512R1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_BP512R1,    DRV_PKE_ECC_TYPE_RFC5639_P512, DRV_PKE_LEN_512 },
#endif
#if defined(MBEDTLS_CURVE25519_USE_HARDWARE)
    { MBEDTLS_ECP_DP_CURVE25519, DRV_PKE_ECC_TYPE_RFC7748,      DRV_PKE_LEN_256 },
#endif
#if defined(MBEDTLS_SECP256K1_USE_HARDWARE)
    { MBEDTLS_ECP_DP_SECP256K1,  DRV_PKE_ECC_TYPE_FIPS_P256K,   DRV_PKE_LEN_256 },
#endif
#if defined(MBEDTLS_CURVE448_USE_HARDWARE)
    { MBEDTLS_ECP_DP_CURVE448,   DRV_PKE_ECC_TYPE_RFC7748_448,  DRV_PKE_LEN_448 },
#endif
};

void get_curve_type( mbedtls_ecp_group_id grp_id, drv_pke_ecc_curve_type *curve_type, unsigned int *klen )
{
    unsigned int i = 0;
    for (i = 0; i < sizeof(g_curve_info_table) / sizeof(g_curve_info_table[0]); i++) {
        if (g_curve_info_table[i].group_id == grp_id) {
            *curve_type = g_curve_info_table[i].curve_type;
            *klen = g_curve_info_table[i].klen;
            return;
        }
    }
    *curve_type = DRV_PKE_ECC_TYPE_INVALID;
    *klen = 0;
}

int check_ecc_harden_can_do( mbedtls_ecp_group *grp, int (*f_rng)(void *, unsigned char *, size_t) )
{
    drv_pke_ecc_curve_type curve_type = DRV_PKE_ECC_TYPE_INVALID;
    unsigned int klen = 0;

    if( IS_PKE_ECC_FUNC_REGISTERED() != TD_TRUE || IS_HASH_FUNC_REGISTERED() != TD_TRUE )
        return ( 0 );

    (void)f_rng;

    get_curve_type( grp->id, &curve_type, &klen );
    return curve_type != DRV_PKE_ECC_TYPE_INVALID;
}

int ecp_priv_key_alloc( const unsigned int klen, drv_pke_data *priv_key )
{
    int ret;

    if( priv_key == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    priv_key->length = klen;
    priv_key->data = mbedtls_calloc( 1, klen );
    if( priv_key->data == NULL )
    {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    return( 0 );

cleanup:
    ecp_priv_key_free(priv_key);
    return( ret );
}

void ecp_priv_key_free( drv_pke_data *priv_key )
{
    if( priv_key == NULL)
        return;

    if( priv_key->data != NULL )
    {
        mbedtls_platform_zeroize( priv_key->data, priv_key->length );
        mbedtls_free( priv_key->data );
        priv_key->data = NULL;
    }
    priv_key->length = 0;
}

int ecp_pub_key_alloc( const unsigned int klen, drv_pke_ecc_point *pub_key )
{
    int ret;

    if( pub_key == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    pub_key->length = klen;
    pub_key->x = mbedtls_calloc( 1, klen );
    pub_key->y = mbedtls_calloc( 1, klen );
    if( pub_key->x == NULL || pub_key->y == NULL )
    {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    return( 0 );

cleanup:
    ecp_pub_key_free(pub_key);
    return( ret );
}

void ecp_pub_key_free( drv_pke_ecc_point *pub_key )
{
    if( pub_key == NULL)
        return;

    if( pub_key->x != NULL )
    {
        mbedtls_platform_zeroize( pub_key->x, pub_key->length );
        mbedtls_free( pub_key->x );
        pub_key->x = NULL;
    }
    if( pub_key->y != NULL )
    {
        mbedtls_platform_zeroize( pub_key->y, pub_key->length );
        mbedtls_free( pub_key->y );
        pub_key->y = NULL;
    }
    pub_key->length = 0;
}

int ecp_priv_key_create( drv_pke_ecc_curve_type ecc_type, unsigned int klen, const mbedtls_mpi *d,
                                  drv_pke_data *priv_key )
{
    int ret;

    if( d == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    ret = ecp_priv_key_alloc( klen, priv_key );
    if( ret != 0 )
        return( ret );

    if( ecc_type == DRV_PKE_ECC_TYPE_RFC7748 )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary_le( d, priv_key->data, klen ) );
    }
    else
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( d, priv_key->data, klen ) );
    }

    return( 0 );

cleanup:
    ecp_priv_key_free(priv_key);
    return( ret );
}

int ecp_pub_key_create( drv_pke_ecc_curve_type ecc_type, unsigned int klen, const mbedtls_ecp_point *pub,
                        drv_pke_ecc_point *pub_key )
{
    int ret;

    if( pub == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    ret = ecp_pub_key_alloc( klen, pub_key );
    if( ret != 0 )
        return( ret );

    if( ecc_type == DRV_PKE_ECC_TYPE_RFC7748 )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary_le( &pub->MBEDTLS_PRIVATE(X), pub_key->x, klen ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary_le( &pub->MBEDTLS_PRIVATE(Y), pub_key->y, klen ) );
    }
    else
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &pub->MBEDTLS_PRIVATE(X), pub_key->x, klen ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &pub->MBEDTLS_PRIVATE(Y), pub_key->y, klen ) );
    }

    return( 0 );

cleanup:
    ecp_pub_key_free( pub_key );
    return( ret );
}