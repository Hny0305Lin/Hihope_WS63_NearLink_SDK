/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Common, (non-)os independant, UART driver interface
 */

#ifndef PLATFORM_DRIVER_H
#define PLATFORM_DRIVER_H

#include "list.h"

/** @addtogroup DRIVERS
  * @{
  */
/** @addtogroup DRIVER_PLATFORM
  * @{
  */
/** @defgroup DRIVER_PLATFORM_PLATFORM_DRIVER CHIP Platform PLATFORM Driver
  * @{
  */
typedef enum {
    DEV_WAKEUP,  // !< Represents this is a wakeup device.
    DEV_ZOMBIE   // !< Represents this is not a wakeup device.
} dev_type_e;

typedef enum {
    DEV_POWER_ON,  // !< Represents this device have power on.
    DEV_POWER_OFF  // !< Represents this device have power off.
} dev_power_e;

struct driver_operation {
    void (*init)(void);
    int (*open)(void);
    int (*close)(void);
    void (*resume)(void);
    void (*suspend)(void);
    int (*deinit)(void);
};

typedef struct plt_dev_t {
    char *name;
    dev_type_e device_t;
    dev_power_e power_status;
    struct list_head device_list;
    struct driver_operation *driver;
} plt_dev_t;

/**
 * @brief  Platform driver init.
 */
int platform_driver_init(void);

/**
 * @brief  Platform device driver register.
 * @param  device       device
 * @return None
 */
int platform_driver_register(plt_dev_t *device);

/**
 * @brief  Platform device resume.restore clock or power on.
 * @return None
 */
void platform_device_resume(void);
/**
 * @brief  Platform device suspend.gating clock or power off.
 * @return None
 */
void platform_device_suspend(void);

/**
  * @}
  */
/**
  * @}
  */
/**
  * @}
  */
#endif
