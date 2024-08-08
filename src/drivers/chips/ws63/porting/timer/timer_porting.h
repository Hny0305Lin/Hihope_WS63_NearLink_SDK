/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides timer port \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef TIMER_PORT_H
#define TIMER_PORT_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_timer Timer
 * @ingroup  drivers_port
 * @{
 */

#define MS_PER_S                    1000
#define US_PER_MS                   1000
#define TIMER_1_CYCLE               1
#define TIMER_2_CYCLE               2
#define HAL_TCXO_TICKS_PER_SECOND   24000000
#define TIMER_COMPENSAT_1_CYCLE_BY_TCXO       (HAL_TCXO_TICKS_PER_SECOND / CONFIG_TIMER_CLOCK_VALUE)
#define TIMER_COMPENSAT_2_CYCLE_BY_TCXO       (2 * HAL_TCXO_TICKS_PER_SECOND / CONFIG_TIMER_CLOCK_VALUE)
#define TIMER_COMPENSAT_3_CYCLE_BY_TCXO       (3 * HAL_TCXO_TICKS_PER_SECOND / CONFIG_TIMER_CLOCK_VALUE)

/**
 * @brief  Definiton of the index of timers.
 */
typedef enum timer_index {
    TIMER_INDEX_0,                      /*!< Timer0 index. */
    TIMER_INDEX_1,                      /*!< Timer1 index. */
    TIMER_INDEX_2,                      /*!< Timer2 index. */
    TIMER_MAX_NUM
} timer_index_t;

/**
 * @if Eng
 * @brief  Get the address of the timers common regs.
 * @return The address of the timers common regs.
 * @else
 * @brief  获取硬件定时器公共寄存器的基地址。
 * @return 硬件定时器公共寄存器的基地址。
 * @endif
 */
uintptr_t timer_porting_comm_addr_get(void);

/**
 * @if Eng
 * @brief  Get the base address of a specified hardware timer.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @return The base address of specified hardware timer.
 * @else
 * @brief  获取指定的硬件定时器的基地址。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @return 指定的硬件定时器的基地址。
 * @endif
 */
uintptr_t timer_porting_base_addr_get(timer_index_t index);

/**
 * @if Eng
 * @brief  Register irq for timer.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  id Timer interruot id.
 * @param  [in]  priority Timer interrupt priority.
 * @else
 * @brief  Timer注册中断。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  id 定时器中断ID。
 * @param  [in]  priority 定时器中断优先级。
 * @endif
 */
void timer_port_register_irq(timer_index_t index, uint32_t id, uint16_t priority);

/**
 * @if Eng
 * @brief  Unregister irq for timer.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  id Timer interruot id.
 * @else
 * @brief  Timer去注册中断。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  id 定时器中断ID。
 * @endif
 */
void timer_port_unregister_irq(timer_index_t index, uint32_t id);

/**
 * @if Eng
 * @brief update the timer's clock freq value for caculating, not really change the hardware clock freq.
 * @param  [in]  clock clock freq value
 * @retval void
 * @else
 * @brief  更新timer的时钟频率值,此值应和真实输入时钟频率保持一致
 * @param  [in]  clock 时钟频率
 * @retval void
 * @endif
 */
void timer_porting_clock_value_set(uint32_t clock);

/**
 * @if Eng
 * @brief  get the timer clock value.
 * @retval uint32_t  the value set by @ref timer_porting_clock_value_set
 * @else
 * @brief  获取timer时钟频率
 * @retval uint32_t  设置的时钟频率 @ref timer_porting_clock_value_set
 * @endif
 */
uint32_t timer_porting_clock_value_get(void);

/**
 * @if Eng
 * @brief  convert us to cycle.
 * @param  [in]  us millisecond
 * @retval uint64_t  Time for load count.
 * @else
 * @brief  毫秒转换为计数
 * @param  [in]  us 时间单位毫秒
 * @retval uint64_t  计数。
 * @endif
 */
uint64_t timer_porting_us_2_cycle(uint32_t us);

/**
 * @if Eng
 * @brief  convert cycle to us.
 * @param  [in]  cycle Time for load count
 * @retval uint32_t  millisecond.
 * @else
 * @brief  毫秒转换为计数
 * @param  [in]  cycle 计数
 * @retval uint32_t  毫秒。
 * @endif
 */
uint32_t timer_porting_cycle_2_us(uint64_t cycle);

uint64_t timer_porting_compensat_by_tcxo(uint64_t diff);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif