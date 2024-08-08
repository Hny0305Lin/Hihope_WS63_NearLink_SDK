/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 hal uart \n
 *
 * History: \n
 * 2023-02-17, Create file. \n
 */
#ifndef HAL_UART_V151_H
#define HAL_UART_V151_H

#include "hal_uart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v151 UART V151
 * @ingroup  drivers_hal_uart
 * @{
 */

hal_uart_funcs_t *hal_uart_v151_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the uart interrupt request.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @else
 * @brief  UART中断处理函数
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @endif
 */
void hal_uart_irq_handler(uart_bus_t bus);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif