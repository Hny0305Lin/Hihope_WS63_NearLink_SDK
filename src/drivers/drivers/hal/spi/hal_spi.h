/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL SPI \n
 *
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"
#include "spi_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_api SPI
 * @ingroup  drivers_hal_spi
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the contorl ID of hal SPI.
 * @else
 * @brief  SPI控制ID定义。
 * @endif
 */
typedef enum hal_spi_ctrl_id {
    SPI_CTRL_SET_ATTR,                  /*!< @if Eng Set SPI basic attribute.
                                             @else   设置基础参数。 @endif */

    SPI_CTRL_GET_ATTR,                  /*!< @if Eng Get SPI basic attribute.
                                             @else   获取基础参数。 @endif */

    SPI_CTRL_SET_EXTRA_ATTR,            /*!< @if Eng Set SPI extral attribute.
                                             @else   设置高级参数。 @endif */

    SPI_CTRL_GET_EXTRA_ATTR,            /*!< @if Eng Get SPI extral attribute.
                                             @else   获取高级参数。 @endif */

    SPI_CTRL_SELECT_SLAVE,              /*!< @if Eng Select the slave.
                                             @else   选择指定的设备。 @endif */

    SPI_CTRL_CHECK_FIFO_BUSY,           /*!< @if Eng Check whether the SPI is busy.
                                             @else   检查SPI是否忙碌状态。 @endif */
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    SPI_CTRL_SET_DMA_CFG,               /*!< @if Eng Set the DMA transfer enable/disable and datalevel.
                                             @else   设置DMA的传输使能和数据level。 @endif */

    SPI_CTRL_GET_DMA_DATA_ADDR,         /*!< @if Eng Get the DMA transfer data address.
                                             @else   获取DMA的传输数据地址。 @endif */
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    SPI_CTRL_EN_RXFI_INT,               /*!< @if Eng Enable the Receive FIFO full interrupt.
                                             @else   使能RX FIFO是否已满中断。 @endif */

    SPI_CTRL_CHECK_RX_FIFO_EMPTY,       /*!< @if Eng Check if rx fifo is empty or not.
                                             @else   判断RX FIFO是否为空。 @endif */

    SPI_CTRL_EN_TXEI_INT,               /*!< @if Eng Enable the transmit FIFO empty interrupt.
                                             @else   使能TX FIFO是否为空中断。 @endif */

    SPI_CTRL_CHECK_TX_FIFO_FULL,        /*!< @if Eng Check if tx fifo is full or not.
                                             @else   判断RX FIFO是否已满。 @endif */

    SPI_CTRL_EN_MULTI_MASTER_ERR_INT,   /*!< @if Eng Enable the Multi-Master contention interrupt.
                                             @else   使能双主机竞争中断。 @endif */
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */

#if defined(CONFIG_SPI_SUPPORT_LPM) && (CONFIG_SPI_SUPPORT_LPM == 1)
    SPI_CTRL_SUSPEND,                   /*!< @if Eng Suspend all of the spi channels.
                                             @else   挂起所有spi通道。 @endif */
    SPI_CTRL_RESUME,                    /*!< @if Eng Resume all of the spi channels.
                                             @else   挂起所有spi通道。 @endif */
#endif  /* CONFIG_SPI_SUPPORT_LPM */
    SPI_CTRL_SET_TMOD,                  /*!< @if Eng Set SPI transfer mode.
                                             @else   设置SPI传输模式。 @endif */
    SPI_CTRL_MAX,
    SPI_CTRL_ID_INVALID = 0xFF
} hal_spi_ctrl_id_t;

/**
 * @if Eng
 * @brief  Frame Format.
 * @else
 * @brief  帧格式。
 * @endif
 */
typedef enum hal_spi_cfg_frame_format {
    SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI,      /*!< @if Eng Motorolla SPI Frame Format.
                                                 @else   摩托罗拉SPI帧格式。 @endif */

    SPI_CFG_FRAME_FORMAT_TEXAS_SSP,         /*!< @if Eng Texas Instruments SSP Frame Format.
                                                 @else   德州仪器SSP帧格式。 @endif */

    SPI_CFG_FRAME_FORMAT_NS_MICROWIRE,      /*!< @if Eng National Microwire Frame Format.
                                                 @else   国家微线帧格式。 @endif */
    SPI_CFG_FRAME_FORMAT_MAX
} hal_spi_cfg_frame_format_t;

/**
 * @if Eng
 * @brief  Clock polarity.
 * @else
 * @brief  时钟极性。
 * @endif
 */
typedef enum hal_spi_cfg_clk_cpol {
    SPI_CFG_CLK_CPOL_0,                     /*!< @if Eng Inactive state of serial clock is low.
                                                 @else   SPI的非激活状态为低电平。 @endif */

    SPI_CFG_CLK_CPOL_1,                     /*!< @if Eng Inactive state of serial clock is high.
                                                 @else   SPI的非激活状态为高电平。 @endif */
    SPI_CFG_CLK_CPOL_MAX
} hal_spi_cfg_clk_cpol_t;

/**
 * @if Eng
 * @brief  Clock phase.
 * @else
 * @brief  时钟相位。
 * @endif
 */
typedef enum hal_spi_cfg_clk_cpha {
    SPI_CFG_CLK_CPHA_0,                     /*!< @if Eng Serial clock toggles in middle of first data bit.
                                                 @else   SPI时钟在第一个数据位中间切换。 @endif */

    SPI_CFG_CLK_CPHA_1,                     /*!< @if Eng Serial clock toggles at start of first data bit.
                                                 @else   SPI时钟在第一个数据位开始时切换。 @endif */
    SPI_CFG_CLK_CPHA_MAX
} hal_spi_cfg_clk_cpha_t;

/**
 * @if Eng
 * @brief  SPI slave select toggle enable.
 * @else
 * @brief  SPI 从机选择切换使能
 * @endif
 */
typedef enum hal_spi_cfg_sste {
    SPI_CFG_SSTE_DISABLE,                   /*!< @if Eng SPI slave select toggle disable.
                                                         When disable, master should reed all data in slave tx_queue
                                                         at ONE time when reading data from slave device. Otherwise,
                                                         data loss occurs.
                                                 @else   SPI 从机选择切换不使能。
                                                         当此配置不使能，主机从从机读取数据时，需要一次性将从机发送
                                                         队列中的数据读完，否则会出现丢失数据问题。@endif */
    SPI_CFG_SSTE_ENABLE,                    /*!< @if Eng SPI slave select toggle enable.
                                                 @else   SPI 从机选择切换使能 @endif */
    SPI_CFG_SSTE_MAX
} hal_spi_cfg_sste_t;

/**
 * @if Eng
 * @brief  Transfer Mode.
 * @else
 * @brief  传输模式。
 * @endif
 */
typedef enum hal_spi_trans_mode {
    HAL_SPI_TRANS_MODE_TXRX = 0,            /*!< @if Eng Transmit and receive mode.
                                                 @else   收发模式。 @endif */

    HAL_SPI_TRANS_MODE_TX,                  /*!< @if Eng Transmit only / Transmit mode.
                                                 @else   发送模式。 @endif */

    HAL_SPI_TRANS_MODE_RX,                  /*!< @if Eng Receive only / Receive mode.
                                                 @else   接收模式。 @endif */

    HAL_SPI_TRANS_MODE_EEPROM,              /*!< @if Eng EEPROM read mode.
                                                 @else   EEPROM模式。 @endif */
    HAL_SPI_TRANS_MODE_MAX
} hal_spi_trans_mode_t;

/**
 * @if Eng
 * @brief  Data Frame Size.
 * @else
 * @brief  数据帧长度。
 * @endif
 */
typedef enum hal_spi_frame_size {
    HAL_SPI_FRAME_SIZE_8    = 0x07,         /*!< @if Eng 8-bit serial data transfer.
                                                 @else   8-位串行数据传输。 @endif */

    HAL_SPI_FRAME_SIZE_16   = 0x0F,         /*!< @if Eng 16-bit serial data transfer(Not supported now).
                                                 @else   16-位串行数据传输（暂不支持）。 @endif */

    HAL_SPI_FRAME_SIZE_24   = 0x17,         /*!< @if Eng 24-bit serial data transfer(Not supported now).
                                                 @else   24-位串行数据传输（暂不支持）。 @endif */

    HAL_SPI_FRAME_SIZE_32   = 0x1F          /*!< @if Eng 32-bit serial data transfer.
                                                 @else   32-位串行数据传输。 @endif */
} hal_spi_frame_size_t;

/**
 * @if Eng
 * @brief  SPI Frame Format.
 * @else
 * @brief  SPI数据帧格式。
 * @endif
 */
typedef enum hal_spi_frame_format {
    HAL_SPI_FRAME_FORMAT_STANDARD = 0,      /*!< @if Eng SPI Standard frame format.
                                                 @else   标准的单线SPI帧格式。 @endif */

    HAL_SPI_FRAME_FORMAT_DUAL,              /*!< @if Eng SPI Dual frame format.
                                                 @else   双线SPI帧格式。 @endif */

    HAL_SPI_FRAME_FORMAT_QUAD,              /*!< @if Eng SPI Quad frame format.
                                                 @else   4线SPI帧格式。 @endif */

    HAL_SPI_FRAME_FORMAT_OCTAL,             /*!< @if Eng SPI Octal frame format.
                                                 @else   8线SPI帧格式。 @endif */

    HAL_SPI_FRAME_FORMAT_DOUBLE_OCTAL,      /*!< @if Eng SPI Double Octal frame format.
                                                 @else   16线SPI帧格式。 @endif */
    HAL_SPI_FRAME_FORMAT_SIXT,
    HAL_SPI_FRAME_FORMAT_MAX_NUM,
    HAL_SPI_FRAME_FORMAT_NONE = HAL_SPI_FRAME_FORMAT_MAX_NUM
} hal_spi_frame_format_t;

/**
 * @if Eng
 * @brief  SPI Transfer Types.
 * @else
 * @brief  SPI传输类型。
 * @endif
 */
typedef enum hal_spi_trans_type {
    HAL_SPI_TRANS_TYPE_INST_S_ADDR_S = 0,   /*!< @if Eng Instruction and Address will be sent in standard SPI mode.
                                                 @else   指令和地址使用单线SPI传输。 @endif */

    HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q,       /*!< @if Eng Instruction will be sent in standard mode and address will
                                                         be sent in mode specified by frame format register.
                                                 @else   指令使用单线SPI传输，
                                                         地址按照帧格式寄存器的配置传输。 @endif */

    HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q,       /*!< @if Eng Both instruction and address will be sent in
                                                         the mode specified by frame format register.
                                                 @else   指令和地址都按照帧格式寄存器的配置传输。 @endif */
    HAL_SPI_TRANS_TYPE_MAX = HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q
} hal_spi_trans_type_t;

/**
 * @if Eng
 * @brief  SPI length of instruction.
 * @else
 * @brief  SPI指令长度定义。
 * @endif
 */
typedef enum hal_spi_inst_len {
    HAL_SPI_INST_LEN_0 = 0,                 /*!< @if Eng 0-bit (no instruction).
                                                 @else   不携带指令。 @endif */

    HAL_SPI_INST_LEN_4,                     /*!< @if Eng 4-bit instruction.
                                                 @else   4-位指令。 @endif */

    HAL_SPI_INST_LEN_8,                     /*!< @if Eng 8-bit instruction.
                                                 @else   8-位指令。 @endif */

    HAL_SPI_INST_LEN_16,                    /*!< @if Eng 16-bit instruction.
                                                 @else   16-位指令。 @endif */
    HAL_SPI_INST_LEN_MAX = HAL_SPI_INST_LEN_16
} hal_spi_inst_len_t;

/**
 * @if Eng
 * @brief  SPI length of address.
 * @else
 * @brief  SPI地址长度定义。
 * @endif
 */
typedef enum hal_spi_addr_len {
    HAL_SPI_ADDR_LEN_0 = 0,                 /*!< @if Eng 0-bit address length.
                                                 @else   0-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_4,                     /*!< @if Eng 4-bit address length.
                                                 @else   4-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_8,                     /*!< @if Eng 8-bit address length.
                                                 @else   8-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_12,                    /*!< @if Eng 12-bit address length.
                                                 @else   12-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_16,                    /*!< @if Eng 16-bit address length.
                                                 @else   16-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_20,                    /*!< @if Eng 20-bit address length.
                                                 @else   20-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_24,                    /*!< @if Eng 24-bit address length.
                                                 @else   24-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_28,                    /*!< @if Eng 28-bit address length.
                                                 @else   28-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_32,                    /*!< @if Eng 32-bit address length.
                                                 @else   32-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_36,                    /*!< @if Eng 36-bit address length.
                                                 @else   36-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_40,                    /*!< @if Eng 40-bit address length.
                                                 @else   40-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_44,                    /*!< @if Eng 44-bit address length.
                                                 @else   44-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_48,                    /*!< @if Eng 48-bit address length.
                                                 @else   48-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_52,                    /*!< @if Eng 52-bit address length.
                                                 @else   52-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_56,                    /*!< @if Eng 56-bit address length.
                                                 @else   56-位地址长度。 @endif */

    HAL_SPI_ADDR_LEN_60,                    /*!< @if Eng 60-bit address length.
                                                 @else   60-位地址长度。 @endif */
    HAL_SPI_ADDR_LEN_MAX = HAL_SPI_ADDR_LEN_60
} hal_spi_addr_len_t;

/**
 * @if Eng
 * @brief  Definition of the event ID of hal spi.
 * @else
 * @brief  HAL层SPI事件ID的定义
 * @endif
 */
typedef enum hal_spi_evt_id {
    SPI_EVT_RX_FULL_ISR,           /*!< @if Eng Rx full isr triggered.
                                        @else   接收满中断触发 @endif */
    SPI_EVT_RX_OVERFLOW_ISR,       /*!< @if Eng Rx overflow isr triggered.
                                        @else   接收溢出中断触发 @endif */
    SPI_EVT_RX_UNDERFLOW_ISR,      /*!< @if Eng Rx underflow isr triggered.
                                        @else   接收空读中断触发 @endif */
    SPI_EVT_TX_EMPTY_ISR,          /*!< @if Eng Tx empty isr triggered.
                                        @else   TX空中断被触发 @endif */
    SPI_EVT_TX_OVERFLOW_ISR,       /*!< @if Eng Tx overflow isr triggered.
                                        @else   TX溢出中断被触发 @endif */
    SPI_EVT_MULTI_MASTER_ISR       /*!< @if Eng Multi-master contention isr triggered.
                                        @else   双主机竞争中断被触发 @endif */
} hal_spi_evt_id_t;

/**
 * @if Eng
 * @brief  QSPI parameters of SPI transfer.
 * @else
 * @brief  SPI传输中QSPI的参数。
 * @endif
 */
typedef struct hal_spi_xfer_qspi_param {
    hal_spi_trans_type_t trans_type;  /*!< @if Eng SPI frame format for instruction and address.
                                           @else   传输类型，用于指定指令和地址的长度。 @endif */

    hal_spi_inst_len_t   inst_len;    /*!< @if Eng Instruction length, support 0, 4, 8, 16bits.
                                           @else   指令长度，支持0、4、8、16位。 @endif */

    hal_spi_addr_len_t   addr_len;    /*!< @if Eng Address length, support 0, 8, 16, 24, 32bits.
                                           @else   地址长度，支持0、8、16、24、32位。 @endif */

    uint32_t             wait_cycles; /*!< @if Eng Indicates the wait cycles.
                                           @else   等待的周期数。 @endif */
} hal_spi_xfer_qspi_param_t;

/**
 * @if Eng
 * @brief  Single SPI parameters of SPI transfer.
 * @else
 * @brief  SPI传输中Single SPI的参数。
 * @endif
 */
typedef struct hal_spi_xfer_sspi_param {
    uint32_t             wait_cycles; /*!< @if Eng Indicates the wait cycles.
                                           @else   等待的周期数。 @endif */
} hal_spi_xfer_sspi_param_t;

/**
 * @if Eng
 * @brief  Definition of SPI basic attributes.
 * @else
 * @brief  SPI基础配置参数定义。
 * @endif
 */
typedef struct hal_spi_attr {
    bool is_slave;                      /*!< @if Eng Indicates if SPI work in slave mode or not.
                                             @else   SPI工作在Master/Slave模式。 @endif */

    uint32_t slave_num;                 /*!< @if Eng Index when selecting a slave.
                                                     - 0: Not select.
                                                     - 1: slave index 0.
                                                     - 2: slave index 1.
                                                     - ...
                                             @else   选择从机时的索引
                                                     - 0：不选择。
                                                     - 1：从机索引0。
                                                     - 2：从机索引1。
                                                     - ...
                                             @endif */

    uint32_t bus_clk;                   /*!< @if Eng Provide ssi_clk for clock freq division calculation.
                                             @else   用于计算SPI的时钟分频系数。 @endif */

    uint32_t freq_mhz;                  /*!< @if Eng Indicates the frequency of SPI.
                                             @else   SPI的工作频率。 @endif */

    uint32_t clk_polarity;              /*!< @if Eng Indicates the clock polarity of SPI.
                                                     For details, see @ref hal_spi_cfg_clk_cpol_t
                                             @else   SPI的时钟极性。参考 @ref hal_spi_cfg_clk_cpol_t @endif */

    uint32_t clk_phase;                 /*!< @if Eng Indicates the clock phase of SPI.
                                                     For details, see @ref hal_spi_cfg_clk_cpha_t
                                             @else   SPI的时钟相位。参考 @ref hal_spi_cfg_clk_cpha_t @endif */

    uint32_t frame_format;              /*!< @if Eng Indicates the which serial protocol transfers the data.
                                                     For details, see @ref hal_spi_cfg_frame_format_t
                                             @else   选择串行传输的协议。参考 @ref hal_spi_cfg_frame_format_t @endif */

    uint32_t spi_frame_format;          /*!< @if Eng Indicates the frame format of SPI.
                                                     For details, see @ref hal_spi_frame_format_t
                                             @else   SPI的帧格式。参考 @ref hal_spi_frame_format_t @endif */

    uint32_t frame_size;                /*!< @if Eng Indicates the frame size of SPI.
                                                     For details, see @ref hal_spi_frame_size_t
                                             @else   SPI的帧长度。参考 @ref hal_spi_frame_size_t @endif */

    uint32_t tmod;                      /*!< @if Eng Indicates the transfer mode.
                                                     For details, see @ref hal_spi_trans_mode_t
                                             @else   SPI的传输模式。参考 @ref hal_spi_trans_mode_t @endif */

    uint32_t ndf;                       /*!< @if Eng Indicates the number of data frames.
                                             @else   SPI的数据帧数。 @endif */

    uint32_t sste;                      /*!< @if Eng Indicates if SPI slave select toggle enable or not.
                                                     When disable, master should reed all data in slave tx_queue
                                                     at ONE time when reading data from slave device. Otherwise,
                                                     data loss occurs.
                                                     For details, see @ref hal_spi_cfg_sste_t
                                             @else   SPI从机选择切换使能/不使能。
                                                     当此配置不使能，主机从从机读取数据时，需要一次性将从机发送
                                                     队列中的数据读完，否则会出现丢失数据问题。
                                                     参考 @ref hal_spi_cfg_sste_t @endif */
} hal_spi_attr_t;

/**
 * @if Eng
 * @brief  Definition of SPI extra attributes.
 * @else
 * @brief  SPI扩展配置参数定义。
 * @endif
 */
typedef struct hal_spi_extra_attr {
    bool tx_use_dma;                        /*!< @if Eng Indicates if SPI use dma or not in TX.
                                                 @else   SPI是否使用DMA发送数据。 @endif */

    bool rx_use_dma;                        /*!< @if Eng Indicates if SPI use dma or not in RX.
                                                 @else   SPI是否使用DMA接收数据。 @endif */

    hal_spi_xfer_qspi_param_t qspi_param;   /*!< @if Eng Indicates the qspi parameters.
                                                 @else   QSPI参数。 @endif */

    hal_spi_xfer_sspi_param_t sspi_param;   /*!< @if Eng Indicates the single spi parameters.
                                                 @else   Single SPI参数。 @endif */
} hal_spi_extra_attr_t;

/**
 * @if Eng
 * @brief  SPI transfer data structure.
 * @else
 * @brief  SPI传输结构体。
 * @endif
 */
typedef struct hal_spi_xfer_data {
    uint8_t *tx_buff;       /*!< @if Eng Buff to send data through tx fifo.
                                 @else   通过tx fifo发送数据的Buff。 @endif */
    uint32_t tx_bytes;      /*!< @if Eng Bytes of data need to send. For details, see @ref hal_spi_attr_t.frame_size.
                                         when frame_size is HAL_SPI_FRAME_SIZE_8, The value must be a multiple of 1.
                                         when frame_size is HAL_SPI_FRAME_SIZE_16, The value must be a multiple of 2.
                                         when frame_size is HAL_SPI_FRAME_SIZE_24, The value must be a multiple of 3.
                                         when frame_size is HAL_SPI_FRAME_SIZE_32, The value must be a multiple of 4.
                                 @else   发送数据的个数。参考 @ref hal_spi_attr_t.frame_size.
                                         如果frame_size为HAL_SPI_FRAME_SIZE_8，则需设定为1的倍数
                                         如果frame_size为HAL_SPI_FRAME_SIZE_16，则需设定为2的倍数
                                         如果frame_size为HAL_SPI_FRAME_SIZE_24，则需设定为3的倍数
                                         如果frame_size为HAL_SPI_FRAME_SIZE_32，则需设定为4的倍数
                                 @endif */
    uint8_t *rx_buff;       /*!< @if Eng Buff to receive data from rx fifo.
                                 @else   通过rx fifo接收数据的Buff。 @endif */
    uint32_t rx_bytes;      /*!< @if Eng Bytes of data need to receive, For details, see @ref hal_spi_attr_t.frame_size.
                                         when frame_size is HAL_SPI_FRAME_SIZE_8, The value must be a multiple of 1.
                                         when frame_size is HAL_SPI_FRAME_SIZE_16, The value must be a multiple of 2.
                                         when frame_size is HAL_SPI_FRAME_SIZE_24, The value must be a multiple of 3.
                                         when frame_size is HAL_SPI_FRAME_SIZE_32, The value must be a multiple of 4.
                                 @else   接收数据的个数。参考 @ref hal_spi_attr_t.frame_size.
                                         如果frame_size为HAL_SPI_FRAME_SIZE_8，则需设定为1的倍数
                                         如果frame_size为HAL_SPI_FRAME_SIZE_16，则需设定为2的倍数
                                         如果frame_size为HAL_SPI_FRAME_SIZE_24，则需设定为3的倍数
                                         如果frame_size为HAL_SPI_FRAME_SIZE_32，则需设定为4的倍数
                                 @endif */
    uint8_t cmd;            /*!< @if Eng Command for QSPI mode.
                                 @else   QSPI模式下的命令。 @endif */
    uint8_t reserved[3];    /*!< @if Eng Reserved.
                                 @else   保留。 @endif */
    uint32_t addr;          /*!< @if Eng Address for QSPI mode.
                                 @else   QSPI模式下的地址。 @endif */
} hal_spi_xfer_data_t;

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
/**
 * @if Eng
 * @brief  DMA SPI parameters of SPI transfer.
 * @else
 * @brief  SPI传输中DMA SPI的参数。
 * @endif
 */
typedef struct hal_spi_dma_cfg_param {
    bool                 is_enable;     /*!< @if Eng DMA transfer enable/disable configuration.
                                             @else   DMA传输使能配置。 @endif */
    uint8_t              dma_rx_level;  /*!< @if Eng DMA receive data level configuration.
                                             @else   DMA RX传输请求的数据level配置。 @endif */
    uint8_t              dma_tx_level;  /*!< @if Eng DMA transmit data level configuration.
                                             @else   DMA TX传输请求的数据level配置。 @endif */
} hal_spi_dma_cfg_param_t;
#endif  /* CONFIG_SPI_SUPPORT_DMA */

/**
 * @if Eng
 * @brief  Callback of SPI.
 * @param  [in]  bus The SPI bus. see @ref spi_bus_t
 * @param  [in]  evt Event ID.
 * @param  [in]  param Parameter of callback.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  SPI的回调函数
 * @param  [in]  bus 串口号， 参考 @ref spi_bus_t
 * @param  [in]  evt 事件ID.
 * @param  [in]  param 传递到回调的参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_spi_callback_t)(spi_bus_t bus, hal_spi_evt_id_t evt, uintptr_t param);

/**
 * @if Eng
 * @brief  Control interface for hal SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @param  [in]  id ID of the SPI control.
 * @param  [in]  param Parameter for callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SPI控制接口。
 * @param  [in]  bus 串口号。参考 @ref spi_bus_t
 * @param  [in]  id SPI控制请求ID。
 * @param  [in]  param 传递给控制回调的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_spi_ctrl_t)(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Init device for hal SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @param  [in]  attr SPI basic attributes.
 * @param  [in]  extra_attr SPI extra attributes.
 * @param  [in]  callback SPI callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SPI的初始化接口。
 * @param  [in]  bus SPI端口号。参考 @ref spi_bus_t
 * @param  [in]  attr SPI的基本配置参数。
 * @param  [in]  extra_attr SPI高级配置参数。
 * @param  [in]  callback SPI回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_spi_init(spi_bus_t bus, const hal_spi_attr_t *attr,
                       const hal_spi_extra_attr_t *extra_attr, hal_spi_callback_t callback);

/**
 * @if Eng
 * @brief  Deinit device for hal SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SPI的去初始化接口。
 * @param  [in]  bus SPI端口号。参考 @ref spi_bus_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_spi_deinit(spi_bus_t bus);

/**
 * @if Eng
 * @brief  Write interface for hal SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @param  [in]  data Transfer data. For details, see @ref hal_spi_xfer_data_t
 * @param  [in]  timeout Timeout duration.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SPI发送数据接口。
 * @param  [in]  bus 串口号。参考 @ref spi_bus_t
 * @param  [in]  data 传输数据。参考 @ref hal_spi_xfer_data_t
 * @param  [in]  timeout 超时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_spi_write(spi_bus_t bus, hal_spi_xfer_data_t *data, uint32_t timeout);

/**
 * @if Eng
 * @brief  Read interface for hal SPI(When timeout is zero, will be exited If the data is full or no data is fifo).
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @param  [out] data Transfer data. For details, see @ref hal_spi_xfer_data_t
 * @param  [in]  timeout Timeout duration.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SPI读取数据接口(当timeout为0时会在数据读满或者fifo中没有数据的时候结束)。
 * @param  [in]  bus 串口号。参考 @ref spi_bus_t
 * @param  [out] data 传输数据。参考 @ref hal_spi_xfer_data_t
 * @param  [in]  timeout 超时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_spi_read(spi_bus_t bus, hal_spi_xfer_data_t *data, uint32_t timeout);

/**
 * @if Eng
 * @brief  Control interface for hal SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @param  [in]  id ID of the SPI control.
 * @param  [in]  param Parameter for callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SPI控制接口。
 * @param  [in]  bus 串口号。参考 @ref spi_bus_t
 * @param  [in]  id SPI控制请求ID。
 * @param  [in]  param 传递给控制回调的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_spi_ctrl(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Init the registers of SPI IPs.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  初始化寄存器基地址列表。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_spi_regs_init(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
