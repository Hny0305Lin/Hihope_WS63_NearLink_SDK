/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V100 HAL spi \n
 *
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_V151_H
#define HAL_SPI_V151_H

#include "hal_spi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_v151 SPI V100
 * @ingroup  drivers_hal_spi
 * @{
 */

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
errcode_t hal_spi_v151_init(spi_bus_t bus, const hal_spi_attr_t *attr,
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
errcode_t hal_spi_v151_deinit(spi_bus_t bus);

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
errcode_t hal_spi_v151_write(spi_bus_t bus, hal_spi_xfer_data_t *data, uint32_t timeout);

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
errcode_t hal_spi_v151_read(spi_bus_t bus, hal_spi_xfer_data_t *data, uint32_t timeout);

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
errcode_t hal_spi_v151_ctrl(spi_bus_t bus, hal_spi_ctrl_id_t id, uintptr_t param);

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
/**
 * @if Eng
 * @brief  Handler of the SPI interrupt request.
 * @param  [in]  bus The spi bus. see @ref spi_bus_t
 * @else
 * @brief  SPI中断处理函数
 * @param  [in]  bus 串口号， 参考 @ref spi_bus_t
 * @endif
 */
void hal_spi_v151_irq_handler(spi_bus_t bus);
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif