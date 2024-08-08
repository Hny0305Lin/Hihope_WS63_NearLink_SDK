/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: drivers of drv_osal_init
 *
 * Create: 2023-01-18
*/

#include "crypto_drv_common.h"
#include "crypto_curve_param.h"
#include "hal_cipher_trng.h"
#include "hal_pke.h"
#include "drv_common.h"
#include "kapi_inner.h"
#include "crypto_drv_irq.h"
#include "soc_osal.h"
#include "crypto_drv_init.h"
#if defined(CONFIG_SM_EFUSE_CHECK)
#include "efuse.h"
#endif /* CONFIG_SM_EFUSE_CHECK */

#define CACHE_LINE_SIZE         32
static td_bool g_wait_func_enable = TD_TRUE;        /* default is true. */
static crypto_mutex g_crypto_mutex;

#define crypto_lock() do {                   \
    crypto_mutex_lock(&g_crypto_mutex);           \
} while (0)
#define crypto_unlock() do {                 \
    crypto_mutex_unlock(&g_crypto_mutex);         \
} while (0)

static td_void *inner_malloc_coherent(td_u32 size, crypto_mem_type mem_type)
{
    crypto_unused(mem_type);
    return osal_kmalloc_align(size, OSAL_GFP_KERNEL, CACHE_LINE_SIZE);
}

static td_void inner_free_coherent(td_void *ptr)
{
    osal_kfree(ptr);
}

static td_phys_addr_t inner_get_phys_addr(td_void *ptr)
{
    return (td_phys_addr_t)(uintptr_t)(ptr);
}

static crypto_cpu_type liteos_get_cpu_type(td_void)
{
    return CRYPTO_CPU_TYPE_ACPU;
}

static td_u32 liteos_get_pke_rom_lib_start_addr(td_void)
{
    return PKE_ROM_LIB_START_ADDR;
}

#if defined(CONFIG_SM_EFUSE_CHECK)
#define EFUSE_SM_ENABLE_OFFSET 41
static td_u8 g_is_sm3_disable;
td_bool crypto_sm_support(crypto_sm_alg alg)
{
    if (alg == CRYPTO_SM_ALG_SM3) {
        return g_is_sm3_disable == TD_TRUE ? TD_FALSE : TD_TRUE;
    }
    return TD_TRUE;
}
#endif /* CONFIG_SM_EFUSE_CHECK */

td_s32 crypto_drv_mod_resume(td_void)
{
    td_s32 ret = TD_SUCCESS;
    crypto_drv_func drv_func_list = {0};

    drv_func_list.malloc_coherent = inner_malloc_coherent;
    drv_func_list.free_coherent = inner_free_coherent;
    drv_func_list.get_phys_addr = inner_get_phys_addr;
    drv_func_list.get_cpu_type = liteos_get_cpu_type;
    drv_func_list.get_pke_rom_lib_start_addr = liteos_get_pke_rom_lib_start_addr;
    (td_void)drv_cipher_register_func(&drv_func_list);

    ret = hal_cipher_trng_init();
    crypto_chk_print(ret != TD_SUCCESS, "trng init failed, ret = 0x%u\n", ret);
    ret = hal_pke_init();
    crypto_chk_print(ret != TD_SUCCESS, "pke init failed, ret = 0x%u\n", ret);
    crypto_curve_param_init();

    ret = crypto_irq_init();
    crypto_chk_print(ret != TD_SUCCESS, "crypto_irq_init failed, ret = 0x%u\n", ret);

    crypto_print("resume load unified_cipher success.\n");

    return ret;
}

td_void crypto_drv_mod_suspend(td_void)
{
    crypto_drv_func drv_func_list = {0};

    drv_func_list.malloc_coherent = TD_NULL;
    drv_func_list.free_coherent = TD_NULL;
    drv_func_list.get_phys_addr = TD_NULL;
    drv_func_list.get_cpu_type = TD_NULL;
    (td_void)drv_cipher_register_func(&drv_func_list);
}

td_s32 crypto_drv_mod_init(td_void)
{
    td_s32 ret = TD_SUCCESS;
    crypto_drv_func drv_func_list = {0};

    drv_func_list.malloc_coherent = inner_malloc_coherent;
    drv_func_list.free_coherent = inner_free_coherent;
    drv_func_list.get_phys_addr = inner_get_phys_addr;
    drv_func_list.get_cpu_type = liteos_get_cpu_type;
    drv_func_list.get_pke_rom_lib_start_addr = liteos_get_pke_rom_lib_start_addr;
    (td_void)drv_cipher_register_func(&drv_func_list);

    ret = crypto_mutex_init(&g_crypto_mutex);
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_mutex_init failed\n");

    ret = kapi_env_init();
    crypto_chk_goto(ret != TD_SUCCESS, error_mutex_destroy, "kapi_env_init failed\n");

    crypto_curve_param_init();

    ret = crypto_irq_init();
    crypto_chk_goto(ret != TD_SUCCESS, error_env_deinit, "crypto_irq_init failed\n");

#if defined(CONFIG_SM_EFUSE_CHECK)
    (void)uapi_efuse_read_bit(&g_is_sm3_disable, EFUSE_SM_ENABLE_OFFSET, 0);
#endif

    crypto_print("normal load unified_cipher success.\n");
    return ret;

error_env_deinit:
    kapi_env_deinit();
error_mutex_destroy:
    crypto_mutex_destroy(&g_crypto_mutex);
    return ret;
}

td_void crypto_drv_mod_deinit(td_void)
{
    crypto_drv_func drv_func_list = {0};

    crypto_irq_deinit();
    kapi_env_deinit();

    drv_func_list.malloc_coherent = TD_NULL;
    drv_func_list.free_coherent = TD_NULL;
    drv_func_list.get_phys_addr = TD_NULL;
    drv_func_list.get_cpu_type = TD_NULL;
    (td_void)drv_cipher_register_func(&drv_func_list);

    crypto_mutex_destroy(&g_crypto_mutex);

    crypto_print("remove unified_cipher success.\n");
}

/* wait function mode control. */
td_void crypto_wait_func_config(td_bool is_enable)
{
    crypto_lock();
    g_wait_func_enable = is_enable;
    crypto_unlock();
}

td_bool crypto_wait_func_is_support(void)
{
    td_bool is_support;
    crypto_lock();
    is_support = g_wait_func_enable;
    crypto_unlock();

    return is_support;
}