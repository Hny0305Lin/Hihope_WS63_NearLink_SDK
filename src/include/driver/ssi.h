/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides SSI driver \n
 *
 * History: \n
 * 2022-08-21, Create file. \n
 */
#ifndef SSI_H
#define SSI_H

#include <stdint.h>
#include "ssi_porting.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_ssi SSI
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the SSI.
 * @else
 * @brief  初始化SSI。
 * @endif
 */
void uapi_ssi_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the SSI.
 * @else
 * @brief  去初始化SSI。
 * @endif
 */
void uapi_ssi_deinit(void);

/**
 * @if Eng
 * @brief  Read SSI 16 bit register.
 * @param  [in]  addr Addr to read.
 * @param  [in]  data Value of register.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  读SSI 16位寄存器。
 * @param  [in]  addr 读的地址。
 * @param  [in]  data 寄存器的值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_ssi_read_addr16_data16(uint16_t addr, uint16_t *data);

/**
 * @if Eng
 * @brief  Write SSI 16 bit register.
 * @param  [in]  addr Addr to write.
 * @param  [in]  value Value to write.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  写SSI 16位寄存器。
 * @param  [in]  addr 写的地址。
 * @param  [in]  value 写入的值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_ssi_write_addr16_data16(uint16_t addr, uint16_t value);

/**
 * @if Eng
 * @brief  Read SSI 32 bit register.
 * @param  [in]  addr Addr to read.
 * @param  [in]  data data of register.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  读SSI 32位寄存器。
 * @param  [in]  addr 读的地址。
 * @param  [in]  data 寄存器的值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_ssi_read_addr32_data16(uint32_t addr, uint16_t *data);

/**
 * @if Eng
 * @brief  Write SSI 32 bit register.
 * @param  [in]  addr Addr to write.
 * @param  [in]  value Value to write.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  写SSI 32位寄存器。
 * @param  [in]  addr 写的地址。
 * @param  [in]  value 写入的值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_ssi_write_addr32_data16(uint32_t addr, uint16_t value);

/**
 * @if Eng
 * @brief  Read SSI 32 bit register with 32 bit value.
 * @param  [in]  addr Addr to read.
 * @param  [in]  data Value of register.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  读SSI 32位寄存器，32位值。
 * @param  [in]  addr 读的地址。
 * @param  [in]  data 寄存器的值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_ssi_read_addr32_data32(uint32_t addr, uint32_t *data);

/**
 * @if Eng
 * @brief  Write SSI 32 bit register with 32 bit value.
 * @param  [in]  addr Addr to write.
 * @param  [in]  value Value to write.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  写入SSI 32位寄存器，32位值。
 * @param  [in]  addr 写的地址。
 * @param  [in]  value 写入的值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_ssi_write_addr32_data32(uint32_t addr, uint32_t value);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif