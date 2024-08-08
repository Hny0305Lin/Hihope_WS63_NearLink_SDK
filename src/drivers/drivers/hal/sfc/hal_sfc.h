/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hal sfc \n
 *
 * History: \n
 * 2022-12-01, Create file. \n
 */
#ifndef HAL_SFC_H
#define HAL_SFC_H

#include <stdint.h>
#include <stdbool.h>
#include <common_def.h>
#include <errcode.h>
#include <sfc_porting.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sfc_api SFC
 * @ingroup  drivers_hal_sfc
 * @{
 */

#if defined(CONFIG_SFC_SUPPORT_RWE_INDEPENDENT) && CONFIG_SFC_SUPPORT_RWE_INDEPENDENT == 1
#define SFC_SECTION __attribute__((section(".FLSLOADERRAMCODE")))
#else
#define SFC_SECTION
#endif

/**
 * @if Eng
 * @brief  Get the Flash ID
 * @param  [out]  flash_id  flash ID is stored in this address.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取Flash ID操作
 * @param  [out]  flash_id  读取到的flash id。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_get_flash_id(uint32_t *flash_id);

/**
 * @if Eng
 * @brief  SFC initialization.
 * @param  [in]  spi_ctrl  For details, see @ref flash_spi_ctrl_t.
 * @param  [in]  mapping_address   Flash mapping address.
 * @param  [in]  flash_size   For details, see @ref bus_flash_size_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 初始化操作。
 * @param  [in]  spi_ctrl  参考 @ref flash_spi_ctrl_t 。
 * @param  [in]  mapping_address   Flash映射地址。
 * @param  [in]  flash_size   参考 @ref bus_flash_size 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_init(flash_spi_ctrl_t *spi_ctrl, uint32_t mapping, uint32_t flash_size);

/**
 * @if Eng
 * @brief  SFC deinitialize.
 * @else
 * @brief  SFC 去初始化操作。
 * @endif
 */
void hal_sfc_deinit(void);

/**
 * @if Eng
 * @brief  SFC read operation in SPI mode.
 * @param  [in]  flash_addr  Start address of the flash memory to be read.
 * @param  [out] read_buffer Pointer to data buffer.
 * @param  [in]  read_size   Amount of data to be read.
 * @param  [in]  read_opreation Read command information.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式读操作。
 * @param  [in]  flash_addr  读数据首地址。
 * @param  [out] read_buffer 读操作数据缓冲区。
 * @param  [in]  read_size   读数据总字节数，在driver层已做参数检查。
 * @param  [in]  read_opreation 读指令信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size,
                           spi_opreation_t read_opreation);

/**
 * @if Eng
 * @brief  SFC write operation in SPI mode.
 * @param  [in]  flash_addr  Start address of the flash memory to be write.
 * @param  [in]  write_data  Pointer to data buffer.
 * @param  [in]  write_size  Amount of data to be write.
 * @param  [in]  write_opreation Write command information.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式写操作。
 * @param  [in]  flash_addr   写数据首地址。
 * @param  [in]  write_data   预计写入的数据。
 * @param  [in]  write_size   写数据总字节数，在driver层已做参数检查。
 * @param  [in]  write_opreation 写操作信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
                            spi_opreation_t write_opreation);

/**
 * @if Eng
 * @brief  SFC erase operation in SPI mode.
 * @param  [in]  flash_addr  Start address of the flash memory to be erase.
 * @param  [in]  erase_opreation  Erase command information.
 * @param  [in]  delete_chip  Indicates whether to erase the entire chip.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式擦除操作。
 * @param  [in]  flash_addr        预擦除的首地址。
 * @param  [in]  erase_opreation   擦数据信息。
 * @param  [in]  delete_chip       是否为整片擦除。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_erase(uint32_t flash_addr, spi_opreation_t erase_opreation, bool delete_chip);

/**
 * @if Eng
 * @brief  SFC other flash operations in SPI mode.
 * @param  [in]  opt_type  Flash operation type.
 * @param  [in]  cmd  Flash command code.
 * @param  [in,out]  buffer Pointer to data buffer
 * @param  [in]  length Length of data buffer. The value is less than 4. Processed at the driver layer
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式Flash其他操作。
 * @param  [in]  opt_type Flash操作类型。
 * @param  [in]  cmd Flash操作指令码。
 * @param  [in,out]  buffer 数据缓冲区。
 * @param  [in]  length 缓冲区长度，driver层限制其值小于4。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_sfc_reg_flash_opreation_t)(uint32_t opt_type, uint8_t cmd, uint8_t *buffer, uint32_t length);

/**
 * @if Eng
 * @brief  SFC other flash operations in SPI mode.
 * @param  [in]  opt_type  Flash operation type.
 * @param  [in]  cmd  Flash command code.
 * @param  [in,out]  buffer Pointer to data buffer
 * @param  [in]  length Length of data buffer. The value is less than 4. Processed at the driver layer
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式Flash其他操作。
 * @param  [in]  opt_type Flash操作类型。
 * @param  [in]  cmd Flash操作指令码。
 * @param  [in,out]  buffer 数据缓冲区。
 * @param  [in]  length 缓冲区长度，driver层限制其值小于4。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_flash_opreations(uint32_t opt_type, uint8_t cmd, uint8_t *buffer, uint32_t length);

#if defined(CONFIG_SFC_SUPPORT_DMA)

/**
 * @if Eng
 * @brief  SFC read operation in DMA mode
 * @param  [in]  flash_addr Start address of the flash memory to be read.
 * @param  [out] read_buffer Pointer to data buffer
 * @param  [in]  read_size Amount of data to be read.
 * @retval ERRCODE_SUCC      Success.
 * @retval Other             Failure. For details, see @ref errcode_t
 * @else
 * @brief  SFC dma模式读操作
 * @param  [in]  flash_addr 读操作flash首地址。
 * @param  [out] read_buffer 读数据缓冲区。
 * @param  [in]  read_size 读数据总字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_dma_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size);

/**
 * @if Eng
 * @brief  SFC read operation in DMA mode.
 * @param  [in]  flash_addr Start address of the flash memory to be write.
 * @param  [in]  write_data Pointer to data buffer
 * @param  [in]  write_size Length of the write data.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC dma模式读操作。
 * @param  [in]  flash_addr  写操作flash首地址。
 * @param  [in]  write_data  预计写入的数据。
 * @param  [in]  write_size  写数据字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_dma_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size);
#endif /* CONFIG_SFC_SUPPORT_DMA */

#if defined(CONFIG_SFC_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  SFC suspend.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起SFC。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_suspend(void);

/**
 * @if Eng
 * @brief  SFC resume.
 * @param  [in]  quad_mode Initialization operation instruction for enabling the QSPI mode of the flash memory.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复SFC。
 * @param  [in]  quad_mode Flash开启QSPI模式的初始化操作指令。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_resume(flash_cmd_execute_t *quad_mode);
#endif

/**
 * @if Eng
 * @brief  Set the base address of registers.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置SFC寄存器的基地址
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_sfc_regs_init(void);

/**
 * @if Eng
 * @brief  Clear the base address of registers has been set by @ref hal_watchdog_regs_init.
 * @else
 * @brief  清除由 @ref hal_sfc_regs_init 设置的基地址
 * @endif
 */
void hal_sfc_regs_deinit(void);

/**
 * @if Eng
 * @brief  Sfc en/disable write protect.
 * @param  [in]  flash_id sfc flash id.
 * @param  [in]  regioon sfc flash protect region.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能或失能SFC写保护。
 * @param  [in]  flash_id sfc flash id。
 * @param  [in]  regioon sfc flash保护的区域。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_lock_protect(uint32_t flash_id, uint8_t region);

/**
 * @if Eng
 * @brief  Sfc disable write protect.
 * @param  [in]  flash_id sfc flash id.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  失能SFC写保护。
 * @param  [in]  flash_id sfc flash id。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_unlock_protect(uint32_t flash_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif