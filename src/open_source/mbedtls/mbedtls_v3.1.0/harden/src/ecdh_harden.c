/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "ecdh_harden.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "cipher_adapt.h"
#include "dfx.h"
#include "ecc_harden_common.h"

#if defined(MBEDTLS_ECDH_USE_HARDWARE)

int ecdh_harden( mbedtls_ecp_group *grp, mbedtls_mpi *z,
                 const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng )
{
    int ret;
    drv_pke_data priv_key;
    drv_pke_ecc_point pub_key;
    drv_pke_data shared_key;
    drv_pke_ecc_curve_type curve_type = DRV_PKE_ECC_TYPE_INVALID;
    unsigned int klen = 0;
    size_t byte_length;

    mbedtls_harden_log_func_enter();

    (void)f_rng;
    (void)p_rng;

    get_curve_type( grp->id, &curve_type, &klen);

    ret = ecp_pub_key_create( curve_type, klen, Q, &pub_key );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("ecp_pub_key_create failed!\n");
        return( ret );
    }

    ret = ecp_priv_key_create( curve_type, klen, d, &priv_key );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("ecp_priv_key_create failed!\n");
        goto cleanup_pub_key;
    }

    shared_key.length = klen;
    shared_key.data = mbedtls_calloc( 1, klen );
    if( shared_key.data == NULL )
    {
        mbedtls_harden_log_err("mbedtls_calloc failed!\n");
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup_priv_key;
    }

    ret = CIPHER_PKE_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_INIT failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup_shared_key;
    }

    ret = CIPHER_PKE_ECC_GEN_ECDH_KEY( curve_type, &pub_key, &priv_key, &shared_key);
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_ECC_GEN_ECDH_KEY failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    byte_length = grp->pbits / 8 + ( ( grp->pbits % 8 ) != 0 );
    if ( grp->id == MBEDTLS_ECP_DP_CURVE25519 )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary_le( z, shared_key.data, byte_length ) );
    /* 
        The alignment length of 521 bits is 68 bytes in the chip and 66 bytes in the mbedtls.
        Therefore, 0s of the first 2 bytes need to be removed.
    */
    else if (grp->id == MBEDTLS_ECP_DP_SECP521R1 )
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( z, shared_key.data + 2, byte_length ) );
    else
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( z, shared_key.data, byte_length ) );

cleanup:
    CIPHER_PKE_DEINIT();
cleanup_shared_key:
    if( shared_key.data != NULL )
    {
        mbedtls_platform_zeroize( shared_key.data, shared_key.length);
        mbedtls_free( shared_key.data );
    }
cleanup_priv_key:
    ecp_priv_key_free( &priv_key );
cleanup_pub_key:
    ecp_pub_key_free( &pub_key );
    mbedtls_harden_log_func_exit();

    return( ret );
}

#endif /* MBEDTLS_ECDH_USE_HARDWARE */