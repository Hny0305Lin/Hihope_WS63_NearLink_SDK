/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides flash config info header file.
 *
 * History:
 * 2022-11-30， Create file.
 */

#ifndef FLASH_CONFIG_INFO_H
#define FLASH_CONFIG_INFO_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @ingroup  drivers_port_sfc
 * @{
 */

#define EFLASH_CMD_LEN_MAX             4
#define SPI_CMD_SUPPORT                0x1

#define FLASH_INFO_TABLE_SIZE g_flash_spi_info_num
/**
 * @if Eng
 * @brief  SPI instruction execution mode.
 * @else
 * @brief  表驱动执行Flash指令的指令格式
 * @endif
*/
typedef enum {
    FLASH_CMD_TYPE_CMD,                      /*!<@if Eng Command for setting the flash attribute
                                                 @else   设置flash属性类型的指令         @endif */
    FLASH_CMD_TYPE_PROCESSING,               /*!<@if Eng Read the flash information and compares a certain bit.
                                                 @else   读取Flash信息并比对某一位的值         @endif */
    FLASH_CMD_TYPE_END,                      /*!<@if Eng Command end flag
                                                 @else   指令结束标志         @endif */
    FLASH_CMD_BUFF = 0xFF
} flash_cmd_type_t;

/**
 * @if Eng
 * @brief  Parameters related to SPI read, write and erase operation.
 * @else
 * @brief  SPI读写擦操作相关参数
 * @endif
*/
typedef struct spi_opreation {
    uint32_t cmd_support : 3;                /*!<@if Eng SPI command support
                                                 @else   是否支持该索引对应的指令 @endif */
    uint32_t cmd : 8;                        /*!<@if Eng SPI command
                                                 @else   SPI指令码             @endif */
    uint32_t iftype : 3;                     /*!<@if Eng SPI interface type
                                                         value:
                                                            000：Standard SPI
                                                            001：Dual-Input/Dual-Output SPI(based on read/write)
                                                            010：Dual-I/O SPI
                                                            101：Quad-Input/Dual-Output SPI(based on read/write)
                                                            110：Quad-I/O SPI
                                                            other: reserved
                                                 @else   SPI 接口类型
                                                         合法值:
                                                            000：标准单线SPI
                                                            001：双线In/双线Out SPI（根据指令的读写模式调整）
                                                            010：双线I/O SPI
                                                            101：四线In/四线Out SPI（根据指令的读写模式调整）
                                                            110：四线I/O SPI
                                                            其他:保留 @endif */
    uint32_t size : 18;                      /*!<@if Eng   erase size for erase and dummy byte for read.
                                                 @else     擦除指令的大小和读指令的dummy字节数     @endif */
} spi_opreation_t;

/**
 * @if Eng
 * @brief  Command format for enabling the Quad SPI flash
 * @else
 * @brief  开启Flash的Quad SPI的指令格式
 * @endif
*/
typedef struct flash_cmd_execute_t {
    flash_cmd_type_t cmd_type;               /*!<@if Eng For details, see @ref flash_cmd_type_t
                                                 @else   参考 @ref flash_cmd_type_t @endif */
    uint8_t cmd_len;                         /*!<@if Eng CMD mode : Length of the SPI command including 1byte data.
                                                         PROCESSING mode : The value is fixed to 3.
                                                 @else   CMD模式包含一字节数据在内的SPI指令长度。
                                                         PROCESSING模式固定为3 @endif */
    uint8_t cmd[EFLASH_CMD_LEN_MAX];         /*!<@if Eng SPI command. The format is as follows:
                                                         CMD: cmd[0] command code.
                                                              cmd[1] One-byte data.
                                                         PROCESS: cmd[0] command code.
                                                                  cmd[1] Expected Compare Bit.
                                                                  cmd[2] Expected value of this bit.
                                                 @else   SPI指令，格式如下
                                                         CMD: cmd[0] 指令码
                                                              cmd[1] 一字节数据
                                                         PROCESS: cmd[0] 指令码
                                                                  cmd[1] 预计比较的位
                                                                  cmd[2] 该位预计的值 @endif */
} flash_cmd_execute_t;

/**
 * @if Eng
 * @brief  Flash basic information struct
 * @else
 * @brief  Flash基本信息结构
 * @endif
*/
typedef struct flash_spi_info {
    uint32_t chip_id;                        /*!<@if Eng Flash manufacture id.
                                                 @else   Flash 制造id @endif */
    uint32_t chip_size;                      /*!<@if Eng Actual size of flash
                                                 @else   Flash实际大小 @endif */
    uint32_t erase_cmd_num;                  /*!<@if Eng Number of erase commands
                                                 @else   擦除指令的个数@endif */
    spi_opreation_t *read_cmds;              /*!<@if Eng Read command form. index:@ref sfc_read_if_t.
                                                 @else   读指令表单 索引 @ref sfc_read_if_t
                                                 @endif */
    spi_opreation_t *write_cmds;             /*!<@if Eng Write command form. index:@ref sfc_write_if_t.
                                                 @else   写指令表单 索引 @ref sfc_write_if_t
                                                 @endif */
    spi_opreation_t *erase_cmds;             /*!<@if Eng Erase command form. Indexes are sorted by erase size in desc.
                                                 @else   写指令表单 索引按照擦除大小降序排列
                                                 @endif */
    flash_cmd_execute_t *quad_mode;          /*!<@if Eng Enable Quad SPI Mode command Form. @ref flash_cmd_execute_t
                                                 @else   开启四线模式指令表单 @ref flash_cmd_execute_t @endif */
} flash_spi_info_t;

flash_spi_info_t *sfc_port_get_flash_spi_infos(void);

uint32_t sfc_port_get_flash_num(void);

flash_spi_info_t *sfc_port_get_unknown_flash_info(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
