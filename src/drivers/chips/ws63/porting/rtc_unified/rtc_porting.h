/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides rtc port \n
 *
 * History: \n
 * 2023-03-02, Create file. \n
 */
#ifndef RTC_PORTING_H
#define RTC_PORTING_H

#include "tcxo_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_rtc RTC
 * @ingroup  drivers_port
 * @{
 */

#define RTC_CLOCK_MAX_NUM    4
#define MS_PER_S                    1000
#define US_PER_MS                   1000
#define MAX_UINT32                  0xFFFFFFFF
#define MAX_UINT64                  0xFFFFFFFFFFFFFFFFULL
#define RTC_MAX_MS (MAX_UINT32 / CONFIG_RTC_CLOCK_VALUE * MS_PER_S - 1)
#define RTC_HW_MAX_MS (MAX_UINT32 / CONFIG_RTC_CLOCK_VALUE * MS_PER_S - 1)
#define RTC_1_CYCLE                 1
#define RTC_2_CYCLE                 2
#define RTC_COMPENSAT_1_CYCLE_BY_TCXO       (HAL_TCXO_TICKS_PER_SECOND / CONFIG_RTC_CLOCK_VALUE)
#define RTC_COMPENSAT_2_CYCLE_BY_TCXO       (2 * HAL_TCXO_TICKS_PER_SECOND / CONFIG_RTC_CLOCK_VALUE)
#define RTC_COMPENSAT_3_CYCLE_BY_TCXO       (3 * HAL_TCXO_TICKS_PER_SECOND / CONFIG_RTC_CLOCK_VALUE)
/**
 * @if Eng
 * @brief  Definiton of the index of rtcs.
 * @else
 * @brief  rtc索引的定义.
 * @endif
 */
typedef enum {
    RTC_0,                            /*!< RTC0 index. */
    RTC_1,                            /*!< RTC1 index. */
    RTC_2,                            /*!< RTC2 index. */
    RTC_3,                            /*!< RTC3 index. */
    RTC_MAX_NUM = RTC_CLOCK_MAX_NUM
} rtc_index_t;

/**
 * @if Eng
 * @brief  Get the address of the rtc common regs.
 * @return The address of the rtc common regs.
 * @else
 * @brief  获取硬件定时器公共寄存器的基地址。
 * @return 硬件定时器公共寄存器的基地址。
 * @endif
 */
uintptr_t rtc_porting_comm_addr_get(void);

/**
 * @if Eng
 * @brief  Get the base address of a specified rtc.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @return The base address of specified rtc.
 * @else
 * @brief  获取指定rtc的基地址。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @return 指定rtc的基地址。
 * @endif
 */
uintptr_t rtc_porting_base_addr_get(rtc_index_t index);

/**
 * @if Eng
 * @brief  Register irq for rtc.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @param  [in]  id rtc interrupt id.
 * @param  [in]  priority rtc interrupt priority.
 * @else
 * @brief  rtc注册中断。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @param  [in]  id 定时器中断ID。
 * @param  [in]  priority 定时器中断优先级。
 * @endif
 */
void rtc_port_register_irq(rtc_index_t index, uint32_t id, uint16_t priority);

/**
 * @if Eng
 * @brief  Unregister irq for rtc.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @param  [in]  id rtc interrupt id.
 * @else
 * @brief  rtc去注册中断。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @param  [in]  id 定时器中断ID。
 * @endif
 */
void rtc_port_unregister_irq(rtc_index_t index, uint32_t id);

/**
 * @if Eng
 * @brief update the RTC's clock freq value for caculating, not really change the hardware clock freq.
 * @param  [in]  clock clock freq value
 * @retval void
 * @else
 * @brief  更新RTC的时钟频率值,此值应和真实输入时钟频率保持一致
 * @param  [in]  clock 时钟频率
 * @retval void
 * @endif
 */
void rtc_porting_clock_value_update(uint32_t clock);

/**
 * @if Eng
 * @brief  get the RTC clock value.
 * @retval uint32_t  the value set by @ref rtc_porting_clock_value_update
 * @else
 * @brief  获取RTC时钟频率
 * @retval uint32_t  设置的时钟频率 @ref rtc_porting_clock_value_update
 * @endif
 */
uint32_t rtc_porting_clock_value_get(void);

uint64_t rtc_porting_ms_2_cycle(uint32_t ms);

uint64_t rtc_hw_porting_ms_2_cycle(uint64_t ms);

uint32_t rtc_porting_cycle_2_us(uint64_t cycle);

uint64_t rtc_porting_compensat_by_tcxo(uint64_t diff);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif