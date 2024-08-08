/*
 *  Elliptic curve DSA. And adapt to the ecc harden API.
 *
 *  Copyright The Mbed TLS Contributors
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * References:
 *
 * SEC1 http://www.secg.org/index.php?action=secg,docs_secg
 */

#include "common.h"
#include "ecc_genkey_harden.h"
#include "ecdh_harden.h"
#include "ecdsa_harden.h"
#include "cipher_adapt.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "ecc_harden_common.h"
#include "dfx.h"

#define ECP_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA )
#define ECP_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )
#define ECDH_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA )
#define ECDH_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )

#define ECDSA_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA )
#define ECDSA_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )


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

#if defined(MBEDTLS_ECC_GEN_KEY_ALT)
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

int mbedtls_ecp_gen_keypair_base( mbedtls_ecp_group *grp,
                     const mbedtls_ecp_point *G,
                     mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    ECP_VALIDATE_RET( grp   != NULL );
    ECP_VALIDATE_RET( d     != NULL );
    ECP_VALIDATE_RET( G     != NULL );
    ECP_VALIDATE_RET( Q     != NULL );

    if( check_ecc_harden_can_do( grp, f_rng ) == 1 )
    {
        return ecc_genkey_harden( grp, G, d, Q, f_rng, p_rng );
    }

    return -1;
}

int mbedtls_ecp_gen_keypair( mbedtls_ecp_group *grp,
                             mbedtls_mpi *d, mbedtls_ecp_point *Q,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng )
{
    ECP_VALIDATE_RET( grp   != NULL );
    ECP_VALIDATE_RET( d     != NULL );
    ECP_VALIDATE_RET( Q     != NULL );

    return( mbedtls_ecp_gen_keypair_base( grp, &grp->G, d, Q, f_rng, p_rng ) );
}

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
int mbedtls_ecdsa_genkey( mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret = 0;
    ECDSA_VALIDATE_RET( ctx   != NULL );

    ret = mbedtls_ecp_group_load( &ctx->grp, gid );
    if( ret != 0 )
        return( ret );

   return( mbedtls_ecp_gen_keypair( &ctx->grp, &ctx->d,
                                    &ctx->Q, f_rng, p_rng ) );
}
#endif /* MBEDTLS_ECDSA_GENKEY_ALT */
#endif /* MBEDTLS_ECC_GEN_KEY_ALT */

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
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

int mbedtls_ecdh_compute_shared( mbedtls_ecp_group *grp, mbedtls_mpi *z,
                         const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng )
{
    ECDH_VALIDATE_RET( grp != NULL );
    ECDH_VALIDATE_RET( Q != NULL );
    ECDH_VALIDATE_RET( d != NULL );
    ECDH_VALIDATE_RET( z != NULL );

    if( check_ecc_harden_can_do( grp, NULL ) == 1 )
    {
        return ecdh_harden( grp, z, Q, d, f_rng, p_rng );
    }
    return -1;
}
#endif /* MBEDTLS_ECDH_COMPUTE_SHARED_ALT */

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
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

int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    ECDSA_VALIDATE_RET( grp   != NULL );
    ECDSA_VALIDATE_RET( r     != NULL );
    ECDSA_VALIDATE_RET( s     != NULL );
    ECDSA_VALIDATE_RET( d     != NULL );
    ECDSA_VALIDATE_RET( buf   != NULL || blen == 0 );

    if( check_ecdsa_sign_harden_can_do( grp, f_rng, blen ) == 1 )
    {
        return ecdsa_sign_harden( grp, r, s, d, buf, blen, f_rng, p_rng );
    }
    return( -1 );
}
#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
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

int mbedtls_ecdsa_verify( mbedtls_ecp_group *grp,
                          const unsigned char *buf, size_t blen,
                          const mbedtls_ecp_point *Q,
                          const mbedtls_mpi *r,
                          const mbedtls_mpi *s)
{
    ECDSA_VALIDATE_RET( grp != NULL );
    ECDSA_VALIDATE_RET( Q   != NULL );
    ECDSA_VALIDATE_RET( r   != NULL );
    ECDSA_VALIDATE_RET( s   != NULL );
    ECDSA_VALIDATE_RET( buf != NULL || blen == 0 );

    if( check_ecdsa_verify_harden_can_do( grp, buf, blen ) == 1 )
    {
        return ecdsa_verify_harden( grp, buf, blen, Q, r, s );
    }
    return( -1 );
}
#endif /* MBEDTLS_ECDSA_VERIFY_ALT */