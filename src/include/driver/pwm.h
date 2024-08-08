/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides PWM driver api \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pwm PWM
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of PWM configuration.
 * @else
 * @brief  PWM配置类型定义。
 * @endif
 */
typedef struct pwm_config {
    uint32_t low_time;               /*!< @if Eng PWM working clock cycle count low level part,
                                                  PWM work frequency @ref uapi_pwm_get_frequency().
                                                  If the PWM operating cycle is Tus,
                                                  the actual low-level time = low_time * Tus.
                                          @else   PWM工作时钟周期计数个数低电平部分，
                                                  频率参考 @ref uapi_pwm_get_frequency()。
                                                  如果PWM工作周期为Tus, 实际低电平时间 = low_time * Tus @endif */
    uint32_t high_time;              /*!< @if Eng PWM working clock cycle count high level part,
                                                  PWM work frequency @ref uapi_pwm_get_frequency().
                                                  If the PWM operating cycle is Tus,
                                                  the actual high-level time = high_time * Tus.
                                          @else   PWM工作时钟周期计数个数高电平部分，
                                                  频率参考 @ref uapi_pwm_get_frequency()。
                                                  如果PWM工作周期为Tus, 实际高电平时间 = high_time * Tus @endif */
    uint32_t offset_time;            /*!< @if Eng PWM offset time.
                                          @else   PWM相位。  @endif */
    uint16_t cycles;                 /*!< @if Eng PWM cycles, range:0~32767 (15bit).
                                          @else   PWM重复周期，范围：0~32767 (15bit)。  @endif */
    bool repeat;                     /*!< @if Eng Flag to indicate PWM should output continuously.
                                          @else   指示PWM应连续输出的标志。  @endif */
} pwm_config_t;

 /**
 * @if Eng
 * @brief  Definition of PWM callback type.
 * @param  [in]  channel PWM device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM 回调类型定义。
 * @param  [in]  channel PWM设备。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*pwm_callback_t)(uint8_t channel);

/**
 * @if Eng
 * @brief  Initialize the PWM.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化PWM。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the PWM.
 * @else
 * @brief  去初始化PWM。
 * @endif
 */
void uapi_pwm_deinit(void);

/**
 * @if Eng
 * @brief  Open a PWM device with the specified configuration.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @param  [in]  cfg Config set to the PWM device. For details, see @ref pwm_config_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  打开具有指定配置的PWM通道。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @param  [in]  cfg PWM设备的配置信息，参考 @ref pwm_config_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_open(uint8_t channel, const pwm_config_t *cfg);

/**
 * @if Eng
 * @brief  Close the specified PWM.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭指定的PWM。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_close(uint8_t channel);

/**
 * @if Eng
 * @brief  Start up PWM.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  开始PWM。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_start(uint8_t channel);

/**
 * @if Eng
 * @brief  Get PWM work frequency.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @retval PWM work frequency, the unit is HZ.
 * @else
 * @brief  获取PWM工作频率。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @retval PWM工作频率，单位为HZ。
 * @endif
 */
uint32_t uapi_pwm_get_frequency(uint8_t channel);

#if defined(CONFIG_PWM_USING_V150)
/**
 * @if Eng
 * @brief  Stop a running PWM.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止正在运行的PWM。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_stop(uint8_t channel);

/**
 * @if Eng
 * @brief  Update the duty ratio on a opened PWM.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @param  [in]  low_time PWM working clock cycle count low level part, PWM work freq @ref uapi_pwm_get_frequency().
                          If the PWM operating cycle is Tus, the actual low-level time = low_time * Tus.
 * @param  [in]  high_time PWM working clock cycle count high level part, PWM work freq @ref uapi_pwm_get_frequency().
                           If the PWM operating cycle is Tus, the actual high-level time = high_time * Tus.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新打开的PWM上的占空比。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @param  [in]  low_time PWM工作时钟周期计数个数低电平部分，频率参考 @ref uapi_pwm_get_frequency()。
                          如果PWM工作周期为Tus, 实际低电平时间 = low_time * Tus。
 * @param  [in]  high_time PWM工作时钟周期计数个数高电平部分，频率参考 @ref uapi_pwm_get_frequency()。
                           如果PWM工作周期为Tus, 实际高电平时间 = high_time * Tus。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_update_duty_ratio(uint8_t channel, uint32_t low_time, uint32_t high_time);
#endif /* CONFIG_PWM_USING_V150 */

/**
 * @if Eng
 * @brief  PWM Interrupt Service Routine.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM中断服务例程。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_isr(uint8_t channel);

/**
 * @if Eng
 * @brief  Register an interrupt callback for PWM.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @param  [in]  callback Callback to register.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  为PWM注册中断回调。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @param  [in]  callback 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_register_interrupt(uint8_t channel, pwm_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister an interrupt callback.
 * @param  [in]  channel PWM channel to use. For details, see @ref pwm_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去注册中断回调。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_unregister_interrupt(uint8_t channel);

#if defined(CONFIG_PWM_USING_V151)
/**
 * @if Eng
 * @brief  Grouping the channels for PWM channels into one group.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_GROUP_NUM.
 * @param  [in]  channel_set Set of channels to be grouped.
 * @param  [in]  channel_set_len Length of the channel set.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  为PWM通道分组。
 * @param  [in]  group PWM组。
 * @param  [in]  channel_set 进行分组设置的通道集合。
 * @param  [in]  channel_set_len 进行分组设置的通道集合长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_set_group(uint8_t group, const uint8_t *channel_set, uint32_t channel_set_len);

/**
 * @if Eng
 * @brief  Clear PWM channel of the group.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_GROUP_NUM.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清理PWM通道分组。
 * @param  [in]  group PWM组。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_clear_group(uint8_t group);

/**
 * @if Eng
 * @brief  Start up PWM of a group.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_GROUP_NUM.
 *                          corresponds to a corresponding channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动指定分组的PWM。
 * @param  [in]  group PWM组。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_start_group(uint8_t group);

/**
 * @if Eng
 * @brief  Stop PWM of a group
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_GROUP_NUM.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止指定分组的PWM。
 * @param  [in]  group PWM组。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_stop_group(uint8_t group);

#if defined(CONFIG_PWM_PRELOAD)
/**
 * @if Eng
 * @brief  PWM preload. When the previous PWM configuration is completed and configuration are auto loaded.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_GROUP_NUM.
 * @param  [in]  channel PWM channel. For details, see @ref pwm_channel_t.
 * @param  [in]  cfg Config set to the PWM device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM预配置，当上一个PWM配置完成时，此配置会自动加载。
 * @param  [in]  group PWM组。
 * @param  [in]  channel PWM通道，参考 @ref pwm_channel_t 。
 * @param  [in]  cfg PWM设备的配置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_config_preload(uint8_t group, uint8_t channel, const pwm_config_t *cfg);
#endif /* CONFIG_PWM_PRELOAD */
#endif /* CONFIG_PWM_USING_V151 */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
