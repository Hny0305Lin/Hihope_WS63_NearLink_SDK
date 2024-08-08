/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides I2S driver api \n
 *
 * History: \n
 * 2023-03-10， Create file. \n
 */

#ifndef I2S_H
#define I2S_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "sio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_i2s I2S
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of I2S RX data structure.
 * @else
 * @brief  I2S RX 传输结构体。
 * @endif
 */
typedef struct i2s_rx_data {
    uint32_t left_buff[CONFIG_DATA_LEN_MAX];    /*!< @if Eng Left data.
                                                     @else   左声道数据。 @endif */
    uint32_t right_buff[CONFIG_DATA_LEN_MAX];   /*!< @if Eng Right data.
                                                     @else   右声道数据。 @endif */
    uint32_t length;                            /*!< @if Eng Data length.
                                                     @else   数据长度。 @endif */
} i2s_rx_data_t;

/**
 * @if Eng
 * @brief  Definition of I2S TX data structure.
 * @else
 * @brief  I2S TX 传输结构体。
 * @endif
 */
typedef struct i2s_tx_data {
    uint32_t *left_buff;                        /*!< @if Eng Data send through tx left FIFO.
                                                     @else   通过TX 左FIFO发送的数据。 @endif */
    uint32_t *right_buff;                       /*!< @if Eng Data send through tx right FIFO.
                                                     @else   通过TX 右FIFO发送的数据。 @endif */
    uint32_t length;                            /*!< @if Eng Bytes of data need to send.
                                                     @else   发送数据的个数。 @endif */
} i2s_tx_data_t;

/**
 * @if Eng
 * @brief  Definition of I2S configuration.
 * @else
 * @brief  I2S 配置定义。
 * @endif
 */
typedef struct i2s_config {
    uint8_t drive_mode;                         /*!< @if Eng I2S divice modes:
                                                 *           - 0: SLAVE
                                                 *           - 1: MASTER
                                                 *   @else   I2S 设备模式：
                                                 *           - 0: 从模式
                                                 *           - 1: 主模式
                                                 *   @endif */
    uint8_t transfer_mode;                      /*!< @if Eng I2S transmission path modes:
                                                 *           - 0: Standard mode
                                                 *           - 1: Multichannel mode
                                                 *   @else   I2S 传输路径模式：
                                                 *           - 0: 标准模式
                                                 *           - 1: 多路模式
                                                 *   @endif */
    uint8_t data_width;                         /*!< @if Eng I2S data width:
                                                 *           - 0: RESERVED
                                                 *           - 1: 16 Bits
                                                 *           - 2: 18 Bits
                                                 *           - 3: 20 Bits
                                                 *           - 4: 24 Bits
                                                 *           - 5: 32 Bits
                                                 *   @else   I2S 数据宽度：
                                                 *           - 0: 保留
                                                 *           - 1: 16位
                                                 *           - 2: 18位
                                                 *           - 3: 20位
                                                 *           - 4: 24位
                                                 *           - 5: 32位
                                                 *   @endif */
    uint8_t channels_num;                       /*!< @if Eng I2S transmission Channels Number:
                                                 *           - 0: 2 Channels
                                                 *           - 1: 4 Channels
                                                 *           - 2: 8 Channels
                                                 *           - 3: 16 Channels
                                                 *   @else   I2S 传输通道数：
                                                 *           - 0: 2通道
                                                 *           - 1: 4通道
                                                 *           - 2: 8通道
                                                 *           - 3: 16通道
                                                 *   @endif */
    uint8_t timing;                             /*!< @if Eng I2S timing mode:
                                                 *           - 0: Standard timing mode
                                                 *           - 1: User-defined timing mode
                                                 *   @else   I2S 时序模式：
                                                 *           - 0: 标准时序模式
                                                 *           - 1: 自定义时序模式
                                                 *   @endif */
    uint8_t clk_edge;                           /*!< @if Eng I2S clock edge mode:
                                                 *           - 0: Falling edge
                                                 *           - 1: Rising edge
                                                 *   @else   I2S 时钟边沿模式：
                                                 *           - 0: 下降沿
                                                 *           - 1: 上升沿
                                                 *   @endif */
    uint8_t div_number;                         /*!< @if Eng Div number, see @ref i2s_config.data_width.
                                                     @else   分频系数，见 @ref i2s_config.data_width 成员。 @endif */
    uint8_t number_of_channels;                 /*!< @if Eng Number of channels, see @ref i2s_config.channels_num.
                                                     @else   通道数，见 @ref i2s_config.channels_num 成员。 @endif */
} i2s_config_t;

/**
 * @if Eng
 * @brief  The callback set to the I2S device.
 * @param  [in]  left_buff Data received by the left channel.
 * @param  [in]  right_buff Data received by the right channel.
 * @param  [in]  length Data length.
 * @else
 * @brief  I2S设备的回调函数。
 * @param  [in]  left_buff 左声道接收的数据。
 * @param  [in]  right_buff 右声道接收的数据。
 * @param  [in]  length 数据的长度。
 * @endif
 */
typedef void (*i2s_callback_t)(uint32_t *left_buff, uint32_t *right_buff, uint32_t length);

/**
 * @if Eng
 * @brief  Initialize the I2S.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  callback The callback set to the I2S device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化I2S。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  callback I2S设备的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_init(sio_bus_t bus, i2s_callback_t callback);

/**
 * @if Eng
 * @brief  Deinitialize the I2S.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化I2S。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_deinit(sio_bus_t bus);

/**
 * @if Eng
 * @brief  Set the configuration of the I2S device.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  config Config set to the I2S device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置I2S设备的配置信息。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  config I2S设备的配置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_set_config(sio_bus_t bus, const i2s_config_t *config);

/**
 * @if Eng
 * @brief  Get the configuration of the I2S device.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [out]  config Config set to the I2S device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取I2S设备配置信息。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [out]  config I2S设备的配置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_get_config(sio_bus_t bus, i2s_config_t *config);

/**
 * @if Eng
 * @brief  Write data to the I2S device in the polling mode.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  data Pointer to transfer data, @ref i2s_tx_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  轮询模式下写入数据。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  data 数据传输指针，参考 @ref i2s_tx_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_write_data(sio_bus_t bus, i2s_tx_data_t *data);

/**
 * @if Eng
 * @brief  Read data from I2S device in the interrupt mode.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  中断模式下读取数据。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_read_start(sio_bus_t bus);

/**
 * @if Eng
 * @brief  I2S loop test.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  data Pointer to transfer data. @ref i2s_tx_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  I2S回路自测。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  data 数据传输指针，参考 @ref i2s_tx_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_loop_trans(sio_bus_t bus, i2s_tx_data_t *data);

/**
 * @if Eng
 * @brief  Get the data received by the I2S device in the interrupt mode.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [out]  data Pointer to receive data, @ref i2s_rx_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取中断模式下I2S设备接收的数据。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [out]  data 数据接收指针，参考 @ref i2s_rx_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_get_data(sio_bus_t bus, i2s_rx_data_t *data);

#if defined(CONFIG_I2S_SUPPORT_LOOPBACK) && (CONFIG_I2S_SUPPORT_LOOPBACK == 1)
/**
 * @if Eng
 * @brief  Open loop mode.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  en Enable loop back mode or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  打开自测模式。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  en 是否开启回环模式。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2s_loopback(sio_bus_t bus, bool en);
#endif  /* CONFIG_I2S_SUPPORT_LOOPBACK */

#if defined(CONFIG_I2S_SUPPORT_DMA)
/**
 * @if Eng
 * @brief  Definition of I2S DMA configuration.
 * @else
 * @brief  I2S DMA配置数据结构定义
 * @endif
 */
typedef struct i2s_dma_config {
    uint8_t src_width;          /*!< @if Eng Transfer data width of the source.
                                 *           - 0: 1byte
                                 *           - 1: 2byte
                                 *           - 2: 4byte
                                 *   @else   源端传输数据宽度 \n
                                 *           - 0: 1字节
                                 *           - 1: 2字节
                                 *           - 2: 4字节
                                 *   @endif */
    uint8_t dest_width;         /*!< @if Eng Transfer data width of the destination.
                                 *            - 0: 1byte
                                 *            - 1: 2byte
                                 *            - 2: 4byte
                                 *   @else   目的端传输数据宽度 \n
                                 *           - 0: 1字节
                                 *           - 1: 2字节
                                 *           - 2: 4字节
                                 *   @endif */
    uint8_t burst_length;       /*!< @if Eng Number of data items, to be written to the destination every time
                                 *           a destination burst transaction request is made from
                                 *           either the corresponding hardware or software handshaking interface.
                                 *           - 0: burst length is 1
                                 *           - 1: burst length is 4
                                 *           - 2: burst length is 8
                                 *           - 3: burst length is 16
                                 *   @else   每次从相应的硬件或软件握手接口发出目的burst请求时,要写入目的端数据量
                                 *           - 0: burst长度是1
                                 *           - 1: burst长度是4
                                 *           - 2: burst长度是8
                                 *           - 3: burst长度是16
                                 *   @endif */
    uint8_t priority;           /*!< @if Eng Transfer channel priority(Minimum: 0 and Maximum: 3).
                                 *   @else   传输通道优先级(最小为0以及最大为3)  @endif */
} i2s_dma_config_t;

/**
 * @if Eng
 * @brief  Definition of I2S DMA attributes.
 * @else
 * @brief  I2S DMA配置参数定义
 * @endif
 */
typedef struct i2s_dma_attr {
    bool tx_dma_enable;                     /*!< @if Eng false: tx not use dma @ref uapi_i2s_write can be used. \n
                                                     true:  tx use dma @ref uapi_i2s_write_by_dma can be used.
                                             @else   false: TX没有使用DMA，使用 @ref uapi_i2s_write 发送数据 \n
                                                     true:  TX使用DMA，使用 @ref uapi_i2s_write_by_dma 发送数据 @endif */
    uint8_t tx_int_threshold;               /*!< @if Eng i2s tx fifo level to trigger interrupt.
                                             @else 触发中断的txfifo水线 @endif */
    bool rx_dma_enable;                     /*!< @if Eng false: rx not use dma @ref uapi_i2s_write can be used. \n
                                                     true:  rx use dma @ref uapi_i2s_write_by_dma can be used.
                                             @else   false: RX没有使用DMA，使用 @ref uapi_i2s_write 发送数据 \n
                                                     true:  RX使用DMA，使用 @ref uapi_i2s_write_by_dma 发送数据 @endif */
    uint8_t rx_int_threshold;               /*!< @if Eng i2s rx fifo level to trigger interrupt.
                                             @else 触发中断的rxfifo水线 @endif */
} i2s_dma_attr_t;

/**
 * @if Eng
 * @brief  configure i2s dma tranfering param.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  i2s_dma_cfg i2s configuration using dma, see @ref i2s_dma_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置i2s dma传输参数。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  i2s_dma_cfg i2s dma传输时i2s的配置参数，参考 @ref i2s_dma_attr_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
int32_t uapi_i2s_dma_config(sio_bus_t bus, i2s_dma_attr_t *i2s_dma_cfg);

/**
 * @if Eng
 * @brief  i2s write data by data using merge model.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  buffer the data buffer needed to be writen.
 * @param  [in]  length data length needed to be writen.
 * @param  [in]  dma_cfg dma configuration for i2, see @ref i2s_dma_config_t.
 * @param  [in]  arg priv data pointer which can be passed to interrupt callback.
 * @param  [in]  block block tranfering or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  merge模式下i2s通过dma写数据。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  buffer 写数据缓存区.
 * @param  [in]  length 需要传输的数据长度.
 * @param  [in]  dma_cfg i2s dma传输时dma的配置参数, 参考 @ref i2s_dma_config_t.
 * @param  [in]  arg 自定义参数指针，可被传递到中断处理函数.
 * @param  [in]  block 阻塞传输还是非阻塞.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
int32_t uapi_i2s_merge_write_by_dma(sio_bus_t bus, const void *buffer, uint32_t length,
    i2s_dma_config_t *dma_cfg, uintptr_t arg, bool block);

/**
 * @if Eng
 * @brief  i2s read data by data using merge model.
 * @param  [in]  bus The SIO bus. For details, see @ref sio_bus_t.
 * @param  [in]  buffer the buffer to store the data reading form i2s.
 * @param  [in]  length data length needed to be read.
 * @param  [in]  dma_cfg dma configuration for i2s, see @ref i2s_dma_config_t..
 * @param  [in]  arg priv data pointer which can be passed to interrupt callback.
 * @param  [in]  block block tranfering or not.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  merge模式下i2s通过dma读数据。
 * @param  [in]  bus 指定的SIO接口，参考 @ref sio_bus_t 。
 * @param  [in]  buffer 读数据缓存区.
 * @param  [in]  length 需要读的数据长度.
 * @param  [in]  dma_cfg i2s dma传输时dma的配置参数, 参考 @ref i2s_dma_config_t.
 * @param  [in]  arg 自定义参数指针，可被传递到中断处理函数.
 * @param  [in]  block 阻塞传输还是非阻塞.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
int32_t uapi_i2s_merge_read_by_dma(sio_bus_t bus, const void *buffer, uint32_t length,
    i2s_dma_config_t *dma_cfg, uintptr_t arg, bool block);
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
