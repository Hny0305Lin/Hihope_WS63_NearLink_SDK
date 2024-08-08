/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides systick port template \n
 *
 * History: \n
 * 2022-07-30， Create file. \n
 */
#ifndef SYSTICK_PORTING_H
#define SYSTICK_PORTING_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_systick Systick
 * @ingroup  drivers_port
 * @{
 */

#define RTC_TIME_OUT_H     0x40005018
#define RTC_TIME_OUT_L     0x4000501C
#define MS_PER_S                    1000
#define US_PER_MS                   1000

/**
 * @if Eng
 * @brief  Get the base address of a specified systick.
 * @return The base address of specified systick.
 * @else
 * @brief  获取指定SYSTICK的基地址。
 * @return 指定SYSTICK的基地址。
 * @endif
 */
uintptr_t systick_porting_base_addr_get(void);

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_systick module.
 * @else
 * @brief  将hal funcs对象注册到hal_systick模块中
 * @endif
 */
void systick_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_systick module.
 * @else
 * @brief  从hal_systick模块注销hal funcs对象
 * @endif
 */
void systick_port_unregister_hal_funcs(void);

/**
 * @if Eng
 * @brief  Cali XCLK.
 * @else
 * @brief  修正XCLK时钟
 * @endif
 */
void systick_port_cali_xclk(void);

/**
 * @if Eng
 * @brief  get systick clock freq.
 * @else
 * @brief  获取systick时钟频率。
 * @endif
 */
uint32_t systick_clock_get(void);

/**
 * @if Eng
 * @brief  set systick clock freq.
 * @else
 * @brief  设置systick时钟频率,用于延时计算。
 * @endif
 */
void systick_clock_set(uint32_t clock);

/**
 * @if Eng
 * @brief  second convert to systick count.
 * @else
 * @brief  秒转换为systick的count计数。
 * @endif
 */
static inline uint64_t convert_s_2_count(uint64_t sec)
{
    return sec / systick_clock_get();
}

/**
 * @if Eng
 * @brief  systick count convert to second.
 * @else
 * @brief  systick count转换为秒。
 * @endif
 */
static inline uint64_t convert_count_2_s(uint64_t count)
{
    return count / systick_clock_get();
}

/**
 * @if Eng
 * @brief  millisecond convert to systick count.
 * @else
 * @brief  毫秒转换为systick的count计数。
 * @endif
 */
static inline uint64_t convert_ms_2_count(uint64_t msec)
{
    return msec * systick_clock_get() / MS_PER_S;
}

/**
 * @if Eng
 * @brief  systick count convert to millisecond.
 * @else
 * @brief  systick count转换为毫秒。
 * @endif
 */
static inline uint64_t convert_count_2_ms(uint64_t count)
{
    return count * MS_PER_S / systick_clock_get();
}

/**
 * @if Eng
 * @brief  mircosecond convert to systick count.
 * @else
 * @brief  微秒转换为systick的count计数。
 * @endif
 */
static inline uint64_t convert_us_2_count(uint64_t usec)
{
    return usec * systick_clock_get() / (MS_PER_S * US_PER_MS);
}

/**
 * @if Eng
 * @brief  systick count convert to mircosecond.
 * @else
 * @brief  systick count转换为微秒。
 * @endif
 */
static inline uint64_t convert_count_2_us(uint64_t count)
{
    return count * MS_PER_S * US_PER_MS / systick_clock_get();
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
