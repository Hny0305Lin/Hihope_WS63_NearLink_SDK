/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   HAL Reboot functionality header file
 *
 * Create:  2018-10-15
 */

#ifndef HAL_REBOOT_H
#define HAL_REBOOT_H

#include "stdint.h"
#include "stdbool.h"

/** @defgroup connectivity_drivers_hal_reboot REBOOT
  * @ingroup  connectivity_drivers_hal
  * @{
  */

/**
 * @brief  Check whether the hard watchdog is restarted during the last restart
 * @param  cause The reset cause
 * @return true is yes; false is no
 */
bool hal_reboot_hard_wdg_timeout(uint16_t cause);

/** De-initialise the reboot subsystem */
void hal_reboot_deinit(void);

/** Perform a software reboot (for chip). */
void hal_reboot_chip(void);

/** Clear reboot history. */
void hal_reboot_clear_history(void);

/**
 * @brief  Return reset code showing why this core was rebooted
 *  @return the reset cause bitfield
 */
uint16_t hal_reboot_get_reset_reason(void);

/**
 * @brief  Set a universal register of the ULP region to a fix value
 */
void hal_reboot_set_ulp_aon_no_poweroff_flag(void);

/**
 * @brief  Get the universal power-on/off flag register of the ULP region
 *  @return true no poweroff, false is re-poweron
 */
bool hal_reboot_get_ulp_aon_no_poweroff_flag(void);

/**
  * @}
  */
#endif
