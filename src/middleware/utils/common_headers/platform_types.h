/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: PLATFORM TYPES
 * Author:
 * Date:
 */
#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/** @defgroup DRIVER_PLATFORM_PLATFORM_TYPES CHIP Platform TYPES Driver
  * @ingroup DRIVER_PLATFORM
  * @{
  */
#include "platform_core.h"
#include "chip_definitions.h"

/**
 * @brief  Generic enum to specify the required core.
 * By using an enum, type safety can be enforced at compile time.
 */
typedef enum {
    CORES_BT_CORE = 0,                              /* !< bt Core. */
    CORES_PROTOCOL_CORE = 1,                        /* !< Hifi Core. */
    CORES_APPS_CORE = 2,                            /* !< Applications Core. */
    CORES_EXTERN0_CORE = 3,                         /* !< GNSS Core for libra. */
#if CHIP_LIBRA
    CORES_GNSS_CORE = CORES_EXTERN0_CORE,
#else
    CORES_HIFI1_CORE = CORES_EXTERN0_CORE,
#endif
    CORES_EXTERN1_CORE = 4,                         /* !< Sec Core for libra. */
    CORES_SEC_CORE = CORES_EXTERN1_CORE,
#if (CORE_NUMS < 3)                                 /* !< For BS25, dsp core is null. */
    CORES_MAX_NUMBER_PHYSICAL = 3,                  /* !< Used to size/range arrays for physical cores where needed. */
#else
    CORES_MAX_NUMBER_PHYSICAL,
#endif
    CORES_NONE = CORES_MAX_NUMBER_PHYSICAL,         /* !< Used to return a NONE value where needed. */
    CORES_ASSET_CORE = CORES_MAX_NUMBER_PHYSICAL,   /* used for asset store */
    CORES_UNKNOWN = CORES_MAX_NUMBER_PHYSICAL + 1,  /* !< Used to return a unknown value. */
} cores_t;

/**
 * @brief  Generic enum to specify the core image.
 * On master C20 the standard Security core is CORE_IMAGES_RECOVERY
 * On production the standard Security core is CORE_IMAGES_BT
 */
typedef enum {
    CORE_IMAGES_BT = 0,           /* !< bt core image A. At the lowest loadable slot in Flash. */
    CORE_IMAGES_PROTOCOL = 1,     /* !< Hifi core image A */
    CORE_IMAGES_APPS = 2,         /* !< Application core image A.  */
    CORE_IMAGES_RECOVERY = 3,     /* !< Recover core image B. */
    CORE_IMAGES_EXTERN0 = 4,      /* !< GNSS image for libra. */
    CORE_IMAGES_EXTERN1 = 5,      /* !< Sec image for libra. */
    CORE_IMAGES_EXTERN1_SSB = 6,  /* !< Sec image for libra. */
#if CHIP_SOCMN1 || CHIP_BS25
    CORE_IMAGES_MAX_NUMBER = 5,
#else
    CORE_IMAGES_MAX_NUMBER = 7,
#endif
} core_images_e;

/**
 * @brief  Interrupt Service Routine Callback Type.
 */
typedef void (*isr_callback)(void);

/**
 * @brief  Interrupt State Type.
 */
typedef enum {
    INTERRUPT_STATE_DISABLED = 0, /* !< Interrupt Disabled */
    INTERRUPT_STATE_ENABLED = 1,  /* !< Interrupt Enabled  */
} interrupt_state_t;

/**
  * @}
  */
#endif
