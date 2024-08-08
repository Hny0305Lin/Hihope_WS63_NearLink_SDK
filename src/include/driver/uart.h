/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides UART driver api \n
 *
 * History: \n
 * 2022-06-01, Create file. \n
 */
#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>
#include "errcode.h"
#include "hal_uart.h"
#include "uart_porting.h"

#if !defined(CONFIG_UART_SUPPORT_TX) && !defined(CONFIG_UART_SUPPORT_RX)
#error UART must support TX or RX
#endif  /* !defined(CONFIG_UART_SUPPORT_TX) && !defined(CONFIG_UART_SUPPORT_RX) */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_uart UART
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of UART pin configuration.
 * @else
 * @brief  UART PIN配置结构体。
 * @endif
 */
typedef hal_uart_pin_config_t uart_pin_config_t;

/**
 * @if Eng
 * @brief  Definition of UART data bit.
 * @else
 * @brief  UART停止位定义。
 * @endif
 */
typedef hal_uart_data_bit_t uart_data_bit_t;

/**
 * @if Eng
 * @brief  Definition of UART parity.
 * @else
 * @brief  UART奇偶校验位定义。
 * @endif
 */
typedef hal_uart_parity_t uart_parity_t;

/**
 * @if Eng
 * @brief  Definition of UART stop bit.
 * @else
 * @brief  UART停止位定义。
 * @endif
 */
typedef hal_uart_stop_bit_t uart_stop_bit_t;

/**
 * @if Eng
 * @brief  Definition of UART attributes.
 * @else
 * @brief  UART基本属性定义。
 * @endif
 */
typedef hal_uart_attr_t uart_attr_t;

/**
 * @if Eng
 * @brief  Definition of UART extral attributes.
 * @else
 * @brief  UART扩展属性定义。
 * @endif
 */
typedef hal_uart_extra_attr_t uart_extra_attr_t;

/**
 * @if Eng
 * @brief  Definition of UART Flow Control.
 * @else
 * @brief  UART流控类型定义。
 * @endif
 */
typedef hal_uart_flow_ctrl_t uart_flow_ctrl_t;

#if defined(CONFIG_UART_SUPPORT_TX) && defined(CONFIG_UART_SUPPORT_DMA)
/**
 * @if Eng
 * @brief  Definition of UART DMA configuration.
 * @else
 * @brief  UART DMA配置数据结构定义。
 * @endif
 */
typedef struct uart_write_dma_config {
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
} uart_write_dma_config_t;
#endif  /* defined(CONFIG_UART_SUPPORT_TX) && defined(CONFIG_UART_SUPPORT_DMA) */

/**
 * @if Eng
 * @brief  UART buffer configuration
 * @else
 * @brief  UART 缓存数据结构定义。
 * @endif
 */
typedef struct uart_buffer_config {
    void *rx_buffer;            /*!< @if Eng Reception buffer pointer.
                                     @else   接收数据的buffer指针  @endif */
    size_t rx_buffer_size;      /*!< @if Eng Reception buffer size in bytes.
                                     @else   接收Buffer的长度  @endif */
} uart_buffer_config_t;

#if defined(CONFIG_UART_SUPPORT_RX)

#define UART_RX_CONDITION_MASK_IDLE            1
#define UART_RX_CONDITION_MASK_SUFFICIENT_DATA 2
#define UART_RX_CONDITION_MASK_FULL            4

/**
 * @if Eng
 * @brief  UART Condition under the RX Callback will be invoked
 * @else
 * @brief  UART在数据接收的时候触发回调的条件定义。
 * @endif
 */
