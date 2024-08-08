/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides pke driver header \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef UNIFIED_CIPHER_PKE_H
#define UNIFIED_CIPHER_PKE_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_pke PKE
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief ECC curve type
 * @note  TDES is not secure, and we advise not to use it.
 * @else
 * @brief ECC曲线类型
 * @note  TDES不安全，不建议使用。
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256 = 0,      /* RFC 5639 - Brainpool P256/384/512 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384,          /* RFC 5639 - Brainpool P256/384/512 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512,          /* RFC 5639 - Brainpool P256/384/512 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256K,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P192R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P224R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P384R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P521R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748,               /* RFC 7748 - Curve25519 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448,           /* RFC 7748 - Curve448 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC8032,               /* RFC 8032 - ED25519 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2,                   /* GMT 0003.2-2012 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_MAX,
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_ecc_curve_type_t;

/**
 * @if Eng
 * @brief Padding mode of the RSA algorithm
 * @note  PKCS1_V15 is not secure, and we advise not to use it.
 * @else
 * @brief RSA算法填充方式
 * @note  PKCS1_V15不安全，不建议使用。
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15 = 0x00,    /* not security, suggest not to use */
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21,
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_MAX,
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_rsa_scheme_t;

/**
 * @if Eng
 * @brief Hash algorithm type used for RSA padding
 * @note  SHA1 and SHA224 is not secure, and we advise not to use it.
 * @else
 * @brief RSA填充使用的hash算法类型
 * @note  SHA1和SHA224不安全，不建议使用。
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1 = 0x00,  /* not security, suggest not to use */
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA224,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA384,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA512,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_MAX,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_hash_type_t;

/**
 * @if Eng
 * @brief Buffer security attribute for RSA input messages
 * @else
 * @brief RSA输入消息的缓冲区安全属性
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_PKE_BUF_NONSECURE = 0x00,
    UAPI_DRV_CIPHER_PKE_BUF_SECURE,
    UAPI_DRV_CIPHER_PKE_BUF_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_buffer_secure_t;

/**
 * @if Eng
 * @brief  Common PKE data structure
 * @else
 * @brief PKE通用数据结构体
 * @endif
 */
typedef struct {
    uint32_t  length;   /*!< @if Eng PKE common data buffer length.
                             @else   PKE通用数据缓冲区长度。 @endif */
    uint8_t  *data;     /*!< @if Eng PKE common data buffer.
                             @else   PKE通用数据缓冲区。 @endif */
} uapi_drv_cipher_pke_data_t;

/**
 * @if Eng
 * @brief  ECC public key structure
 * @else
 * @brief ECC公钥结构体
 * @endif
 */
typedef struct {
    uint8_t *x;    /*!< @if Eng X coordinates of the generated public key, the caller ensures it is padded with leading
                                zeros if the effective size of this key is smaller than ecc key size.
                        @else   公钥的X坐标，调用方确保如果此密钥的有效大小小于ecc密钥大小，则用前导零填充。 @endif */
    uint8_t *y;    /*!< @if Eng Y coordinates of the generated public key, the caller ensures it is padded with leading
                                zeros if the effective size of this key is smaller than ecc key size.
                        @else   公钥的Y坐标，调用方确保如果此密钥的有效大小小于ecc密钥大小，则用前导零填充。 @endif */
    uint32_t length;    /*!< @if Eng ECC public key length.
                             @else   RCC公钥长度。 @endif */
} uapi_drv_cipher_pke_ecc_point_t;

/**
 * @if Eng
 * @brief  ECC signature structure
 * @else
 * @brief ECC签名结构体
 * @endif
 */
typedef struct {
    uint8_t *r;    /*!< @if Eng ECC signature R.
                        @else   ECC签名值R。 @endif */
    uint8_t *s;    /*!< @if Eng ECC signature S.
                        @else   ECC签名值S。 @endif */
    uint32_t length;    /*!< @if Eng Length of the ECC signature.
                             @else   ECC签名数据长度。 @endif */
} uapi_drv_cipher_pke_ecc_sig_t;

/**
 * @if Eng
 * @brief  ECC input message structure
 * @else
 * @brief ECC输入消息结构体
 * @endif
 */
