/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: drivers of irq.
 *
 * Create: 2023-06-16
*/

#include "crypto_drv_irq.h"
#include "crypto_hash_struct.h"
#include "hal_hash.h"
#include "hal_symc.h"
#include "hal_pke.h"

#include "drv_hash.h"
#include "drv_inner.h"

#include "crypto_drv_common.h"
#include "crypto_common_macro.h"
#ifdef CONFIG_SPACC_IRQ_ENABLE
#include "security_unified_porting.h"
#endif

#define IRQ_TIMEOUT_MS                          (1000 * 5)

#ifdef CONFIG_PKE_IRQ_ENABLE
static osal_wait g_pke_wait_queue     = { 0 };
#endif

#ifdef CONFIG_SPACC_IRQ_ENABLE
static osal_wait g_spacc_wait_queue    = { 0 };
#endif

#ifdef CONFIG_SPACC_IRQ_ENABLE
static int spacc_interrupt_irq(int irq, void *param)
{
    td_u32 i;
    (void)irq;
    (void)param;

    crypto_log_dbg("enter %s, irq is %d\r\n", __func__, irq);
    for (i = 0; i < CRYPTO_HASH_HARD_CHN_CNT; i++) {
        if (((1 << i) & CRYPTO_HASH_HARD_CHN_MASK) == 0) {
            continue;
        }
        if (hal_cipher_hash_done_try(i) != 0) {
            crypto_log_dbg("hash chn %d int occurs\r\n", i);
            hal_cipher_hash_done_notify(i);
            osal_wait_wakeup(&g_spacc_wait_queue);
            return 0;
        }
    }
    for (i = 1; i < CRYPTO_SYMC_HARD_CHANNEL_MAX; i++) {
        if (((1 << i) & CRYPTO_SYMC_HARD_CHANNEL_MASK) == 0) {
            continue;
        }
        if (hal_cipher_symc_done_try(i) == TD_SUCCESS) {
            crypto_log_dbg("symc chn %d int occurs\r\n", i);
            hal_cipher_symc_done_notify(i);
            osal_wait_wakeup(&g_spacc_wait_queue);
            return 0;
        }
    }
    return 0;
}
#endif

static td_s32 spacc_irq_init(td_void)
{
#ifdef CONFIG_SPACC_IRQ_ENABLE
    td_s32 ret = TD_SUCCESS;
    td_u32 i;

    /* IRQ Init. */
    if (g_spacc_wait_queue.wait == NULL) {
        ret = osal_wait_init(&g_spacc_wait_queue);
        crypto_chk_return(ret != TD_SUCCESS, ret, "osal_wait_init failed, ret is 0x%x\n", ret);

        ret = osal_irq_request(SPACC_REE_IRQ_NUM, spacc_interrupt_irq, TD_NULL, "nsec_spacc", TD_NULL);
        crypto_chk_goto(ret != TD_SUCCESS, error_spacc_wait_deinit, "osal_irq_request failed, ret is 0x%x\n", ret);
    }

    for (i = 0; i < CRYPTO_HASH_HARD_CHN_CNT; i++) {
        if (((1 << i) & CRYPTO_HASH_HARD_CHN_MASK) == 0) {
            continue;
        }
        (td_void)hal_cipher_hash_register_wait_func(i, &g_spacc_wait_queue,
            (crypto_wait_timeout_interruptible)osal_wait_timeout_interruptible, IRQ_TIMEOUT_MS);
        (td_void)hal_cipher_hash_wait_func_config(i, TD_TRUE);
    }
    for (i = 0; i < CRYPTO_SYMC_HARD_CHANNEL_MAX; i++) {
        if (((1 << i) & CRYPTO_SYMC_HARD_CHANNEL_MASK) == 0) {
            continue;
        }
        (td_void)hal_cipher_symc_register_wait_func(i, &g_spacc_wait_queue,
            (crypto_wait_timeout_interruptible)osal_wait_timeout_interruptible, IRQ_TIMEOUT_MS);
        (td_void)hal_cipher_symc_wait_func_config(i, TD_TRUE);
    }

    osal_irq_enable(SPACC_REE_IRQ_NUM);

    return ret;
error_spacc_wait_deinit:
    osal_wait_destroy(&g_spacc_wait_queue);
    return ret;
#endif
    return CRYPTO_SUCCESS;
}

static td_void spacc_irq_deinit(td_void)
{
#ifdef CONFIG_SPACC_IRQ_ENABLE
    td_u32 i;
    for (i = 0; i < CRYPTO_HASH_HARD_CHN_CNT; i++) {
        if (((1 << i) & CRYPTO_HASH_HARD_CHN_MASK) == 0) {
            continue;
        }
        (td_void)hal_cipher_hash_register_wait_func(i, &g_spacc_wait_queue, TD_NULL, IRQ_TIMEOUT_MS);
        (td_void)hal_cipher_hash_wait_func_config(i, TD_FALSE);
    }
    for (i = 0; i < CRYPTO_SYMC_HARD_CHANNEL_MAX; i++) {
        if (((1 << i) & CRYPTO_SYMC_HARD_CHANNEL_MASK) == 0) {
            continue;
        }
        (td_void)hal_cipher_symc_register_wait_func(i, &g_spacc_wait_queue, TD_NULL, IRQ_TIMEOUT_MS);
        (td_void)hal_cipher_symc_wait_func_config(i, TD_FALSE);
    }
    osal_irq_disable(SPACC_REE_IRQ_NUM);
    osal_irq_free(SPACC_REE_IRQ_NUM, TD_NULL);
    osal_wait_destroy(&g_spacc_wait_queue);
#endif
}

#ifdef CONFIG_PKE_IRQ_ENABLE
static int pke_irq_handler(int a __attribute__((unused)), void *param __attribute__((unused)))
{
    return 0;
}
#endif

td_s32 crypto_irq_init(td_void)
{
    td_s32 ret;
    ret = spacc_irq_init();
    crypto_chk_return(ret != TD_SUCCESS, ret, "spacc_irq_init failed\n");

    return ret;
}

td_void crypto_irq_deinit(td_void)
{
    spacc_irq_deinit();
}