typedef enum uart_rx_condition {
    /** @if Eng  A call-back will be made if the RX data pauses or there is no more RX buffer space. \n
     *           So data is no longer being accepted. When registering this condition, a back-log of.
     *  @else    如果数据接收暂停，或者接收缓存已经满了，就触发数据接收回调，\n
     *           在接收回调处理过程中接收到的UART数据，会直接丢弃。
     *  @endif */
    UART_RX_CONDITION_FULL_OR_IDLE = (UART_RX_CONDITION_MASK_FULL | UART_RX_CONDITION_MASK_IDLE),
    /** @if Eng A call-back will be made as soon as possible after the specified amount of data is
     *          received or there is no more RX buffer space. \n
     *          So data is no longer being accepted. More than the requested data may be
     *          provided to the call-back if there was a back-log of received data.
     *  @else   如果接收缓存已满，或者接收的数据量到达指定的数据长度，就触发数据接收回调，\n
     *          在接收回调处理过程中接收到的UART数据，会直接丢弃。
     *          如果存在接收数据的积压数据，则可以向回调提供超过请求的数据。 @endif */
    UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA = (UART_RX_CONDITION_MASK_FULL | UART_RX_CONDITION_MASK_SUFFICIENT_DATA),

    /** @if Eng A call-back will be made if the RX data buffer is full or
     *          the specified number of bytes has been received or there is a pause. \n
     *          So data is no longer being accepted.
     *  @else   如果接收缓存已满，或者接收的数据量到达指定的数据长度，或者接收数据暂停，就触发数据接收回调，\n
     *          在接收回调处理过程中接收到的UART数据，会直接丢弃。 @endif */
    UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE = (UART_RX_CONDITION_MASK_FULL |
                                                         UART_RX_CONDITION_MASK_SUFFICIENT_DATA |
                                                         UART_RX_CONDITION_MASK_IDLE)
} uart_rx_condition_t;

/**
 * @if Eng
 * @brief  UART Transmission Callback type to call when the RX condition registered on
 *         @ref uapi_uart_register_rx_callback call or on error.
 * @note   This callback is invoked in an interrupt context and the buffer will be freed after the call.
 * @param  buffer pointer to the beginning of the buffer read.
 * @param  length length of the buffer.
 * @param  error true if there was an error on the line for this buffer, false otherwise.
 *         Errors registered are parity, stop bits, data bits.
 * @else
 * @brief  UART接收数据的回调函数，通过 @ref uapi_uart_register_rx_callback 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  buffer 读取数据时用于存储数据的Buffer。
 * @param  length 读取数据时用于存储数据的Buffer的长度。
 * @param  error true表示在接收数据时产生了错误（可能的错误是parity, stop bits, data bits），否则表示正常的数据读取。
 * @endif
 */
typedef void (*uart_rx_callback_t)(const void *buffer, uint16_t length, bool error);

#if defined(CONFIG_UART_SUPPORT_INT_TRIGGER_DMA) && defined(CONFIG_UART_SUPPORT_DMA)
/**
 * @if Eng
 * @brief  UART RX interrupt triggers DMA transfer data
 *         @ref uapi_uart_register_read_by_dma_callback call or on error.
 * @note   This callback is invoked in an interrupt context.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t
 * @param  [in]  buffer Destination address transferred by the DMA.
 * @param  [in]  length Length of the data transferred by the DMA.
 * @param  [in]  dma_cfg Dma configuration of UART. see @ref uart_write_dma_config_t
 * @else
 * @brief  UART RX中断触发DMA搬运数据，通过 @ref uapi_uart_register_read_by_dma_callback 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  buffer DMA搬运的目的地址。
 * @param  [in]  length DMA搬运的数据长度。
 * @param  [in]  dma_cfg 接收数据时候的DMA配置，参考 @ref uart_write_dma_config_t
 * @endif
 */
typedef void (*uart_rx_by_dma_callback_t)(uart_bus_t bus, const void *buffer, uint32_t length,
                                          uart_write_dma_config_t *dma_cfg);
#endif  /* defined(CONFIG_UART_SUPPORT_INT_TRIGGER_DMA) && defined(CONFIG_UART_SUPPORT_DMA) */
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_TX)
/**
 * @if Eng
 * @brief  UART Transmission Callback type to pass to the @ref uapi_uart_write_int function.
 * @note   This callback is invoked in an interrupt context.
 * @param  buffer buffer given on the @ref uapi_uart_write_int call.
 * @param  length length given on the @ref uapi_uart_write_int call.
 * @param  params params given on the @ref uapi_uart_write_int call.
 * @else
 * @brief  UART发送数据的回调函数，在调用 @ref uapi_uart_write_int 时传递给函数。
 * @note   这个函数是在中断上下文中执行的。
 * @param  buffer @ref uapi_uart_write_int 调用时的数据缓存。
 * @param  length @ref uapi_uart_write_int 调用时的数据长度。
 * @param  params @ref uapi_uart_write_int 调用时传递进去的参数。
 * @endif
 */
typedef void (*uart_tx_callback_t)(const void *buffer, uint32_t length, const void *params);
#endif  /* CONFIG_UART_SUPPORT_TX */

/**
 * @if Eng
 * @brief  UART Transmission Callback type to call when the RX condition registered on
 *         uapi_uart_register_frame_error_callback call or uart_register_parity_error_callback on error.
 * @note   This callback is invoked in an interrupt context and the buffer will be freed after the call.
 *         Errors registered are frame.
 * @param  err_info The UART error info data, every data is 32-bit.
 * @param  len The UART error info data, every data is 32-bit.
 * @else
 * @brief  UART错误处理回调函数，通过uapi_uart_register_frame_error_callback注册到驱动中。
 * @note   这个函数是在中断上下文中执行的，而且在执行完成之后会自动释放内存。
 * @param  err_info 错误信息，每一个成员都是32-bit的。
 * @param  len 错误信息的长度。
 * @endif
 */
typedef void (*uart_error_callback_t)(uint32_t *err_info, uint32_t len);

/**
 * @if Eng
 * @brief  Initialize the serial port.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  pins The PINs to use for UART TX, RX, RTS and CTS.
 * @param  [in]  attr Basic configuration of UART. see @ref uart_attr_t.
 * @param  [in]  extra_attr High-level configuration of UART. see @ref uart_extra_attr_t.
 * @param  [in]  uart_buffer_config Specify a reception buffer.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief 初始化指定的串口。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  pins UART中使用的PIN，包括TX, RX, RTS和CTS。
 * @param  [in]  attr UART的基础配置参数，参考 @ref uart_attr_t 。
 * @param  [in]  extra_attr UART的高级配置参数，参考 @ref uart_extra_attr_t 。
 * @param  [in]  uart_buffer_config 指定UART的接收Buffer。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_init(uart_bus_t bus, const uart_pin_config_t *pins,
                         const uart_attr_t *attr, const uart_extra_attr_t *extra_attr,
                         uart_buffer_config_t *uart_buffer_config);

/**
 * @if Eng
 * @brief  Deinitialize the serial port.
 * @param  [in]  bus The UART bus, see @ref uart_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化指定的串口。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_deinit(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Set UART basic configuration.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  attr Basic configuration of UART. see @ref uart_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置串口的基础配置参数。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  attr UART的基础配置参数，参考 @ref uart_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_set_attr(uart_bus_t bus, const uart_attr_t *attr);

/**
 * @if Eng
 * @brief  Get UART basic configuration.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  attr Basic configuration of UART. see @ref uart_attr_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取串口的基础配置参数。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  attr UART的基础配置参数，参考 @ref uart_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_get_attr(uart_bus_t bus, const uart_attr_t *attr);

/**
 * @if Eng
 * @brief  Returns true if there are pending transmissions.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @return true indicates that there're pending transmissions, otherwise false.
 * @else
 * @brief  判断是否存在正在等待的传输。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @return true表示存在正在等待的传输，false表示没有正在等待的传输。
 * @endif
 */
bool uapi_uart_has_pending_transmissions(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Returns true if rx fifo is empty.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @return true indicates that rx fifo is empty, otherwise false.
 * @else
 * @brief  判断RX FIFO是否为空。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @return true表示RX FIFO为空，false表示RX FIFO非空。
 * @endif
 */
bool uapi_uart_rx_fifo_is_empty(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Returns true if tx fifo is empty.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @return true indicates that tx fifo is empty, otherwise false.
 * @else
 * @brief  判断TX FIFO是否为空。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @return true表示TX FIFO为空，false表示TX FIFO非空。
 * @endif
 */
bool uapi_uart_tx_fifo_is_empty(uart_bus_t bus);

#if defined(CONFIG_UART_SUPPORT_RX)
/**
 * @if Eng
 * @brief  Registers a receive callback that will be triggered depending on condition and size.
 * @note   Callback will be called in an interrupt context.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  condition Condition on which the callback will be triggered.
 * @param  [in]  size If condition implies a size this will be used.
 * @param  [in]  callback Receive callback to call.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册接收回调函数，这个回调函数会根据触发条件和Size触发。
 * @note   回调函数在中断上下文执行。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  condition 回调触发的条件，参考 @ref uart_rx_condition_t 。
 * @param  [in]  size 如果触发条件涉及到数据长度，这个参数就表示需要的数据长度。
 * @param  [in]  callback 接收数据的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_register_rx_callback(uart_bus_t bus, uart_rx_condition_t condition,
                                         uint32_t size, uart_rx_callback_t callback);

/**
 * @if Eng
 * @brief  Unregisters a receive callback.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @else
 * @brief  去注册接收回调函数。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @endif
 */
void uapi_uart_unregister_rx_callback(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Registers a parity error callback that will be triggered.
 * @note   Callback will be called in an interrupt context.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  callback Parity error callback to call.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册奇偶校验错误处理的回调函数。
 * @note   回调函数在中断上下文执行。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  callback 奇偶校验错误处理回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_register_parity_error_callback(uart_bus_t bus, uart_error_callback_t callback);

/**
 * @if Eng
 * @brief  Registers a frame error callback that will be triggered.
 * @note   Callback will be called in an interrupt context.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  callback Frame error callback to call.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册帧错误处理回调函数。
 * @note   回调函数在中断上下文执行。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  callback 帧错误错误处理回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_register_frame_error_callback(uart_bus_t bus, uart_error_callback_t callback);

/**
 * @if Eng
 * @brief  Registers a overrun error callback that will be triggered.
 * @note   Callback will be called in an interrupt context.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  callback overrun error callback to call.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册溢出错误处理回调函数。
 * @note   回调函数在中断上下文执行。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  callback 溢出错误错误处理回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_register_overrun_error_callback(uart_bus_t bus, uart_error_callback_t callback);
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_TX)
/**
 * @if Eng
 * @brief  Writes a buffer to an opened UART in polling mode.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t
 * @param  [in]  buffer Pointer to the buffer to write through the UART.
 * @param  [in]  length Length of the buffer to write.
 * @param  [in]  timeout Timeout duration.
 * @return Amount of data write to UART.
 * @else
 * @brief  将数据发送到已经打开的UART上，使用直接发送的方式。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  buffer 要发送的数据Buffer。
 * @param  [in]  length 要发送的数据Buffer长度。
 * @param  [in]  timeout 超时时间。
 * @return 实际发送的数据长度。
 * @endif
 */
int32_t uapi_uart_write(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout);

/**
 * @if Eng
 * @brief  Writes a buffer to an opened UART by interrupt and
 *         calls a finished_with_buffer_func call back when it finish.
 * @note   callback will be called in an interrupt context.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t
 * @param  [in]  buffer Pointer to the buffer to write through the UART.
 * @param  [in]  length Length of the buffer to write.
 * @param  [in]  params Parameter to pass to finished_with_buffer_func.
 * @param  [in]  finished_with_buffer_func Function to call when the buffer is fully transmitted. it will be called in
 *         the way finished_with_buffer_func(buffer, length, params).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使用中断模式将数据发送到已经打开的UART上，当数据发送完成，会调用回调函数。
 * @note   回调函数在中断上下文执行。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  buffer 要发送的数据Buffer。
 * @param  [in]  length 要发送的数据Buffer长度。
 * @param  [in]  params 传递到完成传输的回调函数的参数。
 * @param  [in]  finished_with_buffer_func 数据发送完成的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_write_int(uart_bus_t bus, const uint8_t *buffer, uint32_t length,
                              void *params, uart_tx_callback_t finished_with_buffer_func);

#if defined(CONFIG_UART_SUPPORT_DMA)
/**
 * @if Eng
 * @brief  Write data to the UART by dma.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  buffer Pointer to the write buffer.
 * @param  [in]  length Length of the buffer.
 * @param  [in]  dma_cfg Dma configuration of UART. see @ref uart_write_dma_config_t.
 * @return The count of data have been sent by UART.
 * @else
 * @brief  通过DMA发送数据。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  buffer 要发送的数据Buffer。
 * @param  [in]  length 要发送的数据Buffer长度。
 * @param  [in]  dma_cfg 发送数据时候的DMA配置，参考 @ref uart_write_dma_config_t 。
 * @return 完成发送的数据长度。
 * @endif
 */
int32_t uapi_uart_write_by_dma(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg);

/**
 * @if Eng
 * @brief  Read data from the UART by dma.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t
 * @param  [in]  buffer Pointer to the read buffer.
 * @param  [in]  length Length of the buffer.
 * @param  [in]  dma_cfg Dma configuration of UART. see @ref uart_write_dma_config_t.
 * @return The count of data have been receive by UART.
 * @else
 * @brief  通过DMA读取数据。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  buffer 要发送的数据Buffer。
 * @param  [in]  length 要发送的数据Buffer长度。
 * @param  [in]  dma_cfg 接收数据时候的DMA配置，参考 @ref uart_write_dma_config_t 。
 * @return 完成接收的数据长度。
 * @endif
 */
int32_t uapi_uart_read_by_dma(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg);

#if defined(CONFIG_UART_SUPPORT_INT_TRIGGER_DMA)
/**
 * @if Eng
 * @brief  Registers a rx interrupt trigger dma callback.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  dma_cfg Dma configuration of UART. see @ref uart_write_dma_config_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册中断触发DMA搬运的回调。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  dma_cfg 接收数据时候的DMA配置，参考 @ref uart_write_dma_config_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_register_read_by_dma_callback(uart_bus_t bus, uart_write_dma_config_t *dma_cfg);

/**
 * @if Eng
 * @brief  Unregisters a rx interrupt trigger dma callback.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @else
 * @brief  去注册中断触发DMA搬运的回调。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t。
 * @endif
 */
void uapi_uart_unregister_read_by_dma_callback(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_INT_TRIGGER_DMA */
#endif  /* CONFIG_UART_SUPPORT_DMA */
#endif  /* CONFIG_UART_SUPPORT_TX  */

#if defined(CONFIG_UART_SUPPORT_RX)
/**
 * @if Eng
 * @brief  Read data from UART.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  buffer Pointer to the read buffer.
 * @param  [in]  length Amount of data to be received.
 * @param  [in]  timeout Timeout duration.
 * @return Amount of data read from UART.
 * @else
 * @brief  从UART读取数据。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  buffer 存储接收数据的Buffer。
 * @param  [in]  length 存储接收数据的Buffer长度。
 * @param  [in]  timeout 超时时间。
 * @return 实际读到的数据长度。
 * @endif
 */
int32_t uapi_uart_read(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout);

/**
 * @if Eng
 * @brief  Flush all data held in the supplied UART's RX buffer.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  刷新UART接收Buffer中的数据。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_flush_rx_data(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Update UART's RX buffer address and length.
 * @param  [in]  bus The UART bus. see @ref uart_bus_t.
 * @param  [in]  rx_buffer buffer address.
 * @param  [in]  rx_buffer_size buffer size.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新UART接收Buffer的地址和长度。
 * @param  [in]  bus 串口号，参考 @ref uart_bus_t 。
 * @param  [in]  rx_buffer buffer的地址 。
 * @param  [in]  rx_buffer_size buffer的长度 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_update_rx_buff(uart_bus_t bus, uint8_t *rx_buffer, uint16_t rx_buffer_size);
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend all of the UART channels.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起所有的UART通道。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume all of the UART channels.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复所有的UART通道。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_uart_resume(uintptr_t arg);
#endif  /* CONFIG_UART_SUPPORT_LPM */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif