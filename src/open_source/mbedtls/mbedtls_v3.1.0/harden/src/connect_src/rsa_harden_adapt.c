/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: public key encryption harden func adapt.
 *
 * Create: 2024-01-15
*/

#include "common.h"
#if defined(MBEDTLS_RSA_ALT)
#include "securec.h"
#include "mbedtls/error.h"
#include "dfx.h"
#include "cipher_adapt.h"
#include "md.h"
#include "mbedtls/rsa.h"
#include "mbedtls_harden_adapt_api.h"


static int inner_get_hash_type(mbedtls_md_type_t mbedtls_hash_type, drv_pke_hash_type *drv_hash_type)
{
    switch (mbedtls_hash_type) {
        case MBEDTLS_MD_SHA1:
            *drv_hash_type = DRV_PKE_HASH_TYPE_SHA1;
            break;
        case MBEDTLS_MD_SHA224:
            *drv_hash_type = DRV_PKE_HASH_TYPE_SHA224;
            break;
        case MBEDTLS_MD_SHA256:
            *drv_hash_type = DRV_PKE_HASH_TYPE_SHA256;
            break;
        case MBEDTLS_MD_SHA384:
            *drv_hash_type = DRV_PKE_HASH_TYPE_SHA384;
            break;
        case MBEDTLS_MD_SHA512:
            *drv_hash_type = DRV_PKE_HASH_TYPE_SHA512;
            break;
        default:
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
    return 0;
}

static void inner_get_klen(unsigned int *k_len)
{
    if (*k_len <= DRV_PKE_LEN_1024) {
        *k_len = DRV_PKE_LEN_1024;
    } else if (*k_len <= DRV_PKE_LEN_2048) {
        *k_len = DRV_PKE_LEN_2048;
    } else if (*k_len <= DRV_PKE_LEN_3072) {
        *k_len = DRV_PKE_LEN_3072;
    } else if (*k_len <= DRV_PKE_LEN_4096) {
        *k_len = DRV_PKE_LEN_4096;
    }
}

static int inner_alloc_pub_key(const mbedtls_rsa_context *ctx, drv_pke_rsa_pub_key *pub_key)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t size_align, n_len, e_len = 0;
    unsigned char *buffer = NULL;

    mbedtls_harden_log_func_enter();

    size_align = ctx->len;
    inner_get_klen(&size_align);
    buffer = mbedtls_calloc( 2, size_align );
    if ( buffer == NULL ) {
        mbedtls_harden_log_err("mbedtls_calloc failed!\n");
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
    }
    pub_key->n = buffer;
    pub_key->e = buffer + size_align;
    pub_key->len = size_align;

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->N, pub_key->n, pub_key->len ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->E, pub_key->e, pub_key->len ) );

    mbedtls_harden_log_func_exit();

    return 0;

cleanup:
    if( buffer != NULL ) {
        mbedtls_platform_zeroize( buffer, 2 * size_align );
        mbedtls_free( buffer );
    }

    return ret ;
}

static void inner_free_pub_key(drv_pke_rsa_pub_key *pub_key)
{
    if (pub_key->n != NULL) {
        mbedtls_platform_zeroize(pub_key->n, 2 * pub_key->len);
        mbedtls_free(pub_key->n);
        pub_key->n = NULL;
        pub_key->e = NULL;
        pub_key->len = 0;
    }
}

static int inner_alloc_priv_key_and_out_buf(const mbedtls_rsa_context *ctx, drv_pke_rsa_priv_key *priv_key,
    drv_pke_data *out_buf)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t size_align, n_len, d_len = 0;
    unsigned char *buffer = NULL;

    mbedtls_harden_log_func_enter();

    size_align = ctx->len;
    inner_get_klen(&size_align);
    buffer = mbedtls_calloc( 3, size_align );
    if ( buffer == NULL ) {
        mbedtls_harden_log_err("mbedtls_calloc failed!\n");
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
    }
    priv_key->n = buffer;
    priv_key->d = buffer + size_align;
    priv_key->n_len = size_align;
    priv_key->d_len = size_align;
    if (out_buf != NULL) {
        out_buf->data = buffer + 2 * size_align;
        out_buf->length = size_align;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->N, priv_key->n, priv_key->n_len ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->D, priv_key->d, priv_key->d_len ) );

    mbedtls_harden_log_func_exit();

    return 0;

cleanup:
    if( buffer != NULL ) {
        mbedtls_platform_zeroize( buffer, 3 * size_align );
        mbedtls_free( buffer );
    }

    return ret ;
}

static void inner_free_priv_key_and_out_buf(drv_pke_rsa_priv_key *priv_key, drv_pke_data *out_buf)
{
    if (priv_key->n != NULL) {
        mbedtls_platform_zeroize( priv_key->n, 3 * priv_key->n_len );
        mbedtls_free( priv_key->n );
        priv_key->n = NULL;
        priv_key->n_len = 0;
        priv_key->d = NULL;
        priv_key->d_len = 0;
        if (out_buf != NULL) {
            out_buf->data = NULL;
            out_buf->length = 0;
        }
    }
}

int mbedtls_alt_rsa_public_encrypt(const mbedtls_rsa_context *ctx, const unsigned char *label,
    unsigned int label_len, const unsigned char *input, unsigned int ilen, unsigned char *output, unsigned int olen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    drv_pke_hash_type drv_hash_type = DRV_PKE_HASH_TYPE_INVALID;
    drv_pke_data input_data = {.data = (unsigned char *)input, .length = ilen};
    drv_pke_data output_data = {.data = (unsigned char *)output, .length = olen};
    drv_pke_data label_data = {.data = (unsigned char *)label, .length = label_len};
    drv_pke_rsa_pub_key pub_key = {0};
    mbedtls_harden_log_func_enter();
    ret = inner_alloc_pub_key(ctx, &pub_key);
    if (ret != 0) {
        mbedtls_printf("Error: convert public key failed!\n");
        return ret;
    }
    if (ctx->padding == MBEDTLS_RSA_PKCS_V15) {
        ret = CIPHER_PKE_RSA_PUBLIC_ENCRYPT(DRV_PKE_RSA_SCHEME_PKCS1_V15, drv_hash_type, &pub_key, &input_data, NULL,
            &output_data);
    } else if (ctx->padding == MBEDTLS_RSA_PKCS_V21) {
        ret = inner_get_hash_type(ctx->hash_id, &drv_hash_type);
        if (ret != 0) {
            mbedtls_printf("Error: wrong hash type!\n");
            goto clean_buffer;
        }
        ret = CIPHER_PKE_RSA_PUBLIC_ENCRYPT(DRV_PKE_RSA_SCHEME_PKCS1_V21, drv_hash_type, &pub_key, &input_data, &label_data,
            &output_data);
    }
    mbedtls_harden_log_func_exit();
    if (ret != TD_SUCCESS) {
        mbedtls_printf("Error: RSA public encrypt failed! ret = 0x%x\n", ret);
        goto clean_buffer;
    }
    ret = 0;

clean_buffer:
    inner_free_pub_key(&pub_key);
    return ret;
}

int mbedtls_alt_rsa_private_decrypt(const mbedtls_rsa_context *ctx, const unsigned char *label,
    unsigned int label_len, const unsigned char *input, unsigned int ilen, unsigned char *output, unsigned int olen_max,
    unsigned int *olen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    drv_pke_hash_type drv_hash_type = DRV_PKE_HASH_TYPE_INVALID;
    drv_pke_data input_data = {.data = (unsigned char *)input, .length = ilen};
    drv_pke_data output_data = {0};
    drv_pke_data label_data = {.data = (unsigned char *)label, .length = label_len};
    drv_pke_rsa_priv_key priv_key = {0};
    mbedtls_harden_log_func_enter();
    ret = inner_alloc_priv_key_and_out_buf(ctx, &priv_key, &output_data);
    if (ret != 0) {
        mbedtls_printf("Error: convert private key failed!\n");
        return ret;
    }
    if (ctx->padding == MBEDTLS_RSA_PKCS_V15) {
        ret = CIPHER_PKE_RSA_PRIVATE_DECRYPT(DRV_PKE_RSA_SCHEME_PKCS1_V15, drv_hash_type, &priv_key, &input_data,
            NULL, &output_data);
    } else if (ctx->padding == MBEDTLS_RSA_PKCS_V21) {
        ret = inner_get_hash_type(ctx->hash_id, &drv_hash_type);
        if (ret != 0) {
            mbedtls_printf("Error: wrong hash type!\n");
            goto clean_buffer;
        }
        ret = CIPHER_PKE_RSA_PRIVATE_DECRYPT(DRV_PKE_RSA_SCHEME_PKCS1_V21, drv_hash_type, &priv_key, &input_data,
        &label_data, &output_data);
    }
    mbedtls_harden_log_func_exit();
    if (ret != TD_SUCCESS) {
        mbedtls_printf("Error: RSA private decrypt failed! ret = 0x%x\n", ret);
        goto clean_buffer;
    }
    *olen = output_data.length;
    ret = memcpy_s(output, olen_max, output_data.data, output_data.length);
    if (ret != EOK) {
        mbedtls_printf("Error: copy result failed! ret = 0x%x\n", ret);
        goto clean_buffer;
    }
    ret = 0;

clean_buffer:
    inner_free_priv_key_and_out_buf(&priv_key, &output_data);
    return ret;
}

int mbedtls_alt_rsa_private_sign(const mbedtls_rsa_context *ctx, const unsigned char *hash, unsigned int hashlen,
    unsigned char *sig, unsigned int olen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    drv_pke_hash_type drv_hash_type = DRV_PKE_HASH_TYPE_INVALID;
    drv_pke_data input_hash = {.data = (unsigned char *)hash, .length = hashlen};
    drv_pke_data sign_data = {.data = sig, .length = olen};
    drv_pke_rsa_priv_key priv_key = {0};
    mbedtls_harden_log_func_enter();
    ret = inner_get_hash_type(ctx->hash_id, &drv_hash_type);
    if (ret != 0) {
        mbedtls_printf("Error: wrong hash type!\n");
        return ret;
    }
    ret = inner_alloc_priv_key_and_out_buf(ctx, &priv_key, NULL);
    if (ret != 0) {
        mbedtls_printf("Error: convert private key failed!\n");
        return ret;
    }
    if (ctx->padding == MBEDTLS_RSA_PKCS_V15) {
        ret = CIPHER_PKE_RSA_SIGN(&priv_key, DRV_PKE_RSA_SCHEME_PKCS1_V15, drv_hash_type, &input_hash, &sign_data);
    } else if (ctx->padding == MBEDTLS_RSA_PKCS_V21) {
        ret = CIPHER_PKE_RSA_SIGN(&priv_key, DRV_PKE_RSA_SCHEME_PKCS1_V21, drv_hash_type, &input_hash, &sign_data);
    }
    mbedtls_harden_log_func_exit();
    if (ret != TD_SUCCESS) {
        mbedtls_printf("Error: RSA private sign failed! ret = 0x%x\n", ret);
        goto clean_buffer;
    }
    ret = 0;

clean_buffer:
    inner_free_priv_key_and_out_buf(&priv_key, NULL);
    return ret;
}

int mbedtls_alt_rsa_public_verify(const mbedtls_rsa_context *ctx, const unsigned char *hash, unsigned int hashlen,
    const unsigned char *sig, unsigned int olen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    drv_pke_hash_type drv_hash_type = DRV_PKE_HASH_TYPE_INVALID;
    drv_pke_data input_hash = {.data = (unsigned char *)hash, .length = hashlen};
    drv_pke_data sign_data = {.data = (unsigned char *)sig, .length = olen};
    drv_pke_rsa_pub_key pub_key = {0};
    mbedtls_harden_log_func_enter();
    ret = inner_get_hash_type(ctx->hash_id, &drv_hash_type);
    if (ret != 0) {
        mbedtls_printf("Error: wrong hash type!\n");
        return ret;
    }
    ret = inner_alloc_pub_key(ctx, &pub_key);
    if (ret != 0) {
        mbedtls_printf("Error: convert public key failed!\n");
        return ret;
    }
    if (ctx->padding == MBEDTLS_RSA_PKCS_V15) {
        ret = CIPHER_PKE_RSA_VERIFY(&pub_key, DRV_PKE_RSA_SCHEME_PKCS1_V15, drv_hash_type, &input_hash, &sign_data);
    } else if (ctx->padding == MBEDTLS_RSA_PKCS_V21) {
        ret = CIPHER_PKE_RSA_VERIFY(&pub_key, DRV_PKE_RSA_SCHEME_PKCS1_V21, drv_hash_type, &input_hash, &sign_data);
    }
    mbedtls_harden_log_func_exit();
    if (ret != TD_SUCCESS) {
        mbedtls_printf("Error: RSA public verify failed! ret = 0x%x\n", ret);
        goto clean_buffer;
    }
    ret = 0;

clean_buffer:
    inner_free_pub_key(&pub_key);
    return ret;
}
#endif  /* MBEDTLS_RSA_ALT */