typedef struct {
    uint32_t  length;    /*!< @if Eng Length of the ECC input message buffer.
                              @else   ECC输入消息缓冲区长度。 @endif */
    uint8_t  *data;      /*!< @if Eng ECC input message buffer.
                             @else   ECC输入消息缓冲区。 @endif */
    uapi_drv_cipher_pke_buffer_secure_t buf_sec;
} uapi_drv_cipher_pke_msg_t;

/**
 * @if Eng
 * @brief  RSA private key structure
 * @else
 * @brief RSA私钥结构体
 * @endif
 */
typedef struct {
    uint8_t *n;          /*!< @if Eng RSA public modulus.
                              @else   RSA秘钥参数n。 @endif */
    uint8_t *e;          /*!< @if Eng public exponent.
                              @else   RSA公钥参数e。 @endif */
    uint8_t *d;          /*!< @if Eng private exponent.
                              @else   RSA私钥参数d。 @endif */
    uint8_t *p;          /*!< @if Eng 1st prime factor.
                              @else   RSA第一素数因子。 @endif */
    uint8_t *q;          /*!< @if Eng 2nd prime factor.
                              @else   RSA第二素数因子。 @endif */
    uint8_t *dp;         /*!< @if Eng D % (P - 1).
                              @else   D % (P - 1)的结果。 @endif */
    uint8_t *dq;         /*!< @if Eng D % (Q - 1).
                              @else   D % (Q - 1)的结果。 @endif */
    uint8_t *qp;         /*!< @if Eng 1 / (Q % P).
                              @else   1 / (Q % P)的结果。 @endif */
    uint16_t n_len;      /*!< @if Eng length of public modulus.
                              @else   RSA秘钥参数n的长度。 @endif */
    uint16_t e_len;      /*!< @if Eng length of public exponent.
                              @else   RSA公钥参数e的长度。 @endif */
    uint16_t d_len;      /*!< @if Eng length of private exponent.
                              @else   RSA私钥参数d的长度。 @endif */
    uint16_t p_len;      /*!< @if Eng length of 1st prime factor,should be half of u16NLen.
                              @else   RSA第一素因子的长度，应该是u16NLen的一半。 @endif */
    uint16_t q_len;      /*!< @if Eng length of 2nd prime factor,should be half of u16NLen.
                              @else   RSA第二素因子的长度，应该是u16NLen的一半。 @endif */
    uint16_t dp_len;     /*!< @if Eng length of D % (P - 1),should be half of u16NLen.
                              @else   D % (P - 1)结果的长度，应该是u16NLen的一半。 @endif */
    uint16_t dq_len;     /*!< @if Eng length of D % (Q - 1),should be half of u16NLen.
                              @else   D % (Q - 1)结果的长度，应该是u16NLen的一半。 @endif */
    uint16_t qp_len;     /*!< @if Eng length of 1 / (Q % P),should be half of u16NLen.
                              @else   1 / (Q % P)结果的长度，应该是u16NLen的一半。 @endif */
} uapi_drv_cipher_pke_rsa_priv_key_t;

/**
 * @if Eng
 * @brief  RSA public key structure.
 * @else
 * @brief RSA公钥结构体。
 * @endif
 */
typedef struct {
    uint8_t  *n;            /*!< @if Eng private exponent.
                                 @else   RSA私钥参数d。 @endif */
    uint8_t  *e;            /*!< @if Eng public exponent.
                                 @else   RSA公钥参数e。 @endif */
    uint16_t len;           /*!< @if Eng RSA public key length.
                                 @else   RSA公钥长度。 @endif */
} uapi_drv_cipher_pke_rsa_pub_key_t;

/**
 * @if Eng
 * @brief  Generate an ECC Key Pair.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    input_priv_key Input private key, which can be a null pointer.
 * @param  [out]   output_priv_key Output private key.
 * @param  [out]   output_pub_key Output public key.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  生成ECC秘钥对。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    input_priv_key 输入私钥，可以为空指针。
 * @param  [out]   output_priv_key 输出私钥。
 * @param  [out]   output_pub_key 输出公钥。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_ecc_gen_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key,
    const uapi_drv_cipher_pke_ecc_point_t *output_pub_key);

/**
 * @if Eng
 * @brief  ECC Signature.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    priv_key Input private key.
 * @param  [in]    hash Input digest.
 * @param  [out]   sig Output Signature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  椭圆曲线数字签名。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    priv_key 输入私钥。
 * @param  [in]    hash 输入摘要。
 * @param  [out]   sig 输出签名。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_ecdsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);

/**
 * @if Eng
 * @brief  ECC signature verification.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    pub_key Input public key.
 * @param  [in]    hash Input digest.
 * @param  [out]   sig Input Signature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  椭圆曲线数字ECC验签。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    pub_key 输入公钥。
 * @param  [in]    hash 输入摘要。
 * @param  [out]   sig 输入签名。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_ecdsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);

/**
 * @if Eng
 * @brief  Edwards curve signature.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    priv_key Input public key.
 * @param  [in]    msg Input message.
 * @param  [out]   sig Output Signature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  爱德华兹曲线数字签名。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    priv_key 输入私钥。
 * @param  [in]    msg 输入消息。
 * @param  [out]   sig 输出签名。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_eddsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);

/**
 * @if Eng
 * @brief  Edwards curve signature verification.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    pub_key Input public key.
 * @param  [in]    msg Input message.
 * @param  [in]    sig output Signature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  爱德华兹曲线数字验签。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    pub_key 输入私钥。
 * @param  [in]    msg 输入消息。
 * @param  [in]    sig 输出签名。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_eddsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);

/**
 * @if Eng
 * @brief  Elliptical Curve Diffie-Helman(ECDH) for key exchange.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    input_pub_key Input public key.
 * @param  [in]    input_priv_key Input private key.
 * @param  [out]   output_shared_key Output exchanged key .
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  椭圆曲线迪菲-赫尔曼秘钥协商。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    input_pub_key 输入公钥。
 * @param  [in]    input_priv_key 输入私钥。
 * @param  [out]   output_shared_key 输出协商秘钥。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_ecc_gen_ecdh_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *input_pub_key,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_shared_key);

/**
 * @if Eng
 * @brief  Check whether the point is on the elliptic curve.
 * @param  [in]    curve_type ECC Curve Type.
 * @param  [in]    pub_key Input public key.
 * @param  [out]   is_on_curve Output result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  检查点是否在椭圆曲线上。
 * @param  [in]    curve_type ECC曲线类型。
 * @param  [in]    pub_key 输入公钥。
 * @param  [out]   is_on_curve 输出结果。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_check_dot_on_curve(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key, bool *is_on_curve);

/**
 * @if Eng
 * @brief  SM3 digest calculation for SM2.
 * @param  [in]    sm2_id SM2 ID.
 * @param  [in]    pub_key Input public key.
 * @param  [in]    msg Input message.
 * @param  [out]   hash Output digest.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  SM2杂凑的SM3摘要计算。
 * @param  [in]    sm2_id SM2 ID。
 * @param  [in]    pub_key 输入公钥。
 * @param  [in]    msg 输入消息。
 * @param  [out]   hash 输出摘要。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_sm2_dsa_hash(const uapi_drv_cipher_pke_data_t *sm2_id,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    uapi_drv_cipher_pke_data_t *hash);

/**
 * @if Eng
 * @brief  SM2 public key encryption.
 * @param  [in]    pub_key Input public key.
 * @param  [in]    plain_text Input plain text.
 * @param  [out]   cipher_text Output cipher text.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  SM2公钥加密。
 * @param  [in]    pub_key 输入公钥。
 * @param  [in]    plain_text 输入明文。
 * @param  [out]   cipher_text 输出密文。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_sm2_public_encrypt(const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *plain_text,
    const uapi_drv_cipher_pke_data_t *cipher_text);

/**
 * @if Eng
 * @brief  SM2 private key decryption.
 * @param  [in]    priv_key Input private key.
 * @param  [in]    cipher_text Input cipher text.
 * @param  [out]   plain_text Output plain text.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  SM2私钥解密。
 * @param  [in]    priv_key 输入私钥。
 * @param  [in]    cipher_text 输入密文。
 * @param  [out]   plain_text 输出明文。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_sm2_private_decrypt(const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *cipher_text,
    const uapi_drv_cipher_pke_data_t *plain_text);

/**
 * @if Eng
 * @brief  RSA Signature.
 * @param  [in]    priv_key Input private key
 * @param  [in]    scheme RSA padding mode
 * @param  [in]    hash_type Digest algorithm used for RSA padding
 * @param  [in]    input_hash Input digest
 * @param  [out]   sign Output the signature result
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  RSA签名。
 * @param  [in]    priv_key 输入私钥。
 * @param  [in]    scheme RSA填充方式。
 * @param  [in]    hash_type RSA填充使用的摘要算法。
 * @param  [in]    input_hash 输入摘要。
 * @param  [out]   sign 输出签名结果。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_rsa_sign(const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_data_t *input_hash,
    uapi_drv_cipher_pke_data_t *sign);

/**
 * @if Eng
 * @brief  Generate an RSA private key based on the RSA public key.
 * @param  [in]    pub_key Input public key
 * @param  [in]    scheme RSA padding mode
 * @param  [in]    hash_type Digest algorithm used for RSA padding
 * @param  [in]    input_hash Input digest
 * @param  [out]   sig Output signature
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  RSA验签。
 * @param  [in]    pub_key 输入公钥。
 * @param  [in]    scheme RSA填充方式。
 * @param  [in]    hash_type RSA填充使用的摘要算法。
 * @param  [in]    input_hash 输入摘要。
 * @param  [out]   sig 输出签名。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_rsa_verify(const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    uapi_drv_cipher_pke_data_t *input_hash,
    const uapi_drv_cipher_pke_data_t *sig);

/**
 * @if Eng
 * @brief  RSA public key encryption.
 * @param  [in]    scheme padding mode
 * @param  [in]    hash_type Digest algorithm used for RSA padding, used by OAEP padding mode
 * @param  [in]    pub_key Input public key
 * @param  [in]    input Input plain text
 * @param  [in]    label RSA label, used by OAEP padding mode
 * @param  [out]   output Output cipher text
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  RSA公钥加密。
 * @param  [in]    scheme RSA填充方式。
 * @param  [in]    hash_type RSA填充使用的摘要算法, 仅 OAEP 填充模式时使用此参数。
 * @param  [in]    pub_key 输入公钥。
 * @param  [in]    input 输入明文。
 * @param  [in]    label RSA标签, 仅 OAEP 填充模式时使用此参数。
 * @param  [out]   output 输出密文。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_cipher_pke_rsa_public_encrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    uapi_drv_cipher_pke_data_t *output);

/**
 * @if Eng
 * @brief  RSA public key encryption.
 * @param  [in]    scheme padding mode
 * @param  [in]    hash_type Digest algorithm used for RSA padding, used by OAEP padding mode
 * @param  [in]    priv_key Input private key
 * @param  [in]    input Input cipher text
 * @param  [in]    label RSA label, used by OAEP padding mode
 * @param  [out]   output Output plain text
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  RSA私钥解密。
 * @param  [in]    scheme RSA填充方式。
 * @param  [in]    hash_type RSA填充使用的摘要算法, 仅 OAEP 填充模式时使用此参数。
 * @param  [in]    priv_key 输入私钥。
 * @param  [in]    input 输入密文。
 * @param  [in]    label RSA标签, 仅 OAEP 填充模式时使用此参数。
 * @param  [out]   output 输出明文。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_rsa_private_decrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    const uapi_drv_cipher_pke_data_t *output);

/**
 * @if Eng
 * @brief  DH generate key pair or generate public key from private key.
 * @attention The valid length of module shouldn't be two bytes less than the support length.
 * @param  [in]    g_data The public base data, which is prime, and length shouldn't be larger than mod_n->length.
 * @param  [in]    mod_n The public module, which is prime only support 192/224/256/384/512/521/1024/2048/3072/4096bits.
 * @param  [in]    input_priv_key The input private key, which can be null.
 * If not, the length should be equal to mod_n->length.
 * @param  [out]   output_priv_key The output private key, which length should be equal to mod_n->length.
 * @param  [out]   output_pub_key The output public key, which length should be equal to mod_n->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  DH 公私钥对生成或由私钥生成公钥。
 * @attention 模数的有效字节数与 mod_n->length 支持的规格之差不能大于2字节。
 * @param  [in]    g_data 公开的底数，该数为质数，g_data->length 不能大于 mod_n->length。
 * @param  [in]    mod_n 公开的模数，该数为质数。
 * @param  [in]    input_priv_key 用户输入的私钥，可以为空。若非空，input_priv_key->length 应该与 mod_n->length 相同。
 * @param  [out]   output_priv_key 生成的私钥，output_priv_key->length 应该与 mod_n->length 相同。
 * @param  [out]   output_pub_key 生成的公钥，output_pub_key->length 应该与 mod_n->length 相同。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_dh_gen_key(const uapi_drv_cipher_pke_data_t *g_data,
    const uapi_drv_cipher_pke_data_t *mod_n, const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_data_t *output_pub_key);

/**
 * @if Eng
 * @brief  Diffie-Hellman algorithm.
 * @param  [in]    mod_n The public modulus, which is prime.
 * @param  [in]    input_priv_key The input private key.
 * @param  [in]    input_pub_key The input public key.
 * @param  [out]   output_shared_key The output shared key.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  DH 密钥协商算法。
 * @param  [in]    mod_n 公开的模数，该数为质数。
 * @param  [in]    input_priv_key 用户输入的私钥。
 * @param  [in]    input_pub_key 用户输入的公钥。
 * @param  [out]   output_shared_key 生成的共享密钥。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_dh_compute_key(const uapi_drv_cipher_pke_data_t *mod_n,
    const uapi_drv_cipher_pke_data_t  *input_priv_key, const uapi_drv_cipher_pke_data_t  *input_pub_key,
    const uapi_drv_cipher_pke_data_t  *output_shared_key);

/**
 * @if Eng
 * @brief  Modular addition c = (a + b) mod p.
 * @attention The valid length of modulur shouldn't be two bytes less than the support length.
 * @param  [in]    a The first parameter, which length shouldn't be larger than p->length.
 * @param  [in]    b The second parameter, which length shouldn't be larger than p->length.
 * @param  [in]    p The modulur, which only support 192/224/256/384/512/521/1024/1536/2048/3072/4096bits.
 * @param  [out]   c The output data, which length should be equal to p->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  模加 c = (a + b) mod p。
 * @attention 模数的有效字节数与 p->length 支持的规格之差不能大于2字节。
 * @param  [in]    a 入参，a->length 不能大于 p->length。
 * @param  [in]    b 入参，b->length 不能大于 p->length。
 * @param  [in]    p 模数，p->length 只能为 192/224/256/384/512/521/1024/1536/2048/3072/4096bits。
 * @param  [out]   c 计算结果，c->length 应该与 p->length 相同。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_add_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c);

/**
 * @if Eng
 * @brief  Modular subtraction c = (a - b) mod p.
 * @attention The valid length of modulur shouldn't be two bytes less than the support length.
 * @param  [in]    a The first parameter, which length shouldn't be larger than p->length.
 * @param  [in]    b The second parameter, which length shouldn't be larger than p->length.
 * @param  [in]    p The modulur, which only support 192/224/256/384/512/521/1024/1536/2048/3072/4096bits.
 * @param  [out]   c The output data, which length should be equal to p->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  模减 c = (a - b) mod p。
 * @attention 模数的有效字节数与 p->length 支持的规格之差不能大于2字节。
 * @param  [in]    a 入参，a->length 不能大于 p->length。
 * @param  [in]    b 入参，b->length 不能大于 p->length。
 * @param  [in]    p 模数，p->length 只能为 192/224/256/384/512/521/1024/1536/2048/3072/4096bits。
 * @param  [out]   c 计算结果，c->length 应该与 p->length 相同。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_sub_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c);

/**
 * @if Eng
 * @brief  Modular multiplication c = (a * b) mod p.
 * @attention The valid length of modulur shouldn't be two bytes less than the support length.
 * @param  [in]    a The first parameter, which length shouldn't be larger than p->length.
 * @param  [in]    b The second parameter, which length shouldn't be larger than p->length.
 * @param  [in]    p The modulur, which only support 192/256/384/512/1024/1536/2048/3072/4096bits,
 * and the modulur shouldn't be even.
 * @param  [out]   c The output data, which length should be equal to p->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  模乘 c = (a * b) mod p。
 * @attention 模数的有效字节数与 p->length 支持的规格之差不能大于2字节。
 * @param  [in]    a 入参，a->length 不能大于 p->length。
 * @param  [in]    b 入参，b->length 不能大于 p->length。
 * @param  [in]    p 模数，p->length 只能为 192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数。
 * @param  [out]   c 计算结果，c->length 应该与 p->length 相同。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_mul_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c);

/**
 * @if Eng
 * @brief  Modular inverse c = (a^-1) mod p.
 * @attention The valid length of modulur shouldn't be two bytes less than the support length.
 * @param  [in]    a The input parameter, which length should be equal to p->length.
 * @param  [in]    p The modulur, which only support 192/256/384/512/1024/1536/2048/3072/4096bits,
 * and the modulur shouldn't be even.
 * @param  [out]   c The output data, which length should be equal to p->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  模逆 c = (a^-1) mod p。
 * @attention 模数的有效字节数与 p->length 支持的规格之差不能大于2字节。
 * @param  [in]    a 入参，a->length 应该等于 p->length。
 * @param  [in]    p 模数，p->length 只能为 192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数。
 * @param  [out]   c 计算结果，c->length 应该等于 p->length。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_inv_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p,
    const uapi_drv_cipher_pke_data_t *c);

/**
 * @if Eng
 * @brief  Modular calculate c = a mod p.
 * @attention The valid length of modulur shouldn't be two bytes less than the support length.
 * @param  [in]    a The first parameter, which valid length shouldn't be larger than 2 * valid length of p->length.
 * @param  [in]    p The modulur, which only support 192/256/384/512/1024/1536/2048/3072/4096bits,
 * and the modulur shouldn't be even.
 * @param  [out]   c The output data, which length should be equal to p->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  取模运算 c = a mod p。
 * @attention 模数的有效字节数与 p->length 支持的规格之差不能大于2字节。
 * @param  [in]    a 入参，其有效数据长度不能大于 2 * 模数的有效数据长度。
 * @param  [in]    p 模数，p->length 只能为 192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数。
 * @param  [out]   c 计算结果，c->length 应该等于 p->length。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p,
    const uapi_drv_cipher_pke_data_t *c);

/**
 * @if Eng
 * @brief  Large number multiplication c = a * b.
 * @param  [in]    a The first parameter, which length shouldn't be larger than 2048bits.
 * @param  [in]    b The second parameter, which length shouldn't be larger than 2048bits.
 * @param  [out]   c The output data, which length shouldn't be less than a->length + b->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  大数乘 c = a * b。
 * @param  [in]    a 入参，a->length 不能大于2048比特。
 * @param  [in]    b 入参，b->length 不能大于2048比特。
 * @param  [out]   c 计算结果，c->length 不能小于 a->length + b->length。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_mul(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *c);

/**
 * @if Eng
 * @brief  Modular exponentiation out = (in ^ k) mod n.
 * @attention The valid length of modulur shouldn't be two bytes less than the support length.
 * @param  [in]    n The modulu, which only support 192/256/384/512/1024/1536/2048/3072/4096bits,
 * and the modulur shouldn't be even.
 * @param  [in]    k The power, which length shouldn't be larger than 4096bits.
 * @param  [in]    in The input data, which length shouldn't be larger than 4096bits.
 * @param  [out]   out The output data, which length should be equal to p->length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  模幂 out = (in ^ k) mod n。
 * @attention 模数的有效字节数与 n->length 支持的规格之差不能大于2字节。
 * @param  [in]    n 模数，n->length 只能为 192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数。
 * @param  [in]    k 指数，数据长度不能大于 4096 比特。
 * @param  [in]    in 输入数据，数据长度不能大于 4096 比特。
 * @param  [out]   out 输出结果，out->length 应该等于 n->length。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pke_exp_mod(const uapi_drv_cipher_pke_data_t *n, const uapi_drv_cipher_pke_data_t *k,
    const uapi_drv_cipher_pke_data_t *in, const uapi_drv_cipher_pke_data_t *out);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif