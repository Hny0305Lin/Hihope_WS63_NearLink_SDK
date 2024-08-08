/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: for osal adapt.
 *
 * Create: 2023-04-21
*/

#ifndef CRYPTO_OSAL_LIB_H
#define CRYPTO_OSAL_LIB_H

#include "td_type.h"
#include "soc_osal.h"
#include "osal_wait.h"
#include "securec.h"
#include "tcxo.h"

#include "crypto_security.h"
#include "security_unified_porting.h"
#include "crypto_common_macro.h"
#include "crypto_common_struct.h"
#ifdef CONFIG_SECURITY_UNIFIED_SUPPORT_DEEP_SLEEP
#include "pm_veto.h"
#else
#define uapi_pm_add_sleep_veto(...)
#define uapi_pm_remove_sleep_veto(...)
#define PM_SECURITY_VETO_ID
#endif

typedef osal_wait crypto_wait_t;

#define CRYPTO_EXPORT_SYMBOL(...)

#define crypto_print(fmt, ...)

#define CRYPTO_COPY_XXX_USER_DEF

/* pid&tid. */
#define crypto_gettid()         osal_get_current_tid()
#define crypto_getpid()         osal_get_current_pid()

/* deep sleep vote & other process. */
#define crypto_service_preprocess() do { \
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID); \
} while (0)
#define crypto_service_postprocess()  do { \
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID); \
} while (0)

#define crypto_ioremap_nocache(addr, size)      (void *)(uintptr_t)(addr)
#define crypto_iounmap(virt_addr, size)

unsigned long crypto_osal_get_phys_addr(const crypto_buf_attr *buf);
#define crypto_osal_put_phys_addr(...)

/* Register Read&Write. */
#define crypto_ex_reg_write(addr, val)                   \
    do {                                                 \
        (*(volatile td_u32 *)(uintptr_t)(addr) = (val)); \
        __asm__ __volatile__("fence");                   \
    } while (0)

#define crypto_ex_reg_write_u16(addr, val)                   \
    do {                                                 \
        (*(volatile td_u16 *)(uintptr_t)(addr) = (val)); \
        __asm__ __volatile__("fence");                   \
    } while (0)

#define spacc_reg_read(offset) crypto_reg_read(SPACC_REG_BASE_ADDR + (offset))
#define spacc_reg_write(offset, value) crypto_ex_reg_write(SPACC_REG_BASE_ADDR + (offset), value)

#define pke_reg_read(offset) crypto_reg_read(PKE_REG_BASE_ADDR + (offset))
#define pke_reg_write(offset, value) crypto_ex_reg_write(PKE_REG_BASE_ADDR + (offset), value)

#define km_reg_read(offset) crypto_reg_read(KM_REG_BASE_ADDR + (offset))
#define km_reg_write(offset, value) crypto_ex_reg_write(KM_REG_BASE_ADDR + (offset), value)

#define trng_reg_read(offset) crypto_reg_read(TRNG_REG_BASE_ADDR + (offset))
#define trng_reg_write(offset, value) crypto_ex_reg_write(TRNG_REG_BASE_ADDR + (offset), value)

#define fapc_reg_read(offset)           crypto_reg_read(FAPC_REG_BASE_ADDR + (offset))
#define fapc_reg_write(offset, value)   crypto_ex_reg_write(FAPC_REG_BASE_ADDR + (offset), value)

#endif