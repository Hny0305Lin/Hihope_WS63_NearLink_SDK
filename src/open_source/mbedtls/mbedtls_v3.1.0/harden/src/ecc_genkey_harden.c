/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "ecc_genkey_harden.h"
#include "cipher_adapt.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "ecc_harden_common.h"
#include "dfx.h"

#if defined(MBEDTLS_ECC_GENKEY_USE_HARDWARE)
int ecc_genkey_harden( mbedtls_ecp_group *grp,
        const mbedtls_ecp_point *G,
        mbedtls_mpi *d, mbedtls_ecp_point *Q,
        int (*f_rng)(void *, unsigned char *, size_t),
        void *p_rng )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    drv_pke_ecc_point pub_key;
    drv_pke_data priv_key;
    drv_pke_ecc_curve_type curve_type = DRV_PKE_ECC_TYPE_INVALID;
    unsigned int klen = 0;

    mbedtls_harden_log_func_enter();

    (void)G;
    (void)f_rng;
    (void)p_rng;
    get_curve_type( grp->id, &curve_type, &klen );

    ret = ecp_pub_key_alloc( klen, &pub_key );
    if( ret != 0)
    {
        mbedtls_harden_log_err("ecp_pub_key_alloc failed! ret = 0x%x!\n", ret);
        return( ret );
    }

    ret = ecp_priv_key_alloc( klen, &priv_key );
    if( ret != 0)
    {
        mbedtls_harden_log_err("ecp_priv_key_alloc failed! ret = 0x%x!\n", ret);
        goto cleanup_pub_key;
    }

    ret = CIPHER_PKE_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_INIT failed! ret = 0x%x!\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup_priv_key;
    }

    ret = CIPHER_PKE_ECC_GEN_KEY( curve_type, NULL, &priv_key, &pub_key );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_ECC_GEN_KEY failed! ret = 0x%x!\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( d, priv_key.data, klen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &Q->MBEDTLS_PRIVATE(X), pub_key.x, klen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &Q->MBEDTLS_PRIVATE(Y), pub_key.y, klen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &Q->MBEDTLS_PRIVATE(Z), 1 ) );
cleanup:
    CIPHER_PKE_DEINIT();
cleanup_priv_key:
    ecp_priv_key_free( &priv_key );
cleanup_pub_key:
    ecp_pub_key_free( &pub_key );

    mbedtls_harden_log_func_exit();

    return( ret );
}
#endif /* MBEDTLS_ECC_GENKEY_USE_HARDWARE */