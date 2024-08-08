/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides I2C driver api \n
 *
 * History: \n
 * 2022-06-01, Create file. \n
 */
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"
#include "hal_i2c.h"
#include "i2c_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_i2c I2C
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of I2C TX/RX data.
 * @else
 * @brief  I2C发送/接收数据结构定义。
 * @endif
 */
typedef struct i2c_data {
    uint8_t *send_buf;              /*!< @if Eng Send buffer pointer.
                                         @else   发送数据的buffer指针。  @endif */
    uint32_t send_len;              /*!< @if Eng Send buffer len.
                                         @else   发送数据的buffer长度。  @endif */
    uint8_t *receive_buf;           /*!< @if Eng Receive buffer pointer.
                                         @else   接收数据的buffer指针。  @endif */
    uint32_t receive_len;           /*!< @if Eng Receive buffer pointer.
                                         @else   接收数据的buffer长度。  @endif */
} i2c_data_t;

/**
 * @if Eng
 * @brief  Definition of I2C TX/RX wait condition.
 * @else
 * @brief  I2C发送/接收等待条件数据结构定义。
 * @endif
 */
typedef struct i2c_wait_condition {
    uint32_t evt_mask;              /*!< @if Eng Wait event condition, will trigger task schedule if blocked.
                                         @else   等待事件条件，如果不成立会引起任务调度，释放cpu占用。  @endif */
    uint32_t ctrl_mask;             /*!< @if Eng Wait control condition, will polling if blocked.
                                         @else   等待控制条件，如果不成立一直轮询，不释放cpu占用。  @endif */
} i2c_wait_condition_t;

#if defined(CONFIG_I2C_SUPPORT_MASTER) && (CONFIG_I2C_SUPPORT_MASTER == 1)
/**
 * @if Eng
 * @brief  Initialize the I2C as master according to the specified parameters.
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  baudrate The baudrate of I2C, high limit by ip:
 *                        - Standard speed mode baudrate high limit : 100KHz
 *                        - Fast speed mode baudrate high limit : 400KHz
 *                        - High speed mode baudrate high limit : 3.4MHz
 * @param  [in]  hscode The I2C high speed mode master code, each master has its unique master code, \n
 *                      valid values 0 ~ 7, only configured in high speed mode.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  根据指定的参数初始化该I2C为主机。
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  baudrate I2C波特率，不能超过IP的上限：
 *                        - 标准模式波特率上限：100KHz
 *                        - 快速模式波特率上限：400KHz
 *                        - 高速模式波特率上限：3.4MHz
 * @param  [in]  hscode I2C高速模式主机码, 每个主机有自己唯一的主机码，有效取值范围 0 ~ 7，仅在高速模式下需要配置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode);

/**
 * @if Eng
 * @brief  I2C Master writes a buffer to target slave. There are two ways to write data from master to slave,
 *         one is manual switching mode, the other is automatic switching mode, and the two methods are statically
 *         configured. The manual switching method has the following three transmission modes, but cannot be used in the
 *         same bus at the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in I2C KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  dev_addr The target slave address for master to write data.
 * @param  [in]  data The information pointer of send data, see @ref i2c_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  将数据从主机写入到从机。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考I2C KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  dev_addr 主机发送数据的目标从机地址。
 * @param  [in]  data 发送数据的信息指针，参考 @ref i2c_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_master_write(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data);

/**
 * @if Eng
 * @brief  I2C Master receives a buffer from target slave. There are two ways to read data from slave to master, one is
 *         manual switching mode, the other is automatic switching mode, and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in I2C KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  dev_addr The target slave address for master to receive data.
 * @param  [in]  data The information pointer of receive data, see @ref i2c_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  主机接收来自目标I2C从机的数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考I2C KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  dev_addr 主机接收数据的目标从机地址。
 * @param  [in]  data 接收数据的信息指针，参考 @ref i2c_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_master_read(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data);

/**
 * @if Eng
 * @brief  I2C Master writes a buffer to target slave and receives a buffer from it. There are two ways to write and
 *         read data, one is manual switching mode, the other is automatic switching mode, and the two methods are
 *         statically configured. The manual switching method has the following three transmission modes, but cannot be
 *         used in the same bus at the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in I2C KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t.
 * @param  [in]  dev_addr The target slave address for master to receive data.
 * @param  [in]  data The information pointer of receive data, see @ref i2c_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  主机发送数据到目标I2C从机，并接收来自此从机的数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考I2C KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  dev_addr 主机接收数据的目标从机地址。
 * @param  [in]  data 接收数据的信息指针，参考 @ref i2c_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_master_writeread(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data);
#endif  /* CONFIG_I2C_SUPPORT_MASTER */

#if defined(CONFIG_I2C_SUPPORT_SLAVE) && (CONFIG_I2C_SUPPORT_SLAVE == 1)
/**
 * @if Eng
 * @brief  Initialize the I2C as slave according to the specified parameters.
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  baudrate The baudrate of I2C, need to be consistent with the master.
 * @param  [in]  addr The slave address when the I2C is operating as a slave:
 *                    - 7-bits address range is [0x8, 0x77]
 *                    - 10-bits address range is [0x7800, 0x7BFF]
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  根据指定的参数初始化该I2C为从机。
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  baudrate I2C波特率，需要与主机保持一致：
 *                        - 标准模式波特率上限：100KHz
 *                        - 快速模式波特率上限：400KHz
 *                        - 高速模式波特率上限：3.4MHz
 * @param  [in]  addr I2C作为从机工作时的从机地址：
 *                    - 7比特地址范围 [0x8, 0x77]
 *                    - 10比特地址范围 [0x7800, 0x7BFF]
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */

errcode_t uapi_i2c_slave_init(i2c_bus_t bus, uint32_t baudrate, uint16_t addr);

/**
 * @if Eng
 * @brief  Slave writes a buffer to Master. There are two ways to write data from slave to master, one is manual
 *         switching mode, the other is automatic switching mode, and the two methods are statically configured. The
 *         manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in I2C KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  data The information pointer of send data, see @ref i2c_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机将数据发送给主机。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考I2C KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  data 发送数据的信息指针，参考 @ref i2c_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_slave_write(i2c_bus_t bus, i2c_data_t *data);

/**
 * @if Eng
 * @brief  Slave receives a buffer form Master. There are two ways to read data from master to slave, one is manual
 *         switching mode, the other is automatic switching mode, and the two methods are statically configured. The
 *         manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in I2C KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  data The information pointer of receive data, see @ref i2c_data_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机从主机读取数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考I2C KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @param  [in]  data 接收数据的信息指针，参考 @ref i2c_data_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_slave_read(i2c_bus_t bus, i2c_data_t *data);
#endif /* CONFIG_I2C_SUPPORT_SLAVE */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
/**
 * @if Eng
 * @brief  Set whether to use the interrupt mode to transfer data.
 * @param  [in]  bus The I2C bus. For details, see @ref i2c_bus_t.
 * @param  [in]  irq_en Whether to use the interrupt mode.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置是否使用中断模式传输数据。
 * @param  [in]  bus 指定的I2C接口，参考 @ref i2c_bus_t 。
 * @param  [in]  irq_en 是否使用中断模式。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_set_irq_mode(i2c_bus_t bus, bool irq_en);

/**
 * @if Eng
 * @brief  Definition of I2C irq event.
 * @else
 * @brief  I2C中断事件枚举。
 * @endif
 */
typedef enum {
    I2C_IRQ_EVT_RX_DONE,
    I2C_IRQ_EVT_TX_DONE,
    I2C_IRQ_EVT_I2C_BUSY,
    I2C_IRQ_EVT_I2C_ERR,
}i2c_irq_event_t;

/**
 * @if Eng
 * @brief  i2c irq event callback to be called when irq callback set
           @ref uapi_i2c_register_irq_callback is invoked.
 * @note   This callback is invoked in an interrupt context.
 * @param  bus The I2C bus. For details, see @ref i2c_bus_t.
 * @param  event Pointer to the event for i2c irq @ref i2c_irq_event_t.

 * @else
 * @brief  i2c中断事件的回调函数，通过 @ref uapi_i2c_register_irq_callback 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  bus 指定的I2C接口，参考 @ref i2c_bus_t 。
 * @param  event 指向i2c中断事件。

 * @endif
 */
typedef void (*i2c_irq_callback_t)(i2c_bus_t bus, uint8_t event);
/**
 * @if Eng
 * @brief  register i2c irq event callback.
 * @param  [in]  bus The I2C bus. For details, see @ref i2c_bus_t.
 * @param  [in]  callback  Event callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册I2C中断事件回调函数。
 * @param  [in]  bus 指定的I2C接口，参考 @ref i2c_bus_t 。
 * @param  [in]  callback 事件回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_register_irq_callback(i2c_bus_t bus, i2c_irq_callback_t callback);

/**
 * @if Eng
 * @brief  unregister i2c irq event callback.
 * @param  [in]  bus The I2C bus. For details, see @ref i2c_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  取消注册I2C中断事件回调函数。
 * @param  [in]  bus 指定的I2C接口，参考 @ref i2c_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_unregister_irq_callback(i2c_bus_t bus);

#endif  /* CONFIG_I2C_SUPPORT_INT */

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
/**
 * @if Eng
 * @brief  Enable/disable data transfer function in dma mode.
 * @param  [in]  bus The I2C bus. For details, see @ref i2c_bus_t.
 * @param  [in]  en Enable/disable data transfer.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能/去使能DMA模式下I2C传输。
 * @param  [in]  bus 指定的I2C接口。参考 @ref i2c_bus_t 。
 * @param  [in]  en 是否使能DMA传输。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_set_dma_mode(i2c_bus_t bus, bool en);
#endif  /* CONFIG_I2C_SUPPORT_DMA */

/**
 * @if Eng
 * @brief  Deinitialize the I2C peripheral, valid for both master and slave mode.
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化I2C，支持主从机。
 * @param  [in]  bus I2C总线，参考 @ref i2c_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_deinit(i2c_bus_t bus);

/**
 * @if Eng
 * @brief  Reset I2C baudrate for initialized I2C, valid for both master and slave mode.
 * @param  [in]  bus The I2C bus, see @ref i2c_bus_t.
 * @param  [in]  baudrate The baudrate of I2C, high limit by ip:
 *                        - Standard speed mode baudrate high limit : 100KHz
 *                        - Fast speed mode baudrate high limit : 400KHz
 *                        - High speed mode baudrate high limit : 3.4MHz
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  对已初始化的I2C重置波特率，支持主从机。
 * @param  [in]  bus I2C总线id，参考 @ref i2c_bus_t 。
 * @param  [in]  baudrate I2C波特率，不能超过IP的上限：
 *                        - 标准模式波特率上限：100KHz
 *                        - 快速模式波特率上限：400KHz
 *                        - 高速模式波特率上限：3.4MHz
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_set_baudrate(i2c_bus_t bus, uint32_t baudrate);

#if defined(CONFIG_I2C_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend all of the I2C channels.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起所有I2C通道。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume all of the I2C channels.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复所有I2C通道。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_i2c_resume(uintptr_t arg);
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