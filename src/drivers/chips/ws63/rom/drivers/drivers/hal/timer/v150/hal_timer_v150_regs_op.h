/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 timer register operation API \n
 *
 * History: \n
 * 2022-12-07， Create file. \n
 */
#ifndef HAL_TIMER_V150_REGS_OP_H
#define HAL_TIMER_V150_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "timer_porting.h"
#include "hal_timer_v150_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_timer_v150_regs_op TIMER V150 Regs Operation
 * @ingroup  drivers_hal_timer
 * @{
 */

extern uintptr_t g_timer_comm_regs;
extern uintptr_t g_timer_regs[CONFIG_TIMER_MAX_NUM];

/**
 * @brief  Reg addr associated with timer.
 */
#define timer_regs(index) ((timer_v150_regs_t *)(g_timer_regs[index]))

/**
 * @brief  Set the value of @ref timer_v150_control_reg_data.enable.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  val Enable or not.
 */
static inline void hal_timer_v150_control_reg_set_enable(timer_index_t index, uint32_t val)
{
    timer_v150_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = timer_regs(index)->control;
    timer_control_reg.b.enable = val;
    timer_regs(index)->control = timer_control_reg.d32;
}

/**
 * @brief  Set the value of @ref timer_v150_control_reg_data.mode.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  val The work mode of timer.
 */
static inline void hal_timer_v150_control_reg_set_mode(timer_index_t index, uint32_t val)
{
    timer_v150_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = timer_regs(index)->control;
    timer_control_reg.b.mode = val;
    timer_regs(index)->control = timer_control_reg.d32;
}

/**
 * @brief  Get the value of @ref timer_v150_regs.current_value0.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @return The value of @ref timer_v150_regs.current_value0.
 */
static inline uint32_t hal_timer_v150_get_current_value0(timer_index_t index)
{
    return timer_regs(index)->current_value0;
}

/**
 * @brief  Get the value of @ref timer_v150_regs.current_value1.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @return The value of @ref timer_v150_regs.current_value1.
 */
static inline uint32_t hal_timer_v150_get_current_value1(timer_index_t index)
{
    return timer_regs(index)->current_value1;
}

/**
 * @brief  Set the value of @ref timer_v150_regs.load_count0.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  val The load count value.
 */
static inline void hal_timer_v150_set_load_count0(timer_index_t index, uint32_t val)
{
    timer_regs(index)->load_count0 = val;
}

/**
 * @brief  Set the value of @ref timer_v150_regs.load_count1.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  val The load count value.
 */
static inline void hal_timer_v150_set_load_count1(timer_index_t index, uint32_t val)
{
    timer_regs(index)->load_count1 = val;
}

/**
 * @brief  Clear timer interrupt.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 */
static inline void hal_timer_v150_int_clr(timer_index_t index)
{
    timer_regs(index)->eoi_ren = 0x1;
}

/**
 * @brief  Set the value of @ref timers_v150_regs.lock.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  val The lock password.
 */
static inline void hal_timer_v150_set_load_lock(timer_index_t index, uint32_t val)
{
    unused(index);
    ((timers_v150_regs_t *)g_timer_comm_regs)->lock = val;
}

/**
 * @brief  Set the value of @ref timer_v150_control_reg_data_t.cnt_req.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 */
static inline void hal_timer_v150_ctrl_set_cnt_req(timer_index_t index)
{
    timer_v150_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = timer_regs(index)->control;
    timer_control_reg.b.cnt_req = 1;
    timer_regs(index)->control = timer_control_reg.d32;
}

/**
 * @brief  Get the value of @ref timer_v150_control_reg_data_t.cnt_lock.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @return The value of @ref timer_v150_control_reg_data_t.cnt_lock.
 */
static inline uint32_t hal_timer_v150_ctrl_get_cnt_lock(timer_index_t index)
{
    timer_v150_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = timer_regs(index)->control;
    return timer_control_reg.b.cnt_lock;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif