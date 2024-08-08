/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides cipher mac driver header \n
 *
 * History: \n
 * 2024-01-24, Create file. \n
 */
#ifndef CIPHER_MAC_H
#define CIPHER_MAC_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_cipher_mac Cipher MAC
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief  AES-CMAC compute MAC.
 * @param  [in]    data Data of the MAC value to be calculated.
 * @param  [in]    data_len Data length, in bytes.
 * @param  [in]    key Decryption key.
 * @param  [in]    key_len Decryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the decryption key.
 * @param  [out]   out_mac Stores the calculated MAC value. The capacity is 16 bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two methods to set the key:
            1）Using key/key_len, in which case, the keyslot_handle is set to 0.
            2) Calling keyslot api to create one keyslot_handle and set it, in which case,
                the key is set to NULL and key_len is 0.
            When keyslot is supported, you must use method 2, otherwise you should use method 1.
 * @else
 * @brief  AES-CMAC 计算 MAC 值.
 * @param  [in]    data 待计算 MAC 值的数据.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @param  [in]    key 解密密钥.
 * @param  [in]    key_len 解密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存解密密钥的 keyslot 句柄.
 * @param  [out]   out_mac 保存计算的 MAC 值，容量为 16 字节.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 0.
            2）使用 keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL，key_len 传入为 0.
            若支持 keyslot，则必须使用方法2，否则只能使用方法1.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_aes_cmac(const uint8_t *data, uint32_t data_len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    uint8_t out_mac[16]);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
