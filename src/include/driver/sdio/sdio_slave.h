/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides sdio slave driver api \n
 *
 * History: \n
 * 2023-01-17, Create file. \n
 */

#ifndef SDIO_H
#define SDIO_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "sdio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_sdios SDIOS
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  SDIO message structure.
 * @else
 * @brief  SDIO消息结构体。
 * @endif
 */
typedef struct sdio_msg {
    uint32_t pending_msg;                       /*!< @if Eng Pending message.
                                                     @else   挂起中的消息 @endif */
    uint32_t sending_msg;                       /*!< @if Eng Sending message.
                                                     @else   发送中的消息 @endif */
} sdio_msg_t;

/**
 * @if Eng
 * @brief  SDIO works status.
 * @else
 * @brief  SDIO工作状态
 * @endif
 */
typedef enum {
    SDIO_CHANNEL_ERR    = 0x0,          /*!< @if Eng SDIO err.
                                         @else SDIO错误 @endif */
    SDIO_CHANNEL_RESET,                 /*!< @if Eng Reset SDIO.
                                         @else 重置SDIO @endif */
    SDIO_CHANNEL_INIT,                  /*!< @if Eng Initialize the SDIO.
                                         @else 初始化SDIO @endif */
    SDIO_CHANNEL_SLEEP,                 /*!< @if Eng Sleep the SDIO.
                                         @else SDIO睡眠状态 @endif */
    SDIO_CHANNEL_WAKE,                  /*!< @if Eng Wake the SDIO.
                                         @else 唤醒SDIO @endif */
    SDIO_CHANNEL_WORK,                  /*!< @if Eng SDIO work.
                                         @else SDIO工作状态 @endif */
    SDIO_CHANNEL_BUTT                   /*!< @if Eng SDIO status number.
                                         @else SDIO状态枚举数 @endif */
} sdio_channel_t;

/**
 * @if Eng
 * @brief  SDIO status informatio.
 * @else
 * @brief  SDIO状态信息。
 * @endif
 */
typedef struct sdio_status_info {
    uint8_t     allow_sleep;                    /*!< @if Eng Allow sleep.
                                                     @else   允许睡眠 @endif */
    uint8_t     tx_status;                      /*!< @if Eng Tx status.
                                                     @else   tx状态 @endif */
    uint8_t     sleep_status;                   /*!< @if Eng Sleep status.
                                                     @else   睡眠状态 @endif */
    sdio_channel_t  work_status;         /*!< @if Eng Work status.
                                                     @else   工作状态 @endif */
} sdio_status_info_t;

#if defined(CONFIG_SDIO_EXTEND_INFO)
/**
 * @if Eng
 * @brief  SDIO extend function structure.
 * @else
 * @brief  SDIO扩展功能结构体。
 * @endif
 */
typedef struct sdio_extendfunc {
    uint32_t                   int_stat;
    uint32_t                   msg_stat;
    uint32_t                   xfer_count;
    uint32_t                   credit_info;
    uint8_t                    comm_reg[SDIO_EXTENDREG_COUNT];
    int32_t                    valid_commreg_cnt;
    int8_t                     commreg_isvalid;
    int8_t                     credit_isvalid;
} sdio_extendfunc_t;
#endif

/**
 * @if Eng
 * @brief  SDIO status structure.
 * @else
 * @brief  SDIO状态结构体。
 * @endif
 */
typedef struct sdio_status {
    uint16_t          rd_arg_invalid_cnt;
    uint16_t          wr_arg_invlaid_cnt;
    uint16_t          unsupport_int_cnt;
    uint16_t          mem_int_cnt;
    uint16_t          fn1_wr_over;
    uint16_t          fn1_rd_over;
    uint16_t          fn1_rd_error;
    uint16_t          fn1_rd_start;
    uint16_t          fn1_wr_start;
    uint16_t          fn1_rst;
    uint16_t          fn1_msg_rdy;
    uint16_t          fn1_ack_to_arm_int_cnt;
    uint16_t          fn1_adma_end_int;
    uint16_t          fn1_suspend;
    uint16_t          fn1_resume;
    uint16_t          fn1_adma_int;
    uint16_t          fn1_adma_err;
    uint16_t          fn1_en_int;
    uint16_t          fn1_msg_isr;
    uint16_t          soft_reset_cnt;
} sdio_status_t;

/**
 * @if Eng
 * @brief  SDIO transfer channel structure.
 * @else
 * @brief  SDIO传输通道结构体。
 * @endif
 */
typedef struct sdio_chan_info {
    uint32_t                 send_data_len;
    uint16_t                 last_msg;
    uint16_t                 panic_forced_timeout;
    uint16_t                 chan_msg_cnt[D2H_MSG_COUNT];
} sdio_chan_info_t;

/**
 * @if Eng
 * @brief  SDIO information structure.
 * @else
 * @brief  SDIO信息结构体。
 * @endif
 */
typedef struct sdio_info {
    uint8_t                 volt_switch_flag;

    uint8_t                 host_to_device_msg_flag;

    uint16_t                reinit_times;
    uint16_t                gpio_int_times;
    uint16_t                pad;
    sdio_status_t           sdio_status;
    sdio_chan_info_t        chan_info;
    sdio_msg_t         sdio_msg_status;
} sdio_info_t;

/**
 * @if Eng
 * @brief  SDIO interrupt callback structure.
 * @else
 * @brief  SDIO中断回调函数结构体
 * @endif
 */
typedef struct sdio_callback_func {
    uint32_t (*read_start_callback)(uint32_t len, uint8_t *dma_tbl);    /*!< @if Eng HOST reading.
                                                                             @else DEVICE感知到HOST发起了读操作 @endif */
    uint32_t (*read_over_callback)(void);                               /*!< @if Eng HOST reading over.
                                                                             @else DEVICE感知到HOST读操作结束 @endif */
    void (*read_err_callback)(void);                                    /*!< @if Eng HOST read error.
                                                                             @else DEVICE感知到HOST读数据错误 @endif */
    uint32_t (*write_start_callback)(uint32_t len, uint8_t *dma_tbl);   /*!< @if Eng HOST writting.
                                                                             @else DEVICE感知到HOST发起了写操作 @endif */
    uint32_t (*write_over_callback)(void);                              /*!< @if Eng HOST write over.
                                                                             @else DEVICE感知到HOST写操作结束 @endif */
    void (*process_msg_callback)(uint32_t);                             /*!< @if Eng HOST getting message.
                                                                             @else DEVICE接收到HOST发来的消息 @endif */
    void (*soft_rst_callback)(void);                                    /*!< @if Eng HOST getting reset interruption.
                                                                             @else DEVICE接收到HOST发来的软复位 @endif */
} sdio_callback_func_t;

/**
 * @if Eng
 * @brief  use this callback to notify host msg event occurs.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @else
 * @brief  通知host消息或数据事件存在的回调。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @endif
 */
typedef void (*notify_host_event_t)(sdio_bus_t bus);

/**
 * @if Eng
 * @brief  Initialize the SDIO.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化SDIO。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_init(sdio_bus_t bus);

/**
 * @if Eng
 * @brief  Deinitialize the SDIO.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @else
 * @brief  去初始化SDIO。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @endif
 */
void uapi_sdio_slave_deinit(sdio_bus_t bus);

/**
 * @if Eng
 * @brief  Reinitiate the SDIO.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  重新初始化SDIO。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_reinit(sdio_bus_t bus);

/**
 * @if Eng
 * @brief  SDIO IP chip reset function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @else
 * @brief  IP芯片复位函数。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @endif
 */
void uapi_sdio_slave_soft_reset(sdio_bus_t bus);

 /**
 * @if Eng
 * @brief  SDIO register interrupt callback function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [out]  fun SDIO interrupt callback structure. see @ref sdio_callback_func_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SDIO 注册中断回调函数。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [out]  fun SDIO 中断回调函数结构体，参考 @ref sdio_callback_func_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_register_callback(sdio_bus_t bus, const sdio_callback_func_t *fun);

 /**
 * @if Eng
 * @brief  Configuring the end flag of the EDMA descriptor.
 * @param  [out]  adma_tab adma table first address.
 * @param  [in]  adma_index adma Transmission channel number.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置内部EDMA 描述符 结束标志。
 * @param  [out]  adma_tab adma table首地址。
 * @param  [in]  adma_index 传输通道号。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_complete_send(uint8_t *adma_tab, uint32_t adma_index);

 /**
 * @if Eng
 * @brief  set sdio pad adma table function.
 * @param  [out]  adma_tab adma table first address.
 * @param  [in]  adma_index adma Transmission channel number.
 * @param  [in]  padlen Length of data to be sent after data alignment.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置数据对齐后的ADMA表。
 * @param  [out]  adma_tab adma table首地址。
 * @param  [in]  adma_index 传输通道号。
 * @param  [in]  padlen 据对齐后要发送的数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_set_pad_admatab(uint8_t *adma_tab, uint32_t adma_index, uint32_t padlen);

#if defined(CONFIG_SDIO_EXTEND_INFO)
 /**
 * @if Eng
 * @brief  write extend information function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [out]  extfunc Extended information structure pointer. see @ref sdio_extendfunc_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  写扩展信息接口。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [out]  extfunc 扩展信息结构体指针，参考 @ref sdio_extendfunc_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_write_extend_info(sdio_bus_t bus, sdio_extendfunc_t* extfunc);

 /**
 * @if Eng
 * @brief  get sdio extend configuration.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @retval pointer to extend info buffer. For details, see @ref sdio_extendfunc_t.
 * @else
 * @brief  获取扩展区配置信息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @retval 扩展区配置信息指针，参考 @ref sdio_extendfunc_t 。
 * @endif
 */
sdio_extendfunc_t* uapi_sdio_slave_get_extend_info(sdio_bus_t bus);
#endif

 /**
 * @if Eng
 * @brief  start to send data.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  data_len Length of sent data.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动数据发送接口。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  data_len 发送数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_prepare_send_data(sdio_bus_t bus, uint32_t data_len);

 /**
 * @if Eng
 * @brief  set adma table.
 * @param  [out]  adma_tab adma table first address.
 * @param  [in]  adma_index adma Transmission channel number.
 * @param  [out]  data_addr adma transmission destination address.
 * @param  [in]  data_len adma Transmission data length.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置adma传输通道。
 * @param  [out]  adma_tab adma table首地址。
 * @param  [in]  adma_index 传输通道号。
 * @param  [out]  data_addr 传输目的地址。
 * @param  [in]  data_len 传输数据长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_set_admatab(uint8_t *adma_tab, uint32_t adma_index, const uint32_t *data_addr,
                                      uint32_t data_len);

 /**
 * @if Eng
 * @brief  schedule sdio pending message.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other   If sdio not in work status or there is msg sending, return failure. For details, see @ref errcode_t.
 * @else
 * @brief  调度sdio消息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @retval ERRCODE_SUCC 如果没有挂起的消息，或者将挂起的消息发送成功，返回成功。
 * @retval Other        如果sdio不在工作状态，或者sdio正在发送消息，返回失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_sched_msg(sdio_bus_t bus);

 /**
 * @if Eng
 * @brief  sync send message.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  msg The message, range [0-31].
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  将消息加入消息队列并发送。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  msg 给定消息，范围[0-31]。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_sync_msg(sdio_bus_t bus, uint32_t msg);

 /**
 * @if Eng
 * @brief  send given message ack.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  msg the current sending msg will be overwrite by this msg.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  发送指定消息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  msg 当前正在发送的消息将该消息覆盖。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_send_msg_ack(sdio_bus_t bus, uint32_t msg);

 /**
 * @if Eng
 * @brief  Clear given msg and add new msg to pending msg and send.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  send_msg Message in range [0~31] which will be sent.
 * @param  [in]  clear_msg Message in range [0~31] which will be cleard.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清除消息队列中挂起的指定消息，将新消息加入消息队列并发送。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  send_msg 指定发送的消息号，范围[0-31]。
 * @param  [in]  clear_msg 指定清除的消息号，范围[0-31]。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_process_msg(sdio_bus_t bus, uint32_t send_msg, uint32_t clear_msg);

 /**
 * @if Eng
 * @brief  sdio is pending given message.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  msg The message, range [0-31].
 * @retval true   The message at pending status.
 * @retval false  The message at other status.
 * @else
 * @brief  判断sdio是否挂起指定的消息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  msg 给定消息，范围[0-31]。
 * @retval true  指定消息处于挂起状态。
 * @retval false 指定消息不处于挂起状态。
 * @endif
 */
bool uapi_sdio_slave_is_pending_msg(sdio_bus_t bus, uint32_t msg);

 /**
 * @if Eng
 * @brief  Check whether the sdio is sending a specified message.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  msg The message, range [0-31].
 * @retval true   The message at sending status.
 * @retval false  The message at other status.
 * @else
 * @brief  判断sdio是否正在发送指定的消息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  msg 给定消息，范围[0-31]。
 * @retval true  指定消息处于发送状态。
 * @retval false 指定消息不处于发送状态。
 * @endif
 */
bool uapi_sdio_slave_is_sending_msg(sdio_bus_t bus, uint32_t msg);

 /**
 * @if Eng
 * @brief  register callback to notify host msg or data event occurs.
 * @param  [in]  event_callback callback function when sending msg or data. see @ref notify_host_event_t
 * @else
 * @brief  注册通知Host发送了消息或数据的回调函数。
 * @param  [in]  event_callback 消息或数据发送时调用的回调函数，参考 @ref notify_host_event_t
 * @endif
 */
void uapi_sdio_slave_register_notify_message_callback(notify_host_event_t event_callback);

 /**
 * @if Eng
 * @brief  SDIO read retry when read err event occurs.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  read_bytes Length of read data.
 * @else
 * @brief  SDIO 读错误事件存在时尝试重新读取。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  read_bytes 读取的数据长度。
 * @endif
 */
void uapi_sdio_slave_read_retry_when_read_err(sdio_bus_t bus, uint32_t read_bytes);

 /**
 * @if Eng
 * @brief  Init sdio with no wait .
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @else
 * @brief  非阻塞初始化。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @endif
 */
void uapi_sdio_slave_init_no_wait(sdio_bus_t bus);

 /**
 * @if Eng
 * @brief  Get whether host clock ready.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @retval true  Ready.
 * @retval false Not ready.
 * @else
 * @brief  判断HOST时钟是否就绪。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @retval true  准备就绪。
 * @retval false 没有准备就绪。
 * @endif
 */
bool uapi_sdio_slave_host_clk_ready(sdio_bus_t bus);

 /**
 * @if Eng
 * @brief  get current sdio status.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [out]  satus_info Storage status information BUFFER pointer. see @ref sdio_status_info_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取当前SDIO通道状态信息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [out]  satus_info 存储状态信息BUFFER指针，参考 @ref sdio_status_info_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_get_status(sdio_bus_t bus, sdio_status_info_t* satus_info);

 /**
 * @if Eng
 * @brief  set current sdio status.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [out]  satus_info Pointing to the storage status information buffer. see @ref sdio_status_info_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置当前SDIO通道状态信息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [out]  satus_info 指向存储状态信息buffer，参考 @ref sdio_status_info_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_sdio_slave_set_status(sdio_bus_t bus, const sdio_status_info_t* satus_info);

 /**
 * @if Eng
 * @brief  SDIO memory init function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @else
 * @brief  SDIO 内存初始化函数。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @endif
 */
void uapi_sdio_slave_memory_init(sdio_bus_t bus);

 /**
 * @if Eng
 * @brief  Get SDIO info function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @else
 * @brief  获取SDIO状态信息。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @endif
 */
sdio_info_t* uapi_sdio_slave_get_info(sdio_bus_t bus);

#if defined(CONFIG_SDIO_EXTEND_INFO)
 /**
 * @if Eng
 * @brief  write extend information value function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  offset The address offset.
 * @param  [in]  val The value of setting.
 * @else
 * @brief  写扩展信息接口。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  offset 地址偏移量。
 * @param  [in]  val 设置的值。
 * @endif
 */
void uapi_sdio_slave_set_extend_val_info(sdio_bus_t bus, uint32_t offset, uint32_t val);

 /**
 * @if Eng
 * @brief  get extend information value function.
 * @param  [in]  bus SDIO bus. see @ref sdio_bus_t
 * @param  [in]  offset The address offset.
 * @retval get value of extend info address.
 * @else
 * @brief  获取扩展信息接口。
 * @param  [in]  bus SDIO通道， 参考 @ref sdio_bus_t
 * @param  [in]  offset 地址偏移量。
 * @retval 获取扩展信息地址的值。
 * @endif
 */
uint32_t uapi_sdio_slave_get_extend_val_info(sdio_bus_t bus, uint32_t offset);
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