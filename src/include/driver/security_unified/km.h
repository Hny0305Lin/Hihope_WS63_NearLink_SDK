/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides KM driver header \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef KM_H
#define KM_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_km KM
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief  Keyslot type selection.
 * @else
 * @brief  keyslot 类型选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KEYSLOT_TYPE_MCIPHER = 0,
    UAPI_DRV_KEYSLOT_TYPE_HMAC,
    UAPI_DRV_KEYSLOT_TYPE_FLASH,
} uapi_drv_keyslot_type_t;

/**
 * @if Eng
 * @brief  Root key selection during KDF key derivation.
 * @else
 * @brief  KDF 密钥派生时根密钥选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KDF_OTP_KEY_MRK1 = 0,
    UAPI_DRV_KDF_OTP_KEY_USK,
    UAPI_DRV_KDF_OTP_KEY_RUSK
} uapi_drv_kdf_otp_key_t;
 
/**
 * @if Eng
 * @brief  Symmetric algorithm selection during KDF key derivation.
 * @else
 * @brief  KDF 密钥派生时对称算法选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KDF_UPDATE_ALG_AES = 0,
    UAPI_DRV_KDF_UPDATE_ALG_SM4
} uapi_drv_kdf_update_alg_t;
 
/**
 * @if Eng
 * @brief  Select the derived key type during KDF key derivation.
 * @else
 * @brief  KDF 密钥派生时派生key类型选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK0  = 0x03000000,
    UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_SBRK2,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK2,
    UAPI_DRV_KDF_HARD_KEY_TYPE_DRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_DRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_PSK,
    UAPI_DRV_KDF_HARD_KEY_TYPE_FDRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ODRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ODRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_OARK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK0,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK1,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK2,
    UAPI_DRV_KDF_HARD_KEY_TYPE_MDRK3,

    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_REE,
    UAPI_DRV_KDF_HARD_KEY_TYPE_ABRK_TEE,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_REE,
    UAPI_DRV_KDF_HARD_KEY_TYPE_RDRK_TEE,
} uapi_drv_kdf_hard_key_type_t;
 
/**
 * @if Eng
 * @brief  KDF key derivation, hash algorithm selection when the hardware PBKDF2 algorithm is used.
 * @else
 * @brief  KDF 密钥派生，硬件PBKDF2算法时HASH算法选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KDF_HARD_ALG_SHA256 = 0,
    UAPI_DRV_KDF_HARD_ALG_SM3,
    UAPI_DRV_KDF_HARD_ALG_MAX
} uapi_drv_kdf_hard_alg_t;
 
/**
 * @if Eng
 * @brief  The klad target module's algorithm engine, determining the algorithm supported by the sent key.
 * @else
 * @brief  klad 目标模块算法引擎，决定送出的 key 支持哪个算法。
 * @endif
 */
typedef enum {
    UAPI_DRV_KLAD_ENGINE_AES = 0x20,
    UAPI_DRV_KLAD_ENGINE_LAE = 0x40,
    UAPI_DRV_KLAD_ENGINE_SM4 = 0x50,
    UAPI_DRV_KLAD_ENGINE_TDES = 0x70,
    UAPI_DRV_KLAD_ENGINE_SHA1_HMAC = 0xA0,
    UAPI_DRV_KLAD_ENGINE_SHA2_HMAC = 0xA1,
    UAPI_DRV_KLAD_ENGINE_SM3_HMAC = 0xA2,
    UAPI_DRV_KLAD_ENGINE_MAX
} uapi_drv_klad_engine_t;
 
/**
 * @if Eng
 * @brief  The klad target module，determining the module to which the key is sent.
 * @else
 * @brief  klad 目标模块，决定 key 送给哪个模块使用。
 * @endif
 */
typedef enum {
    UAPI_DRV_KLAD_DEST_MCIPHER = 0,
    UAPI_DRV_KLAD_DEST_HMAC,
    UAPI_DRV_KLAD_DEST_FLASH,
    UAPI_DRV_KLAD_DEST_NPU,
    UAPI_DRV_KLAD_DEST_AIDSP,
    UAPI_DRV_KLAD_DEST_MAX,
} uapi_drv_klad_dest_t;
 
/**
 * @if Eng
 * @brief  Flash online decryption mode, determining the mode used after the key is sent.
 * @else
 * @brief  Flash 在线解密模式，决定送 key 后使用哪种模式
 * @endif
 */
typedef enum {
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_REE_DEC = 0x00,  /* REE flash online decryption key */
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_TEE_DEC,         /* TEE flash online decryption key */
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_TEE_AUT,         /* TEE flash online authentication key */
    UAPI_DRV_KLAD_FLASH_KEY_TYPE_INVALID,
} uapi_drv_klad_flash_key_type_t;
 
/**
 * @if Eng
 * @brief  Symmetric key length. Determines the length of the final working key.
 * @else
 * @brief  对称密钥长度。决定了最终的工作密钥的长度
 * @endif
 */
typedef enum {
    UAPI_DRV_KLAD_KEY_SIZE_128BIT,
    UAPI_DRV_KLAD_KEY_SIZE_192BIT,
    UAPI_DRV_KLAD_KEY_SIZE_256BIT,
    UAPI_DRV_KLAD_KEY_SIZE_INVALID = 0xffffffff
} uapi_drv_klad_key_size_t;
 
/**
 * @if Eng
 * @brief  When the target engine is HMAC, determine the HAMC algorithm to be used.
 * @note  SHA1 and SHA224 are insecure and are not recommended..
 * @else
 * @brief  当目标引擎为HMAC时，决定具体使用的HAMC算法。
 * @note  SHA1和SHA224模式不安全，不推荐使用.
 * @endif
 */
typedef enum {
    UAPI_DRV_KLAD_HMAC_TYPE_SHA1 = 0x20,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA224,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA256,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA384,
    UAPI_DRV_KLAD_HMAC_TYPE_SHA512,
    UAPI_DRV_KLAD_HMAC_TYPE_SM3 = 0x30,
    UAPI_DRV_KLAD_HMAC_TYPE_MAX,
    UAPI_DRV_KLAD_HMAC_TYPE_INVALID = 0xffffffff,
} uapi_drv_klad_hmac_type_t;
 
/**
 * @if Eng
 * @brief  Clear key structure when klad sends a clear key.
 * @else
 * @brief  klad送明文key时，明文key的结构。
 * @endif
 */
typedef struct {
    uint8_t *key;     /*!< @if Eng Clear key content.
                        @else   明文key内容。 @endif */
    uint32_t key_length;  /*!< @if Eng Length of the clear key, in bytes.
                                For the symmetric algorithm, the value can only be 16, 24, or 32.
                                For HMAC-SH1/SHA224/SHA256/SM3, the value cannot exceed 64.
                                For HMAC-SHA384/SHA512, the value cannot exceed 128.
                            @else   明文key长度，单位为字节。 对于对称算法，只能是16/24/32；
                                对于HMAC-SH1/SHA224/SHA256/SM3，长度不超过64；
                                对于HMAC-SHA384/SHA512，长度不超过128。@endif */
    bool key_parity; /*!< @if Eng Indicates the parity attribute of a key.
                                Valid when the target is a symmetric algorithm engine and key_length is set to 16.
                            @else   key的奇偶属性。当目标为对称算法引擎且key_length为16时生效。 @endif */
    uapi_drv_klad_hmac_type_t hmac_type; /*!< @if Eng Indicates the HMAC algorithm.
                                            Valid only when the target is the HMAC algorithm engine..
                                        @else   hmac 算法。当目标为HMAC算法引擎时生效。 @endif */
} uapi_drv_klad_clear_key_t;
 
/**
 * @if Eng
 * @brief  Keyladder root key type selection.
 * @else
 * @brief  Keyladder 根密钥类型选择。
 * @endif
 */
typedef struct {
    uapi_drv_kdf_hard_key_type_t rootkey_type;     /*!< @if Eng Type of rootkey to generate.
                                                        @else   要生成的根密钥的类型。 @endif */
} uapi_drv_klad_config_t;
 
/**
 * @if Eng
 * @brief  Keyladder working key attribute configuration.
 * @else
 * @brief  Keyladder 工作密钥属性配置。
 * @endif
 */
typedef struct {
    uapi_drv_klad_engine_t engine;  /*!< @if Eng The working key can be used for which algorithm of the crypto engine.
                                    @else   工作密钥可用于加密引擎的哪种算法。 @endif */
    bool decrypt_support;    /*!< @if Eng The working key can be used for decrypting.
                                    @else   工作密钥可用于解密。 @endif */
    bool encrypt_support;    /*!< @if Eng The working key can be used for encrypting.
                                    @else   工作密钥可用于加密。 @endif */
} uapi_drv_klad_key_config_t;
 
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
    bool key_sec;    /*!< @if Eng Secure key can only be used by TEE CPU and AIDSP locked cipher and hash channel.
                            @else   安全密钥只能由TEE CPU和AIDSP锁定的对称通道或哈希通道使用。 @endif */
    bool master_only_enable; /*!< @if Eng Only the cipher or hash channel which is locked by same CPU as keyladder
                                        can use this key, valid only for TEE CPU and AIDSP.
                                    @else   只有与Keylader相同的CPU锁定的密码或哈希通道才能使用此密钥，
                                        当TEE CPU或AIDSP时生效。 @endif */
    bool dest_buf_sec_support;   /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的目标缓冲区可以是安全的。 @endif */
    bool dest_buf_non_sec_support; /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的目标缓冲区可以是非安全的。 @endif */
    bool src_buf_sec_support;      /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的源缓冲区可以是安全的。 @endif */
    bool src_buf_non_sec_support;  /*!< @if Eng The destination buffer of target engine can be secure.
                                        @else   目标引擎的源缓冲区可以是非安全的。 @endif */
} uapi_drv_klad_key_secure_config_t;
 
/**
 * @if Eng
 * @brief  Keyladder configuration attributes.
 * @else
 * @brief  Keyladder 配置属性。
 * @endif
 */
typedef struct {
    uapi_drv_klad_config_t klad_cfg;    /*!< @if Eng The keyladder configuration, valid for harware key.
                                        @else   KeyLader配置，对硬件密钥有效。 @endif */
    uapi_drv_klad_key_config_t key_cfg; /*!< @if Eng The working key configuration.
                                        @else   工作密钥配置。 @endif */
    uapi_drv_klad_key_secure_config_t key_sec_cfg;  /*!< @if Eng The working key security configuration.
                                                    @else   工作密钥安全配置。 @endif */
    uint32_t rkp_sw_cfg;                /*!< @if Eng Used by the NPU module.
                                                Other modules do not need to be configured.
                                            @else   NPU模块使用，其他模块不需要配置。 @endif */
} uapi_drv_klad_attr_t;

/**
 * @if Eng
 * @brief  Keyladder hardware key configuration attributes.
 * @else
 * @brief  Keyladder硬件key参数配置。
 * @endif
 */
typedef struct {
    uapi_drv_kdf_hard_alg_t kdf_hard_alg;   /*!< @if Eng HMAC algorithm used for key derivation.
                                                @else   key派生时使用的hmac算法。 @endif */
    bool key_parity; /*!< @if Eng Indicates the parity attribute of a key.
                                Valid when the target is a symmetric algorithm engine and key_length is set to 16.
                            @else   key的奇偶属性。当目标为对称算法引擎且key_length为16时生效。 @endif */
    uapi_drv_klad_key_size_t key_size;  /*!< @if Eng Length of the key to be derived.
                                                @else   需要派生的key的长度。 @endif */
    uint8_t *salt;    /*!< @if Eng Salt content. Used as user input materials for key derivation.
                            The final working key varies according to the salt value.
                        @else   盐值内容。作为用户输入材料参与密钥派生，盐值不同，最终的工作密钥也不同。 @endif */
    uint32_t salt_length; /*!< @if Eng Salt length, in bytes. It can only be 28.
                            @else   盐值长度，单位是字节。只能为28。 @endif */
    bool oneway; /*!< @if Eng Default zero. if set 1, you'll not generate the same key,
                            even if use the same derived materials.
                    @else 密钥派生的单一性，默认为0。如果设置为1，即使使用相同的密钥派生材料也无法派生出相同的密钥。 @endif */
} uapi_drv_klad_effective_key_t;

/**
 * @if Eng
 * @brief  KM initialization.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  KM初始化。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_km_init(void);

/**
 * @if Eng
 * @brief  KM deinitialization.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  KM去初始化。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_km_deinit(void);

/**
 * @if Eng
 * @brief  Creates a keyslot handle.
 * @param  [out] keyslot_handle The channel handle for storing key.
 * @param  [in]  keyslot_type Key usage type.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  创建keyslot句柄。
 * @param  [out] keyslot_handle 用于保存key的句柄。
 * @param  [in]  keyslot_type key的用途类型。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_keyslot_create(uint32_t *keyslot_handle, uapi_drv_keyslot_type_t keyslot_type);

/**
 * @if Eng
 * @brief  Destroy the keyslot handle.
 * @param  [in]  keyslot_handle Channel handle of the key to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  销毁keyslot句柄。
 * @param  [in]  keyslot_handle 要销毁的key的通道句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_keyslot_destroy(uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Creates a key ladder channel handle.
 * @param  [out]  klad_handle Handle of the keyladder channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  创建keyladder通道句柄。
 * @param  [out]  klad_handle keyladder通道句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_create(uint32_t *klad_handle);

/**
 * @if Eng
 * @brief  Destroy the key ladder channel handle.
 * @param  [in]  klad_handle Handle of the keyladder channel to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  销毁keyladder通道句柄。
 * @param  [in]  klad_handle 要销毁的keyladder通道句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_destroy(uint32_t klad_handle);

/**
 * @if Eng
 * @brief  Associate the keyslot handle with the klad handle.
 * @param  [in]  klad_handle Handle of the key ladder channel to be associated.
 * @param  [in]  klad_type Klad target module.
 * @param  [in]  keyslot_handle Keyslot channel handle to be associated.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  将keyslot句柄与klad句柄关联上。
 * @param  [in]  klad_handle 要关联的keyladder通道句柄。
 * @param  [in]  klad_type klad 目标模块。
 * @param  [in]  keyslot_handle 要关联的keyslot通道句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_attach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Disassociate the keyslot handle from the klad handle.
 * @param  [in]  klad_handle Handle of the key ladder channel to be deassociated.
 * @param  [in]  klad_type Klad target module.
 * @param  [in]  keyslot_handle Keyslot channel handle to be deassociated.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  将keyslot句柄与klad句柄解关联。
 * @param  [in]  klad_handle 要解关联的keyladder通道句柄。
 * @param  [in]  klad_type klad 目标模块。
 * @param  [in]  keyslot_handle 要解关联的keyslot通道句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_detach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Sets the attributes of the keyladder.
 * @param  [in]  klad_handle Keyladder channel handle
 * @param  [in]  attr Keyladder configuration attributes
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置keyladder的属性。
 * @param  [in]  klad_handle Keyladder通道句柄。
 * @param  [in]  attr Keyladder配置属性。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_set_attr(uint32_t klad_handle, const uapi_drv_klad_attr_t *attr);

/**
 * @if Eng
 * @brief  Gets the attributes of the keyladder.
 * @param  [in]   klad_handle Keyladder channel handle
 * @param  [out]  attr Keyladder configuration attributes
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取keyladder的属性。
 * @param  [in]   klad_handle Keyladder通道句柄。
 * @param  [out]  attr Keyladder配置属性。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_get_attr(uint32_t klad_handle, uapi_drv_klad_attr_t *attr);

/**
 * @if Eng
 * @brief  Sets effective key.
 * @param  [in]   klad_handle Keyladder channel handle
 * @param  [out]  key Effective key configuration attributes
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置effective key。
 * @param  [in]   klad_handle Keyladder通道句柄。
 * @param  [out]  key Effective key配置属性。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_set_effective_key(uint32_t klad_handle, const uapi_drv_klad_effective_key_t *key);

/**
 * @if Eng
 * @brief  Sets effective key.
 * @param  [in]   klad_handle Keyladder channel handle
 * @param  [out]  key Clear key configuration attributes
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置clear key。
 * @param  [in]   klad_handle Keyladder通道句柄。
 * @param  [out]  key Clear key配置属性。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_klad_set_clear_key(uint32_t klad_handle, const uapi_drv_klad_clear_key_t *key);

/**
 * @if Eng
 * @brief  Km deob update.
 * @param  [in]   otp_key Type of the OTP key to be updated
 * @param  [in]   alg Algorithm type used by the update
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  更新根密钥。
 * @param  [in]   otp_key 要更新的密钥类型
 * @param  [in]   alg 更新时的算法类型
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_kdf_update(uapi_drv_kdf_otp_key_t otp_key, uapi_drv_kdf_update_alg_t alg);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
