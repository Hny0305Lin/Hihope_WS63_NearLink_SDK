/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 HAL watchdog \n
 *
 * History: \n
 * 2022-11-26, Create file. \n
 */
#ifndef HAL_WATCHDOG_V151_REGS_OP_H
#define HAL_WATCHDOG_V151_REGS_OP_H

#include <stdint.h>
#include "hal_watchdog_v151_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_watchdog_v151_regs_op Watchdog V151 Regs Operation
 * @ingroup  drivers_hal_watchdog
 * @{
 */

extern uintptr_t g_watchdog_regs;

/**
 * @brief  Set the value of @ref wdt_v151_lock_data.
 * @param  [in] val  The value of @ref wdt_v151_lock_data.
 */
static inline void hal_watchdog_wdt_lock_set(uint32_t val)
{
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_lock = val;
}

/**
 * @brief  Get the value of @ref wdt_v151_lock_data.
 * @return The value of @ref wdt_v151_lock_data.
 */
static inline uint32_t hal_watchdog_wdt_lock_get(void)
{
    return ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_lock;
}

/**
 * @brief  Get the value of @ref wdt_v151_load_data.wdt_load.
 * @return The value of @ref wdt_v151_load_data.wdt_load.
 */
static inline uint32_t hal_watchdog_wdt_load_get_wdt_load(void)
{
    wdt_v151_load_data_t wdt_load;
    wdt_load.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_load;
    return wdt_load.b.wdt_load;
}

/**
 * @brief  Set the value of @ref wdt_v151_load_data.wdt_load.
 * @param  [in] val The value of @ref wdt_v151_load_data.wdt_load.
 */
static inline void hal_watchdog_wdt_load_set_wdt_load(uint32_t val)
{
    wdt_v151_load_data_t wdt_load;
    wdt_load.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_load;
    wdt_load.b.wdt_load = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_load = wdt_load.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_load_data.reserved0_7.
 * @return The value of @ref wdt_v151_load_data.reserved0_7.
 */
static inline uint32_t hal_watchdog_wdt_load_get_wdt_reserved(void)
{
    wdt_v151_load_data_t wdt_load;
    wdt_load.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_load;
    return wdt_load.b.reserved0_7;
}

/**
 * @brief  Set the value of @ref wdt_v151_restart_data.
 * @param  [in] val The value of @ref wdt_v151_restart_data.
 */
static inline void hal_watchdog_wdt_restart_set(uint32_t val)
{
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_restart = val;
}

/**
 * @brief  Get the value of @ref wdt_v151_restart_data.
 * @return The value of @ref wdt_v151_restart_data.
 */
static inline uint32_t hal_watchdog_wdt_restart_get(void)
{
    return ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_restart;
}

/**
 * @brief  Set the value of @ref wdt_v151_eoi_data.wdt_eoi.
 * @param  [in] val The value of @ref wdt_v151_eoi_data.wdt_eoi.
 */
static inline void hal_watchdog_wdt_eoi_set_wdt_eoi(uint32_t val)
{
    wdt_v151_eoi_data_t wdt_eoi;
    wdt_eoi.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_eoi;
    wdt_eoi.b.wdt_eoi = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_eoi = wdt_eoi.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_cr_data.wdt_en.
 * @return The value of @ref wdt_v151_cr_data.wdt_en.
 */
static inline uint32_t hal_watchdog_v151_wdt_cr_get_wdt_en(void)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.wdt_en;
}

/**
 * @brief  Set the value of @ref wdt_v151_cr_data.wdt_en.
 * @param  [in] val The value of @ref wdt_v151_cr_data.wdt_en.
 */
static inline void hal_watchdog_v151_wdt_cr_set_wdt_en(uint32_t val)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.wdt_en = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_cr_data.rst_en.
 * @return The value of @ref wdt_v151_cr_data.rst_en.
 */
static inline uint32_t hal_watchdog_wdt_cr_get_rst_en(void)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.rst_en;
}

/**
 * @brief  Set the value of @ref wdt_v151_cr_data.rst_en.
 * @param  [in] val The value of @ref wdt_v151_cr_data.rst_en.
 */
static inline void hal_watchdog_wdt_cr_set_rst_en(uint32_t val)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.rst_en = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_cr_data.rst_pl.
 * @return The value of @ref wdt_v151_cr_data.rst_pl.
 */
static inline uint32_t hal_watchdog_wdt_cr_get_rst_pl(void)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.rst_pl;
}

/**
 * @brief  Set the value of @ref wdt_v151_cr_data.rst_pl.
 * @param  [in]  val The value of @ref wdt_v151_cr_data.rst_pl.
 */
static inline void hal_watchdog_wdt_cr_set_rst_pl(uint32_t val)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.rst_pl = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_cr_data.wdt_imsk.
 * @return The value of @ref wdt_v151_cr_data.wdt_imsk.
 */
static inline uint32_t hal_watchdog_wdt_cr_get_wdt_imsk(void)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.wdt_imsk;
}

/**
 * @brief  Set the value of @ref wdt_v151_cr_data.wdt_imsk.
 * @param  [in]  val The value of @ref wdt_v151_cr_data.wdt_imsk.
 */
static inline void hal_watchdog_wdt_cr_set_wdt_imsk(uint32_t val)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.wdt_imsk = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_cr_data.wdt_mode.
 * @return The value of @ref wdt_v151_cr_data.wdt_mode.
 */
static inline uint32_t hal_watchdog_wdt_cr_get_wdt_mode(void)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.wdt_mode;
}

/**
 * @brief  Set the value of @ref wdt_v151_eoi_data.wdt_mode.
 * @param  [in]  val The value of @ref wdt_v151_eoi_data.wdt_mode.
 */
static inline void hal_watchdog_wdt_cr_set_wdt_mode(uint32_t val)
{
    wdt_v151_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.wdt_mode = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_cnt_data.
 * @return The value of @ref wdt_v151_cnt_data.
 */
static inline uint32_t hal_watchdog_wdt_cnt_get(void)
{
    return ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_cnt;
}

/**
 * @brief  Get the value of @ref wdt_v151_raw_intr_data.wdt_raw_intr.
 * @return The value of @ref wdt_v151_raw_intr_data.wdt_raw_intr.
 */
static inline uint32_t hal_watchdog_wdt_raw_intr_get_wdt_raw_intr(void)
{
    wdt_v151_raw_intr_data_t wdt_raw_intr;
    wdt_raw_intr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_raw_intr;
    return wdt_raw_intr.b.wdt_raw_intr;
}

/**
 * @brief  Get the value of @ref wdt_v151_intr_data.wdt_intr.
 * @return The value of @ref wdt_v151_intr_data.wdt_intr.
 */
static inline uint32_t hal_watchdog_wdt_intr_get_wdt_intr(void)
{
    wdt_v151_intr_data_t wdt_intr;
    wdt_intr.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_intr;
    return wdt_intr.b.wdt_intr;
}

/**
 * @brief  Get the value of @ref wdt_v151_lpif_state_data.wdt_lpif_state.
 * @return The value of @ref wdt_v151_lpif_state_data.wdt_lpif_state.
 */
static inline uint32_t hal_watchdog_wdt_lpif_state_get_wdt_lpif_state(void)
{
    wdt_v151_lpif_state_data_t wdt_lpif_state;
    wdt_lpif_state.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_lpif_state;
    return wdt_lpif_state.b.wdt_lpif_state;
}

/**
 * @brief  Set the value of @ref wdt_v151_lpif_state_data.wdt_lpif_state.
 * @param  [in] val The value of @ref wdt_v151_lpif_state_data.wdt_lpif_state.
 */
static inline void hal_watchdog_wdt_lpif_state_set_wdt_lpif_state(uint32_t val)
{
    wdt_v151_lpif_state_data_t wdt_lpif_state;
    wdt_lpif_state.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_lpif_state;
    wdt_lpif_state.b.wdt_lpif_state = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_lpif_state = wdt_lpif_state.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_status_data.wdt_status.
 * @return The value of @ref wdt_v151_status_data.wdt_status.
 */
static inline uint32_t hal_watchdog_wdt_status_get_wdt_status(void)
{
    wdt_v151_status_data_t wdt_status;
    wdt_status.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_status;
    return wdt_status.b.wdt_status;
}

/**
 * @brief  Set the value of @ref wdt_v151_status_data.wdt_status.
 * @param  [in] val The value of @ref wdt_v151_status_data.wdt_status.
 */
static inline void hal_watchdog_wdt_status_set_wdt_status(uint32_t val)
{
    wdt_v151_status_data_t wdt_status;
    wdt_status.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_status;
    wdt_status.b.wdt_status = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_status = wdt_status.d32;
}

/**
 * @brief  Set the value of @ref wdt_v151_ccvr_en_data.wdt_ccvr_req.
 * @param  [in] val The value of @ref wdt_v151_ccvr_en_data.wdt_ccvr_req.
 */
static inline void hal_watchdog_wdt_ccvr_en_set_wdt_ccvr_req(uint32_t val)
{
    wdt_v151_ccvr_en_data_t wdt_ccvr_en;
    wdt_ccvr_en.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_ccvr_en;
    wdt_ccvr_en.b.wdt_ccvr_req = val;
    ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_ccvr_en = wdt_ccvr_en.d32;
}

/**
 * @brief  Get the value of @ref wdt_v151_ccvr_en_data.wdt_ccvr_req.
 * @return The value of @ref wdt_v151_ccvr_en_data.wdt_ccvr_req.
 */
static inline uint32_t hal_watchdog_wdt_ccvr_en_get_wdt_ccvr_req(void)
{
    wdt_v151_ccvr_en_data_t wdt_ccvr_en;
    wdt_ccvr_en.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_ccvr_en;
    return wdt_ccvr_en.b.wdt_ccvr_req;
}

/**
 * @brief  Get the value of @ref wdt_v151_ccvr_en_data.wdt_ccvr_lock.
 * @return The value of @ref wdt_v151_ccvr_en_data.wdt_ccvr_lock.
 */
static inline uint32_t hal_watchdog_wdt_ccvr_en_get_wdt_ccvr_lock(void)
{
    wdt_v151_ccvr_en_data_t wdt_ccvr_en;
    wdt_ccvr_en.d32 = ((watchdog_v151_regs_t *)g_watchdog_regs)->wdt_ccvr_en;
    return wdt_ccvr_en.b.wdt_ccvr_lock;
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