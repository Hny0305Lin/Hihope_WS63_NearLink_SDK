/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal trng. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "hal_cipher_trng.h"
#include "hal_trng_reg.h"

#include "crypto_drv_common.h"

#define TRNG_OSC_SEL   0x02
#define TRNG_POWER_ON  0x05
#define TRNG_POWER_OFF 0x0a

#define TRNG_COMPAT_ERRNO(err_code)     HAL_COMPAT_ERRNO(ERROR_MODULE_TRNG, err_code)

static td_bool g_hal_trng_init_flag = TD_FALSE;

#if defined(CONFIG_TRNG_RING_ENABLE)
static void inner_trng_ring_enable(td_bool enable)
{
    trng_sec_cfg ring_en = { 0 };

    ring_en.u16 = crypto_reg_read_u16(TRNG_SEC_CFB);
    if (enable == TD_TRUE) {
        if (ring_en.bits.trng_ring_en != TRNG_RING_ENABLE) {
            ring_en.bits.trng_ring_en = TRNG_RING_ENABLE;
            crypto_ex_reg_write_u16(TRNG_SEC_CFB, ring_en.u16);
        }
    } else {
        if (ring_en.bits.trng_ring_en != TRNG_RING_DISABLE) {
            ring_en.bits.trng_ring_en = TRNG_RING_DISABLE;
            crypto_ex_reg_write_u16(TRNG_SEC_CFB, ring_en.u16);
        }
    }
}
#endif /* CONFIG_TRNG_RING_ENABLE */

#define TRNG_RING_ENABLE_WAIT_US 200
td_s32 hal_cipher_trng_init(td_void)
{
#if defined(CONFIG_ENTROPY_SOURCES_FROM_FRO)
    /* Select the entropy source and change the default configuration of the register.
     This parameter is required only for cfbb. */
    /* Step 1: Configure the output clock of the Fro ring in the TRN. */
    hisc_com_trng_fro_sample_clk_sel clk_sel = {0};
    hisc_com_trng_fro_div_cnt div_cnt = {0};
    clk_sel.u32 = trng_reg_read(HISC_COM_TRNG_FRO_SAMPLE_CLK_SEL);
    /* If it is not configured, configure it. */
    if (clk_sel.bits.fro_sample_clk_sel != TRNG_INNER_FRO_CLK) {
        /* Configure it to use the output clock of the TRNG's internal FRO ring */
        clk_sel.bits.fro_sample_clk_sel = TRNG_INNER_FRO_CLK;
        trng_reg_write(HISC_COM_TRNG_FRO_SAMPLE_CLK_SEL, clk_sel.u32);
        /* Divide the frequency of the output clock of the FRO ring by 54. */
        div_cnt.u32 = trng_reg_read(HISC_COM_TRNG_FRO_DIV_CNT);
        div_cnt.bits.fro_div_cnt = TRNG_INNER_FRO_DIV_CNT;
        trng_reg_write(HISC_COM_TRNG_FRO_DIV_CNT, div_cnt.u32);
    }
#endif /* CONFIG_ENTROPY_SOURCES_FROM_FRO */
#if defined(CONFIG_TRNG_RING_ENABLE)
    inner_trng_ring_enable(TD_TRUE);
    crypto_udelay(TRNG_RING_ENABLE_WAIT_US);
#endif /* CONFIG_TRNG_RING_ENABLE */
    g_hal_trng_init_flag = TD_TRUE;
    return TD_SUCCESS;
}

td_s32 hal_cipher_trng_deinit(void)
{
#if defined(CONFIG_TRNG_RING_ENABLE)
    inner_trng_ring_enable(TD_FALSE);
#endif /* CONFIG_TRNG_RING_ENABLE */
    g_hal_trng_init_flag = TD_FALSE;
    return TD_SUCCESS;
}

#define CRYPTO_TRNG_TIMEOUT_IN_US       1000000   /* 10ms. */
static td_bool is_trng_ready(void)
{
    hisc_com_trng_fifo_ready trng_ready = {0};

    trng_ready.u32 = 0;  /* trng not done, and not ready. */
    trng_ready.u32 = trng_reg_read(HISC_COM_TRNG_FIFO_READY);

    if ((trng_ready.bits).trng_done != TRNG_DONE ||
        (trng_ready.bits).trng_data_ready != TRNG_DATA_READY) {
            return TD_FALSE;
    }
    return TD_TRUE;
}

td_s32 hal_cipher_trng_get_random(td_u32 *randnum)
{
    td_u32 times = 0;
    td_u32 chk_randnum = 0;

    crypto_chk_return(g_hal_trng_init_flag == TD_FALSE, TRNG_COMPAT_ERRNO(ERROR_NOT_INIT), "call init first!\n");
    crypto_chk_return(randnum == TD_NULL, TRNG_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), "randnum is NULL\n");

    while (times < CRYPTO_TRNG_TIMEOUT_IN_US) {
        times++;
        if (is_trng_ready() == TD_FALSE) {
            continue;
        }

        if (is_trng_ready() == TD_TRUE) {
            *randnum = trng_reg_read(HISC_COM_TRNG_FIFO_DATA);
            chk_randnum = trng_reg_read(HISC_COM_TRNG_FIFO_DATA);
            if ((*randnum != 0x00000000) && (*randnum != 0xffffffff) && (*randnum != chk_randnum)) {
                return TD_SUCCESS;
            }
        }
    }

    return TRNG_COMPAT_ERRNO(ERROR_GET_TRNG_TIMEOUT);
}