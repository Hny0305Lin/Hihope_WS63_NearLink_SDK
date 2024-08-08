/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 HAL i2c \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#ifndef HAL_I2C_V150_COMM_H
#define HAL_I2C_V150_COMM_H

#include <stdint.h>
#include "common_def.h"
#include "soc_osal.h"
#include "hal_i2c.h"
#include "hal_i2c_v150_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v150_comm I2C_V150 Common
 * @ingroup  drivers_hal_i2c
 * @{
 */

#define I2C_TRUE 1
#define I2C_FALSE 0

#define I2C_V150_DEFAULT_WRITE_NUM 0xFFFFFFFF
#define I2C_V150_DEFAULT_READ_NUM 0xFFFFFFFF

#define I2C_CFG_SCL_H_DEFAULT_VAL 1
#define I2C_CFG_SCL_L_DEFAULT_VAL 1

/**
 * @if Eng
 * @brief  Definition of I2C trans mode.
 * @else
 * @brief  I2C读写模式定义
 * @endif
 */
typedef enum hal_i2c_trans_mode {
    I2C_TRANS_MODE_INVALID = 0,
    I2C_TRANS_MODE_WRITE = 1,
    I2C_TRANS_MODE_READ = 2,
    I2C_TRANS_MODE_WRITE_BEFORE_READ = 3,
    I2C_TRANS_MODE_STOP_AFTER_WRITE = 4,
    I2C_TRANS_TYPE_MAX_NUM
} hal_i2c_trans_mode_t;

/**
 * @if Eng
 * @brief  Definition of I2C work type.
 * @else
 * @brief  I2C工作模式定义
 * @endif
 */
typedef enum hal_i2c_work_type {
    I2C_WORK_TYPE_POLL_NOFIFO = 0,
    I2C_WORK_TYPE_POLL_FIFO = 1,
    I2C_WORK_TYPE_INT_NOFIFO = 2,
    I2C_WORK_TYPE_INT_FIFO = 3,
    I2C_WORK_TYPE_MAX_NUM
} hal_i2c_work_type_t;

/**
 * @if Eng
 * @brief  Definition of I2C addr width.
 * @else
 * @brief  I2C地址位宽定义
 * @endif
 */
typedef enum hal_i2c_addr_width {
    I2C_7_BITS_ADDR,                /*!< @if Eng I2C start transfers in 7-bit addressing mode.
                                         @else   I2C以7位寻址模式开始传输。 @endif */
    I2C_10_BITS_ADDR,               /*!< @if Eng I2C start transfers in 10-bit addressing mode.
                                         @else   I2C以10位寻址模式开始传输。 @endif */
    I2C_INVALID_BITS_ADDR
} hal_i2c_addr_width_t;

/**
 * @if Eng
 * @brief  Definition of hal I2C transport information.
 * @else
 * @brief  HAL层I2C的传输信息数据结构定义
 * @endif
 */
typedef struct hal_i2c_trans_info {
    uint8_t trans_mode;
    uint32_t trans_cnt;
    uint32_t total_len;
} hal_i2c_trans_info_t;

/**
 * @if Eng
 * @brief  Definition of hal I2C ctrl information.
 * @else
 * @brief  HAL层I2C的控制信息数据结构定义
 * @endif
 */
typedef struct hal_i2c_ctrl_info {
    uint8_t init;
    uint8_t cfg_scl_h;
    uint8_t cfg_scl_l;
    uint8_t work_type;
    uint32_t timeout_us;
    uint32_t baudrate;
    uint8_t *ext;
} hal_i2c_ctrl_info_t;

/**
 * @if Eng
 * @brief  Control interface for hal I2C.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @param  [in]  param Parameter pointer for callback.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层I2C控制接口。
 * @param  [in]  bus I2C索引。参考 @ref i2c_bus_t
 * @param  [in]  param 传递给控制回调的参数指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_i2c_inner_ctrl_t)(i2c_bus_t bus, uintptr_t param);

hal_i2c_ctrl_info_t *hal_i2c_v150_get_ctrl_info(i2c_bus_t bus);
void hal_i2c_v150_load_ctrl_func(i2c_bus_t bus, hal_i2c_inner_ctrl_t *func_table);
void hal_i2c_v150_unload_ctrl_func(i2c_bus_t bus);
void hal_i2c_v150_register_callback(hal_i2c_callback_t callback);
hal_i2c_trans_info_t *hal_i2c_v150_get_trans_info(i2c_bus_t bus);
errcode_t hal_i2c_v150_ctrl_check_default(i2c_bus_t bus, uintptr_t param);
errcode_t hal_i2c_v150_ctrl_check_default_false(i2c_bus_t bus, uintptr_t param);
errcode_t hal_i2c_v150_ctrl_proc_default(i2c_bus_t bus, uintptr_t param);
errcode_t hal_i2c_v150_get_write_num(i2c_bus_t bus, uintptr_t param);
errcode_t hal_i2c_v150_get_read_num(i2c_bus_t bus, uintptr_t param);
errcode_t hal_i2c_v150_send_byte(i2c_bus_t bus, uint8_t data, uint8_t should_start);
errcode_t hal_i2c_v150_receive_byte(i2c_bus_t bus, uint8_t *data, uint32_t remain_len);
errcode_t hal_i2c_v150_write(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data);
errcode_t hal_i2c_v150_read(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data);
errcode_t hal_i2c_v150_deinit(i2c_bus_t bus);
void hal_i2c_v150_cfg_clk(i2c_bus_t bus, uint32_t baudrate, uint8_t scl_h, uint8_t scl_l);
void hal_i2c_v150_init_comp_param(i2c_bus_t bus);
errcode_t hal_i2c_v150_ctrl(i2c_bus_t bus, hal_i2c_ctrl_id_t id, uintptr_t param);
void hal_i2c_v150_irq_handler(i2c_bus_t bus);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

