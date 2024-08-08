/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: security_unified init header file.
 *
 * Create: 2023-03-29
*/

#ifndef SECURITY_INIT_H
#define SECURITY_INIT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_env_init env_init
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief  security_unified initialization, called by system initialize.
 * @else
 * @brief  安全驱动模块初始化，系统起来跑安全相关业务前调用，不需要用户调用。
 * @endif
 */
void uapi_drv_cipher_env_init(void);

/**
 * @if Eng
 * @brief  security_unified deinitialization, called by system deinitialize.
 * @else
 * @brief  安全驱动模块去初始化，系统深睡前调用，不需要用户调用。
 * @endif
 */
void uapi_drv_cipher_env_deinit(void);

/**
 * @if Eng
 * @brief  security_unified initialization, called by system wakeup resume function.
 * @else
 * @brief  安全驱动模块初始化，系统深睡唤醒时调用，不需要用户调用。
 * @endif
 */
void uapi_drv_cipher_env_resume(void);

/**
 * @if Eng
 * @brief  security_unified deinitialization, called by system deepsleep suspend function.
 * @else
 * @brief  安全驱动模块去初始化，系统深睡前调用，不需要用户调用。
 * @endif
 */
void uapi_drv_cipher_env_suspend(void);

/**
 * @if Eng
 * @brief  The interrupt mode of the safety drive module is deactivated,
    and the interrupt of the safety drive must be deactivated in the NMI.
 * @else
 * @brief  安全驱动模块关闭中断模式，在 NMI 中要关闭安全驱动的中断。
 * @endif
 */
void uapi_drv_cipher_wait_func_disable_all(void);

/**
 * @if Eng
 * @brief  The safety driver module turns on interrupt mode. Interrupts are on by default.
 * @else
 * @brief  安全驱动模块打开中断模式。默认情况中断是打开的。
 * @endif
 */
void uapi_drv_cipher_wait_func_enable_all(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* SECURITY_INIT_H */