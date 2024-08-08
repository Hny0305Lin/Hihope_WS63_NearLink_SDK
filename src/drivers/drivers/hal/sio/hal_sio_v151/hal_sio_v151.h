/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 hal sio \n
 *
 * History: \n
 * 2023-03-07, Create file. \n
 */
#ifndef HAL_SIO_V151_H
#define HAL_SIO_V151_H

#include "hal_sio_v151_regs_op.h"
#include "hal_sio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_v151 SIO V151
 * @ingroup  drivers_hal_sio
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface between sio driver and sio hal.
 * @else
 * @brief  获取Driver层SIO和HAL层SIO的接口
 * @endif
 */
hal_sio_funcs_t *hal_sio_v151_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the sio interrupt request.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @else
 * @brief  SIO中断处理函数
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @endif
 */
void hal_sio_v151_irq_handler(sio_bus_t bus);

/**
 * @if Eng
 * @brief  sio tx rx disable.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @else
 * @brief  sio 接收发送去使能。
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @endif
 */
void hal_sio_v151_txrx_disable(sio_bus_t bus);

/**
 * @if Eng
 * @brief  sio tx rx enable.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @else
 * @brief  sio 接收发送使能。
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @endif
 */
void hal_sio_v151_txrx_enable(sio_bus_t bus);

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
void hal_sio_v151_crg_clock_enable(sio_bus_t bus, bool enable);

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
void hal_sio_v151_set_tx_enable(sio_bus_t bus, uint32_t val);

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
void hal_sio_v151_set_rx_enable(sio_bus_t bus, uint32_t val);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif