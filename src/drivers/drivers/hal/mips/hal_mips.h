/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  calculate mips
 *
 * Create:  2018-10-15
 */
#ifndef HAL_MIPS_H
#define HAL_MIPS_H

/** @defgroup connectivity_drivers_hal_mips MIPS
  * @ingroup  connectivity_drivers_hal
  * @{
  */
typedef void (*hal_exception_dump_mips_callback)(void);

/* register mips callback function */
void hal_register_mips_start_callback(hal_exception_dump_mips_callback callback);
void hal_register_mips_stop_callback(hal_exception_dump_mips_callback callback);

/* start calculating ticks  */
void hal_calculate_mips_start(void);

/* start calculating ticks  */
void hal_calculate_mips_stop(void);

/**
  * @}
  */
#endif
