/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides efuse driver \n
 *
 * History: \n
 * 2024-02-08, Create file. \n
 */
#ifndef EFUSE_USER_H
#define EFUSE_USER_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_efuse eFuse
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Reads multiple bytes from the eFuse space reserved by the user into the provided buffer.
 * @param  [in] offset The Start offset address of the eFUSE space to be read in the area reserved for the user,
           The unit is byte.
 * @param  [in] buffer The buffer for storing read data.
 * @param  [in] length The length of the data, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从用户预留的eFuse空间中读取多个字节，进入提供的缓冲区。
 * @param  [in] offset 待读取的efuse空间在用户预留区域中的起始偏移地址，以字节为单位。
 * @param  [in] buffer 保存读取数据的缓冲区。
 * @param  [in] length 数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_user_read_buffer(uint32_t offset, uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Writes multiple bytes from the provided buffer to the eFuse space reserved by the user.
 * @param  [in] offset The start offset address of the eFUSE space to be written in the area reserved for the user.
           The unit is byte.
 * @param  [in] buffer A buffer containing the data to be written.
 * @param  [in] length The length of the data, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从提供的缓冲区向用户预留的eFuse空间写入多个字节。
 * @param  [in] offset 待写入的efuse空间在用户预留区域中的起始偏移地址，以字节为单位。
 * @param  [in] buffer 包含要写入的数据的缓冲区。
 * @param  [in] length 数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_user_write_buffer(uint32_t offset, const uint8_t *buffer, uint16_t length);

#ifdef EFUSE_BIT_OPERATION
/**
 * @if Eng
 * @brief  Write 1 to the corresponding bit in the eFUSE space reserved for the user.
 * @param  [in] byte_offset The byte offset address of the bit to be written in the space reserved for the user.
 * @param  [in] bit_pos The bit position of the bit to be written in the corresponding byte of the user reserved space.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  向用户预留eFuse空间中的对应bit写1。
 * @param  [in] byte_offset 待写入位在用户预留空间中的字节偏移地址。
 * @param  [in] bit_pos 待写入位在用户预留空间中对应字节中的bit位置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_user_write_bit(uint32_t byte_offset, uint8_t bit_pos);

/**
 * @if Eng
 * @brief  Read a bit from the eFuse space reserved by the user.
 * @param  [in] byte_offset The byte offset address of the bit to be read in the space reserved for the user.
 * @param  [in] bit_pos The bit position of the bit to be read in the corresponding byte of the user reserved space.
 * @param  [in] value The value of the bit read.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从用户预留的eFuse空间中读取一位。
 * @param  [in] byte_offset 待读取位在用户预留空间中的字节偏移地址。
 * @param  [in] bit_pos 待读取位在用户预留空间中对应字节中的bit位置。
 * @param  [in] value 读取的位值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_user_read_bit(uint32_t byte_offset, uint8_t bit_pos, uint8_t *value);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

