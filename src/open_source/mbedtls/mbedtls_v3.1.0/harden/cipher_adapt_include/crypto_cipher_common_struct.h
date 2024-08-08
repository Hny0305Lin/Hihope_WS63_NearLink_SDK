/**
 * Copyright ( c ) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef CRYPTO_CIPHER_COMMON_STRUCT_H
#define CRYPTO_CIPHER_COMMON_STRUCT_H

#include "mbedtls_crypto_type.h"
#include "mbedtls_platform_hardware_config.h"

#define CRYPTO_IV_LEN_IN_BYTES      16
#define CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD 16      // for SHA-512
#define CRYPTO_HASH_BLOCK_SIZE_MAX 128              // for SHA-512

typedef enum {
    CRYPTO_BUF_SECURE,
    CRYPTO_BUF_NONSECURE,
} crypto_buffer_secure;

typedef enum {
    CRYPTO_HASH_TYPE_SHA1 = 0xf690a0,
    CRYPTO_HASH_TYPE_SHA224 = 0x10690e0,
    CRYPTO_HASH_TYPE_SHA256 = 0x1169100,
    CRYPTO_HASH_TYPE_SHA384 = 0x127a180,
    CRYPTO_HASH_TYPE_SHA512 = 0x137a200,
    CRYPTO_HASH_TYPE_SM3 = 0x2169100,

    CRYPTO_HASH_TYPE_HMAC_SHA1 = 0x10f690a0,
    CRYPTO_HASH_TYPE_HMAC_SHA224 = 0x110690e0,
    CRYPTO_HASH_TYPE_HMAC_SHA256 = 0x11169100,
    CRYPTO_HASH_TYPE_HMAC_SHA384 = 0x1127a180,
    CRYPTO_HASH_TYPE_HMAC_SHA512 = 0x1137a200,
    CRYPTO_HASH_TYPE_HMAC_SM3 = 0x12169100,

    CRYPTO_HASH_TYPE_INVALID = 0xffffffff,
} crypto_hash_type;

typedef enum {
    CRYPTO_SYMC_ALG_TDES = 0x0,
    CRYPTO_SYMC_ALG_AES = 0x1,
    CRYPTO_SYMC_ALG_SM4 = 0x2,
    CRYPTO_SYMC_ALG_LEA = 0x3,
    CRYPTO_SYMC_ALG_DMA = 0x4,
    CRYPTO_SYMC_ALG_MAX,
    CRYPTO_SYMC_ALG_INVALID = 0xffffffff,
} crypto_symc_alg;

typedef enum {
    CRYPTO_SYMC_TYPE_NORMAL = 0x0,
    CRYPTO_SYMC_TYPE_REG,
    CRYPTO_SYMC_TYPE_MAX,
    CRYPTO_SYMC_TYPE_INVALID = 0xffffffff,
} crypto_symc_type;

typedef enum {
    CRYPTO_SYMC_WORK_MODE_ECB = 0x0,
    CRYPTO_SYMC_WORK_MODE_CBC,
    CRYPTO_SYMC_WORK_MODE_CTR,
    CRYPTO_SYMC_WORK_MODE_OFB,
    CRYPTO_SYMC_WORK_MODE_CFB,
    CRYPTO_SYMC_WORK_MODE_CCM,
    CRYPTO_SYMC_WORK_MODE_GCM,
    CRYPTO_SYMC_WORK_MODE_CBC_MAC,
    CRYPTO_SYMC_WORK_MODE_CMAC,
    CRYPTO_SYMC_WORK_MODE_MAX,
    CRYPTO_SYMC_WORK_MODE_INVALID = 0xffffffff,
} crypto_symc_work_mode;

typedef enum {
    CRYPTO_SYMC_KEY_64BIT =  0x0,
    CRYPTO_SYMC_KEY_128BIT = 0x1,
    CRYPTO_SYMC_KEY_192BIT = 0x2,
    CRYPTO_SYMC_KEY_256BIT = 0x3,
    CRYPTO_SYMC_KEY_LENGTH_MAX,
    CRYPTO_SYMC_KEY_LENGTH_INVALID = 0xffffffff,
} crypto_symc_key_length;

typedef enum {
    CRYPTO_SYMC_KEY_EVEN =  0x0,
    CRYPTO_SYMC_KEY_ODD  =  0x1,
    CRYPTO_SYMC_KEY_PARITY_MAX,
    CRYPTO_SYMC_KEY_PARITY_INVALID = 0xffffffff,
} crypto_symc_key_parity;

typedef enum {
    CRYPTO_SYMC_BIT_WIDTH_1BIT = 0x0,
    CRYPTO_SYMC_BIT_WIDTH_8BIT = 0x1,
    CRYPTO_SYMC_BIT_WIDTH_64BIT = 0x2,
    CRYPTO_SYMC_BIT_WIDTH_128BIT = 0x3,
    CRYPTO_SYMC_BIT_WIDTH_MAX,
    CRYPTO_SYMC_BIT_WIDTH_INVALID = 0xffffffff,
} crypto_symc_bit_width;

typedef enum {
    CRYPTO_SYMC_IV_DO_NOT_CHANGE = 0,
    CRYPTO_SYMC_IV_CHANGE_ONE_PKG,
    CRYPTO_SYMC_IV_CHANGE_ALL_PKG,
    /* GCM. */
    CRYPTO_SYMC_GCM_IV_DO_NOT_CHANGE,
    CRYPTO_SYMC_GCM_IV_CHANGE_START,
    CRYPTO_SYMC_GCM_IV_CHANGE_UPDATE,
    CRYPTO_SYMC_GCM_IV_CHANGE_FINISH,
    /* CCM. */
    CRYPTO_SYMC_CCM_IV_DO_NOT_CHANGE,
    CRYPTO_SYMC_CCM_IV_CHANGE_START,
    CRYPTO_SYMC_CCM_IV_CHANGE_UPDATE,
    CRYPTO_SYMC_CCM_IV_CHANGE_FINISH,
    CRYPTO_SYMC_IV_CHANGE_MAX,
    CRYPTO_SYMC_IV_CHANGE_INVALID = 0xffffffff,
} crypto_symc_iv_change_type;

typedef enum {
    DRV_PKE_RSA_SCHEME_PKCS1_V15 = 0x00,  /* PKCS#1 V15 */
    DRV_PKE_RSA_SCHEME_PKCS1_V21,         /* PKCS#1 V21, PSS for signning, OAEP for encryption */
    DRV_PKE_RSA_SCHEME_MAX,
    DRV_PKE_RSA_SCHEME_INVALID = 0xffffffff,
} drv_pke_rsa_scheme;

typedef enum {
    DRV_PKE_HASH_TYPE_SHA1 = 0x00,   /* Suggest Not to use */
    DRV_PKE_HASH_TYPE_SHA224,
    DRV_PKE_HASH_TYPE_SHA256,
    DRV_PKE_HASH_TYPE_SHA384,
    DRV_PKE_HASH_TYPE_SHA512,
    DRV_PKE_HASH_TYPE_SM3,
    DRV_PKE_HASH_TYPE_MAX,
    DRV_PKE_HASH_TYPE_INVALID = 0xffffffff,
} drv_pke_hash_type;

typedef enum {
    DRV_PKE_BUF_NONSECURE = 0x00,
    DRV_PKE_BUF_SECURE,
    DRV_PKE_BUF_INVALID = 0xffffffff,
} drv_pke_buffer_secure;

typedef enum {
    DRV_PKE_LEN_192 = 24,
    DRV_PKE_LEN_224 = 32,
    DRV_PKE_LEN_256 = 32,
    DRV_PKE_LEN_384 = 48,
    DRV_PKE_LEN_448 = 56,
    DRV_PKE_LEN_512 = 64,
    DRV_PKE_LEN_521 = 68,
    DRV_PKE_LEN_576 = 72,
    DRV_PKE_LEN_1024 = 128,
    DRV_PKE_LEN_2048 = 256,
    DRV_PKE_LEN_3072 = 384,
    DRV_PKE_LEN_4096 = 512,
    DRV_PKE_LEN_MAX,
    DRV_PKE_LEN_INVALID = 0xffffffff,
} drv_pke_len;

typedef enum {
    DRV_PKE_ECC_TYPE_RFC5639_P256 = 0,      /* RFC 5639 - Brainpool P256/384/512 */
    DRV_PKE_ECC_TYPE_RFC5639_P384,          /* RFC 5639 - Brainpool P256/384/512 */
    DRV_PKE_ECC_TYPE_RFC5639_P512,          /* RFC 5639 - Brainpool P256/384/512 */
    DRV_PKE_ECC_TYPE_FIPS_P256K,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
#if defined(MBEDTLS_SECP192R1_USE_HARDWARE)
    DRV_PKE_ECC_TYPE_FIPS_P192R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
#endif
#if defined(MBEDTLS_SECP224R1_USE_HARDWARE)
    DRV_PKE_ECC_TYPE_FIPS_P224R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
#endif
    DRV_PKE_ECC_TYPE_FIPS_P256R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P384R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P521R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_RFC7748,               /* RFC 7748 - Curve25519 */
#if defined(MBEDTLS_CURVE448_USE_HARDWARE)
    DRV_PKE_ECC_TYPE_RFC7748_448,           /* RFC 7748 - Curve448 */
#endif
    DRV_PKE_ECC_TYPE_RFC8032,               /* RFC 8032 - ED25519 */
    DRV_PKE_ECC_TYPE_SM2,                   /* GMT 0003.2-2012 */
    DRV_PKE_ECC_TYPE_MAX,
    DRV_PKE_ECC_TYPE_INVALID = 0xffffffff,
} drv_pke_ecc_curve_type;

typedef struct {
    unsigned long long uapi_mem_handle;     /* Handle to buffer header address */
    unsigned long long addr_offset;         /* buffer offset, unused. */
    td_void *kapi_mem_handle;

    unsigned long phys_addr;
    void *virt_addr;
    crypto_buffer_secure buf_sec;           /* NONSECURE or SECURE */
} crypto_buf_attr;

typedef struct {
    td_u8 *key;
    td_u32 key_len;
    td_handle drv_keyslot_handle;
    crypto_hash_type hash_type;
    td_bool is_keyslot;
    td_bool is_long_term;
} crypto_hash_attr;

typedef struct {
    td_u32 length[2];
    td_u32 state[CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD];
    td_u32 tail_len;
    crypto_hash_type hash_type;
#if defined(CRYPTO_SOFT_HMAC_SUPPORT)
    td_u8 o_key_pad[CRYPTO_HASH_BLOCK_SIZE_MAX];
    td_u8 i_key_pad[CRYPTO_HASH_BLOCK_SIZE_MAX];
#endif
    td_u8 tail[CRYPTO_HASH_BLOCK_SIZE_MAX];
} crypto_hash_clone_ctx;

typedef struct {
    crypto_hash_type hmac_type;
    td_u8 *salt;
    td_u32 salt_length;
    td_u8 *ikm;
    td_u32 ikm_length;
} crypto_hkdf_extract_t;

typedef struct {
    crypto_hash_type hmac_type;
    td_u8 *prk;
    td_u32 prk_length;
    td_u8 *info;
    td_u32 info_length;
} crypto_hkdf_expand_t;

typedef struct {
    crypto_hash_type hmac_type;
    td_u8 *salt;
    td_u32 salt_length;
    td_u8 *ikm;
    td_u32 ikm_length;
    td_u8 *info;
    td_u32 info_length;
} crypto_hkdf_t;

typedef struct {
    crypto_hash_type hash_type;
    td_u8 *password;
    td_u32 plen;
    td_u8 *salt;
    td_u32 slen;
    td_u16 count;
} crypto_kdf_pbkdf2_param;

typedef struct {
    crypto_symc_alg symc_alg;
    crypto_symc_work_mode work_mode;
    crypto_symc_type symc_type;
    td_bool is_long_term;
} crypto_symc_attr;

typedef struct {
    crypto_symc_alg symc_alg;
    crypto_symc_work_mode work_mode;
    crypto_symc_key_length symc_key_length;
    crypto_symc_key_parity key_parity;
    crypto_symc_bit_width symc_bit_width;
    crypto_symc_iv_change_type iv_change_flag;
    td_u8 iv[CRYPTO_IV_LEN_IN_BYTES];
    td_u32 iv_length;
    td_void *param;
} crypto_symc_ctrl_t;

typedef struct {
    crypto_buf_attr aad_buf;
    td_u32 aad_len;         /* Addition Data Length In Bytes. */
    td_u32 data_len;        /* Crypto Data Length In Bytes. */
    td_u32 tag_len;         /* Tag Length In Bytes. */
} crypto_symc_config_aes_ccm_gcm;

typedef struct {
    td_bool is_long_term;
    crypto_symc_alg symc_alg;
    crypto_symc_work_mode work_mode;
    crypto_symc_key_length symc_key_length;
    td_u32 keyslot_chn;
} crypto_symc_mac_attr;

typedef struct {
    td_u32  length;
    td_u8  *data;
} drv_pke_data;

typedef struct {
    td_u8 *x;   /* X coordinates of the generated public key, the caller ensures it is padded with leading
                   zeros if the effective size of this key is smaller than ecc key size. */
    td_u8 *y;   /* Y coordinates of the generated public key, the caller ensures it is padded with leading
                   zeros if the effective size of this key is smaller than ecc key size. */
    td_u32 length;
} drv_pke_ecc_point;

typedef struct {
    td_u8 *r;   /* r component of the signature. */
    td_u8 *s;   /* s component of the signature. */
    td_u32 length;
} drv_pke_ecc_sig;

typedef struct {
    td_u8 *n;          /* *< public modulus */
    td_u8 *e;          /* *< public exponent */
    td_u8 *d;          /* *< private exponent */
    td_u8 *p;          /* *< 1st prime factor */
    td_u8 *q;          /* *< 2nd prime factor */
    td_u8 *dp;         /* *< D % (P - 1) */
    td_u8 *dq;         /* *< D % (Q - 1) */
    td_u8 *qp;         /* *< 1 / (Q % P) */
    td_u16 n_len;      /* *< length of public modulus */
    td_u16 e_len;      /* *< length of public exponent */
    td_u16 d_len;      /* *< length of private exponent */
    td_u16 p_len;      /* *< length of 1st prime factor,should be half of u16NLen */
    td_u16 q_len;      /* *< length of 2nd prime factor,should be half of u16NLen */
    td_u16 dp_len;     /* *< length of D % (P - 1),should be half of u16NLen */
    td_u16 dq_len;     /* *< length of D % (Q - 1),should be half of u16NLen */
    td_u16 qp_len;     /* *< length of 1 / (Q % P),should be half of u16NLen */
} drv_pke_rsa_priv_key;

/* * struct of RSA public key */
typedef struct {
    td_u8  *n;            /* point to public modulus */
    td_u8  *e;            /* point to public exponent */
    td_u16 len;           /* length of public modulus, max value is 512Byte */
} drv_pke_rsa_pub_key;

typedef struct {
    td_u32  length;
    td_u8  *data;
    drv_pke_buffer_secure buf_sec;
} drv_pke_msg;

#endif /* CRYPTO_CIPHER_COMMON_STRUCT_H */