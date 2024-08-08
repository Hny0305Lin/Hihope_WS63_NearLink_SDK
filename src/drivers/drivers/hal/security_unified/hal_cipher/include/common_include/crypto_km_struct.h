/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: km struct.
 *
 * Create: 2023-01-18
*/
#ifndef CRYPTO_KM_STRUCT_H
#define CRYPTO_KM_STRUCT_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif  /* __cplusplus */

#define  SESSION_KEY_LEN 16

typedef td_bool (*drv_kdf_wait_condition_func)(const td_void *param);

typedef td_bool (*drv_klad_wait_condition_func)(const td_void *param);

typedef td_s32 (*osal_kdf_wait_timeout_uninterruptible)(const td_void *wait, drv_kdf_wait_condition_func func,
    const td_void *param, const td_u32 timeout_ms);

typedef td_s32 (*osal_klad_wait_timeout_uninterruptible)(const td_void *wait, drv_klad_wait_condition_func func,
    const td_void *param, const td_u32 timeout_ms);

/**
 * @if Eng
 * @brief  Root key selection during KDF key derivation.
 * @else
 * @brief  KDF 密钥派生时根密钥选择。
 * @endif
 */
typedef enum {
    CRYPTO_KDF_OTP_KEY_MRK1 = 0,
    CRYPTO_KDF_OTP_KEY_USK,
    CRYPTO_KDF_OTP_KEY_RUSK,
    CRYPTO_KDF_OTP_KEY_MAX
} crypto_kdf_otp_key;
 
/**
 * @if Eng
 * @brief  Symmetric algorithm selection during KDF key derivation.
 * @else
 * @brief  KDF 密钥派生时对称算法选择。
 * @endif
 */
typedef enum {
    CRYPTO_KDF_UPDATE_ALG_AES = 0,
    CRYPTO_KDF_UPDATE_ALG_SM4,
    CRYPTO_KDF_UPDATE_ALG_MAX
} crypto_kdf_update_alg;
 
/**
 * @if Eng
 * @brief  Hash algorithm selection when the software PBKDF2 algorithm is used.
 * @else
 * @brief  软件PBKDF2算法时HASH算法选择。
 * @endif
 */
typedef enum {
    CRYPTO_KDF_SW_ALG_SHA1 = 0,
    CRYPTO_KDF_SW_ALG_SHA256,
    CRYPTO_KDF_SW_ALG_SHA384,
    CRYPTO_KDF_SW_ALG_SHA512,
    CRYPTO_KDF_SW_ALG_SM3
} crypto_kdf_sw_alg;
 
/**
 * @if Eng
 * @brief  Select the derived key type during KDF key derivation.
 * @else
 * @brief  KDF 密钥派生时派生key类型选择。
 * @endif
 */
typedef enum {
    CRYPTO_KDF_HARD_KEY_TYPE_SBRK0  = 0x03000000,
    CRYPTO_KDF_HARD_KEY_TYPE_SBRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_SBRK2,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK2,
    CRYPTO_KDF_HARD_KEY_TYPE_DRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_DRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_PSK,
    CRYPTO_KDF_HARD_KEY_TYPE_FDRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_ODRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_ODRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_OARK0,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK2,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK3,

    CRYPTO_KDF_HARD_KEY_TYPE_ABRK_REE,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK_TEE,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK_REE,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK_TEE,
} crypto_kdf_hard_key_type;
 
/**
 * @if Eng
 * @brief  KDF key derivation, hash algorithm selection when the hardware PBKDF2 algorithm is used.
 * @else
 * @brief  KDF 密钥派生，硬件PBKDF2算法时HASH算法选择。
 * @endif
 */
typedef enum {
    CRYPTO_KDF_HARD_ALG_SHA256 = 0,
    CRYPTO_KDF_HARD_ALG_SM3,
    CRYPTO_KDF_HARD_ALG_MAX
} crypto_kdf_hard_alg;

typedef enum {
    CRYPTO_KDF_HARD_KEY_SIZE_128BIT = 0,
    CRYPTO_KDF_HARD_KEY_SIZE_192BIT,
    CRYPTO_KDF_HARD_KEY_SIZE_256BIT,
} crypto_kdf_hard_key_size;
 
typedef struct {
    crypto_kdf_hard_key_type hard_key_type;
    crypto_kdf_hard_alg hard_alg;
    crypto_kdf_hard_key_size hard_key_size;
    td_u8 *salt;
    td_u32 salt_length;
    td_bool is_oneway;
} crypto_kdf_hard_calc_param;
 
/**
 * @if Eng
 * @brief  The klad target module's algorithm engine, determining the algorithm supported by the sent key.
 * @else
 * @brief  klad 目标模块算法引擎，决定送出的 key 支持哪个算法。
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_ENGINE_AES = 0x20,
    CRYPTO_KLAD_ENGINE_SM4 = 0x50,
    CRYPTO_KLAD_ENGINE_TDES = 0x70,
    CRYPTO_KLAD_ENGINE_SHA1_HMAC = 0xA0,
    CRYPTO_KLAD_ENGINE_SHA2_HMAC = 0xA1,
    CRYPTO_KLAD_ENGINE_SM3_HMAC = 0xA2,
    CRYPTO_KLAD_ENGINE_MAX
} crypto_klad_engine;

typedef enum {
    CRYPTO_KLAD_SEC_DISABLE = 0,
    CRYPTO_KLAD_SEC_ENABLE,
    CRYPTO_KLAD_SEC_MAX,
    CRYPTO_KLAD_SEC_INVALID = 0xffffffff,
} crypto_klad_sec;

/* * Key parity, valid when key length is not more than 128bit */
/* * CNcomment: 密钥奇偶属性 */
typedef enum {
    CRYPTO_KLAD_KEY_EVEN =  0x0,   /* *< even key  */
    CRYPTO_KLAD_KEY_ODD  =  0x1,   /* *< odd key */
    CRYPTO_KLAD_KEY_PARITY_MAX,
    CRYPTO_KLAD_KEY_PARITY_INVALID = 0xffffffff,
} crypto_klad_key_parity;
 
/**
 * @if Eng
 * @brief  The klad target module，determining the module to which the key is sent.
 * @else
 * @brief  klad 目标模块，决定 key 送给哪个模块使用。
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_DEST_MCIPHER = 0,
    CRYPTO_KLAD_DEST_HMAC,
    CRYPTO_KLAD_DEST_FLASH,
    CRYPTO_KLAD_DEST_NPU,
    CRYPTO_KLAD_DEST_AIDSP,
    CRYPTO_KLAD_DEST_MAX,
} crypto_klad_dest;
 
/**
 * @if Eng
 * @brief  Flash online decryption mode, determining the mode used after the key is sent.
 * @else
 * @brief  Flash 在线解密模式，决定送 key 后使用哪种模式
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_FLASH_KEY_TYPE_REE_DEC = 0x00,  /* REE flash online decryption key */
    CRYPTO_KLAD_FLASH_KEY_TYPE_TEE_DEC,         /* TEE flash online decryption key */
    CRYPTO_KLAD_FLASH_KEY_TYPE_TEE_AUT,         /* TEE flash online authentication key */
    CRYPTO_KLAD_FLASH_KEY_TYPE_INVALID,
} crypto_klad_flash_key_type;
 
/**
 * @if Eng
 * @brief  Symmetric key length. Determines the length of the final working key.
 * @else
 * @brief  对称密钥长度。决定了最终的工作密钥的长度
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_KEY_SIZE_128BIT,
    CRYPTO_KLAD_KEY_SIZE_192BIT,
    CRYPTO_KLAD_KEY_SIZE_256BIT,
} crypto_klad_key_size;
 
/**
 * @if Eng
 * @brief  When the target engine is HMAC, determine the HAMC algorithm to be used.
 * @else
 * @brief  当目标引擎为HMAC时，决定具体使用的HAMC算法。
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_HMAC_TYPE_SHA1 = 0x20, /*!< @if Eng Insecure algorithm, not recommended.
                                            @else   不安全算法，不推荐使用。 @endif */
    CRYPTO_KLAD_HMAC_TYPE_SHA224,
    CRYPTO_KLAD_HMAC_TYPE_SHA256,
    CRYPTO_KLAD_HMAC_TYPE_SHA384,
    CRYPTO_KLAD_HMAC_TYPE_SHA512,
    CRYPTO_KLAD_HMAC_TYPE_SM3 = 0x30,
    CRYPTO_KLAD_HMAC_TYPE_MAX,
    CRYPTO_KLAD_HMAC_TYPE_INVALID = 0xffffffff,
} crypto_klad_hmac_type;
 
/**
 * @if Eng
 * @brief  Determines the current derived key level during klad key derivation.
 * @else
 * @brief  klad密钥派生时，决定当前派生的密钥层级。
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_LEVEL_SEL_FIRST = 0,
    CRYPTO_KLAD_LEVEL_SEL_SECOND
} crypto_klad_level_sel;
 
/**
 * @if Eng
 * @brief  Determines the symmetric algorithm used for derivation during klad key derivation.
 * @else
 * @brief  klad密钥派生时，决定当前派生使用的对称算法。
 * @endif
 */
typedef enum {
    CRYPTO_KLAD_ALG_SEL_TDES = 0,
    CRYPTO_KLAD_ALG_SEL_AES,
    CRYPTO_KLAD_ALG_SEL_SM4,
} crypto_klad_alg_sel;
 
/**
 * @if Eng
 * @brief  Clear key structure when klad sends a clear key.
 * @else
 * @brief  klad送明文key时，明文key的结构。
 * @endif
 */
typedef struct {
    td_u8 *key;     /*!< @if Eng Clear key content.
                        @else   明文key内容。 @endif */
    td_u32 key_length;  /*!< @if Eng Length of the clear key, in bytes.
                                For the symmetric algorithm, the value can only be 16, 24, or 32.
                                For HMAC-SH1/SHA224/SHA256/SM3, the value cannot exceed 64.
                                For HMAC-SHA384/SHA512, the value cannot exceed 128.
                            @else   明文key长度，单位为字节。 对于对称算法，只能是16/24/32；
                                对于HMAC-SH1/SHA224/SHA256/SM3，长度不超过64；
                                对于HMAC-SHA384/SHA512，长度不超过128。@endif */
    td_bool key_parity; /*!< @if Eng Indicates the parity attribute of a key.
                                Valid when the target is a symmetric algorithm engine and key_length is set to 16.
                            @else   key的奇偶属性。当目标为对称算法引擎且key_length为16时生效。 @endif */
    crypto_klad_hmac_type hmac_type; /*!< @if Eng Indicates the HMAC algorithm.
                                            Valid only when the target is the HMAC algorithm engine..
                                        @else   hmac 算法。当目标为HMAC算法引擎时生效。 @endif */
} crypto_klad_clear_key;
 
/**
 * @if Eng
 * @brief  Keyladder root key type selection.
 * @else
 * @brief  Keyladder 根密钥类型选择。
 * @endif
 */
typedef struct {
    crypto_kdf_hard_key_type rootkey_type;
} crypto_klad_config;
 
/**
 * @if Eng
 * @brief  Keyladder working key attribute configuration.
 * @else
 * @brief  Keyladder 工作密钥属性配置。
 * @endif
 */
typedef struct {
    crypto_klad_engine engine;  /*!< @if Eng The working key can be used for which algorithm of the crypto engine.
                                    @else   工作密钥可用于加密引擎的哪种算法。 @endif */
    td_bool decrypt_support;    /*!< @if Eng The working key can be used for decrypting.
                                    @else   工作密钥可用于解密。 @endif */
    td_bool encrypt_support;    /*!< @if Eng The working key can be used for encrypting.
                                    @else   工作密钥可用于加密。 @endif */
} crypto_klad_key_config;
 
/**
 * @if Eng
 * @brief  Security attribute of the key.
    when cipher work mode is CBC_MAC, dest_buf_sec_support and dest_buf_non_sec_support cannot be both false
 * @else
 * @brief  key 的安全属性。
    当加密工作模式为CBC_MAC时，dest_buf_sec_support和dest_buf_non_sec_support不能同时为false。
 * @endif
 */
typedef struct {
    td_bool key_sec;    /*!< @if Eng Secure key can only be used by TEE CPU and AIDSP locked cipher and hash channel.
                            @else   安全密钥只能由TEE CPU和AIDSP锁定的对称通道或哈希通道使用。 @endif */
    td_bool master_only_enable; /*!< @if Eng Only the cipher or hash channel which is locked by same CPU as keyladder
                                        can use this key, valid only for TEE CPU and AIDSP.
                                    @else   只有与Keylader相同的CPU锁定的密码或哈希通道才能使用此密钥，
                                        当TEE CPU或AIDSP时生效。 @endif */
    td_bool dest_buf_sec_support;   /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的目标缓冲区可以是安全的。 @endif */
    td_bool dest_buf_non_sec_support; /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的目标缓冲区可以是非安全的。 @endif */
    td_bool src_buf_sec_support;      /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的源缓冲区可以是安全的。 @endif */
    td_bool src_buf_non_sec_support;  /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的源缓冲区可以是非安全的。 @endif */
} crypto_klad_key_secure_config;
 
/**
 * @if Eng
 * @brief  Keyladder configuration attributes.
 * @else
 * @brief  Keyladder 配置属性。
 * @endif
 */
typedef struct {
    crypto_klad_config klad_cfg;    /*!< @if Eng The keyladder configuration, valid for harware key.
                                        @else   KeyLader配置，对硬件密钥有效。 @endif */
    crypto_klad_key_config key_cfg; /*!< @if Eng The working key configuration.
                                        @else   工作密钥配置。 @endif */
    crypto_klad_key_secure_config key_sec_cfg;  /*!< @if Eng The working key security configuration.
                                                    @else   工作密钥安全配置。 @endif */
    td_u32 rkp_sw_cfg;
} crypto_klad_attr;

/**
 * @if Eng
 * @brief  Keyladder hardware key configuration attributes.
 * @else
 * @brief  Keyladder硬件key参数配置。
 * @endif
 */
typedef struct {
    crypto_kdf_hard_alg kdf_hard_alg;
    td_bool key_parity; /*!< @if Eng Indicates the parity attribute of a key.
                                Valid when the target is a symmetric algorithm engine and key_length is set to 16.
                            @else   key的奇偶属性。当目标为对称算法引擎且key_length为16时生效。 @endif */
    crypto_klad_key_size key_size;
    td_u8 *salt;    /*!< @if Eng Salt content. Used as user input materials for key derivation.
                            The final working key varies according to the salt value.
                        @else   盐值内容。作为用户输入材料参与密钥派生，盐值不同，最终的工作密钥也不同。 @endif */
    td_u32 salt_length; /*!< @if Eng Salt length, in bytes. It can only be 28.
                            @else   盐值长度，单位是字节。只能为28。 @endif */
    td_bool oneway; /*!< @if Eng Salt length, in bytes.
                        @else   盐值长度，单位是字节。 @endif */
} crypto_klad_effective_key;

/**
 * @if Eng
 * @brief  Session key configuration attributes.
 * @else
 * @brief  Session key参数配置。
 * @endif
 */
typedef struct {
    td_u8 key[SESSION_KEY_LEN];    /*!< @if Eng Session key.
                                        @else   会话密钥。 @endif */
    td_u32 key_length;              /*!< @if Eng Session key length.
                                        @else   会话密钥长度。 @endif */
    crypto_klad_level_sel level;    /*!< @if Eng Derived key hierarchy.
                                        @else   派生的密钥层级。 @endif */
    crypto_klad_alg_sel alg;        /*!< @if Eng Symmetric algorithm used in derivation.
                                        @else  派生使用的对称算法。 @endif */
} crypto_klad_session_key;

/**
 * @if Eng
 * @brief  Keyslot type selection.
 * @else
 * @brief  keyslot 类型选择。
 * @endif
 */
typedef enum {
    CRYPTO_KEYSLOT_TYPE_MCIPHER = 0,
    CRYPTO_KEYSLOT_TYPE_HMAC,
    CRYPTO_KEYSLOT_TYPE_FLASH,
} crypto_keyslot_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

#endif /* OT_KM_STRUCT_H */
