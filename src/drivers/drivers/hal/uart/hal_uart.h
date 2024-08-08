/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal uart \n
 *
 * History: \n
 * 2023-02-17, Create file. \n
 */
#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "uart_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_api UART API
 * @ingroup  drivers_hal_uart
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of UART pin configuration.
 * @else
 * @brief  UART的PIN配置数据结构定义
 * @endif
 */
typedef struct {
    pin_t tx_pin;                   /*!< @if Eng Transmission PIN.
                                         @else   发送引脚 @endif */
    pin_t rx_pin;                   /*!< @if Eng Reception PIN.
                                         @else   接收引脚 @endif */
    pin_t cts_pin;                  /*!< @if Eng Clear to send PIN.
                                         @else   发送就绪引脚 @endif */
    pin_t rts_pin;                  /*!< @if Eng Request to send PIN to use.
                                         @else   接收就绪引脚 @endif */
} hal_uart_pin_config_t;

/**
 * @if Eng
 * @brief  Definition of UART data bit.
 * @else
 * @brief  UART数据位定义
 * @endif
 */
typedef enum hal_uart_data_bit {
    UART_DATA_BIT_5,                /*!< @if Eng UART data bit 5.
                                         @else   UART数据位为5-bit @endif */
    UART_DATA_BIT_6,                /*!< @if Eng UART data bit 6.
                                         @else   UART数据位为6-bit @endif */
    UART_DATA_BIT_7,                /*!< @if Eng UART data bit 7.
                                         @else   UART数据位为7-bit @endif */
    UART_DATA_BIT_8                 /*!< @if Eng UART data bit 8.
                                         @else   UART数据位为8-bit @endif */
} hal_uart_data_bit_t;

/**
 * @if Eng
 * @brief  Definition of UART parity.
 * @else
 * @brief  UART奇偶校验位定义
 * @endif
 */
typedef enum hal_uart_parity {
    UART_PARITY_NONE,               /*!< @if Eng UART parity none.
                                         @else   UART无奇偶校验 @endif */
    UART_PARITY_ODD,                /*!< @if Eng UART parity odd.
                                         @else   UART奇校验 @endif */
    UART_PARITY_EVEN                /*!< @if Eng UART parity even.
                                         @else   UART偶校验 @endif */
} hal_uart_parity_t;

/**
 * @if Eng
 * @brief  Definition of UART stop bit.
 * @else
 * @brief  UART停止位定义
 * @endif
 */
typedef enum hal_uart_stop_bit {
    UART_STOP_BIT_1,                /*!< @if Eng UART stop bit 1.
                                         @else   UART停止位为1-bit @endif */
    UART_STOP_BIT_2                 /*!< @if Eng UART stop bit 2.
                                         @else   UART停止位为2-bit @endif */
} hal_uart_stop_bit_t;

/**
 * @if Eng
 * @brief  Definition of Uart RX FIFO Level.
 * @else
 * @brief  HAL层UART触发RX中断水线定义
 * @endif
 */
typedef enum {
    UART_FIFO_INT_RX_LEVEL_1_CHARACTER = 0, // only for uart v100 and v151
    UART_FIFO_INT_RX_LEVEL_1_8 = 0, // only for uart v120
    UART_FIFO_INT_RX_LEVEL_1_4 = 1,
    UART_FIFO_INT_RX_LEVEL_1_2 = 2,
    UART_FIFO_INT_RX_LEVEL_2_LESS_THAN_FULL = 3, // only for uart v100 and v151
    UART_FIFO_INT_RX_LEVEL_3_4 = 3,
    UART_FIFO_INT_RX_LEVEL_7_8 = 4, // only for uart v120
} uart_fifo_rx_int_lvl_t;

/**
 * @if Eng
 * @brief  Definition of Uart TX FIFO Level.
 * @else
 * @brief  HAL层UART触发TX中断水线定义
 * @endif
 */
typedef enum {
    UART_FIFO_INT_TX_LEVEL_EQ_0_CHARACTER = 0, // only for uart v100 and v151
    UART_FIFO_INT_TX_LEVEL_1_8 = 0, // only for uart v120
    UART_FIFO_INT_TX_LEVEL_EQ_2_CHARACTER = 1, // only for uart v100 and v151
    UART_FIFO_INT_TX_LEVEL_1_4 = 2,
    UART_FIFO_INT_TX_LEVEL_1_2 = 3,
    UART_FIFO_INT_TX_LEVEL_3_4 = 4, // only for uart v120
    UART_FIFO_INT_TX_LEVEL_7_8 = 5, // only for uart v120
} uart_fifo_tx_int_lvl_t;

/**
 * @if Eng
 * @brief  Definition of the contorl ID of hal uart.
 * @else
 * @brief  UART控制ID定义
 * @endif
 */
typedef enum hal_uart_ctrl_id {
    UART_CTRL_SET_ATTR = 0,        /*!< @if Eng Set uart basic attribute.
                                        @else   设置基础参数 @endif */
    UART_CTRL_GET_ATTR,            /*!< @if Eng Get uart basic attribute.
                                        @else   设置扩展参数 @endif */
#if defined(CONFIG_UART_SUPPORT_TX_INT)
    UART_CTRL_EN_TX_INT,           /*!< @if Eng Enable the tx interrupt.
                                        @else   使能TX中断 @endif */
#endif
    UART_CTRL_EN_RX_INT,           /*!< @if Eng Enable the parity error interrupt.
                                        @else   使能RX中断 @endif */
    UART_CTRL_EN_IDLE_INT,         /*!< @if Eng Enable the RX IDLE interrupt.
                                        @else   使能接收IDLE中断 @endif */
    UART_CTRL_EN_PARITY_ERR_INT,   /*!< @if Eng Enable the frame error interrupt.
                                        @else   使能奇偶校验错误中断 @endif */
    UART_CTRL_EN_FRAME_ERR_INT,    /*!< @if Eng Enable the tx interrupt.
                                        @else   使能帧错误中断 @endif */
    UART_CTRL_CHECK_TX_FIFO_FULL,  /*!< @if Eng Check if tx fifo is full or not.
                                        @else   判断TX FIFO是否为满 @endif */
    UART_CTRL_CHECK_TX_BUSY,       /*!< @if Eng Check if tx data have been sended.
                                        @else   判断是否正在发送数据 @endif */
    UART_CTRL_CHECK_RX_FIFO_EMPTY, /*!< @if Eng Check if rx fifo is full or not.
                                        @else   判断RX FIFO是否为空 @endif */
    UART_CTRL_FIFO_ENABLE,         /*!< @if Eng En(dis)able the fifo.
                                        @else   使能FIFO @endif */
    UART_CTRL_SET_RX_FIFO_LEVEL,   /*!< @if Eng Set rx fifo level.
                                        @else   设置RX FIFO的Level @endif */
    UART_CTRL_SET_TX_FIFO_LEVEL,   /*!< @if Eng Set tx fifo level.
                                        @else   设置TX FIFO的Level @endif */
    UART_CTRL_GET_REG_ADDRS,       /*!< @if Eng Get data register addr.for dma.
                                        @else   获取寄存器地址 @endif */
    UART_CTRL_TX_DMA_PROCESS,      /*!< @if Eng Process the DMA for TX.
                                        @else   处理TX的DMA @endif */
    UART_CTRL_FLOW_CTRL,           /*!< @if Eng Set flow ctrl.
                                        @else   设置流控 @endif */
    UART_CTRL_RESTORE,             /*!< @if Eng Restore the registers.
                                        @else   恢复寄存器值 @endif */
    UART_CTRL_CHECK_UART_BUSY,     /*!< @if Eng Check the uart is busy.
                                        @else   查看uart bus状态是否在忙 @endif */
    UART_CTRL_GET_RX_FIFO_PASSNUM, /*!< @if Eng Get rx fifo level passnum.
                                      @else   获取指定 uart 的 rx 水线可通过数目 @endif */
#if defined(CONFIG_UART_SUPPORT_LPM)
    UART_CTRL_SUSPEND, /*!< @if Eng Suspend the registers.
                            @else   挂起设备，保存寄存器值。 @endif */
    UART_CTRL_RESUME,  /*!< @if Eng Restore the registers.
                            @else   恢复设备，重新配置寄存器值。 @endif */
#endif                 /* CONFIG_UART_SUPPORT_LPM */
#if defined(CONFIG_UART_SUPPORT_DMA)
    UART_CTRL_GET_DMA_DATA_ADDR,
#endif
    UART_CTRL_GET_RX_FIFO_THRESHOLD, /*!< @if Eng get uart rx fifo threshold.
                                        @else   获取uart fifo的阈值 @endif */
    UART_CTRL_MAX,
    UART_CTRL_ID_INVALID = 0xFF
} hal_uart_ctrl_id_t;

/**
 * @if Eng
 * @brief  Definition of the event ID of hal uart.
 * @else
 * @brief  HAL层UART事件ID的定义
 * @endif
 */
typedef enum hal_uart_evt_id {
    UART_EVT_RX_ISR = 1,            /*!< @if Eng Rx isr triggered.
                                         @else 接收中断触发 @endif */
    UART_EVT_TX_ISR,                /*!< @if Eng Tx isr triggered.
                                         @else   TX中断被触发 @endif */
    UART_EVT_IDLE_ISR,              /*!< @if Eng Rx timeout(idle) isr triggered.
                                         @else   IDLE中断被触发 @endif */
    UART_EVT_PARITY_ERR_ISR,        /*!< @if Eng Parity error isr triggered.
                                         @else   奇偶校验错误中断被触发 @endif */
    UART_EVT_FRAME_ERR_ISR,         /*!< @if Eng Frame error isr triggered.
                                         @else   帧错误中断被触发 @endif */
    UART_EVT_BREAK_ERR_ISR,         /*!< @if Eng Break error isr triggered.
                                         @else   线中止错误被触发 @endif */
    UART_EVT_OVERRUN_ERR_ISR,       /*!< @if Eng Overrun error isr triggered.
                                         @else   超载错误被触发 @endif */
} hal_uart_evt_id_t;

/**
 * @if Eng
 * @brief  Definition of the types of DMA.
 * @else
 * @brief  DMA处理类型定义
 * @endif
 */
typedef enum {
    UART_DMA_PROCESS_PREPARE,       /*!< @if Eng DMA process of prepare.
                                         @else   DMA准备处理 @endif */
    UART_DMA_PROCESS_FINISH,        /*!< @if Eng DMA process of finish.
                                         @else   DMA完成处理 @endif */
    UART_DMA_PROCESS_INVALID = 0xFF
} hal_uart_dma_process_t;

/**
 * @brief  Definition of UART Flow Control.
 */
typedef enum {
    UART_FLOW_CTRL_NONE = 0x0,
    UART_FLOW_CTS = 0x1,
    UART_FLOW_RTS = 0x2,
    UART_FLOW_CTRL_RTS_CTS = UART_FLOW_CTS | UART_FLOW_RTS,
    UART_FLOW_CTRL_SOFT = 0x4
} hal_uart_flow_ctrl_t;

/**
 * @if Eng
 * @brief  Definition of UART attributes.
 * @else
 * @brief  UART基础配置参数定义
 * @endif
 */
typedef struct uart_attr {
    uint32_t baud_rate;                 /*!< @if Eng UART baud rate
                                             @else UART波特率 @endif */
    uint8_t data_bits;                  /*!< @if Eng UART data bits, For details see .
                                             @else   UART数据位，参考 @ref uart_data_bit_t @endif */
    uint8_t stop_bits;                  /*!< @if Eng UART stop bits, For details see .
                                             @else   UART数据位，参考 @ref uart_stop_bit_t @endif */
    uint8_t parity;                     /*!< @if Eng UART parity, For details see .
                                             @else   UART 奇偶校验位，参考 @ref uart_parity_t @endif */
    uint8_t flow_ctrl;                  /*!< @if Eng UART flow control, For details see @ref uart_flow_ctrl_t.
                                             @else   UART 流控，参考 @ref uart_flow_ctrl_t @endif */
} hal_uart_attr_t;

/**
 * @if Eng
 * @brief  Definition of UART extral attributes.
 * @else
 * @brief  UART扩展配置参数定义
 * @endif
 */
typedef struct uart_extra_attr {
    bool tx_dma_enable;                     /*!< @if Eng false: tx not use dma @ref uapi_uart_write can be used. \n
                                                     true:  tx use dma @ref uapi_uart_write_by_dma can be used.
                                             @else   false: TX没有使用DMA，使用 @ref uapi_uart_write 发送数据 \n
                                                     true:  TX使用DMA，使用 @ref uapi_uart_write_by_dma 发送数据 @endif */
    uint8_t tx_int_threshold;               /*!< @if Eng uart tx fifo level to trigger interrupt.
                                             @else 触发中断的txfifo水线 @endif */
    bool rx_dma_enable;                     /*!< @if Eng false: rx not use dma @ref uapi_uart_write can be used. \n
                                                     true:  rx use dma @ref uapi_uart_write_by_dma can be used.
                                             @else   false: RX没有使用DMA，使用 @ref uapi_uart_write 发送数据 \n
                                                     true:  RX使用DMA，使用 @ref uapi_uart_write_by_dma 发送数据 @endif */
    uint8_t rx_int_threshold;               /*!< @if Eng uart rx fifo level to trigger interrupt.
                                             @else 触发中断的rxfifo水线 @endif */
} hal_uart_extra_attr_t;

/**
 * @if Eng
 * @brief  Callback of UART.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  evt Event ID.
 * @param  [in]  param Parameter of callback.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  UART的回调函数
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  evt 事件ID.
 * @param  [in]  param 传递到回调的参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_uart_callback_t)(uart_bus_t bus, hal_uart_evt_id_t evt, uintptr_t param);

/**
 * @if Eng
 * @brief  Init device for hal uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  callback Callback of the hal uart.
 * @param  [in]  pins The PINs to use for UART TX, RX, RTS and CTS.
 * @param  [in]  attr UART attributes.
 * @param  [in]  flow_ctrl UART flow control type.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART的初始化接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  callback 回调函数
 * @param  [in]  pins UART的引脚，包括TX, RX, RTS 和 CTS
 * @param  [in]  attr UART的基本配置参数
 * @param  [in]  flow_ctrl UART流控类型
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_uart_init_t)(uart_bus_t bus, hal_uart_callback_t callback, const hal_uart_pin_config_t *pins,
                                     const hal_uart_attr_t *attr, hal_uart_flow_ctrl_t flow_ctrl);
/**
 * @if Eng
 * @brief  Deinit device for hal uart.
 * @param  [in] bus The uart bus. see @ref uart_bus_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART去初始化接口
 * @param  [in] bus 串口号， 参考 @ref uart_bus_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_uart_deinit_t)(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Control interface for hal uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  id ID of the uart control.
 * @param  [in]  param Parameter for callback.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART控制接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  id UART控制请求ID
 * @param  [in]  param 传递给控制回调的参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_uart_ctrl_t)(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Write interface for hal uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  data Data buffer to write.
 * @param  [in]  len Data len to write.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART发送数据接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  data 要发送的数据Buffer
 * @param  [in]  len 要发送的数据Buffer长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_uart_write_t)(uart_bus_t bus, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Read interface for hal uart.
 * @param  [in]  bus The device pointer to manager the hal uart.
 * @param  [in]  data Data buffer to read.
 * @param  [in]  len Data len to read.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART读取数据接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  data 用于存放读取数据的Buffer
 * @param  [in]  len 用于存放读取数据的Buffer长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef int32_t (*hal_uart_read_t)(uart_bus_t bus, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Configuring DMA Transmission interface for hal uart.
 * @param  [in]  bus The device pointer to manager the hal uart.
 * @param  [in]  extra_attr High-level configuration of uart. see @ref hal_uart_extra_attr_t.
 * @else
 * @brief  HAL层配置DMA传输接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  extra_attr UART的高级配置参数，参考 @ref hal_uart_extra_attr_t 。
 * @endif
 */
typedef void (*hal_uart_dma_cfg_t)(uart_bus_t bus, const hal_uart_extra_attr_t *extra_attr);

/**
 * @if Eng
 * @brief  Interface between uart driver and uart hal.
 * @else
 * @brief  Driver层UART和HAL层UART的接口
 * @endif
 */
typedef struct {
    hal_uart_init_t   init;               /*!< @if Eng Init device interface.
                                               @else   HAL层UART的初始化接口 @endif */
    hal_uart_deinit_t deinit;             /*!< @if Eng Deinit device interface.
                                               @else   HAL层UART去初始化接口 @endif */
    hal_uart_ctrl_t   ctrl;               /*!< @if Eng Control device interface.
                                               @else   HAL层UART控制接口 @endif */
    hal_uart_write_t  write;              /*!< @if Eng Write data interface.
                                               @else   HAL层UART发送数据接口 @endif */
    hal_uart_read_t   read;               /*!< @if Eng Read data interface.
                                               @else   HAL层UART读取数据接口 @endif */
    hal_uart_dma_cfg_t   dma_cfg;         /*!< @if Eng DMA config data interface.
                                               @else   HAL层配置DMA传输接口 @endif */
} hal_uart_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_uart_funcs_t into the g_hal_uarts_funcs.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  funcs Interface between uart driver and uart hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_uart_funcs_t 到 g_hal_uarts_funcs
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  funcs Driver层UART和HAL层UART的接口实例
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_register_funcs(uart_bus_t bus, hal_uart_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_uart_funcs_t from the g_hal_uarts_funcs.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从g_hal_uarts_funcs注销 @ref hal_uart_funcs_t
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_unregister_funcs(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Get the base address of sepecified UART.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The base address of sepecified UART.
 * @else
 * @brief  获取指定UART的基地址。
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t 。
 * @return 指定UART的基地址。
 * @endif
 */
uintptr_t hal_uart_base_addrs_get(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Init the UART which will set the base address of registers.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  初始化UART，设置寄存器的基地址
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_regs_init(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Get interface between uart driver and uart hal, see @ref hal_uart_funcs_t.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return Interface between uart driver and uart hal, see @ref hal_uart_funcs_t.
 * @else
 * @brief  获取Driver层UART和HAL层UART的接口实例，参考 @ref hal_uart_funcs_t.
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @return Driver层UART和HAL层UART的接口实例，参考 @ref hal_uart_funcs_t.
 * @endif
 */
hal_uart_funcs_t *hal_uart_get_funcs(uart_bus_t bus);

/**
 * @if Eng
 * @brief  UART set tx fifo level for triggering interrupt.
 * @param  bus The uart bus, see @ref uart_bus_t.
 * @param  level the fifo level value, see @ref uart_fifo_tx_int_lvl_t.
 * @else
 * @brief  设置uart触发中断的发送fifo阈值
 * @param  bus 串口号， 参考 @ref uart_bus_t
 * @param  level 触发中断的水线值 @ref uart_fifo_tx_int_lvl_t
 * @endif
 */
errcode_t hal_uart_set_tx_fifo_int_level(uart_bus_t bus, uart_fifo_tx_int_lvl_t level);

/**
 * @if Eng
 * @brief  UART set rx fifo level for triggering rx interrupt.
 * @param  bus The uart bus, see @ref uart_bus_t.
 * @param  level the fifo level value,see @ref uart_fifo_rx_int_lvl_t.
 * @else
 * @brief  设置uart触发接收中断的接收fifo阈值
 * @param  bus 串口号， 参考 @ref uart_bus_t
 * @param  level 触发中断的水线值 @ref uart_fifo_rx_int_lvl_t 。
 * @endif
 */
errcode_t hal_uart_set_rx_fifo_int_level(uart_bus_t bus, uart_fifo_rx_int_lvl_t level);

/**
 * @if Eng
 * @brief  Init device for hal uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  callback Callback of the hal uart.
 * @param  [in]  pins The PINs to use for UART TX, RX, RTS and CTS.
 * @param  [in]  attr UART attributes.
 * @param  [in]  flow_ctrl UART flow control type.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART的初始化接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  callback 回调函数
 * @param  [in]  pins UART的引脚，包括TX, RX, RTS 和 CTS
 * @param  [in]  attr UART的基本配置参数
 * @param  [in]  flow_ctrl UART流控类型
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_init(uart_bus_t bus, hal_uart_callback_t callback, const hal_uart_pin_config_t *pins,
                        const hal_uart_attr_t *attr, hal_uart_flow_ctrl_t flow_ctrl);
/**
 * @if Eng
 * @brief  Deinit device for hal uart.
 * @param  [in] bus The uart bus. see @ref uart_bus_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART去初始化接口
 * @param  [in] bus 串口号， 参考 @ref uart_bus_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_deinit(uart_bus_t bus);

/**
 * @if Eng
 * @brief  Control interface for hal uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  id ID of the uart control.
 * @param  [in]  param Parameter for callback.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART控制接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  id UART控制请求ID
 * @param  [in]  param 传递给控制回调的参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_ctrl(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Write interface for hal uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  data Data buffer to write.
 * @param  [in]  len Data len to write.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART发送数据接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  data 要发送的数据Buffer
 * @param  [in]  len 要发送的数据Buffer长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_uart_write(uart_bus_t bus, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Read interface for hal uart.
 * @param  [in]  bus The device pointer to manager the hal uart.
 * @param  [in]  data Data buffer to read.
 * @param  [in]  len Data len to read.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层UART读取数据接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  data 用于存放读取数据的Buffer
 * @param  [in]  len 用于存放读取数据的Buffer长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
int32_t hal_uart_read(uart_bus_t bus, const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Configuring DMA Transmission interface for hal uart.
 * @param  [in]  bus The device pointer to manager the hal uart.
 * @param  [in]  extra_attr High-level configuration of uart. see @ref hal_uart_extra_attr_t.
 * @else
 * @brief  HAL层配置DMA传输接口
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @param  [in]  extra_attr UART的高级配置参数，参考 @ref hal_uart_extra_attr_t 。
 * @endif
 */
void hal_uart_set_dma_config(uart_bus_t bus, const hal_uart_extra_attr_t *extra_attr);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
