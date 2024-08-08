/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides efuse driver \n
 *
 * History: \n
 * 2022-07-26, Create file. \n
 */
#ifndef EFUSE_H
#define EFUSE_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define EFUSE_WRITE_PROTECT_FLAG            0x5A5A5A5A
/**
 * @defgroup drivers_driver_efuse eFuse
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the eFuse.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化eFuse。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the eFuse.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化eFuse。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_deinit(void);

#ifdef EFUSE_BIT_OPERATION
/**
 * @if Eng
 * @brief  Reads a bit from the eFuse.
 * @param  [in] value The value of the bit read.
 * @param  [in] byte_number The source OTP bit byte_number of the bit to be read.
 * @param  [in] bit_pos Position of the bit.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从eFuse中读取一位。
 * @param  [in] value 读取的位值。
 * @param  [in] byte_number 要读取位的源eFuse位地址。
 * @param  [in] bit_pos 位的位置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_read_bit(uint8_t *value, uint32_t byte_number, uint8_t bit_pos);
#endif

/**
 * @if Eng
 * @brief  Reads multiple bytes from the eFuse into the provided buffer.
 * @param  [in] buffer The value of the bit read.
 * @param  [in] byte_number The source eFuse bit byte_number of the bit to be read.
 * @param  [in] length The length of the data, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从eFuse中读取多个字节，进入提供的缓冲区。
 * @param  [in] buffer 保存读取数据的缓冲区。
 * @param  [in] byte_number 要读取的数据的初始源eFuse字节地址。
 * @param  [in] length 数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_read_buffer(uint8_t *buffer, uint32_t byte_number, uint16_t length);

#ifdef EFUSE_BIT_OPERATION
/**
 * @if Eng
 * @brief  Writes a bit to the eFuse.
 * @param  [in] byte_number The destination OTP bit byte_number of the bit to be written.
 * @param  [in] bit_pos A non-zero value indicates the bit will be set,
           a zero value indicates a write should not actually occur.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  向eFuse写入一位。
 * @param  [in] byte_number 要写入位的目标OTP位地址。
 * @param  [in] bit_pos 非零值表示将设置该位。零值表示不应实际发生写入。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_write_bit(uint32_t byte_number, uint8_t bit_pos);

/**
 * @if Eng
 * @brief  Writes a bit to the eFuse when the protection flag is correct.
 * @param  [in] byte_number The destination OTP bit byte_number of the bit to be written.
 * @param  [in] bit_pos A non-zero value indicates the bit will be set,
           a zero value indicates a write should not actually occur.
 * @param  [in] flag protection flag.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  在保护标志正确的情况下向eFuse写入一位。
 * @param  [in] byte_number 要写入位的目标OTP位地址。
 * @param  [in] bit_pos 非零值表示将设置该位。零值表示不应实际发生写入。
 * @param  [in] flag 保护标志。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_write_bit_with_flag(uint32_t byte_number, uint8_t bit_pos, uint32_t flag);
#endif

/**
 * @if Eng
 * @brief  Writes multiple bytes to the eFuse from the provided buffer.
 * @param  [in] byte_number The initial destination eFuse byte byte_number of the data to be written.
 * @param  [in] buffer A buffer containing the data to write.
 * @param  [in] length The length of the data, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从提供的缓冲区向eFuse写入多个字节。
 * @param  [in] byte_number 要写入数据的初始目的eFuse字节地址。
 * @param  [in] buffer 包含要写入的数据的缓冲区。
 * @param  [in] length 数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_write_buffer(uint32_t byte_number, const uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Writes multiple bytes to the eFuse from the provided buffer when the protection flag is correct.
 * @param  [in] byte_number The initial destination eFuse byte byte_number of the data to be written.
 * @param  [in] buffer A buffer containing the data to write.
 * @param  [in] length The length of the data, in bytes.
 * @param  [in] flag protection flag.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  在保护标志正确的情况下从提供的缓冲区向eFuse写入多个字节。
 * @param  [in] byte_number 要写入数据的初始目的eFuse字节地址。
 * @param  [in] buffer 包含要写入的数据的缓冲区。
 * @param  [in] length 数据的长度，以字节为单位。
 * @param  [in] flag 保护标志。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_write_buffer_with_flag(uint32_t byte_number, const uint8_t *buffer,
                                            uint16_t length, uint32_t flag);

/**
 * @if Eng
 * @brief  Obtains the Die-ID of the eFuse.
 * @param  [out] buffer OTP byte value of the die id.
 * @param  [in] length The length of the Die-ID, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取efuse的Die-ID。
 * @param  [in] buffer 保存读取数据的缓冲区
 * @param  [in] length Die-ID数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_get_die_id(uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Obtains the Chip-ID of the eFuse.
 * @param  [out] buffer OTP byte value of the chip id.
 * @param  [in] length The length of the chip_id, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取efuse的Chip-ID。
 * @param  [in] buffer 保存读取数据的缓冲区。
 * @param  [in] length Chip-ID数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_get_chip_id(uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Calculate efuse zero-counting CRC for a block.
 * @param  [in] buffer A buffer containing the data to write.
 * @param  [in] length The length of the Chip-ID, in bytes.
 * @param  [in] crc Buffer for storing CRC values.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  计算efuse零计数CRC。
 * @param  [in] buffer 保存读取数据的缓冲区。
 * @param  [in] length 数据长度，以字节为单位。
 * @param  [in] crc 保存CRC值的缓冲区
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_calc_crc(const uint8_t *buffer, uint8_t length, uint8_t *crc);

/**
 * @if Eng
 * @brief  Obtain SoC-ID.
 * @param  [in]  id The address of the returned value, ID is a length of 20 uint8_t type array.
 * @param  [in]  id_length The length of ID param, The length needs to be greater than 20 bytes.
 * @else
 * @brief  获取SoC-ID。
 * @param  [in]  id 用于存储返回的ID，ID是一个长度为20字节的数组。
 * @param  [in]  id_length ID数组的长度，这个长度必须是20字节以上的长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_soc_read_id(uint8_t *id, uint16_t id_length);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

