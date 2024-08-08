/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden struct.
 *
 * Create: 2023-12-23
*/

#ifndef MBEDTLS_HARDEN_STRUCT_H
#define MBEDTLS_HARDEN_STRUCT_H

#define MBEDTLS_ALT_HASH_RESULT_SIZE_MAX_IN_WORD 16      // for SHA-512
#define MBEDTLS_ALT_HASH_BLOCK_SIZE_MAX 128              // for SHA-512

typedef enum {
    MBEDTLS_ALT_HASH_TYPE_SHA1 = 0xf690a0,
    MBEDTLS_ALT_HASH_TYPE_SHA224 = 0x10690e0,
    MBEDTLS_ALT_HASH_TYPE_SHA256 = 0x1169100,
    MBEDTLS_ALT_HASH_TYPE_SHA384 = 0x127a180,
    MBEDTLS_ALT_HASH_TYPE_SHA512 = 0x137a200,
    MBEDTLS_ALT_HASH_TYPE_SM3 = 0x2169100,

    MBEDTLS_ALT_HASH_TYPE_HMAC_SHA1 = 0x10f690a0,
    MBEDTLS_ALT_HASH_TYPE_HMAC_SHA224 = 0x110690e0,
    MBEDTLS_ALT_HASH_TYPE_HMAC_SHA256 = 0x11169100,
    MBEDTLS_ALT_HASH_TYPE_HMAC_SHA384 = 0x1127a180,
    MBEDTLS_ALT_HASH_TYPE_HMAC_SHA512 = 0x1137a200,
    MBEDTLS_ALT_HASH_TYPE_HMAC_SM3 = 0x12169100,

    MBEDTLS_ALT_HASH_TYPE_INVALID = 0xffffffff,
} mbedtls_alt_hash_type;

typedef struct {
    unsigned int length[2];
    unsigned int state[MBEDTLS_ALT_HASH_RESULT_SIZE_MAX_IN_WORD];
    unsigned int tail_len;
    mbedtls_alt_hash_type hash_type;
    unsigned char tail[MBEDTLS_ALT_HASH_BLOCK_SIZE_MAX];
} mbedtls_alt_hash_clone_ctx;

typedef enum {
    MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P256 = 0,      /* RFC 5639 - Brainpool P256/384/512 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P384,          /* RFC 5639 - Brainpool P256/384/512 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P512,          /* RFC 5639 - Brainpool P256/384/512 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P256K,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P192R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P224R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P256R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P384R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P521R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    MBEDTLS_ALT_ECP_CURVE_TYPE_RFC7748,               /* RFC 7748 - Curve25519 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_RFC7748_448,           /* RFC 7748 - Curve448 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_RFC8032,               /* RFC 8032 - ED25519 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_SM2,                   /* GMT 0003.2-2012 */
    MBEDTLS_ALT_ECP_CURVE_TYPE_MAX,
    MBEDTLS_ALT_ECP_CURVE_TYPE_INVALID = 0xffffffff,
} mbedtls_alt_ecp_curve_type;

typedef struct {
    unsigned int length;
    unsigned char *data;
} mbedtls_alt_ecp_data;

typedef struct {
    unsigned char *x;
    unsigned char *y;
    unsigned int length;
} mbedtls_alt_ecp_point;

#endif