/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: for osal adapt.
 *
 * Create: 2023-04-21
*/

#ifndef CRYPTO_OSAL_LIB_H
#define CRYPTO_OSAL_LIB_H

#include "securec.h"

#include "crypto_security.h"
#include "security_unified_porting.h"
#include "crypto_common_macro.h"
#include "boot_delay.h"
#include "tcxo.h"

typedef void crypto_wait_t;

void crypto_boot_debug_message(const char *fmt, ...);

#if defined(ROMBOOT_DEBUG_ON)
#define crypto_print(fmt, ...)      crypto_boot_debug_message(fmt, ##__VA_ARGS__)
#else
#define crypto_print(fmt, ...)
#endif

#define crypto_getpid()                 0

#define CRYPTO_COPY_XXX_USER_DEF

#define crypto_ioremap_nocache(addr, size)      (void *)(addr)
#define crypto_iounmap(virt_addr, size)

/* Register Read&Write. */
#define crypto_ex_reg_write(addr, val) do {                   \
    (*(volatile td_u32 *)(uintptr_t)(addr) = (val)); \
    __asm__ __volatile__("fence");                   \
} while (0)

#define crypto_ex_reg_write_u16(addr, val) do {                   \
    (*(volatile td_u16 *)(uintptr_t)(addr) = (val)); \
    __asm__ __volatile__("fence");                   \
} while (0)

#define spacc_reg_read(offset)          crypto_reg_read(SPACC_REG_BASE_ADDR + (offset))
#define spacc_reg_write(offset, value)  crypto_ex_reg_write(SPACC_REG_BASE_ADDR + (offset), value)

#define pke_reg_read(offset)            crypto_reg_read(PKE_REG_BASE_ADDR + (offset))
#define pke_reg_write(offset, value)    crypto_ex_reg_write(PKE_REG_BASE_ADDR + (offset), value)

#define km_reg_read(offset)             crypto_reg_read(KM_REG_BASE_ADDR + (offset))
#define km_reg_write(offset, value)     crypto_ex_reg_write(KM_REG_BASE_ADDR + (offset), value)

#define trng_reg_read(offset)           crypto_reg_read(TRNG_REG_BASE_ADDR + (offset))
#define trng_reg_write(offset, value)   crypto_ex_reg_write(TRNG_REG_BASE_ADDR + (offset), value)

#define fapc_reg_read(offset)           crypto_reg_read(FAPC_REG_BASE_ADDR + (offset))
#define fapc_reg_write(offset, value)   crypto_ex_reg_write(FAPC_REG_BASE_ADDR + (offset), value)

#endif