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
typedef errcode_t (*hal_sfc_get_flash_id_t)(uint32_t *flash_id);

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
typedef errcode_t (*hal_sfc_init_t)(flash_spi_ctrl_t *spi_ctrl, uint32_t mapping_address, uint32_t flash_size);

/**
 * @if Eng
 * @brief  SFC deinitialize.
 * @else
 * @brief  SFC 去初始化操作。
 * @endif
 */
typedef void (*hal_sfc_deinit_t)(void);

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
typedef errcode_t (*hal_sfc_reg_read_t)(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size,
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
typedef errcode_t (*hal_sfc_reg_write_t)(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
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
typedef errcode_t (*hal_sfc_reg_erase_t)(uint32_t flash_addr, spi_opreation_t erase_opreation, bool delete_chip);

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
typedef errcode_t (*hal_sfc_dma_read_t)(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size);

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
typedef errcode_t (*hal_sfc_dma_write_t)(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size);
#endif /* CONFIG_SFC_SUPPORT_DMA */

/**
 * @if Eng
 * @brief  Interface of watchdog hal.
 * @else
 * @brief  HAL层SFC的接口
 * @endif
 */
typedef struct hal_sfc_funcs {
    hal_sfc_get_flash_id_t get_flash_id;                /*!< @if Eng SFC get flash id interface.
                                                             @else   HAL层获取flash id接口 @endif */
    hal_sfc_init_t init;                                /*!< @if Eng SFC init interface.
                                                             @else   HAL层SFC初始化接口 @endif */
    hal_sfc_deinit_t deinit;                            /*!< @if Eng SFC deinit interface.
                                                             @else   HAL层SFC去初始化接口 @endif */
    hal_sfc_reg_read_t reg_read;                        /*!< @if Eng SFC read in SPI mode interface.
                                                             @else   HAL层SFC内嵌SPI模式读接口 @endif */
    hal_sfc_reg_write_t reg_write;                      /*!< @if Eng SFC write in SPI mode interface.
                                                             @else   HAL层SFC内嵌SPI模式写接口 @endif */
    hal_sfc_reg_erase_t reg_erase;                      /*!< @if Eng SFC erase in SPI mode interface.
                                                             @else   HAL层SFC内嵌SPI模式擦除接口 @endif */
    hal_sfc_reg_flash_opreation_t flash_opreation;      /*!< @if Eng SFC other flash operations in SPI mode interface.
                                                             @else   HAL层SFC内嵌SPI模式Flash操作接口 @endif */
#if defined(CONFIG_SFC_SUPPORT_DMA)
    hal_sfc_dma_read_t dma_read;                        /*!< @if Eng SFC DMA read interface.
                                                             @else   HAL层DMA模式读接口 @endif */
    hal_sfc_dma_write_t dma_write;                      /*!< @if Eng SFC DMA write interface.
                                                             @else   HAL层DMA模式写接口 @endif */
#endif
} hal_sfc_funcs_t;

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
 * @brief  Register @ref hal_sfc_funcs_t into the g_hal_sfcs_funcs.
 * @param  [in]  funcs Interface of SFC driver and SFC hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_sfc_funcs_t 到 g_hal_sfcs_funcs
 * @param  [in]  funcs Driver层SFC和HAL层SFC的接口实例
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_sfc_register_funcs(hal_sfc_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_sfc_funcs_t from the g_hal_sfcs_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  从g_hal_sfcs_funcs注销 @ref hal_sfc_funcs_t
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
void hal_sfc_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface of SFC driver and SFC hal, see @ref hal_sfc_funcs_t.
 * @return Interface of SFC driver and SFC hal, see @ref hal_sfc_funcs_t.
 * @else
 * @brief  获取Driver层SFC和HAL层SFC的接口实例，参考 @ref hal_sfc_funcs_t.
 * @return Driver层SFC和HAL层SFC的接口实例，参考 @ref hal_sfc_funcs_t.
 * @endif
 */
hal_sfc_funcs_t *hal_sfc_get_funcs(void);


/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif