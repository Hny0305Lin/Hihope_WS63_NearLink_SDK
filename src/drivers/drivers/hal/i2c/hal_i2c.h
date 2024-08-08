/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL i2c \n
 *
 * History: \n
 * 2022-06-07, Create file. \n
 */
#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>
#include "errcode.h"
#include "i2c_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_api I2C
 * @ingroup  drivers_hal_i2c
 * @{
 */

#define I2C_SS_MODE_BAUDRATE_HIGH_LIMIT  (100 * 1000)    /*!< @if Eng I2C baudrate high limit in standard mode.
                                                              @else   标准模式下I2C波特率最大限值 @endif */
#define I2C_FS_MODE_BAUDRATE_HIGH_LIMIT  (400 * 1000)    /*!< @if Eng I2C baudrate high limit in fast mode.
                                                              @else   快速模式下I2C波特率最大限值 @endif */
#define I2C_HS_MODE_BAUDRATE_HIGH_LIMIT  (3400 * 1000)   /*!< @if Eng I2C baudrate high limit in high speed mode.
                                                              @else   高速模式下I2C波特率最大限值 @endif */

#define I2C_HS_MODE_MASTER_CODE_MAX  7   /*!< @if Eng I2C max master code in high speed mode.
                                              @else   高速模式下I2C主机码最大值 @endif */

/**
 * @if Eng
 * @brief  Definition of I2C speed mode.
 * @else
 * @brief  I2C速率模式定义
 * @endif
 */
typedef enum hal_i2c_speed_mode {
    I2C_SPEED_MODE_HARDCODE = 0,
    I2C_SPEED_MODE_SS = 0,              /*!< @if Eng I2C standard speed mode.
                                             @else   I2C标速模式。 @endif */
    I2C_SPEED_MODE_FS,                  /*!< @if Eng I2C fast speed mode.
                                             @else   I2C快速模式。 @endif */
    I2C_SPEED_MODE_HS,                  /*!< @if Eng I2C high speed mode.
                                             @else   I2C高速模式。 @endif */
    I2C_SPEED_MODE_MAX_NUM
} hal_i2c_speed_mode_t;

/**
 * @if Eng
 * @brief  Definition of I2C data operation type.
 * @else
 * @brief  I2C数据操作类型定义。
 * @endif
 */
typedef enum hal_i2c_data_operation_type {
    I2C_DATA_OPERATION_TYPE_POLL,    /*!< @if Eng I2C data operation type is poll.
                                          @else   I2C 对数据进行轮询处理 @endif */
    I2C_DATA_OPERATION_TYPE_INT,     /*!< @if Eng I2C data operation type is interrupt.
                                          @else   I2C 对数据进行中断处理 @endif */
    I2C_DATA_OPERATION_TYPE_DMA,     /*!< @if Eng I2C data operation type is dma.
                                          @else   I2C 对数据进行DMA处理 @endif */
    I2C_DATA_OPERATION_TYPE_INVLAID
} hal_i2c_data_operation_type_t;

/**
 * @if Eng
 * @brief  Definition of the event ID of hal i2c.
 * @else
 * @brief  I2C事件ID的定义。
 * @endif
 */
typedef enum hal_i2c_evt_id {
    I2C_EVT_TX_READY = 0,           /*!< @if Eng Tx fifo is at or below tx threshold, TX_EMPTY interrupt.
                                         @else   发送FIFO等于小于发送阈值，TX_EMPTY中断。 @endif */
    I2C_EVT_RX_READY,               /*!< @if Eng Rx fifo reaches or goes above the rx threshold, RX_FULL interrupt.
                                         @else   接收FIFO到达或者超过接收阈值，RX_FULL中断。 @endif */
    I2C_EVT_TX_AVAILABLE,           /*!< @if Eng I2C acts as a slave and receives master read command, RD_REQ interrupt.
                                         @else   I2C作为从机收到主机读取数据指令，RD_REQ中断。 @endif */
    I2C_EVT_TRANSMITION_START,      /*!< @if Eng New Send or receive process start,START_DET interrupt.
                                         @else   新的传输或者接收任务开始，START_DET中断。 @endif */
    I2C_EVT_TRANSMITION_DONE,       /*!< @if Eng Send or receive process end, STOP_DET interrupt.
                                         @else   传输或者接收任务结束，STOP_DET中断。 @endif */
    I2C_EVT_TRANSMITION_ABRT,       /*!< @if Eng Send or receive process abrt, TX_ABRT interrupt.
                                         @else   传输或者接收出现异常，TX_ABRT中断。 @endif */
    I2C_EVT_TRANSMITION_BUSY,       /*!< @if Eng Bus busy, BUS_BUSY interrupt.
                                         @else   总线忙，BUS_BUSY中断。 @endif */
    I2C_EVT_INVALID = 0xF
} hal_i2c_evt_id_t;

/**
 * @if Eng
 * @brief  Definition of the contorl ID of hal i2c.
 * @else
 * @brief  I2C控制ID定义。
 * @endif
 */
typedef enum hal_i2c_ctrl_id {
    I2C_CTRL_WRITE_PREPARE = 0,     /*!< @if Eng Prepare configuration before write.
                                         @else   发送前的准备配置。 @endif */
    I2C_CTRL_WRITE_RESTORE,         /*!< @if Eng Restore configuration after write.
                                         @else   发送完成后恢复配置。 @endif */
    I2C_CTRL_READ_PREPARE,          /*!< @if Eng Prepare configuration before read.
                                         @else   接收数据前准备配置。 @endif */
    I2C_CTRL_READ_RESTORE,          /*!< @if Eng Restore configuration after read.
                                         @else   发送完成后恢复配置。 @endif */
    I2C_CTRL_GET_WRITE_NUM,         /*!< @if Eng Get write num can be used.
                                         @else   获取可以发送的数据长度。 @endif */
    I2C_CTRL_GET_READ_NUM,          /*!< @if Eng Get read fifo num can be used.
                                         @else   获取可以发送的数据长度。 @endif */
    I2C_CTRL_CHECK_TX_AVAILABLE,    /*!< @if Eng Check whether tx fifo can be filled
                                         @else   检查是否可以对tx fifo 进行填充。 @endif */
    I2C_CTRL_CHECK_RX_AVAILABLE,    /*!< @if Eng Check whether rx fifo is accessible
                                         @else   检查是否可以对rx fifo 进行访问。 @endif */
    I2C_CTRL_FLUSH_RX_FIFO,         /*!< @if Eng Flush rx fifo.
                                         @else   刷新RX FIFO。 @endif */
    I2C_CTRL_CHECK_TX_PROCESS_DONE, /*!< @if Eng Ensure send process done.
                                         @else   确认发送传输过程是否完成。 @endif */
    I2C_CTRL_CHECK_RX_PROCESS_DONE, /*!< @if Eng Ensure receive process done.
                                         @else   确认接收传输过程是否完成。 @endif */
    I2C_CTRL_CHECK_RESTART_READY,   /*!< @if Eng Ensure can start cfg restart.
                                         @else   确认可以开始配置restart。 @endif */
    I2C_CTRL_NORMAL_MAX = I2C_CTRL_CHECK_RESTART_READY,
    I2C_CTRL_CHECK_TRANSMIT_ABRT,   /*!< @if Eng Check transmit abnormal abort status.
                                         @else   检查传输异常取消状态。 @endif */
    I2C_CTRL_GET_DMA_DATA_ADDR,     /*!< @if get data cmd register.
                                         @else   获取数据命令寄存器。 @endif */
    I2C_CTRL_CHECK_TX_FIFO_EMPTY,   /*!< @if get Check whether tx fifo is empth.
                                         @else   检查tx fifo 是否为空。 @endif */
    I2C_CTRL_MAX,
    I2C_CTRL_ID_INVALID = 0x1F
} hal_i2c_ctrl_id_t;

/**
 * @if Eng
 * @brief  Definition of I2C prepare params for send or receive.
 * @else
 * @brief  I2C准备收发的参数数据结构定义。
 * @endif
 */
typedef struct hal_i2c_prepare_config {
    uint8_t operation_type;         /*!< @if Eng Operation type of send or receive.
                                         @else   收发的操作类型。 @endif */
    uint8_t reserved;
    uint16_t addr;                  /*!< @if Eng I2C Address for send or receive, valid only for master
                                         @else   主机I2C要对接收发的从机地址。 @endif */
    uint32_t total_len;             /*!< @if Eng total_len len for send or receive, valid only for master
                                         @else   主机I2C要收发的数据长度。 @endif */
} hal_i2c_prepare_config_t;

/**
 * @if Eng
 * @brief  Definition of I2C data buffer wrap.
 * @else
 * @brief  I2C缓存数据封装数据结构定义。
 * @endif
 */
typedef struct hal_i2c_buffer_wrap {
    uint8_t *buffer;                /*!< @if Eng Buffer pointer.
                                         @else   缓存数据的buffer指针。  @endif */
    uint32_t len;                   /*!< @if Eng Buffer size in bytes.
                                         @else   缓存数据Buffer的长度。  @endif */
    uint8_t stop_flag;              /*!< @if Eng Stop flag，stop issused after send or receive.
                                         @else   缓存数据发送或者接收完成后是否要停止命令。  @endif */
    uint8_t restart_flag;           /*!< @if Eng Restart flag，restart issused before send or receive.
                                         @else   缓存数据发送或者接收是否要发重新开始命令。  @endif */
    uint8_t reserved[2];
} hal_i2c_buffer_wrap_t;

/**
 * @if Eng
 * @brief  Callback of I2C.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  evt Event ID. see @ref hal_i2c_evt_id_t
 * @param  [in]  param Parameter pointer of callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  I2C的回调函数
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  evt 事件ID。参考 @ref hal_i2c_evt_id_t
 * @param  [in]  param 传递给回调的参数指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_i2c_callback_t)(i2c_bus_t bus, hal_i2c_evt_id_t evt, uintptr_t param);

/**
 * @if Eng
 * @brief  Init device as master for hal I2C.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  baudrate Baudrate of the i2c controller.
 * @param  [in]  hscode High speed mode master code of the i2c controller, need be unique.
 * @param  [in]  callback Callback of the i2c controller. see @ref hal_i2c_callback_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C初始化为主机。
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  baudrate I2C控制器的波特率。
 * @param  [in]  hscode I2C控制器的主机码，用于高速仲裁，需要唯一。
 * @param  [in]  callback I2C控制器的中断回调。参考 @ref hal_i2c_callback_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_i2c_master_init(i2c_bus_t bus, uint32_t baudrate,
                              uint8_t hscode, hal_i2c_callback_t callback);

/**
 * @if Eng
 * @brief  Init device as slave for hal I2C.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  baudrate Baudrate of the i2c controller.
 * @param  [in]  addr I2C slave address of the i2c controller.
 * @param  [in]  callback Callback of the i2c controller. see @ref hal_i2c_callback_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C初始化为主机。
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  baudrate I2C控制器的波特率。
 * @param  [in]  addr I2C控制器的从机地址。
 * @param  [in]  callback I2C控制器的中断回调。参考 @ref hal_i2c_callback_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_i2c_slave_init(i2c_bus_t bus, uint32_t baudrate,
                             uint16_t addr, hal_i2c_callback_t callback);

/**
 * @if Eng
 * @brief  Deinit device for hal I2C.
 * @param  [in] bus The i2c bus. see @ref i2c_bus_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C去初始化接口
 * @param  [in] bus I2C索引。参考 @ref i2c_bus_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_i2c_deinit(i2c_bus_t bus);

/**
 * @if Eng
 * @brief  Control interface for hal I2C.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  id Control ID of the I2C controller. see @ref hal_i2c_ctrl_id_t
 * @param  [in]  param Parameter for callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C控制接口。
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  id I2C控制请求ID。参考 @ref hal_i2c_ctrl_id_t
 * @param  [in]  param 传递给控制回调的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_i2c_ctrl(i2c_bus_t bus, hal_i2c_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Write interface for hal I2C.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  data Send data buffer and send configuration parameters. see @ref hal_i2c_buffer_wrap_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C发送数据接口。
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  data 发送数据的缓冲地址及发送配置参数。参考 @ref hal_i2c_buffer_wrap_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_i2c_write(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data);

/**
 * @if Eng
 * @brief  Read interface for hal I2C.
 * @param  [in]  bus The device pointer to manager the hal I2C.
 * @param  [in]  data Read data buffer and read configuration parameters. see @ref hal_i2c_buffer_wrap_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C读取数据接口。
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  data 读取数据的缓冲地址及读取配置参数。参考 @ref hal_i2c_buffer_wrap_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_i2c_read(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
