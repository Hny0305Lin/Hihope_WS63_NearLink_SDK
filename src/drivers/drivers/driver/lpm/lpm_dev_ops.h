/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Low-power management operations for peripheral devices.
 *
 * Create:  2021-4-28
 */

#ifndef LPM_DEV_OPS_H
#define LPM_DEV_OPS_H
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup connectivity_drivers_non_os_lpm LPM
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
/**
 * @brief  The peripheral device operation functions.
 * @note   The functions are registered by pmu/clocks and is used by peripheral devices.
 */
typedef struct {
    int (*power_on)(bool);          /*!< Power on or power off the device. */
    int (*power_sts)(void);         /*!< Get the device power status. */
    int (*set_voltage)(int);        /*!< Config the device voltage. */
    int (*get_voltage)(void);       /*!< Get the device voltage. */
    int (*clock_en)(bool);          /*!< Enable or disable the device clock switch. */
    int (*clock_sts)(void);         /*!< Get the device clock switch status. */
    int (*set_freq)(int);           /*!< Set the clock frequency of the device. */
    int (*get_freq)(void);          /*!< Get the clock frequency of the device. */
    int (*set_div_num)(int);        /*!< Set the clock frequency divider of the device. */
    int (*get_div_num)(void);       /*!< Get the clock frequency divider of the device. */
    int (*sub_clken)(int, bool);    /*!< Enables or disables the clock of a subdevice. */
    int (*resume)(void);            /*!< Resume the device. */
    int (*suspend)(void);           /*!< Suspend the device. */
} lpm_dev_ops_t;

/**
 * @brief  Peripheral device ID.
 */
typedef enum {
    DEV_ADC,
    DEV_I2C,
    DEV_DMA,
    DEV_SPI,
    DEV_QSPI,
    DEV_UART,
    DEV_CAP,
    DEV_OPI,
    DEV_PSRAM,
    DEV_IR,
    DEV_EMMC,
    DEV_SDIO,
    DEV_DSS,
    DEV_GPU,
    DEV_PCIE,
    DEV_PWM,
    DEV_MAX,
} lpm_dev_id_t;

/**
 * @brief  Function return value.
 */
typedef enum {
    LPM_RET_OK = 0,      /*!< Run successfully. */
    LPM_RET_NOREG,       /*!< Not register. */
    LPM_RET_ON,          /*!< The switch status is ON. */
    LPM_RET_OFF,         /*!< The switch status is OFF. */
    LPM_RET_ERR,         /*!< Run error. */
    LPM_RET_UNINIT,      /*!< Uninitialized. */
    LPM_RET_MAX,
} lpm_ret_t;

typedef void (*lpm_clock_init_callback)(void);

/**
 * @brief  The peripheral device operations init.
 * @return Result of the operation, true indicates success, false for failed.
 */
bool lpm_dev_ops_init(void);

/**
 * @brief  The peripheral device operations deinit.
 * @return Result of the operation, true indicates success, false for failed.
 */
bool lpm_dev_ops_deinit(void);

/**
 * @brief  Register low-power operation interface of the peripheral device.
 * @param  id Peripheral device ID.
 * @param  ops Peripheral device operation methods.
 * @return Result of the operation, true indicates success, false for failed.
 */
bool lpm_dev_ops_register(lpm_dev_id_t id, lpm_dev_ops_t *ops);

/**
 * @brief  Deregister low-power operation interface of the peripheral device.
 * @param  id Peripheral device ID.
 * @return Result of the operation, true indicates success, false for failed.
 */
bool lpm_dev_ops_unregister(lpm_dev_id_t id);

/**
 * @brief  Update the operation function of the device.
 * @param id Peripheral device ID.
 * @param ops ops Peripheral device operation methods.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_ops_update(lpm_dev_id_t id, lpm_dev_ops_t *ops);

/**
 * @brief  Power on or power off the peripheral device.
 * @param  id Peripheral device ID.
 * @param  on Power on or Power off.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_power_on(lpm_dev_id_t id, bool on);

/**
 * @brief  Get the power status of peripheral Devices.
 * @param  id Peripheral device ID.
 * @return Power status, for details, refer to lpm_ret_t.
 */
int lpm_dev_get_power_sts(lpm_dev_id_t id);

/**
 * @brief  Config the peripheral device voltage.
 * @param  id Peripheral device ID.
 * @param  vset Voltage to be configured.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_set_voltage(lpm_dev_id_t id, int vset);

/**
 * @brief  Get the voltage of peripheral device.
 * @param  id Peripheral device ID.
 * @return Current voltage of the peripheral device.
 */
int lpm_dev_get_voltage(lpm_dev_id_t id);

/**
 * @brief  Enable or disable the device clock switch.
 * @param  id Peripheral device ID.
 * @param  on Enable or disable.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_clock_en(lpm_dev_id_t id, bool on);

/**
 * @brief  Get the device clock switch status.
 * @param  id Peripheral device ID.
 * @return Clock enable status, for details, refer to lpm_ret_t.
 */
int lpm_dev_get_clock_sts(lpm_dev_id_t id);

/**
 * @brief  Set the clock frequency of the peripheral device.
 * @param  id Peripheral device ID.
 * @param  freq Frequency to be configured.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_set_freq(lpm_dev_id_t id, int freq);

/**
 * @brief  Get the clock frequency of the peripheral device.
 * @param  id Peripheral device ID.
 * @return Current frequency of the peripheral device.
 */
int lpm_dev_get_freq(lpm_dev_id_t id);

/**
 * @brief  Set the divider number of the peripheral device clock.
 * @param  id Peripheral device ID.
 * @param  clk_div Divider number to be configured.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_set_div_num(lpm_dev_id_t id, int clk_div);

/**
 * @brief  Get the divider number of the peripheral device clock.
 * @param  id Peripheral device ID.
 * @return Current divider number of the peripheral device clock.
 */
int lpm_dev_get_div_num(lpm_dev_id_t id);

/**
 * @brief  Enables or disables the clock of a subdevice.
 * @param  id Peripheral device ID.
 * @param  bus Sub device bus number.
 * @param  on Enable or disable.
 * @return Success or not, for details, refer to lpm_ret_t.
 */
int lpm_dev_sub_bus_clken(lpm_dev_id_t id, int bus, bool on);

/**
 * @brief Registering the callback function for clock initialization configuration of the display module.
 * @param callback Callback function.
 */
void lpm_display_clocks_init_register_callback(lpm_clock_init_callback callback);

/**
 * @brief Clock initialization configuration of the display module.
 */
void lpm_display_clocks_init(void);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif