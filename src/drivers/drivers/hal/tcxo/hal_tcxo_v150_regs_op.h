/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 tcxo register operation api \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#ifndef HAL_TCXO_V150_REGS_OP_H
#define HAL_TCXO_V150_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_tcxo_v150_regs_def.h"
#include "tcxo_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tcxo_v150_regs_op TCXO V150 Regs Operation
 * @ingroup  drivers_hal_tcxo
 * @{
 */

extern tcxo_regs_t *g_tcxo_regs;

/**
 * @brief  Set the value of @ref tcxo_status_data.enable.
 */
static inline void hal_tcxo_status_set_enable(void)
{
    tcxo_status_data_t status;
    status.d32 = g_tcxo_regs->status;
    status.b.enable = 1;
    g_tcxo_regs->status = status.d32;
}

/**
 * @brief  Get the value of @ref tcxo_status_data.enable.
 */
static inline uint32_t hal_tcxo_status_get_enable(void)
{
    tcxo_status_data_t status;
    status.d32 = g_tcxo_regs->status;
    return status.b.enable;
}

/**
 * @brief  Set the value of @ref tcxo_status_data.clear.
 */
static inline void hal_tcxo_status_set_clear(void)
{
    tcxo_status_data_t status;
    status.d32 = g_tcxo_regs->status;
    status.b.clear = 1;
    g_tcxo_regs->status = status.d32;
}

/**
 * @brief  Set the value of @ref tcxo_status_data.refresh.
 */
static inline void hal_tcxo_status_set_refresh(void)
{
    tcxo_status_data_t status;
    status.d32 = g_tcxo_regs->status;
    status.b.refresh = 1;
    g_tcxo_regs->status = status.d32;
}

/**
 * @brief  get the value of @ref tcxo_status_data.valid.
 */
static inline uint32_t hal_tcxo_status_get_valid(void)
{
    tcxo_status_data_t status;
    status.d32 = g_tcxo_regs->status;
    return status.b.valid;
}

/**
 * @brief  Init the tcxo which will set the base address of registers.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 */
errcode_t hal_tcxo_v150_regs_init(void);

/**
 * @brief  Deinit the tcxo which will clear the base address of registers has been set by @ref hal_tcxo_v150_regs_init.
 */
void hal_tcxo_v150_regs_deinit(void);

/**
 * @brief  Get the value of count.
 * @return  The value of count.
 */
uint64_t hal_tcxo_reg_count_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif