/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides SFC driver api \n
 *
 * History: \n
 * 2022-11-29， Create file. \n
 */
#ifndef SFC_H
#define SFC_H

#include <stdint.h>
#include <errcode.h>
#include <common_def.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_sfc SFC
 * @ingroup  drivers_driver
 * @{
 *
 */

#define NULL_FLASH   0

/**
 * @if Eng
 * @brief  Definition of SPI type of the read operation.
 * @else
 * @brief  读操作使用的SPI接口类型。
 * @endif
 */
typedef enum sfc_read_if {
    STANDARD_READ = 0x0,               /*!< @if Eng Read in standard SPI mode (generally 0x03).
                                            @else   以标准SPI模式执行读操作(指令一般为0x03)。 @endif */
    FAST_READ = 0x1,                   /*!< @if Eng Fast Read in standard SPI mode (generally 0x0B).
                                            @else   以标准SPI模式执行快速读操作(指令一般为0x0B)。 @endif */
    FAST_READ_DUAL_OUTPUT = 0x2,       /*!< @if Eng Read in Dual SPI Output mode (generally 0x3B).
                                            @else   以双线Out SPI模式执行读操作(指令一般为0x3B)。 @endif */
    FAST_READ_DUAL_IO = 0x3,           /*!< @if Eng Read in Dual SPI I/O mode (generally 0xBB).
                                            @else   以双线In/Out SPI模式执行读操作(指令一般为0xBB)。 @endif */
    FAST_READ_QUAD_OUTPUT = 0x4,       /*!< @if Eng Read in Quad SPI Output mode (generally 0xBB).
                                            @else   以四线Out SPI模式执行读操作(指令一般为0x6B)。 @endif */
    FAST_READ_QUAD_IO = 0x5            /*!< @if Eng Read in Quad SPI I/O mode (generally 0xEB).
                                            @else   以四线In/Out SPI模式执行读操作(指令一般为0xEB)。 @endif */
} sfc_read_if_t;

/**
 * @if Eng
 * @brief  Definition of SPI type of the write operation.
 * @else
 * @brief  写操作使用的SPI接口类型。
 * @endif
 */
typedef enum sfc_write_if {
    WRITE_DISABLE = 0x0,               /*!< @if Eng Write operation disabled.
                                            @else   写操作禁止。 @endif */
    PAGE_PROGRAM = 0x1,                /*!< @if Eng Write in standard SPI mode
                                            @else   以标准SPI模式执行写操作。 @endif */
    DUAL_INPUT_PAGE_PROGRAM = 0x2,     /*!< @if Eng Write in Dual Input SPI mode
                                            @else   以双线In SPI模式执行写操作。 @endif */
    DUAL_IO_PAGE_PROGRAM = 0x3,        /*!< @if Eng Write in Dual I/O SPI mode
                                            @else   以双线I/O SPI模式执行写操作。 @endif */
    QUAD_INPUT_PAGE_PROGRAM = 0x4,     /*!< @if Eng Write in Quad Input SPI mode
                                            @else   以四线In SPI模式执行写操作。 @endif */
    QUAD_IO_PAGE_PROGRAM = 0x5         /*!< @if Eng Write in Quad Input SPI mode
                                            @else   以四线In SPI模式执行写操作。 @endif */
} sfc_write_if_t;

/**
 * @if Eng
 * @brief  Definition of Type of the Flash register operation.
 * @else
 * @brief  Flash寄存器操作类型。
 * @endif
 */
typedef enum sfc_flash_op {
    READ_TYPE = 0x0,                   /*!< @if Eng Read flash info.
                                            @else   读取Flash相关信息。 @endif */
    WRITE_TYPE = 0x1                   /*!< @if Eng Set flash attribute.
                                            @else   配置Flash状态。 @endif */
} sfc_flash_op_t;

/**
 * @if Eng
 * @brief  Initialize the SFC parameters.
 * @else
 * @brief  SFC初始化参数。
 * @endif
 */
typedef struct sfc_flash_config {
    sfc_read_if_t read_type;           /*!< @if Eng SPI type of the read operation.
                                            @else   读操作SPI类型。 @endif */
    sfc_write_if_t write_type;         /*!< @if Eng SPI type of the write operation.
                                            @else   写操作SPI类型。 @endif */
    uint32_t mapping_addr;             /*!< @if Eng Base address for flash memory mapping.
                                        *           The value corresponds to address 0 of the flash memory.
                                        *           Needs to be mapped to the SFC internal bus space.
                                        *           Only the upper 16 bits are valid.
                                        *   @else   Flash映射基地址，对应为Flash的0地址。
                                        *           需要映射在SFC内部总线空间，仅高16位有效。 @endif */
    uint32_t mapping_size;             /*!< @if Eng Size of the flash memory mapping.
                                        *           The mapping space can be customized.
                                        *           The parameter must be in the format of 64 KB * 2^n, where n >= 0.
                                        *           The minimum size is 64 KB.
                                        *           The end address needs to be in the SFC internal bus space.
                                        *           If this parameter exceeds the size configured in the form,
                                        *           this parameter will be setted to the size configured in the form.
                                        *   @else   Flash映射的大小，可自定义映射空间。
                                        *           参数形式必须为64KB * 2^n，其中n >= 0。
                                        *           尾地址需要在SFC内部总线空间。
                                        *           超出了表单配置的Flash大小时会将大小置为表单中配置的大小。 @endif */
} sfc_flash_config_t;

/**
 * @if Eng
 * @brief  Initialize and Configure the SFC.The unidentified flash adopts the standard read/write of 512 KB by default.
 * @param  [in]  config Initialization parameter, For details, see @ref sfc_flash_config_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化并配置SFC，未识别的Flash默认采用单线读写512KB的配置。
 * @param  [in]  config 初始化参数 参考 @ref sfc_flash_config_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_init(sfc_flash_config_t *config);

/**
 * @if Eng
 * @brief  Initialize and Configure the SFC.The unidentified flash adopts the standard read/write of 512 KB by default.
 * @param  [in]  config Initialization parameter, For details, see @ref sfc_flash_config_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化并配置SFC。rom 默认采用单线读写512KB的配置。
 * @param  [in]  config 初始化参数 参考 @ref sfc_flash_config_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_init_rom(sfc_flash_config_t *config);

/**
 * @if Eng
 * @brief  Deinitialize the SFC.
 * @else
 * @brief  去初始化SFC。
 * @endif
 */
void uapi_sfc_deinit(void);

/**
 * @if Eng
 * @brief  Provide the read function in register mode. The read data is stored in the read_buffer by byte.
 *         This function cannot be called in an interrupt.
 * @param  [in]  flash_addr The start address of the data.
 * @param  [out]  read_buffer Buffer for receiving data.
 * @param  [in]  read_size Number of bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  提供寄存器模式读功能，读取的数据将按字节存入read_buffer中。不允许在中断中调用。
 * @param  [in]  flash_addr 数据所在的Flash首地址。
 * @param  [out] read_buffer 用于接收数据的buffer。
 * @param  [in]  read_size 读取的字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_reg_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size);

/**
 * @if Eng
 * @brief  Write data in register mode. The data to be written is stored in write_data by byte.
 *         This function cannot be called in an interrupt.
 * @param  [in]  flash_addr The start address of the data.
 * @param  [in]  write_data Data to be written.
 * @param  [in]  write_size Number of bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  提供寄存器模式写功能，预计写入的数据按字节存入write_data中。不允许在中断中调用。
 * @param  [in]  flash_addr 目标Flash首地址。
 * @param  [in]  write_data 预计写入的数据。
 * @param  [in]  write_size 写入数据的字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_reg_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size);

/**
 * @if Eng
 * @brief  Use the register mode to erase the Flash memory.Enforce flash_addr and erase_size alignment by sector when
 *         writeback is disabled.This function cannot be called in an interrupt.
 * @param  [in]  flash_addr Start address for erasing.
 * @param  [in]  erase_size Size of the flash memory to be erased.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使用寄存器模式进行对Flash的擦除，不使能写回时强制要求地址和大小按扇区对齐。不允许在中断中调用。
 * @param  [in]  flash_addr 擦除的首地址。
 * @param  [in]  erase_size 擦除的Flash空间的大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_reg_erase(uint32_t flash_addr, uint32_t erase_size);

/**
 * @if Eng
 * @brief  Use the register mode to erase the entire Flash memory.This function cannot be called in an interrupt.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使用寄存器模式对整片Flash进行擦除。不允许在中断中调用。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_reg_erase_chip(void);

/**
 * @if Eng
 * @brief  Use the register mode to read and write Flash attribute.This function cannot be called in an interrupt.
 * @param  [in]  cmd_type Sets the read/write type of this command.
 * @param  [in]  cmd SPI command.
 * @param  [in,out]  buffer data buffer.
 * @param  [in]  length Length of the data to be read or written. The value must be less than 4.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使用寄存器模式对Flash属性进行读写。不允许在中断中调用。
 * @param  [in]  cmd_type 设置指令的读写类型。
 * @param  [in]  cmd SPI指令。
 * @param  [in,out]  buffer 数据缓冲区。
 * @param  [in]  length 需要读/写的数据长度，其值需要小于4。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_reg_other_flash_opt(sfc_flash_op_t cmd_type, uint8_t cmd, uint8_t *buffer, uint32_t length);

#if defined(CONFIG_SFC_SUPPORT_DMA)
/**
 * @if Eng
 * @brief  Provide the read function in DMA mode. The read data is stored in the read_buffer by byte.
 *         This function cannot be called in an interrupt.
 * @param  [in]  flash_addr The start address of the data.
 * @param  [out] read_buffer Buffer for receiving data.
 * @param  [in]  read_size Number of bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  提供DMA模式读功能，读取的数据将按字节存入read_buffer中。不允许在中断中调用。
 * @param  [in]  flash_addr 数据的Flash首地址。
 * @param  [out] read_buffer 用于接收数据的buffer。
 * @param  [in]  read_size 读取的字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_dma_read(uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size);

/**
 * @if Eng
 * @brief  Write data in register mode. The data to be written is stored in write_data by byte.
 *         This function cannot be called in an interrupt.
 * @param  [in]  flash_addr The start address of the data.
 * @param  [in]  write_buffer Data to be written.
 * @param  [in]  write_size Number of bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  提供寄存器模式写功能，预计写入的数据按字节存入write_data中。不允许在中断中调用。
 * @param  [in]  flash_addr 目标Flash首地址。
 * @param  [in]  write_buffer 预计写入的数据。
 * @param  [in]  write_size 写入数据的字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_dma_write(uint32_t flash_addr, uint8_t *write_buffer, uint32_t write_size);

#endif /* CONFIG_SFC_SUPPORT_DMA */

#if defined(CONFIG_SFC_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend the SFC.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起SFC。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume the SFC.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复SFC。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sfc_resume(uintptr_t arg);
#endif  /* CONFIG_SFC_SUPPORT_LPM */

/**
 * @}
*/
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
