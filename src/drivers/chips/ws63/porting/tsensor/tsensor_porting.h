/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides tsensor port \n
 *
 * History: \n
 * 2023-02-28， Create file. \n
 */

#ifndef TSENSOR_PORTING_H
#define TSENSOR_PORTING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_tsensor TSENSOR
 * @ingroup  drivers_port
 * @{
 */

#if defined(CONFIG_TSENSOR_MULTILEVEL)
/**
 * @if Eng
 * @brief  Tsensor multilevel interrupt enable level.
 * @else
 * @brief  Tsensor多级中断使能。
 * @endif
 */
typedef enum tsensor_multilevel_en {
    TSENSOR_MULTILEVEL_EN_LTO0,  /*!< @if Eng Enable interrupt when the temperature falls in the L - 0 position.
                                          @else   温度降至L-0位置时使能中断。  @endif */
    TSENSOR_MULTILEVEL_EN_0TO1,      /*!< @if Eng Enable interrupt when the temperature falls in the 0 - 1 position.
                                          @else   温度降至0-1位置时使能中断。  @endif */
    TSENSOR_MULTILEVEL_EN_1TO2,      /*!< @if Eng Enable interrupt when the temperature falls in the 1 - 2 position.
                                          @else   温度降至1-2位置时使能中断。  @endif */
    TSENSOR_MULTILEVEL_EN_2TO3,      /*!< @if Eng Enable interrupt when the temperature falls in the 2 - 3 position.
                                          @else   温度降至2-3位置时使能中断。  @endif */
    TSENSOR_MULTILEVEL_EN_3TOH,      /*!< @if Eng Enable interrupt when the temperature falls in the 3 - H position.
                                          @else   温度降至3-H位置时使能中断。  @endif */
    TSENSOR_MULTILEVEL_EN_MAX
} tsensor_multilevel_en_t;

/**
 * @if Eng
 * @brief  Tsensor multilevel threshold value level.
 * @else
 * @brief  Tsensor多级阈值级别。
 * @endif
 */
typedef enum tsensor_multilevel_value {
    TSENSOR_MULTILEVEL_VAL_0,  /*!< @if Eng Setting the temperature threshold for gear 0.
                                        @else   设置0档温度阈值。  @endif */
    TSENSOR_MULTILEVEL_VAL_1,      /*!< @if Eng Setting the temperature threshold for gear 1.
                                        @else   设置1档温度阈值。  @endif */
    TSENSOR_MULTILEVEL_VAL_2,      /*!< @if Eng Setting the temperature threshold for gear 2.
                                        @else   设置2档温度阈值。  @endif */
    TSENSOR_MULTILEVEL_VAL_3,      /*!< @if Eng Setting the temperature threshold for gear 3.
                                        @else   设置3档温度阈值。  @endif */
    TSENSOR_MULTILEVEL_VAL_MAX
} tsensor_multilevel_value_t;
#endif /* CONFIG_TSENSOR_MULTILEVEL */

/**
 * @if Eng
 * @brief  Get the base address of a specified Tsensor.
 * @return The base address of specified Tsensor.
 * @else
 * @brief  获取指定Tsensor的基地址。
 * @return 指定Tsensor的基地址。
 * @endif
 */
uintptr_t tsensor_port_base_addr_get(void);

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_tsensor module.
 * @else
 * @brief  将hal funcs对象注册到hal_tsensor模块中。
 * @endif
 */
void tsensor_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_tsensor module.
 * @else
 * @brief  从hal_tsensor模块注销hal funcs对象。
 * @endif
 */
void tsensor_port_unregister_hal_funcs(void);

/**
 * @if Eng
 * @brief  Register irq for Tsensor.
 * @else
 * @brief  Tsensor注册中断。
 * @endif
 */
void tsensor_port_register_irq(void);

/**
 * @if Eng
 * @brief  Unregister irq for Tsensor.
 * @else
 * @brief  Tsensor去注册中断。
 * @endif
 */
void tsensor_port_unregister_irq(void);

/**
 * @if Eng
 * @brief  Set the divider number of the peripheral device clock.
 * @param [in] value Enable or disable.
 * @else
 * @brief  设置外设时钟分频数。
 * @param [in] value 启用或禁用。
 * @endif
 */
void tsensor_port_power_on_and_fre_div(bool value);

/**
 * @if Eng
 * @brief  Lock of Tsensor interrupt.
 * @else
 * @brief  Tsensor中断锁定。
 * @endif
 */
uint32_t tsensor_port_irq_lock(void);

/**
 * @if Eng
 * @brief  Unlock of Tsensor interrupt.
 * @else
 * @brief  Tsensor中断解锁。
 * @endif
 */
void tsensor_port_irq_unlock(uint32_t flag);

/**
 * @if Eng
 * @brief  Get system time MS.
 * @else
 * @brief  获取系统时间MS。
 * @endif
 */
uint64_t tsensor_port_get_ms(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
