/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 HAL i2c \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#ifndef HAL_I2C_V150_MASTER_H
#define HAL_I2C_V150_MASTER_H

#include <stdint.h>
#include "hal_i2c.h"
#include "hal_i2c_v150_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v150_master I2C_V150 Master
 * @ingroup  drivers_hal_i2c
 * @{
 */

#define I2C_10BIT_ADDR_MASK             0x03FF
#define I2C_10BIT_ADDR_MASK_H           0x0300
#define I2C_10BIT_ADDR_MASK_H_OFFSET    7
#define I2C_10BIT_ADDR_MASK_H_TAG       0xF0
#define I2C_10BIT_ADDR_MASK_L           0x00FF
#define I2C_10BIT_ADDR_MASK_L_OFFSET    0

#define I2C_7BIT_ADDR_MASK 0x7F

#define I2C_ADDR_READ_TAG  0x01
#define I2C_ADDR_WRITE_TAG 0xFE

#define I2C_ADDR_TYPE_7BIT      0
#define I2C_ADDR_TYPE_10BIT     1
#define I2C_ADDR_TYPE_INVALID   2

/**
 * @if Eng
 * @brief  Initialize I2C bus as master.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  baudrate Baudrate to configure I2C SCL_CNT.
 * @param  [in]  hscode I2C high speed mode master code.
 * @param  [in]  callback I2C interrupt callback function. see @ref hal_i2c_callback_t
 * @else
 * @brief  初始化I2C作为主机
 * @param  [in]  bus I2C的索引。参考 @ref i2c_bus_t
 * @param  [in]  baudrate 配置I2C SCL_CNT所使用的波特率。
 * @param  [in]  hscode I2C高速模式下主机码, V150不支持。
 * @param  [in]  callback I2C中断回调函数。参考 @ref hal_i2c_callback_t
 * @endif
 */
errcode_t hal_i2c_v150_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode, hal_i2c_callback_t callback);

/**
 * @if Eng
 * @brief  Initialize I2C bus as slave.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  baudrate Baudrate to configure I2C SCL_CNT, slave is same as msater.
 * @param  [in]  addr I2C slave address.
 * @param  [in]  callback I2C interrupt callback function. see @ref hal_i2c_callback_t
 * @else
 * @brief  初始化I2C作为从机。
 * @param  [in]  bus I2C的索引。参考 @ref i2c_bus_t
 * @param  [in]  baudrate 配置I2C SCL_CNT所使用的波特率,从机与主机一致。
 * @param  [in]  addr I2C从机地址。
 * @param  [in]  callback I2C中断回调函数。参考 @ref hal_i2c_callback_t
 * @endif
 */
errcode_t hal_i2c_v150_slave_init(i2c_bus_t bus, uint32_t baudrate,
                                  uint16_t addr, hal_i2c_callback_t callback);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

