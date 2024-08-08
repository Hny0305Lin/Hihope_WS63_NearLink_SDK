/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provide SPI driver API \n
 *
 * History: \n
 * 2022-09-15, Create file. \n
 */
#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "errcode.h"
#include "hal_spi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_spi SPI
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of SPI attributes.
 * @else
 * @brief  SPI基本属性定义。
 * @endif
 */
typedef hal_spi_attr_t spi_attr_t;

/**
 * @if Eng
 * @brief  Definition of SPI extral attributes.
 * @else
 * @brief  SPI扩展属性定义。
 * @endif
 */
typedef hal_spi_extra_attr_t spi_extra_attr_t;

/**
 * @if Eng
 * @brief  SPI transfer data structure.
 * @else
 * @brief  SPI传输结构体。
 * @endif
 */
typedef hal_spi_xfer_data_t spi_xfer_data_t;

/**
 * @if Eng
 * @brief  Definition of SPI DMA configuration.
 * @else
 * @brief  SPI DMA配置数据结构定义。
 * @endif
 */
typedef struct spi_dma_config {
    uint8_t src_width;          /*!< @if Eng Transfer data width of the source.
                                 *           - 0: 1byte
                                 *           - 1: 2byte
                                 *           - 2: 4byte
                                 *   @else   源端传输数据宽度。 \n
                                 *           - 0: 1字节
                                 *           - 1: 2字节
                                 *           - 2: 4字节
                                 *   @endif */
    uint8_t dest_width;         /*!< @if Eng Transfer data width of the destination.
                                 *            - 0: 1byte
                                 *            - 1: 2byte
                                 *            - 2: 4byte
                                 *   @else   目的端传输数据宽度。 \n
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
                                 *   @else   每次从相应的硬件或软件握手接口发出目的burst请求时,要写入目的端数据量。
                                 *           - 0: burst长度是1
                                 *           - 1: burst长度是4
                                 *           - 2: burst长度是8
                                 *           - 3: burst长度是16
                                 *   @endif */
    uint8_t priority;           /*!< @if Eng Transfer channel priority(Minimum: 0 and Maximum: 3).
                                 *   @else   传输通道优先级(最小为0以及最大为3)。  @endif */
} spi_dma_config_t;

/**
 * @if Eng
 * @brief  Initialize the SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  attr Basic configuration of SPI. For details, see @ref spi_attr_t.
 * @param  [in]  extra_attr High-level configuration of SPI. For details, see @ref spi_extra_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化 SPI。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  attr SPI的基础配置参数，参考 @ref spi_attr_t 。
 * @param  [in]  extra_attr SPI的高级配置参数，参考 @ref spi_extra_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_init(spi_bus_t bus, spi_attr_t *attr, spi_extra_attr_t *extra_attr);

/**
 * @if Eng
 * @brief  Deinitialize the SPI.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, For details, see @ref errcode_t.
 * @else
 * @brief  去初始化 SPI。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_deinit(spi_bus_t bus);

/**
 * @if Eng
 * @brief  Set SPI Transmit mode.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  tmod The SPI Transfer Mode. For details, see @ref hal_spi_trans_mode_t.
 * @param  [in]  data_frame_num The Number of the SPI Rx Data frame.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, For details, see @ref errcode_t.
 * @else
 * @brief  设置SPI的传输模式。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  tmod SPI传输模式，参考 @ref hal_spi_trans_mode_t 。
 * @param  [in]  data_frame_num SPI RX数据帧的数量。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_tmod(spi_bus_t bus, hal_spi_trans_mode_t tmod, uint8_t data_frame_num);

/**
 * @if Eng
 * @brief  Set SPI basic configuration.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  attr Basic configuration of SPI. For details, see @ref spi_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置SPI的基础配置参数。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  attr SPI的基础配置参数，参考 @ref spi_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_attr(spi_bus_t bus, spi_attr_t *attr);

/**
 * @if Eng
 * @brief  Get SPI basic configuration.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [out]  attr Basic configuration of SPI. For details, see @ref spi_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, For details, see @ref errcode_t.
 * @else
 * @brief  读取SPI的基础配置参数。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [out] attr SPI的基础配置参数，参考 @ref spi_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_get_attr(spi_bus_t bus, spi_attr_t *attr);

/**
 * @if Eng
 * @brief  Set SPI extral configuration.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  extra_attr Extral configuration of SPI. For details, see @ref spi_extra_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置SPI的高级配置参数。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  extra_attr SPI的高级配置参数，参考 @ref spi_extra_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_extra_attr(spi_bus_t bus, spi_extra_attr_t *extra_attr);

/**
 * @if Eng
 * @brief  Get SPI extral configuration.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [out]  extra_attr Extral configuration of SPI. For details, see @ref spi_extra_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  读取SPI的高级配置参数。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [out] extra_attr SPI的高级配置参数，参考 @ref spi_extra_attr_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_get_extra_attr(spi_bus_t bus, spi_extra_attr_t *extra_attr);

#if defined(CONFIG_SPI_SUPPORT_MASTER) && (CONFIG_SPI_SUPPORT_MASTER == 1)
/**
 * @if Eng
 * @brief  Select slave device in master mode.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  cs The slave need to select.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  Master模式下选择需要对通的Slave设备。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  cs 被选中的Slave设备。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_select_slave(spi_bus_t bus, spi_slave_t cs);

/**
 * @if Eng
 * @brief  Write data from host to slave. There are two ways to write data from master to slave,
 *         one is manual switching mode, the other is automatic switching mode,
 *         and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in SPI KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  data Pointer to transfer data. @ref spi_xfer_data_t.
 * @param  [in]  timeout Timeout of the current transfer(In Polling mode it is polling count. In DMA mode it is
 *         timeout,unit ms. In Interrupt mode Param does not take effect).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  将数据从主机写入到从机。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考SPI KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  data 数据传输指针，参考 @ref spi_xfer_data_t 。
 * @param  [in]  timeout 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_master_write(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);

/**
 * @if Eng
 * @brief  Read data from slave. There are two ways to read data from slave to master,
 *         one is manual switching mode, the other is automatic switching mode,
 *         and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in SPI KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [out] data Pointer to transfer data. @ref spi_xfer_data_t.
 * @param  [in]  timeout Timeout of the current transfer(In Polling mode it is polling count. In DMA mode it is
 *         timeout,unit ms. In Interrupt mode Param does not take effect).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从slave读取数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考SPI KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [out] data 数据传输指针，参考 @ref spi_xfer_data_t 。
 * @param  [in]  timeout 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_master_read(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);

/**
 * @if Eng
 * @brief  Write and read data in host. There are two ways to write and read data,
 *         one is manual switching mode, the other is automatic switching mode,
 *         and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in SPI KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in, out]  data Pointer to transfer data. @ref spi_xfer_data_t.
 * @param  [in]  timeout Timeout of the current transfer(In Polling mode it is polling count. In DMA mode it is
 *         timeout,unit ms. In Interrupt mode Param does not take effect).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  主机写入和读取数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下两种种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考SPI KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in, out]  data 数据传输指针，参考 @ref spi_xfer_data_t 。
 * @param  [in]  timeout 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_master_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);
#endif  /* CONFIG_SPI_SUPPORT_MASTER */

#if defined(CONFIG_SPI_SUPPORT_SLAVE) && (CONFIG_SPI_SUPPORT_SLAVE == 1)
/**
 * @if Eng
 * @brief  Write data from slave to host. There are two ways to write data from slave to master,
 *         one is manual switching mode, the other is automatic switching mode,
 *         and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in SPI KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode the threshold
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  data Pointer to transfer data. @ref spi_xfer_data_t.
 * @param  [in]  timeout Timeout of the current transfer(In Polling mode it is polling count. In DMA mode it is
 *         timeout,unit ms. In Interrupt mode Param does not take effect).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机向主机写入数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考SPI KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  data 数据传输指针，参考 @ref spi_xfer_data_t 。
 * @param  [in]  timeout 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_slave_write(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);

/**
 * @if Eng
 * @brief  Read data from host. There are two ways to read data from master to slave,
 *         one is manual switching mode, the other is automatic switching mode,
 *         and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in SPI KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [out] data Pointer to transfer data. @ref spi_xfer_data_t.
 * @param  [in]  timeout Timeout of the current transfer(In Polling mode it is polling count. In DMA mode it is
 *         timeout,unit ms. In Interrupt mode Param does not take effect).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从主机读取数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下三种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考SPI KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [out]  data 数据传输指针，参考 @ref spi_xfer_data_t 。
 * @param  [in]  timeout 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_slave_read(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);

/**
 * @if Eng
 * @brief  Write and read data from host. There are two ways to write and read data,
 *         one is manual switching mode, the other is automatic switching mode,
 *         and the two methods are statically configured.
 *         The manual switching method has the following three transmission modes, but cannot be used in the same bus at
 *         the same time.
 *         - Manually switching modes includes:
 *           - Polling mode
 *           - DMA mode
 *           - Interrupt mode
 *         - The automatic switching mode (automatically switching between the polling mode and the DMA mode by
 *           comparing the data length and the threshold value. For the threshold value, please refer to POLL and DMA
 *           automatic switching threshold setting in SPI KCONFIG) includes:
 *           - Polling mode: When the length of transmitted data is less than or equal to the threshold, it defaults
 *                           to polling mode
 *           - DMA mode: When the length of the transmitted data exceeds the threshold, it automatically switches to
 *                       DMA mode
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in, out]  data Pointer to transfer data. @ref spi_xfer_data_t.
 * @param  [in]  timeout Timeout of the current transfer(In Polling mode it is polling count. In DMA mode it is
 *         timeout,unit ms. In Interrupt mode Param does not take effect).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从机写入和读取数据。有两种方式,一种是手动切换模式，另外一种是自动切换模式，两种方式是静态配置的。
 *         手动切换方式一共有以下两种种传输模式，但是不能在同一bus中同时使用。
 *         - 手动切换模式包括：
 *           - 轮询模式
 *           - DMA模式
 *           - 中断模式
 *         - 自动切换模式（通过比较数据长度和阈值的大小自动切换轮询模式与DMA模式, 阈值具体请参考SPI KCONFIG中POLL和DMA自动切换阈值设置）包括：
 *           - 轮询模式：当传输数据长度小于等于阈值时，默认为轮询模式
 *           - DMA模式：当传输数据长度大于阈值时，自动切换为DMA模式
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in, out]  data 数据传输指针，参考 @ref spi_xfer_data_t 。
 * @param  [in]  timeout 当前传输的超时时间（轮询模式下为轮询次数；DMA模式为超时时间，单位：ms；中断模式参数不生效）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_slave_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */

#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
/**
 * @if Eng
 * @brief  Enable/disable data transfer function in DMA mode.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  en Enable/disable data transfer
 * @param  [in]  dma_cfg Pointer to the configuration structure of DMA. @ref spi_dma_config_t.
 *                       If want to set the mode to disable, configuration this parameter to NULL.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能/去使能DMA模式下SPI传输。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  en 是否使能DMA传输。
 * @param  [in]  dma_cfg DMA配置结构体指针，参考 @ref spi_dma_config_t 。如果模式为去使能，这里配置为NULL。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_dma_mode(spi_bus_t bus, bool en, const spi_dma_config_t *dma_cfg);
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
/**
 * @if Eng
 * @brief  SPI Transmission Callback function to be called when the RX condition registered on
           @ref uapi_spi_set_irq_mode is invoked.
 * @note   This callback is invoked in an interrupt context.
 * @param  buffer Pointer to the buffer to stored the data to be write. The value is given from data write interface.
 * @param  length Length of the data that has been write. The value is given from data write interface.
 * @else
 * @brief  SPI发送数据的回调函数，通过 @ref uapi_spi_set_irq_mode 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  buffer 指向缓冲区的指针，用于存储写入的数据。由写接口函数的参数传入。
 * @param  length 写入的数据长度。由写接口函数的参数传入。
 * @endif
 */
typedef void (*spi_tx_callback_t)(const void *buffer, uint32_t length);

/**
 * @if Eng
 * @brief  SPI Transmission Callback function to be called when the RX condition registered on
           @ref uapi_spi_set_irq_mode is invoked or the error is happened.
 * @note   This callback is invoked in an interrupt context.
 * @param  buffer Pointer to the buffer to stored the data to be read. The value is given from data read interface.
 * @param  length Length of the data that has been read. The value is given from data read interface.
 * @param  error Indicates if there is error on the SPI lines or not. Error lists:
 *               - RX overflow error
 *               - RX underflow error
 * @else
 * @brief  SPI接收数据的回调函数，通过 @ref uapi_spi_set_irq_mode 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  buffer 指向缓冲区的指针，用于存储要读取的数据。由读取接口函数的参数传入。
 * @param  length 读取到的数据长度。由读取接口函数的参数传入。
 * @param  error 表示SPI传输是否存在错误，错误情况：
 *               - RX上限溢出错误
 *               - RX下限溢出错误
 * @endif
 */
typedef void (*spi_rx_callback_t)(const void *buffer, uint32_t length, bool error);

/**
 * @if Eng
 * @brief  Set whether to use the interrupt mode to transfer data in master mode.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t
 * @param  [in]  irq_en Whether to use the interrupt mode.
 * @param  [in]  rx_callback Receive callback to called when data read is completed.
 * @param  [in]  tx_callback Transmit callback to called when data write is completed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置是否使用中断模式在主机模式下传输数据。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  irq_en 是否使用中断模式。
 * @param  [in]  rx_callback 接收数据完成时的回调函数。
 * @param  [in]  tx_callback 数据发送完成时的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_irq_mode(spi_bus_t bus, bool irq_en, spi_rx_callback_t rx_callback,
                                spi_tx_callback_t tx_callback);
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

#if defined(CONFIG_SPI_SUPPORT_LOOPBACK) && (CONFIG_SPI_SUPPORT_LOOPBACK == 1)
/**
 * @if Eng
 * @brief  Whether to set to loopback test mode.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  loopback_en Loopback mode enable/disable.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  是否设置为环回测试模式。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  loopback_en 环回模式使能/去使能。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_loop_back_mode(spi_bus_t bus, bool loopback_en);
#endif  /* CONFIG_SPI_SUPPORT_LOOPBACK */

#if defined(CONFIG_SPI_SUPPORT_CRC) && (CONFIG_SPI_SUPPORT_CRC == 1)
/**
 * @if Eng
 * @brief SPI CRC configuration parameters.
 * @else
 * @brief SPI CRC 配置参数。
 * @endif
 */
typedef struct spi_crc_config {
    uint32_t  tx_crc_len;       /*!< @if Eng Send a crc virified data before this length.
                                     @else   逻辑在此长度之前发送CRC验证数据。@endif */
    uint32_t  rx_crc_len;       /*!< @if Eng Logic receive a crc virified data before this length.
                                     @else   逻辑在此长度之前接收CRC验证数据。@endif */
    uint32_t  tx_crc_ini;       /*!< @if Eng Tx init val configuration.
                                     @else   tx初始化值配置。@endif */
    uint32_t  rx_crc_ini;       /*!< @if Eng Rx init val configuration.
                                     @else   rx初始化值配置。@endif */
    uint32_t  tx_crc_poly;      /*!< @if Eng Tx polynomial configuration.
                                     @else   tx多项式配置。@endif */
    uint32_t  rx_crc_poly;      /*!< @if Eng Rx polynomial configuration.
                                     @else   tx多项式配置。@endif */
    uint32_t  tx_crc_xor_out;   /*!< @if Eng Tx result XOR configuration.
                                     @else   tx结果异或配置。@endif */
    uint32_t  rx_crc_xor_out;   /*!< @if Eng Rx result XOR configuration.
                                     @else   rx结果异或配置。@endif */
    bool      tx_crc_refin;     /*!< @if Eng tx Input value inversion configuration.
                                     @else   tx输入值翻转配置。@endif */
    bool      tx_crc_refout;    /*!< @if Eng tx Output value inversion configuration.
                                     @else   tx输出值翻转配置。@endif */
    bool      rx_crc_refin;     /*!< @if Eng rx Input value inversion configuration.
                                     @else   rx输入值翻转配置。@endif */
    bool      rx_crc_refout;    /*!< @if Eng rx Output value inversion configuration.
                                     @else   rx输出值翻转配置。@endif */
} spi_crc_config_t;

/**
 * @if Eng
 * @brief  SPI CRC error callback.
 * @else
 * @brief  SPI CRC错误处理回调函数。
 * @endif
 */
typedef void (*spi_crc_err_callback_t)(spi_bus_t bus);

/**
 * @if Eng
 * @brief  Set SPI tx and rx crc mode.
 * @param  [in]  bus The SPI bus. For details, see @ref spi_bus_t.
 * @param  [in]  crc_config CRC configuration parameters.
 * @param  [in]  cb Ccallback function for CRC check errors.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置SPI发送和接收CRC模式。
 * @param  [in]  bus 指定的SPI接口，参考 @ref spi_bus_t 。
 * @param  [in]  crc_config 配置SPI的crc参数。
 * @param  [in]  cb crc检验错误回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_set_crc_mode(spi_bus_t bus, const spi_crc_config_t *crc_config, spi_crc_err_callback_t cb);
#endif  /* CONFIG_SPI_SUPPORT_CRC */

#if defined(CONFIG_SPI_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend all of the SPI channels.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起所有SPI通道。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume all of the SPI channels.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复所有SPI通道。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_spi_resume(uintptr_t arg);
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