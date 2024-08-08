/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides hal sio \n
 *
 * History: \n
 * 2023-01-15, Create file. \n
 */
#ifndef HAL_SIO_H
#define HAL_SIO_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "sio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_api SIO
 * @ingroup  drivers_hal_sio
 * @{
 */

/**
 * @if Eng
 * @brief  SIO divice modes.
 * @else
 * @brief  SIO设备模式。
 * @endif
 */
typedef enum hal_sio_driver_mode {
    SLAVE,                                      /*!< @if Eng SLAVE.
                                                     @else   从模式。 @endif */
    MASTER                                      /*!< @if Eng MASTER.
                                                     @else   主模式。 @endif */
} hal_sio_driver_mode_t;

/**
 * @if Eng
 * @brief  SIO modes.
 * @else
 * @brief  SIO 模式。
 * @endif
 */
typedef enum hal_sio_mode {
    I2S_MODE = 0,                                   /*!< @if Eng I2S mode.
                                                     @else   I2S 模式。 @endif */
    PCM_MODE,                                       /*!< @if Eng PCM mode.
                                                     @else   PCM 模式。 @endif */
    NONE_SIO_MODE
} hal_sio_mode_t;

/**
 * @if Eng
 * @brief  SIO interrupts.
 * @else
 * @brief  SIO 中断类型。
 * @endif
 */
typedef enum hal_sio_interrupt {
    RX_INTR = 0,                                /*!< @if Eng RX FIFO higher than threshold interrupt.
                                                     @else   接收fifo高于阈值中断。 @endif */
    TX_INTR,                                    /*!< @if Eng TX FIFO higher than threshold interrupt.
                                                     @else   发送fifo高于阈值中断。 @endif */
    RX_RIGHT_FIFO_OVER,                         /*!< @if Eng RX RIGHT FIFO Overflow interrupt.
                                                     @else   右声道接收fifo上溢中断。 @endif */
    RX_LEFT_FIFO_OVER,                          /*!< @if Eng RX LEFT FIFO Overflow interrupt.
                                                     @else   左声道接收fifo上溢中断。 @endif */
    TX_RIGHT_FIFO_UNDER,                        /*!< @if Eng TX RIGHT FIFO Overflow interrupt.
                                                     @else   右声道发送fifo上溢中断。 @endif */
    TX_LEFT_FIFO_UNDER                          /*!< @if Eng TX LEFT FIFO Overflow interrupt.
                                                     @else   左声道发送fifo上溢中断。 @endif */
} hal_sio_interrupt_t;

/**
 * @if Eng
 * @brief  SIO timing mode.
 * @else
 * @brief  SIO时序模式。
 * @endif
 */
typedef enum hal_sio_iming_mode {
    PCM_STD_MODE = 0,                           /*!< @if Eng PCM standard timing mode.
                                                     @else   PCM标准时序模式。 @endif */
    PCM_UD_MODE,                                /*!< @if Eng PCM user-defined timing mode.
                                                     @else   PCM自定义时序模式。 @endif */
    NONE_TIMING_MODE
} hal_sio_timing_mode_t;

/**
 * @if Eng
 * @brief  SIO transmission path modes.
 * @else
 * @brief  SIO 传输路径模式。
 * @endif
 */
typedef enum {
    STD_MODE = 0,                               /*!< @if Eng Standard mode.
                                                     @else   标准模式。 @endif */
    MULTICHANNEL_MODE                           /*!< @if Eng Multichannel mode.
                                                     @else   多路模式。 @endif */
} hal_sio_transfer_mode_t;

/**
 * @if Eng
 * @brief  SIO clk edge.
 * @else
 * @brief  SIO 时钟边沿。
 * @endif
 */
typedef enum hal_sio_clk_edge {
    FALLING_EDGE = 0,                           /*!< @if Eng Falling edge.
                                                     @else   下降沿。 @endif */
    RISING_EDGE,                                /*!< @if Eng Rising edge
                                                     @else   上升沿。 @endif */
    NONE_EDGE
} hal_sio_clk_edge_t;

/**
 * @if Eng
 * @brief  SIO voice_channel.
 * @else
 * @brief  SIO 声道。
 * @endif
 */
typedef enum hal_sio_voice_channel {
    SIO_LEFT = 0,                               /*!< @if Eng SIO left voice channel.
                                                     @else   SIO 左声道。 @endif */
    SIO_RIGHT,                                  /*!< @if Eng SIO right voice channel.
                                                     @else   SIO 右声道。 @endif */
    NONE_MODE
} hal_sio_voice_channel_t;

/**
 * @if Eng
 * @brief  SIO data width.
 * @else
 * @brief  SIO 数据位宽。
 * @endif
 */
typedef enum hal_sio_data_width {
    RESERVED,                                   /*!< @if Eng RESERVED.
                                                     @else   保留。 @endif */
    SIXTEEN_BIT,                                /*!< @if Eng 16 BITS.
                                                     @else   16位。 @endif */
    EIGHTEEN_BIT,                               /*!< @if Eng 18 BITS.
                                                     @else   18位。 @endif */
    TWENTY_BIT,                                 /*!< @if Eng 20 BITS.
                                                     @else   20位。 @endif */
    TWENTY_FOUR_BIT,                            /*!< @if Eng 24 BITS.
                                                     @else   24位。 @endif */
    THIRTY_TWO_BIT                              /*!< @if Eng 32 BITS.
                                                     @else   32位。 @endif */
} hal_sio_data_width_t;

/**
 * @if Eng
 * @brief  SIO transmission Channels number.
 * @else
 * @brief  SIO 传输通道数。
 * @endif
 */
typedef enum hal_sio_channel_number {
    TWO_CH,                                     /*!< @if Eng 2 Channels.
                                                     @else   2通道。 @endif */
    FOUR_CH,                                    /*!< @if Eng 4 Channels.
                                                     @else   4通道。 @endif */
    EIGHT_CH,                                   /*!< @if Eng 8 Channels.
                                                     @else   8通道。 @endif */
    SIXTEEN_CH                                  /*!< @if Eng 16 Channels.
                                                     @else   16通道。 @endif */
} hal_sio_channel_number_t;

/**
 * @if Eng
 * @brief  SIO tx data structure.
 * @else
 * @brief  SIO tx传输结构体。
 * @endif
 */
typedef struct hal_sio_tx_data {
    uint32_t *left_buff;                        /*!< @if Eng Data send through tx left fifo.
                                                     @else   通过tx 左fifo发送的数据。 @endif */
    uint32_t *right_buff;                       /*!< @if Eng Data send through tx right fifo.
                                                     @else   通过tx 右fifo发送的数据。 @endif */
    uint32_t length;                            /*!< @if Eng Bytes of data need to send
                                                     @else   发送数据的个数 @endif */
} hal_sio_tx_data_t;

/**
 * @if Eng
 * @brief  SIO received data.
 * @else
 * @brief  SIO 接收数据。
 * @endif
 */
typedef struct hal_sio_received_data {
    uint32_t left_buff[CONFIG_DATA_LEN_MAX];    /*!< @if Eng Left data.
                                                     @else   左声道数据。 @endif */
    uint32_t right_buff[CONFIG_DATA_LEN_MAX];   /*!< @if Eng Right data.
                                                     @else   右声道数据。 @endif */
    uint32_t length;                            /*!< @if Eng Data length.
                                                     @else   数据长度。 @endif */
} hal_sio_rx_data_t;

/**
 * @if Eng
 * @brief  Definition of SIO attributes.
 * @else
 * @brief  SIO配置参数定义。
 * @endif
 */
typedef struct hal_sio_config {
    hal_sio_driver_mode_t drive_mode;           /*!< @if Eng SIO divice modes.
                                                     @else   SIO 设备模式。 @endif */
    hal_sio_transfer_mode_t transfer_mode;      /*!< @if Eng SIO transmission path modes.
                                                     @else   SIO 传输路径模式。 @endif */
    hal_sio_data_width_t data_width;            /*!< @if Eng SIO data width.
                                                     @else   SIO 数据宽度。 @endif */
    hal_sio_channel_number_t channels_num;      /*!< @if Eng Number of SIO transmission Channels.
                                                     @else   SIO 传输通道数。 @endif */
    hal_sio_timing_mode_t timing;               /*!< @if Eng SIO timing mode.
                                                     @else   SIO 时序模式。 @endif */
    hal_sio_clk_edge_t clk_edge;                /*!< @if Eng PCM clock edge mode.
                                                     @else   PCM 时钟边沿模式。 @endif */
    uint8_t div_number;                         /*!< @if Eng Div number, see @ref hal_sio_config.data_width.
                                                     @else   分频系数，见 @ref hal_sio_config.data_width 成员 @endif */
    uint8_t number_of_channels;                 /*!< @if Eng Number of channels, see @ref hal_sio_config.channels_num.
                                                     @else   通道数，见 @ref hal_sio_config.channels_num 成员。 @endif */
} hal_sio_config_t;

#if defined(CONFIG_I2S_SUPPORT_DMA)

/**
 * @if Eng
 * @brief  Definition of I2S DMA attributes.
 * @else
 * @brief  I2S DMA配置参数定义
 * @endif
 */
typedef struct hal_i2s_dma_attr {
    bool tx_dma_enable;                     /*!< @if Eng false: tx not use dma @ref uapi_i2s_write_data can be used. \n
                                                     true:  tx use dma @ref uapi_i2s_merge_write_by_dma can be used.
                                             @else   false: TX没有使用DMA，使用 @ref uapi_i2s_write_data 发送数据 \n
                                                     true:  TX使用DMA，使用 @ref uapi_i2s_merge_write_by_dma 发送数据 @endif */
    uint8_t tx_int_threshold;               /*!< @if Eng i2s tx fifo level to trigger interrupt.
                                             @else 触发中断的txfifo水线 @endif */
    bool rx_dma_enable;                     /*!< @if Eng false: rx not use dma @ref uapi_i2s_write_data can be used. \n
                                                     true:  rx use dma @ref uapi_i2s_merge_write_by_dma can be used.
                                             @else   false: RX没有使用DMA，使用 @ref uapi_i2s_write_data 发送数据 \n
                                                     true:  RX使用DMA，使用 @ref uapi_i2s_merge_write_by_dma 发送数据 @endif */
    uint8_t rx_int_threshold;               /*!< @if Eng i2s rx fifo level to trigger interrupt.
                                             @else 触发中断的rxfifo水线 @endif */
} hal_i2s_dma_attr_t;

/**
 * @if Eng
 * @brief  Configuring DMA Transmission interface for hal sio.
 * @param  [in]  bus The device pointer to manager the hal sio.
 * @param  [in]  attr dma configuration of sio.
 * @else
 * @brief  HAL层配置DMA传输接口
 * @param  [in]  bus sio号， 参考 @ref sio_bus_t
 * @param  [in]  attr SIO的高级配置参数。
 * @endif
 */
typedef void (*hal_sio_dma_cfg_t)(sio_bus_t bus, const uintptr_t attr);
#endif

/**
 * @if Eng
 * @brief  Definition of sio callback type.
 * @else
 * @brief  HAL层SIO回调函数。
 * @endif
 */
typedef void (*hal_sio_callback_t)(uint32_t *left_buff, uint32_t *right_buff, uint32_t length);

/**
 * @if Eng
 * @brief  Initialize device for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SIO的初始化接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_sio_init_t)(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Deinitialize device for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @else
 * @brief  HAL层SIO的去初始化接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @endif
 */
typedef void (*hal_sio_deinit_t)(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Config device interface for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  config The SIO config. For details, see @ref hal_sio_config_t
 * @else
 * @brief  HAL层SIO配置接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  config SIO配置参数。参考 @ref hal_sio_config_t
 * @endif
 */
typedef void (*hal_sio_set_config_t)(sio_bus_t bus, const hal_sio_config_t *config);

/**
 * @if Eng
 * @brief  Get config of device interface for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  config The SIO config. For details, see @ref hal_sio_config_t
 * @else
 * @brief  HAL层SIO获取配置接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  config SIO配置参数。参考 @ref hal_sio_config_t
 * @endif
 */
typedef errcode_t (*hal_sio_get_config_t)(sio_bus_t bus, hal_sio_config_t *config);

/**
 * @if Eng
 * @brief  RX enable interface for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  en Enable rx or not.
 * @else
 * @brief  HAL层SIO rx使能接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  en 是否开启rx。
 * @endif
 */
typedef void (*hal_sio_rx_enable_t)(sio_bus_t bus, bool en);

/**
 * @if Eng
 * @brief  Write data interface for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  data The SIO data. For details, see @ref hal_sio_tx_data_t
 * @param  [in]  mode The SIO mode. For details, see @ref hal_sio_mode_t
 * @else
 * @brief  HAL层SIO的写接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  data SIO数据。参考 @ref hal_sio_tx_data_t
 * @param  [in]  mode SIO模式。参考 @ref hal_sio_mode_t
 * @endif
 */
typedef void (*hal_sio_write_t)(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode);

/**
 * @if Eng
 * @brief  Get data interface for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [out]  data data of the sio read. For details, see @ref hal_sio_rx_data_t.
 * @else
 * @brief  HAL层SIO获取数据接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [out]  data SIO读取的数据。参考 @ref hal_sio_rx_data_t
 * @endif
 */
typedef void (*hal_sio_get_data_t)(sio_bus_t bus, hal_sio_rx_data_t *data);

/**
 * @if Eng
 * @brief  Register callback func for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  callback The SIO callback. For details, see @ref hal_sio_tx_data_t
 * @else
 * @brief  HAL层SIO注册回调函数接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  callback SIO回调函数。参考 @ref hal_sio_callback_t
 * @endif
 */
typedef void (*hal_sio_register_t)(sio_bus_t bus, hal_sio_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister callback func for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @else
 * @brief  HAL层SIO去注册回调函数接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @endif
 */
typedef void (*hal_sio_unregister_t)(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Loop mode interface for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  en Enable loop back mode or not.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SIO自测模式接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  en 是否开启回环模式。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef void (*hal_sio_loop_t)(sio_bus_t bus, bool en);

/**
 * @if Eng
 * @brief  SIO loop test for hal sio.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t
 * @param  [in]  data The SIO data. For details, see @ref hal_sio_tx_data_t
 * @param  [in]  mode The SIO mode. For details, see @ref hal_sio_mode_t
 * @else
 * @brief  HAL层SIO自测接口。
 * @param  [in]  bus SIO端口号。参考 @ref sio_bus_t
 * @param  [in]  data SIO的数据。参考 @ref hal_sio_rx_data_t
 * @param  [in]  mode SIO的模式。 参考 @ref hal_sio_mode_t
 * @endif
 */
typedef void (*hal_sio_loop_trans_t)(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode);

/**
 * @if Eng
 * @brief  Interface between sio driver and sio hal.
 * @else
 * @brief  Driver层SIO和HAL层SIO的接口。
 * @endif
 */
typedef struct hal_sio_funcs {
    hal_sio_init_t              init;                       /*!< @if Eng Init device interface.
                                                                 @else   HAL层SIO的初始化接口。 @endif */
    hal_sio_deinit_t            deinit;                     /*!< @if Eng Deinit device interface.
                                                                 @else   HAL层SIO去初始化接口。 @endif */
    hal_sio_set_config_t        set_config;                 /*!< @if Eng Config device interface.
                                                                 @else   HAL层SIO配置接口。 @endif */
    hal_sio_get_config_t        get_config;                 /*!< @if Eng Get_config device interface.
                                                                 @else   HAL层SIO获取配置接口。 @endif */
    hal_sio_rx_enable_t         rx_enable;                       /*!< @if Eng RX enable interface.
                                                                 @else   HAL层SIO rx使能接口。 @endif */
    hal_sio_write_t             write;                      /*!< @if Eng Write data interface.
                                                                 @else   HAL层SIO发送数据接口。 @endif */
    hal_sio_get_data_t          get_data;                   /*!< @if Eng Get data interface.
                                                                 @else   HAL层SIO获取数据接口 @endif */
    hal_sio_register_t          registerfunc;               /*!< @if Eng Register callback func interface.
                                                                 @else   HAL层SIO注册回调函数接口。 @endif */
    hal_sio_unregister_t        unregisterfunc;             /*!< @if Eng Unregister callback func interface.
                                                                 @else   HAL层SIO去注册回调函数接口。 @endif */
    hal_sio_loop_t              loop;                       /*!< @if Eng Loop mode interface.
                                                                 @else   HAL层SIO自测模式接口。 @endif */
    hal_sio_loop_trans_t        loop_trans;                 /*!< @if Eng SIO loop test interface
                                                                 @else   HAL层SIO回路自测函数接口。 @endif */
#if defined(CONFIG_I2S_SUPPORT_DMA)
    hal_sio_dma_cfg_t           dma_cfg;                    /*!< @if Eng SIO dma config interface
                                                                 @else   HAL层SIO DMA配置接口 @endif */
#endif
} hal_sio_funcs_t;

/**
 * @if Eng
 * @brief  Init the registers of SIO IPs.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  初始化寄存器基地址列表。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_sio_regs_init(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Deinit the registers of SIO IPs.
 * @else
 * @brief  去初始化寄存器基地址列表。
 * @endif
 */
void hal_sio_regs_deinit(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Register @ref hal_sio_funcs_t into the g_hal_sios_funcs.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t
 * @param  [in]  funcs Interface between sio driver and sio hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_sio_funcs_t 到 g_hal_sios_funcs。
 * @param  [in]  bus 串口号。参考 @ref sio_bus_t
 * @param  [in]  funcs Driver层sio和HAL层sio的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_sio_register_funcs(sio_bus_t bus, hal_sio_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_sio_funcs_t from the g_hal_sios_funcs.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从g_hal_sios_funcs注销 @ref hal_sio_funcs_t 。
 * @param  [in]  bus 串口号。参考 @ref sio_bus_t
 * @return ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_sio_unregister_funcs(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Get interface between sio driver and sio hal, see @ref hal_sio_funcs_t
 * @param  [in]  bus The sio bus. see @ref sio_bus_t
 * @return Interface between sio driver and sio hal, see @ref hal_sio_funcs_t
 * @else
 * @brief  获取Driver层sio和HAL层sio的接口实例。参考 @ref hal_sio_funcs_t
 * @param  [in]  bus 串口号。 参考 @ref sio_bus_t
 * @return Driver层sio和HAL层sio的接口实例。参考 @ref hal_sio_funcs_t
 * @endif
 */
hal_sio_funcs_t *hal_sio_get_funcs(sio_bus_t bus);

#if defined(CONFIG_I2S_SUPPORT_DMA)
/**
 * @if Eng
 * @brief  sio crg clock enable.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @param  [in]  enable true or false.
 * @else
 * @brief  sio crg clock 使能。
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @param  [in]  enable true 或者 false.
 * @endif
 */
void hal_sio_set_crg_clock_enable(sio_bus_t bus, bool enable);

/**
 * @if Eng
 * @brief  sio tx enable.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @param  [in]  val 0 or 1.
 * @else
 * @brief  sio tx 使能。
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @param  [in]  val 0 或者 1.
 * @endif
 */
void hal_sio_set_tx_enable(sio_bus_t bus, uint32_t val);

/**
 * @if Eng
 * @brief  sio rx enable.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @param  [in]  val 0 or 1.
 * @else
 * @brief  sio rx 使能。
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @param  [in]  val 0 或者 1.
 * @endif
 */
void hal_sio_set_rx_enable(sio_bus_t bus, uint32_t val);
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