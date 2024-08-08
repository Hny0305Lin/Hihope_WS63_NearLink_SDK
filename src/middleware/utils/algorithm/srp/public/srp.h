/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: srp lib head.
 * This file should be changed only infrequently and with great care.
 */

#ifndef __SRP_H__
#define __SRP_H__
#include "mbedtls/bignum.h"
#include "errcode.h"
#include "dfx_adapt_layer.h"

#define SRP_SUCC 1
#define SRP_FAIL 0
#define SHA256_DIGEST_LENGTH 32
#define SRP_RANDOM_SALT_LEN  20
#define SRP_HASH_LEN 32
#define srp_err_printf(fmt...) dfx_log_err(fmt)

typedef mbedtls_mpi bignum;
typedef enum {
    SRP_GN_1024,
    SRP_GN_3072,
    SRP_GN_MAX
} srp_gntype;
typedef struct {
    bignum *g;
    bignum *n;
} srp_gn;

static inline uint32_t bn_num_bytes(const mbedtls_mpi *x)
{
    return mbedtls_mpi_size(x);
}
static inline int32_t bn_ucmp(const mbedtls_mpi *x, const mbedtls_mpi *y)
{
    return mbedtls_mpi_cmp_abs(x, y);
}
static inline int32_t bn_cmp(const mbedtls_mpi *x, const mbedtls_mpi *y)
{
    return mbedtls_mpi_cmp_mpi(x, y);
}

uint32_t uapi_hash_start(uint32_t *handle);
uint32_t uapi_hash_update(uint32_t handle, const uint8_t *input_data, uint32_t input_data_len);
uint32_t uapi_hash_final(uint32_t handle, uint8_t *output_hash, uint32_t output_hash_len);
uint32_t uapi_hash_sha256(uint8_t *input_data, uint32_t input_data_len, uint8_t *output_hash);

bignum *bn_new(void);
void bn_free(bignum *a);
bignum *bn_bin2bn(const uint8_t *s, uint32_t len, bignum *ret);
int bn_bn2bin(const bignum *a, unsigned char *to);

uint32_t srp_get_random_data(uint8_t *random_data, uint16_t length);
srp_gn *srp_get_default_gn(srp_gntype type);
bignum *srp_calc_b(bignum *b, bignum *n, bignum *g, bignum *v);
int srp_verify_b_mod_n(bignum *b, bignum *n);
bignum *srp_calc_u(bignum *a, bignum *b, bignum *n);
bignum *srp_calc_server_key(bignum *a, bignum *v, bignum *u, bignum *b, bignum *n);
int srp_create_verifier_bn(const char *user, const char *pass,
    bignum **salt, bignum **verifier, srp_gn *gn);

#endif
