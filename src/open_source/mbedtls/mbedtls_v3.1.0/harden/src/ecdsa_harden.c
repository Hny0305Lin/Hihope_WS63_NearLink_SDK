/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "ecdsa_harden.h"
#include "cipher_adapt.h"
#include "mbedtls/error.h"
#include "dfx.h"
#include "ecc_harden_common.h"
#include "mbedtls/platform_util.h"

static int ecdsa_sig_alloc( const unsigned int klen, drv_pke_ecc_sig *sig );
static void ecdsa_sig_free( drv_pke_ecc_sig *sig );

static int ecdsa_sig_alloc( const unsigned int klen, drv_pke_ecc_sig *sig )
{
    int ret;

    if( sig == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    sig->length = klen;
    sig->r = mbedtls_calloc( 1, klen );
    sig->s = mbedtls_calloc( 1, klen );
    if( sig->r == NULL || sig->s == NULL )
    {
        ret = MBEDTLS_ERR_ECP_ALLOC_FAILED;
        goto cleanup;
    }

    return( 0 );

cleanup:
    ecdsa_sig_free(sig);
    return( ret );
}

static void ecdsa_sig_free( drv_pke_ecc_sig *sig )
{
    if( sig == NULL)
        return;

    if( sig->r != NULL )
    {
        mbedtls_platform_zeroize( sig->r, sig->length );
        mbedtls_free( sig->r );
        sig->r = NULL;
    }
    if( sig->s != NULL )
    {
        mbedtls_platform_zeroize( sig->s, sig->length );
        mbedtls_free( sig->s );
        sig->s = NULL;
    }
    sig->length = 0;
}

#if defined(MBEDTLS_ECDSA_SIGN_USE_HARDWARE)

int check_ecdsa_sign_harden_can_do( mbedtls_ecp_group *grp,
    int (*f_rng)(void *, unsigned char *, size_t), size_t blen )
{
    if( check_ecc_harden_can_do( grp, f_rng ) != 1 || blen < 20 || blen > 64 )
        return( 0 );

    switch( grp->id )
    {
#ifdef MBEDTLS_ECP_DP_CURVE25519_ENABLED
        case MBEDTLS_ECP_DP_CURVE25519:
            return 0;
#endif
#ifdef MBEDTLS_ECP_DP_CURVE448_ENABLED
        case MBEDTLS_ECP_DP_CURVE448:
            return 0;
#endif
        default:
            return 1;
    }
}

int ecdsa_sign_harden( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret;
    drv_pke_ecc_sig sig;
    drv_pke_data priv_key;
    drv_pke_ecc_curve_type curve_type = DRV_PKE_ECC_TYPE_INVALID;
    unsigned int klen = 0;
    drv_pke_data input = {0};

    mbedtls_harden_log_func_enter();

    (void)f_rng;
    (void)p_rng;

    get_curve_type( grp->id, &curve_type, &klen);

    ret = ecp_priv_key_create( curve_type, klen, d, &priv_key );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("ecp_priv_key_create failed!\n");
        return( ret );
    }

    ret = ecdsa_sig_alloc( klen, &sig );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("ecdsa_sig_alloc failed!\n");
        goto cleanup_priv_key;
    }

    input.data = (unsigned char *) buf;
    input.length = blen;

    ret = CIPHER_PKE_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_INIT failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup_sig;
    }
    ret = CIPHER_PKE_ECDSA_SIGN( curve_type, &priv_key, &input, &sig );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_ECDSA_SIGN failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( r, sig.r, klen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( s, sig.s, klen ) );

cleanup:
    CIPHER_PKE_DEINIT();
cleanup_sig:
    ecdsa_sig_free( &sig );
cleanup_priv_key:
    ecp_priv_key_free( &priv_key );

    mbedtls_harden_log_func_exit();
    return( ret );
}

#endif /* MBEDTLS_ECDSA_SIGN_USE_HARDWARE */

#if defined(MBEDTLS_ECDSA_VERIFY_USE_HARDWARE)
static int check_hash_zero( const unsigned char *buf, size_t blen )
{
    int ret = 1;
    size_t i = 0;
    for( i = 0; i < blen; i++ )
    {
        if( buf[i] != 0 )
        {
            ret = 0;
            break;
        }
    }
    return ret;
}

int check_ecdsa_verify_harden_can_do( mbedtls_ecp_group *grp, const unsigned char *buf, size_t blen )
{
    if( check_ecc_harden_can_do( grp, NULL ) != 1 || blen < 20 || blen > 64 ||
        check_hash_zero( buf, blen ) == 1 )
        return( 0 );

    switch( grp->id )
    {
#ifdef MBEDTLS_ECP_DP_CURVE25519_ENABLED
        case MBEDTLS_ECP_DP_CURVE25519:
            return 0;
#endif
#ifdef MBEDTLS_ECP_DP_CURVE448_ENABLED
        case MBEDTLS_ECP_DP_CURVE448:
            return 0;
#endif
        default:
            return 1;
    }
}

int ecdsa_verify_harden( mbedtls_ecp_group *grp,
        const unsigned char *buf, size_t blen,
        const mbedtls_ecp_point *Q,
        const mbedtls_mpi *r,
        const mbedtls_mpi *s)
{
    int ret;
    drv_pke_ecc_sig sig;
    drv_pke_ecc_point pub_key;
    drv_pke_data input;
    drv_pke_ecc_curve_type curve_type = DRV_PKE_ECC_TYPE_INVALID;
    unsigned int klen = 0;

    mbedtls_harden_log_func_enter();

    get_curve_type( grp->id, &curve_type, &klen);

    ret = ecp_pub_key_create( curve_type, klen, Q, &pub_key );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("ecp_pub_key_create failed!\n");
        return( ret );
    }

    ret = ecdsa_sig_alloc( klen, &sig );
    if( ret != 0 )
    {
        mbedtls_harden_log_err("ecdsa_sig_alloc failed!\n");
        goto cleanup_pub_key;
    }

    ret = CIPHER_PKE_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_INIT failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup_sig;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( r, sig.r, klen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( s, sig.s, klen ) );

    input.data = (unsigned char *) buf;
    input.length = blen;

    ret = CIPHER_PKE_ECDSA_VERIFY( curve_type, &pub_key, &input, &sig );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_ECDSA_VERIFY failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    ret = 0;

cleanup:
    CIPHER_PKE_DEINIT();
cleanup_sig:
    ecdsa_sig_free( &sig );
cleanup_pub_key:
    ecp_pub_key_free( &pub_key );

    mbedtls_harden_log_func_exit();
    return( ret );
}
#endif /* MBEDTLS_ECDSA_VERIFY_USE_HARDWARE */