/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hal efuse \n
 *
 * History: \n
 * 2022-07-26, Create file. \n
 */

#ifndef HAL_EFUSE_H
#define HAL_EFUSE_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"
#include "efuse_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse_api Efuse API
 * @ingroup  drivers_hal_efuse
 * @{
 */

/**
 * @if Eng
 * @brief  Init device for hal efuse.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的初始化接口
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_init_t)(void);

/**
 * @if Eng
 * @brief  Deinit device for hal efuse.
 * @else
 * @brief  HAL层EFUSE的去初始化接口
 * @endif
 */
typedef void (*hal_efuse_deinit_t)(void);

/**
 * @if Eng
 * @brief  Flush select otp region, write the value in register to efuse.
 * @param  [in]  region Select OTP region
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的刷新写入接口
 * @param  [in]  region 选择OTP区域
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_flush_write_t)(hal_efuse_region_t region);

/**
 * @if Eng
 * @brief  Refresh read region to read mode after write.
 * @param  [in]  region Select OTP region
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的刷新读取接口
 * @param  [in]  region 选择OTP区域
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_refresh_read_t)(hal_efuse_region_t region);

/**
 * @if Eng
 * @brief  Reads the byte at the given OTP memory location
 * @param  [in]  byte_address OTP byte address of the byte to be read
 * @param  [out]  value       OTP byte value of the byte to be read
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的读单字节接口
 * @param  [in]  byte_address 要读取的字节的OTP字节地址
 * @param  [in]  value 要读取的字节的OTP字节值.
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_read_byte_t)(uint32_t byte_address, uint8_t *value);

/**
 * @if Eng
 * @brief  Writes the value to the given OTP memory location
 * @param  [in]  byte_address The OTP byte address of the byte to write
 * @param  [in]  value        The OTP byte value of the byte to write
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的写单字节接口
 * @param  [in]  byte_address 要写入的字节的OTP字节地址
 * @param  [in]  value        要写入的字节的OTP字节值
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_write_byte_t)(uint32_t byte_address, uint8_t value);

/**
 * @if Eng
 * @brief  Clear all write register to protect next efuse write.
 * @param  [in]  region  The region of OTP to write
 * @else
 * @brief  HAL层EFUSE的清除写区域接口
 * @param  [in]  region 要写入的OTP区域
 * @endif
 */
typedef void (*hal_efuse_clear_all_write_regs_t)(hal_efuse_region_t region);

/**
 * @if Eng
 * @brief  Writes the value to the given OTP memory location
 * @param  [in]  address The byte address of OTP to write to
 * @param  [in]  value  The value to write to OTP memory
 * @param  [in]  region  The region of OTP to write
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的固定区域写接口
 * @param  [in]  address 要写入的OTP的字节地址
 * @param  [in]  value   要写入OTP内存的值
 * @param  [in]  region  要写入的OTP区域
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_write_operation_t)(uint32_t address, uint8_t value, hal_efuse_region_t region);

/**
 * @if Eng
 * @brief  Writes the buffer to the given OTP memory location
 * @param  [in]  address The byte address of OTP to write to
 * @param  [in]  buffer  The buffer to write to OTP memory
 * @param  [in]  length  The length of buffer to write to OTP memory
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层EFUSE的缓冲区写接口
 * @param  [in]  address 要写入的OTP的字节地址
 * @param  [in]  buffer  要写入OTP存储器的缓冲区
 * @param  [in]  length  要写入OTP内存的缓冲区长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_write_buffer_operation_t)(uint32_t address, const uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Obtains the die_id of the EFUSE.
 * @param  [out] buffer OTP byte value of the die id.
 * @param  [in] length The length of the die_id, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取efuse的die_id。
 * @param  [in] buffer 包含要写入的数据的缓冲区
 * @param  [in] length die_id数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_get_die_id_t)(uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Obtains the chip_id of the EFUSE.
 * @param  [out] buffer OTP byte value of the chip id.
 * @param  [in] length The length of the chip_id, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取efuse的chip_id。
 * @param  [in] buffer 包含要写入的数据的缓冲区
 * @param  [in] length chip_id数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_efuse_get_chip_id_t)(uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Interface between efuse driver and efuse hal.
 * @else
 * @brief  Driver层EFUSE和HAL层EFUSE的接口
 * @endif
 */
typedef struct {
    hal_efuse_init_t                    init;                     /*!< @if Eng Init efuse interface.
                                                                  @else HAL层EFUSE的初始化接口 @endif */
    hal_efuse_deinit_t                  deinit;                   /*!< @if Eng Deinit efuse interface.
                                                                  @else HAL层EFUSE去初始化接口 @endif */
    hal_efuse_flush_write_t             flush_write;              /*!< @if Eng Flush select interface.
                                                                  @else HAL层EFUSE的刷新选择EFUSE区域接口 @endif */
    hal_efuse_refresh_read_t            refresh_read;             /*!< @if Eng Refresh read region to read interface.
                                                                  @else HAL层EFUSE写入后将读取区域刷新为读取模式的接口 @endif */
    hal_efuse_read_byte_t               read_byte;                /*!< @if Eng EFUSE byte read interface.
                                                                  @else HAL层EFUSE的单字节读接口 @endif */
    hal_efuse_write_byte_t              write_byte;               /*!< @if Eng EFUSE byte write interface.
                                                                  @else HAL层EFUSE的单字节写接口 @endif */
    hal_efuse_clear_all_write_regs_t    clear;                    /*!< @if Eng Clear all write register interface.
                                                                  @else HAL层EFUSE的清除所有写入寄存器接口 @endif */
    hal_efuse_write_operation_t         write_op;                 /*!< @if Eng Writes the value to the OTPinterface.
                                                                  @else HAL层EFUSE的将值写入给定的OTP内存位置接口 @endif */
    hal_efuse_write_buffer_operation_t  write_buffer_op;          /*!< @if Eng Writes the buffer to the OTP interface.
                                                                  @else HAL层EFUSE的将缓冲区写入给定的OTP内存位置接口 @endif */
    hal_efuse_get_die_id_t              get_die_id;               /*!< @if Eng Interface for Obtaining the Die ID.
                                                                  @else HAL层EFUSE的获取die_id接口 @endif */
    hal_efuse_get_chip_id_t             get_chip_id;              /*!< @if Eng Interface for Obtaining the Chip ID.
                                                                  @else HAL层EFUSE的获取chip_id接口 @endif */
} hal_efuse_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_efuse_funcs_t into the g_hal_efuses_funcs.
 * @param  [in]  funcs Interface between efuse driver and efuse hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_efuse_funcs_t 到 g_hal_efuses_funcs
 * @param  [in]  funcs Driver层EFUSE和HAL层EFUSE的接口实例
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_efuse_register_funcs(hal_efuse_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_efuse_funcs_t into the g_hal_efuses_funcs.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_efuse_funcs_t 到 g_hal_efuses_funcs
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_efuse_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between efuse driver and efuse hal, see @ref hal_efuse_funcs_t.
 * @return Interface between efuse driver and efuse hal, see @ref hal_efuse_funcs_t.
 * @else
 * @brief  获取Driver层EFUSE和HAL层EFUSE的接口实例，参考 @ref hal_efuse_funcs_t.
 * @return Driver层EFUSE和HAL层EFUSE的接口实例，参考 @ref hal_efuse_funcs_t.
 * @endif
 */
hal_efuse_funcs_t *hal_efuse_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
