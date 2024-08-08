/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   HAL PMU CPU DRIVER HEADER.
 *
 * Create: 2020-01-15
 */

#ifndef SRC_DRIVERS_HAL_REG_CONFIG_H
#define SRC_DRIVERS_HAL_REG_CONFIG_H

#include "chip_io.h"

/** @defgroup connectivity_drivers_hal_reg_config REG Config
  * @ingroup  connectivity_drivers_hal
  * @{
  */
/**
 * @brief  Check register value
 * @param  addr Register addr
 * @param  offset Register bit offset
 * @param  on Bit set/clear
 * @param  timeout Check timeout value
 * @return None
 */
void hal_reg_status_check_timeout(uint32_t addr, uint16_t offset, switch_type_t on, uint32_t timeout);

/**
 * @brief  Set register value
 * @param  addr Register addr
 * @param  position Register bit offset
 * @param  on Bit set/clear
 * @return None
 */
void hal_reg_config_bit(uint32_t addr, switch_type_t on, REG16_POS position);

/**
  * @}
  */
#endif
