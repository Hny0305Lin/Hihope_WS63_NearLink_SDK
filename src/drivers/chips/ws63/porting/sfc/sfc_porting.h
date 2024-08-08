/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#ifndef SFC_PORTING_H
#define SFC_PORTING_H

#include <stdbool.h>

#include "errcode.h"
#include "flash_config_info.h"
#include "sfc_protect.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup drivers_port_sfc SFC
 * @ingroup  drivers_port
 * @{
 */
#define FLASH_CHIP_PROTECT_END      0x400000
#define SFC_SAFE_OFFSET             0x10000

/**
 * @if Eng
 * @brief  Read, write and erase operations.
 * @else
 * @brief  Flash的基本信息和操作信息
 * @endif
*/
typedef struct flash_spi_ctrl {
    uint32_t        chip_size;                                   /*!<@if Eng Size of the flash mapped to the SFC
                                                                     @else   Flash映射到SFC的大小      @endif */
    spi_opreation_t read_opreation;                              /*!<@if Eng Read operation
                                                                     @else   读操作               @endif */
    uint32_t        erase_cmd_num;                               /*!<@if Eng Number of erase commands
                                                                     @else   擦除指令的个数@endif */
    spi_opreation_t write_opreation;                             /*!<@if Eng Write opreation
                                                                     @else   写操作               @endif */
    spi_opreation_t *erase_opreation_array;                       /*!<@if Eng Erase opreations
                                                                     @else   擦除操作      @endif */
    flash_cmd_execute_t *quad_mode;                              /*!<@if Eng Quad SPI Enable opreations
                                                                     @else   Quad SPI 使能操作      @endif */
} flash_spi_ctrl_t;

/**
 * @if Eng
 * @brief  Get SFC bus space start address.
 * @retval SFC bus space start address.
 * @else
 * @brief  获取SFC总线空间首地址。
 * @retval SFC总线空间首地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_start_addr(void);

/**
 * @if Eng
 * @brief  Get SFC bus space end address.
 * @retval SFC bus space end address.
 * @else
 * @brief  获取SFC总线空间尾地址。
 * @retval SFC总线空间尾地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_end_addr(void);

/**
 * @if Eng
 * @brief  Get base address for SFC bus registers.
 * @retval Base address for SFC bus registers.
 * @else
 * @brief  获取SFC公共配置相关寄存器基地址。
 * @retval SFC公共配置相关寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_global_conf_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for SFC bus registers.
 * @retval Base address for SFC bus registers.
 * @else
 * @brief  获取SFC总线模式访问相关寄存器基地址。
 * @retval SFC总线模式访问相关寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_bus_regs_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for SFC DMA registers.
 * @retval Base address for SFC DMA registers.
 * @else
 * @brief  获取SFC DMA操作相关寄存器基地址。
 * @retval SFC DMA操作相关寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_bus_dma_regs_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for SFC command registers.
 * @retval Base address for SFC command registers.
 * @else
 * @brief  获取SFC SPI操作寄存器相关基地址。
 * @retval SFC SPI操作寄存器相关基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_cmd_regs_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for SFC command data buffer registers.
 * @retval Base address for SFC command data buffer registers.
 * @else
 * @brief  获取SFC SPI操作数据缓冲区寄存器基地址。
 * @retval SFC SPI操作数据缓冲区寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_cmd_databuf_base_addr(void);

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_sfc module.
 * @else
 * @brief  将hal funcs对象注册到hal_sfc模块中。
 * @endif
 */
void sfc_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_sfc module.
 * @else
 * @brief  从hal_sfc模块注销hal funcs对象。
 * @endif
 */
void sfc_port_unregister_hal_funcs(void);

/**
 * @if Eng
 * @brief  Set the single delay time for querying the flash WIP bit.
 * @param  [in]  delay_us Delay time for querying the flash WIP bit.
 * @else
 * @brief  设置查询Flash WIP位的单次延时时间。
 * @param  [in]  delay_us 查询Flash WIP位的单次延时时间。
 * @endif
 */
void sfc_port_set_delay_once_time(uint32_t delay_us);

/**
 * @if Eng
 * @brief  Get the single delay time for querying the flash WIP bit
 * @retval single delay time for querying the flash WIP bit.The unit is us.
 * @else
 * @brief  获取查询Flash WIP位的单次延时时间
 * @retval 查询Flash WIP位的单次延时时间，单位为us
 * @endif
 */
uint32_t sfc_port_get_delay_once_time(void);

/**
 * @if Eng
 * @brief  Set delay times for querying the flash WIP bit
 * @param  [in]  delay_times Delay times for querying the flash WIP bit
 * @else
 * @brief  配置查询Flash WIP位的延时次数
 * @param  [in]  delay_times 查询Flash WIP位的延时次数
 * @endif
 */
void sfc_port_set_delay_times(uint32_t delay_times);

/**
 * @if Eng
 * @brief  Get delay times for querying the flash WIP bit
 * @retval delay times for querying the flash WIP bit
 * @else
 * @brief  获取查询Flash WIP位的延时次数
 * @retval 查询Flash WIP位的延时次数
 * @endif
 */
uint32_t sfc_port_get_delay_times(void);

/**
 * @if Eng
 * @brief  sfc lock initialize.
 * @else
 * @brief  SFC锁初始化。
 * @endif
 */
void sfc_port_lock_init(void);

/**
 * @if Eng
 * @brief  sfc lock.
 * @retval lock status.
 * @else
 * @brief  SFC上锁。
 * @retval 锁状态。
 * @endif
 */
uint32_t sfc_port_lock(void);

/**
 * @if Eng
 * @brief  sfc unlock.
 * @param [in] lock_sts lock status.
 * @else
 * @brief  SFC解锁。
 * @param [in] lock_sts 锁状态，传入的为由lock接口返回的值
 * @endif
 */
void sfc_port_unlock(uint32_t lock_sts);

#ifndef BUILD_NOOSAL
errcode_t hal_sfc_get_flash_id(uint32_t *flash_id);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
