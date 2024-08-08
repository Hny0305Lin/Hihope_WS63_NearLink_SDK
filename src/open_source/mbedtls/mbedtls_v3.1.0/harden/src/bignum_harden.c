/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal source file.
 *
 * Create: 2023-05-10
*/

#include "bignum_harden.h"
#include "cipher_adapt.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/platform.h"
#include "dfx.h"

#define max(a, b)                   (a) > (b) ? (a) : (b)
#define min(a, b)                   (a) < (b) ? (a) : (b)
#define max_three(a, b, c)          max( max( a, b ), c )
#define min_three(a, b, c)          min( min( a, b ), c )

#if defined(MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE) || defined(MBEDTLS_BIGNUM_MOD_USE_HARDWARE)
static void get_size_align( size_t size_a, size_t size_b, size_t size_c, size_t *size_align )
{
    size_t sizes[] = {
#if defined(DRV_PKE_LEN_1024_SUPPORT)
        DRV_PKE_LEN_1024,
#endif
        DRV_PKE_LEN_2048, DRV_PKE_LEN_3072, DRV_PKE_LEN_4096 };
    int sizes_count = sizeof( sizes ) / sizeof( sizes[0] );
    *size_align = max_three( size_a, size_b, size_c );

    for( int i = 0; i < sizes_count - 1; i++ )
    {
        if ( *size_align <= sizes[i] )
        {
            *size_align = sizes[i];
            return;
        }
    }
    *size_align = sizes[sizes_count - 1];
}
#endif /* MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE || MBEDTLS_BIGNUM_MOD_USE_HARDWARE */

#if defined(MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE)
int check_exp_mod_harden_can_do( const mbedtls_mpi *A,
        const mbedtls_mpi *E, const mbedtls_mpi *N )
{
    size_t size_a, size_e, size_n, size_max;
    size_a = mbedtls_mpi_size( A );
    size_e = mbedtls_mpi_size( E );
    size_n = mbedtls_mpi_size( N );
    size_max = max_three( size_a, size_e, size_n );

    if( IS_PKE_EXP_MOD_FUNC_REGISTERED() != TD_TRUE )
        return ( 0 );
    if (size_max > DRV_PKE_LEN_4096)
        return( 0 );

#if defined(DRV_PKE_LEN_1024_SUPPORT)
    get_size_align( size_a, size_e, size_n, &size_max );
    return( ( size_max - size_n ) <= 2 );
#else
    return( 1 );
#endif
}

int exp_mod_harden( mbedtls_mpi *X, const mbedtls_mpi *A,
                    const mbedtls_mpi *E, const mbedtls_mpi *N )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    drv_pke_data hw_data[3];
    size_t size_align = 0;
    const mbedtls_mpi *inputs[] = { A, E, N };
    unsigned char *buffer = NULL;

    mbedtls_harden_log_func_enter();

    get_size_align( mbedtls_mpi_size( A ), mbedtls_mpi_size( E ), mbedtls_mpi_size( N ), &size_align );

    buffer = mbedtls_calloc( 3, size_align );
    if ( buffer == NULL )
    {
        mbedtls_harden_log_err("mbedtls_calloc failed!\n");
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
    }

    for (int i = 0; i < 3; i++)
    {
        hw_data[i].data = buffer + (i * size_align);
        hw_data[i].length = size_align;
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( inputs[i], hw_data[i].data, hw_data[i].length ) );
    }

    ret = CIPHER_PKE_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_INIT failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    ret = CIPHER_PKE_EXP_MOD( &hw_data[2], &hw_data[1], &hw_data[0], &hw_data[0] );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( X, hw_data[0].data, hw_data[0].length ) );
    ret = 0;

cleanup:
    CIPHER_PKE_DEINIT();
    if( buffer != NULL )
    {
        mbedtls_platform_zeroize( buffer, 3 * size_align );
        mbedtls_free( buffer );
    }

    mbedtls_harden_log_func_exit();

    return( ret );
}
#endif /* MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE */

#if defined(MBEDTLS_BIGNUM_MOD_USE_HARDWARE)
int check_mod_harden_can_do( const mbedtls_mpi *A, const mbedtls_mpi *B )
{
    size_t size_a, size_b, size_max;
    size_a = mbedtls_mpi_size( A );
    size_b = mbedtls_mpi_size( B );
    size_max = max( size_a, size_b );

    if( IS_PKE_MOD_FUNC_REGISTERED() != TD_TRUE )
        return ( 0 );
    if ( A->MBEDTLS_PRIVATE(s) < 0 || mbedtls_mpi_cmp_int( B, 0 ) == 0 || size_max > DRV_PKE_LEN_4096 )
        return( 0 );

#if defined(DRV_PKE_LEN_1024_SUPPORT)
    if ( size_a > 2 * size_b || mbedtls_mpi_get_bit( B, 0 ) == 0 )
        return( 0 );
    return( 1 );
#else
    return( 1 );
#endif
}

int mod_harden( mbedtls_mpi *R, const mbedtls_mpi *A, const mbedtls_mpi *B )
{
    int ret;
    size_t size_align = 0;
    drv_pke_data hw_data[3];
    unsigned char *buffer = NULL;

    mbedtls_harden_log_func_enter();

    get_size_align( mbedtls_mpi_size( A ), mbedtls_mpi_size( B ), 0, &size_align );

    buffer = mbedtls_calloc( 3, size_align );
    if ( buffer == NULL )
    {
        mbedtls_harden_log_err("mbedtls_calloc failed!\n");
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
    }

    for (int i = 0; i < 3; i++)
    {
        hw_data[i].data = buffer + (i * size_align);
        hw_data[i].length = size_align;
    }
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( A, hw_data[0].data, hw_data[0].length ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( B, hw_data[1].data, hw_data[1].length ) );

    ret = CIPHER_PKE_INIT();
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_INIT failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    ret = CIPHER_PKE_MOD( &hw_data[0], &hw_data[1], &hw_data[2] );
    if( ret != MBEDTLS_HARDEN_SUCCESS )
    {
        mbedtls_harden_log_err("CIPHER_PKE_MOD failed! ret = 0x%x\n", ret);
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( R, hw_data[2].data, hw_data[2].length ) );

cleanup:
    CIPHER_PKE_DEINIT();
    if( buffer != NULL )
    {
        mbedtls_platform_zeroize( buffer, 3 * size_align );
        mbedtls_free( buffer );
    }

    mbedtls_harden_log_func_exit();

    return( ret );
}
#endif /* MBEDTLS_BIGNUM_MOD_USE_HARDWARE */