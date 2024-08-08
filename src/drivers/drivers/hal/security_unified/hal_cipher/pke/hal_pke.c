/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: PKE driver HAL API and logical algorithm compelement.
 *
 * Create: 2022-08-09
*/

#include "hal_pke.h"
#include "hal_pke_reg.h"
#include "crypto_errno.h"
#include "crypto_osal_adapt.h"
#include "crypto_drv_common.h"
#include "crypto_common_macro.h"

#include "hal_cipher_trng.h"

/************************************************** global define start************************************/
#define PKE_COMPAT_ERRNO(err_code)      HAL_COMPAT_ERRNO(ERROR_MODULE_PKE, err_code)
#if (defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 2 || CRYPTO_LOG_LEVEL == 3 || CRYPTO_LOG_LEVEL == 4))
#define crypto_dump_buffer(buffer_name, buffer, len) crypto_dump_data(buffer_name, buffer, len)
#else
#define crypto_dump_buffer(fmt, args...)
#endif

typedef struct {
    crypto_wait_t *wait;
    crypto_wait_timeout_interruptible wait_func;
    td_u32 timeout_ms;
    td_bool wait_done;
} pke_wait_event_type;

td_u32 g_pke_initialize;

pke_wait_event_type g_pke_wait_event = {
    .wait = TD_NULL,
    .wait_func = TD_NULL,
    .timeout_ms = 0,
    .wait_done = TD_FALSE
};

/* define error code information */
static const char *g_pke_err_string[PKE_FAIL_FLAG_CNT] = {
    "The calculated data bit width is invalid, the configured value is 0",
    "Start SINGLE_INSTR_START_0 process but not configure instr0",
    "Instr0 configuration is incorrect",
    "Start SINGLE_INSTR_START_1 process but not configure instr1",
    "Instr1 configuration is incorrect",
    "Start BATCH_INSTR_START process but not configure instr addr or length",
    "Instr length is incorectly configured, which is less than 4 or a non_inte     ger multiple of 4",
    "Register PKE_START configuration is incorrect",
    "The instr operation code of BATCH_INSTR_START is abnormal",
    "The mod number of mod_mul is abnormal",
    "AHB Master bus is abnormal"
};

/************************************************** global define end************************************/

/************************************************** hal inner API start************************************/
/* function define */
td_s32 pke_lock_condition(const td_void *param __attribute__((unused)));
td_void hal_pke_calc_int_enable(void);
td_void hal_pke_calc_int_disable(void);
td_s32 hal_pke_check_robust_warn(void);
td_s32 hal_pke_error_code(void);
td_s32 check_instr_rdy(pke_mode mode);
void hal_pke_start_pre_process(void);
/**
 * @brief start PKE calculate in single instruction0 mode
 */
void hal_pke_start0(void);

/**
 * @brief start PKE calculate in sigle instruction1 mode
 */
void hal_pke_start1(void);

/**
 * @brief start PKE calculate in batch processing mode
 */
void hal_pke_batch_start(void);

/**
 * @brief PKE wait for free in loop
 *
 * @return td_s32
 */
td_s32 hal_pke_wait_free(void);

td_s32 wait_free_condition(const td_void *param __attribute__((unused)));

/**
 * @brief PKE wait for free by interrupt
 *
 * @return td_s32
 */
td_s32 hal_pke_wait_free_int(void);


/* function realize */
td_s32 pke_lock_condition(const td_void *param __attribute__((unused)))
{
    pke_lock_status lock_status = {.u32 = PKE_NON_SPECIAL_VAL};
    lock_status.u32 = pke_reg_read(PKE_LOCK_STATUS);
    if (lock_status.bits.pke_lock_stat == (uintptr_t)param) {
        g_pke_wait_event.wait_done = TD_TRUE;
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_void hal_pke_calc_int_enable(void)
{
    pke_int_enable int_en = {.u32 = PKE_NON_SPECIAL_VAL};
    int_en.u32 = pke_reg_read(PKE_INT_ENABLE);
    if (int_en.bits.finish_int_enable == 1) {
        return;
    }

    int_en.bits.finish_int_enable = 1;
    pke_reg_write(PKE_INT_ENABLE, int_en.u32);

    /* call back for secure enhancement */
    int_en.u32 = pke_reg_read(PKE_INT_ENABLE);
    val_enhance_chk(int_en.bits.finish_int_enable, 1);

    return;
}

td_void hal_pke_calc_int_disable(void)
{
    pke_int_enable int_en = {.u32 = PKE_NON_SPECIAL_VAL};
    int_en.u32 = pke_reg_read(PKE_INT_ENABLE);
    if (int_en.bits.finish_int_enable == 0) {
        return;
    }

    int_en.bits.finish_int_enable = 0;
    pke_reg_write(PKE_INT_ENABLE, int_en.u32);

    /* call back for secure enhancement */
    int_en.u32 = pke_reg_read(PKE_INT_ENABLE);
    val_enhance_chk(int_en.bits.finish_int_enable, 0);

    return;
}

td_s32 hal_pke_check_robust_warn(void)
{
    pke_alarm_status result = {.u32 = PKE_NON_SPECIAL_VAL};

    result.u32 = pke_reg_read(PKE_ALARM_STATUS);
    if (result.bits.alarm_int == PKE_ALARM_STATUS_EFFECTIVE_CODE) {
        result.u32 = PKE_ALARM_STATUS_CLEAN_CODE; // clear warn interrupt
        pke_reg_write(PKE_ALARM_STATUS, result.u32);
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 hal_pke_error_code(void)
{
    pke_failure_flag result = {.u32 = PKE_NON_SPECIAL_VAL};
    td_u32 i;
    crypto_unused(g_pke_err_string);

    result.u32 = pke_reg_read(PKE_FAILURE_FLAG);
    if (result.u32 == 0) {
        return TD_SUCCESS;
    }

    for (i = 0; i < PKE_FAIL_FLAG_CNT; i++) {
        if ((result.bits.pke_failure_flag & (1 << i)) != 0) {
            crypto_log_err("PKE opeartion failed: %s\n", g_pke_err_string[i]);
        }
    }
    crypto_log_err("Hardware Error Code: 0x%x\n", result.u32);
    return PKE_COMPAT_ERRNO(ERROR_PKE_LOGIC);
}

td_s32 check_instr_rdy(pke_mode mode)
{
    pke_instr_rdy instr_rdy;
    td_u32 mode_rdy_location;
    td_u32 i;

    switch (mode) {
        case PKE_SINGLE_INSTR0: {
            mode_rdy_location = 0x1; /* single_instr0: 0x1 */
            break;
        }
        case PKE_SINGLE_INSTR1: {
            mode_rdy_location = 0x2; /* single_instr0: 0x2 */
            break;
        }
        case PKE_BATCH_INSTR: {
            mode_rdy_location = 0x4; /* batch_instr: 0x4 */
            break;
        }
        default:
            crypto_log_err("error, pke_instr_start_mode invaild!\n");
            return TD_FAILURE;
    }

    /* wait ready */
    for (i = 0; i < PKE_TIME_OUT_US; i++) {
        instr_rdy.u32  = pke_reg_read(PKE_INSTR_RDY);
        if ((instr_rdy.u32 & mode_rdy_location) == mode_rdy_location) {
            break;
        }
        crypto_udelay(1); /* 1us */
    }

    if (i >= PKE_TIME_OUT_US) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_WAIT_DONE_TIMEOUT);
    }

    return TD_SUCCESS;
}

void hal_pke_start_pre_process(void)
{
    /* 1.clear interrupt */
    pke_int_no_mask_status result = {.u32 = PKE_NON_SPECIAL_VAL};
    result.u32 = pke_reg_read(PKE_INT_NOMASK_STATUS);
    result.bits.finish_int_nomask = 1;
    pke_reg_write(PKE_INT_NOMASK_STATUS, result.u32);

    /* call back for secure enhancement */
    result.u32 = pke_reg_read(PKE_INT_NOMASK_STATUS);
    val_enhance_chk(result.bits.finish_int_nomask, 1);

    /* 2.set calculate interrupt enable if the wait func is registed */
    if (g_pke_wait_event.wait_func != TD_NULL) {
        hal_pke_calc_int_enable();
    }
}

void hal_pke_start0(void)
{
    hal_pke_start_pre_process();

    pke_start start = {.u32 = PKE_NON_SPECIAL_VAL};
    start.u32 = PKE_START0_CODE;
    pke_reg_write(PKE_START, start.u32);

    /* call back for secure enhancement */
    reg_callback_chk(PKE_START, PKE_START0_CODE);

    return;
}

void hal_pke_start1(void)
{
    hal_pke_start_pre_process();

    pke_start start = {.u32 = PKE_NON_SPECIAL_VAL};
    start.u32 = PKE_START1_CODE;
    pke_reg_write(PKE_START, start.u32);

    /* call back for secure enhancement */
    reg_callback_chk(PKE_START, PKE_START1_CODE);

    return;
}

void hal_pke_batch_start(void)
{
    hal_pke_start_pre_process();

    pke_start start = {.u32 = PKE_NON_SPECIAL_VAL};
    start.u32 = PKE_BATCH_START_CODE;
    pke_reg_write(PKE_START, start.u32);

    /* call back for secure enhancement */
    reg_callback_chk(PKE_START, PKE_BATCH_START_CODE);

    return;
}

td_s32 hal_pke_wait_free(void)
{
    td_u32 i = 0;
    td_s32 ret = TD_FAILURE;
    pke_busy busy = {.u32 = PKE_NON_SPECIAL_VAL};

    /* wait ready */
    for (i = 0; i < PKE_TIME_OUT_US; i++) {
        busy.u32 = pke_reg_read(PKE_BUSY);
        if (busy.bits.pke_busy == 0) {
            break;
        }
        crypto_udelay(1); /* 1us */
    }

    /* get interrupt status */
    pke_int_no_mask_status int_status = {.u32 = PKE_NON_SPECIAL_VAL};
    int_status.u32 = pke_reg_read(PKE_INT_NOMASK_STATUS);

    if (i < PKE_TIME_OUT_US && int_status.bits.finish_int_nomask == PKE_INT_NOMASK_FINISH_EFFECTIVE_CODE) {
        /* clear interrupt */
        int_status.bits.finish_int_nomask = 1;
        pke_reg_write(PKE_INT_NOMASK_STATUS, int_status.u32);
        ret = TD_SUCCESS;
    } else {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_WAIT_DONE_TIMEOUT);
    }
    crypto_chk_print((ret != TD_SUCCESS), "error, pke wait free timeout\n");

    return ret;
}

td_s32 wait_free_condition(const td_void *param __attribute__((unused)))
{
    pke_busy busy = {.u32 = PKE_NON_SPECIAL_VAL};
    busy.u32 = pke_reg_read(PKE_BUSY);
    if (busy.bits.pke_busy == 0) {
        g_pke_wait_event.wait_done = TD_TRUE;
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_s32 hal_pke_wait_free_int(void)
{
    g_pke_wait_event.wait_done = TD_FALSE;
    /* wait ready */
    td_s32 ret = g_pke_wait_event.wait_func(g_pke_wait_event.wait, wait_free_condition, TD_NULL,
        g_pke_wait_event.timeout_ms);
    if (ret > 0) {
        return TD_SUCCESS;
    }
    return PKE_COMPAT_ERRNO(ERROR_PKE_WAIT_DONE_TIMEOUT);
}

/************************************************** hal inner API end************************************/

/************************************************** hal outter API start************************************/

td_s32 hal_pke_init(void)
{
    if (g_pke_initialize == TD_TRUE) {
        return TD_SUCCESS;
    }

    g_pke_initialize = TD_TRUE;
    return TD_SUCCESS;
}

td_s32 hal_pke_deinit(void)
{
    if (g_pke_initialize == TD_TRUE) {
        g_pke_initialize = TD_FALSE;
        return TD_SUCCESS;
    }

    return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
}

td_s32 get_lock_code(td_u32 *lock_code)
{
    crypto_cpu_type cpu_type = crypto_get_cpu_type();
    switch (cpu_type) {
        case CRYPTO_CPU_TYPE_SCPU:
            *lock_code = CPU_ID_SCPU;
            break;
        case CRYPTO_CPU_TYPE_ACPU:
            *lock_code = CPU_ID_ACPU;
            break;
        case CRYPTO_CPU_TYPE_PCPU:
            *lock_code = CPU_ID_PCPU;
            break;
        case CRYPTO_CPU_TYPE_AIDSP:
            *lock_code = CPU_ID_AIDSP;
            break;
        default:
            *lock_code = CPU_ID_ACPU;
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 hal_pke_lock(void)
{
    td_u32 i = 0;
    td_s32 ret = TD_FAILURE;
    pke_lock_ctrl lock_ctrl = {.u32 = PKE_NON_SPECIAL_VAL};
    pke_lock_status lock_status = {.u32 = PKE_NON_SPECIAL_VAL};
    td_u32 lock_code = CPU_ID_SCPU;

    ret = get_lock_code(&lock_code);
    crypto_chk_func_return(get_lock_code, ret);

    /* lock pke */
    lock_ctrl.u32 = pke_reg_read(PKE_LOCK_CTRL);
    lock_ctrl.bits.pke_lock_type = 0; /* lock command */
    lock_ctrl.bits.pke_lock = 1;
    pke_reg_write(PKE_LOCK_CTRL, lock_ctrl.u32);

    /* call back for secure enhancement */
    lock_ctrl.u32 = pke_reg_read(PKE_LOCK_CTRL);
    val_enhance_chk(lock_ctrl.bits.pke_lock_type, 0);

    if (g_pke_wait_event.wait_func != TD_NULL) {
        g_pke_wait_event.wait_done = TD_FALSE;
        ret = g_pke_wait_event.wait_func(g_pke_wait_event.wait, pke_lock_condition, (td_void *)(uintptr_t)lock_code,
            g_pke_wait_event.timeout_ms);
        if (ret > 0) {
            return TD_SUCCESS;
        } else {
            return PKE_COMPAT_ERRNO(ERROR_PKE_LOCK_TIMEOUT);
        }
    } else {
        for (i = 0; i < PKE_TIME_OUT_US; i++) {
            /* check lock result */
            lock_status.u32 = pke_reg_read(PKE_LOCK_STATUS);
            if (lock_status.bits.pke_lock_stat == lock_code) {
                break;
            }
            crypto_udelay(1); // 1 us is empirical value of register lock read
        }
        if (i < PKE_TIME_OUT_US) {
            ret = TD_SUCCESS;
        } else {
            ret = PKE_COMPAT_ERRNO(ERROR_PKE_LOCK_TIMEOUT);
        }
    }
    crypto_chk_print((ret != TD_SUCCESS), "pke lock timeout\n");
    return ret;
}

void hal_pke_unlock(void)
{
    pke_lock_ctrl lock_ctrl = {.u32 = PKE_NON_SPECIAL_VAL};

    /* unlock pke */
    lock_ctrl.u32 = pke_reg_read(PKE_LOCK_CTRL);
    lock_ctrl.bits.pke_lock_type = 1; /* unlock command */
    lock_ctrl.bits.pke_lock = 1;
    pke_reg_write(PKE_LOCK_CTRL, lock_ctrl.u32);
}

void hal_pke_enable_noise(void)
{
    pke_noise_en noise = {
        .u32 = PKE_NON_SPECIAL_VAL
    };

    /* enable noise */
    noise.u32 = pke_reg_read(PKE_NOISE_EN);
    noise.bits.noise_en = 1;
    pke_reg_write(PKE_NOISE_EN, noise.u32);

    /* call back for secure enhancement */
    noise.u32 = pke_reg_read(PKE_NOISE_EN);
    val_enhance_chk(noise.bits.noise_en, 1);
}

void hal_pke_disable_noise(void)
{
    pke_noise_en noise = {.u32 = PKE_NON_SPECIAL_VAL};

    /* disable noise */
    noise.u32 = pke_reg_read(PKE_NOISE_EN);
    noise.bits.noise_en = 0;
    pke_reg_write(PKE_NOISE_EN, noise.u32);
}

td_s32 hal_pke_pre_process(void)
{
    /* 2. set claculate will use random number for mask */
    pke_mask_rng_cfg mask_rng = {.u32 = PKE_NON_SPECIAL_VAL};
    mask_rng.u32 = pke_reg_read(PKE_MASK_RNG_CFG);
    mask_rng.bits.mask_rng_cfg = 1;
    pke_reg_write(PKE_MASK_RNG_CFG, mask_rng.u32);
    /* call back for secure enhancement */
    reg_callback_chk(PKE_MASK_RNG_CFG, mask_rng.u32);

    /* 3. set mask random number */
    td_u32 random_num = DEFAULT_MASK_CODE;
    random_num = pke_reg_read(PKE_DRAM_MASK);
    crypto_log_dbg("before write random: PKE_DRAM_MASK = 0x%x\r\n", random_num);
    td_s32 ret = hal_cipher_trng_get_random(&random_num);
    crypto_log_dbg("the random number is: 0x%x\r\n", random_num);
    crypto_chk_func_return(hal_cipher_trng_get_random, ret);
    pke_reg_write(PKE_DRAM_MASK, random_num);
    /* call back for secure enhancement */
    reg_callback_chk(PKE_DRAM_MASK, random_num);
    crypto_log_dbg("after write the reg value is: 0x%x\r\n", pke_reg_read(PKE_DRAM_MASK));

    return ret;
}

void hal_pke_set_wait_event(crypto_wait_t *wait, const crypto_wait_timeout_interruptible wait_func,
    const td_u32 timeout_ms, const td_bool wait_done)
{
    g_pke_wait_event.wait = wait;
    g_pke_wait_event.wait_func = wait_func;
    g_pke_wait_event.timeout_ms = timeout_ms;
    g_pke_wait_event.wait_done = wait_done;
}

td_s32 hal_pke_check_free(void)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    pke_busy busy = {.u32 = PKE_NON_SPECIAL_VAL};

    /* wait ready */
    for (i = 0; i < PKE_TIME_OUT_US; i++) {
        busy.u32 = pke_reg_read(PKE_BUSY);
        if (busy.bits.pke_busy == 0) {
            break;
        }
        crypto_udelay(1); /* 1us */
    }

    if (i < PKE_TIME_OUT_US) {
        ret = TD_SUCCESS;
    } else {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_WAIT_DONE_TIMEOUT);
    }
    crypto_chk_print((ret != TD_SUCCESS), "error, pke wait free timeout\n");

    return ret;
}

td_void hal_pke_set_ram(td_u32 ram_section, const td_u8 *data, td_u32 data_len, td_u32 aligned_len CIPHER_CHECK_WORD)
{
    td_u32 i = 0;
    td_u32 j = 0;
    td_u32 val = 0;
    td_u32 random_num = DEFAULT_MASK_CODE;
    crypto_hal_func_enter();
    if (data == TD_NULL) {
        crypto_log_err("\r\n%s:%d data is NULL\n", __func__, __LINE__);
        return;
    }

    /* secure enhancement */
    check_sum_assert(ram_section, data, data_len, aligned_len);
    crypto_dump_buffer("data", data, data_len);

    /* 1. get mask number */
    random_num = pke_reg_read(PKE_DRAM_MASK);
    crypto_log_dbg("random_num: 0x%x\r\n", random_num);

    /* 2. set data into DRAM */
    /* Input the data which is aligned with 8 bytes;
       For little-endian system, on reading one word from ram to val, byte sequence should be adjusted as -
       in ram: Byte1 | Byte2 | Byte3 | Byte4 (section low <--> section high)
       to val: Byte4 | Byte3 | Byte2 | Byte1 (MSB <--> LSB)
    */
    /* not use dynamic micro in rom code, otherwise it won't be able to be changed after chip delivery. */
    td_u32 start_dram_addr = PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;
    for (i = data_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        val = 0;
        val = data[i - 4] << 24; /* i - 4 index shift 24 bits */
        val |= data[i - 3] << 16; /* i - 3 index shift 16 bits */
        val |= data[i - 2] << 8; /* i - 2 index shift 8 bits */
        val |= data[i - 1];
        val ^= random_num;
        pke_reg_write(start_dram_addr, val);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
    /* secure enhancement */
    val_enhance_chk(i < CRYPTO_WORD_WIDTH, TD_TRUE);

    /* Input the data which is not aligned with 4 bytes */
    if (i != 0) {
        val = 0;
        for (j = 0; j < i; j++) {
            val |= data[j] << (CRYPTO_BITS_IN_BYTE * (i - j - 1));
        }
        /* secure enhancement */
        val_enhance_chk(j, i);
        val ^= random_num;
        crypto_log_dbg("write val for not aligned for 4bytes: 0x%x\r\n", val);
        pke_reg_write(start_dram_addr, val);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
    /* secure enhancement */
    td_u32 write_len = (data_len + CRYPTO_WORD_WIDTH - 1) / CRYPTO_WORD_WIDTH * CRYPTO_WORD_WIDTH;
    val_enhance_chk(start_dram_addr, PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH + write_len);

    /* 3. padding with 0x00 for not aligned with 64bits data */
    for (; write_len < aligned_len; write_len += CRYPTO_WORD_WIDTH) {
        val = 0x0;
        val ^= random_num;
        crypto_log_dbg("write val for padding: 0x%x\r\n", val);
        pke_reg_write(start_dram_addr, val);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
    /* secure enhancement */
    val_enhance_chk(start_dram_addr, PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH + aligned_len);

    crypto_hal_func_exit();
    return;
}

td_void hal_pke_get_ram(td_u32 ram_section, td_u8 *data, td_u32 data_len CIPHER_CHECK_WORD)
{
    td_u32 val = 0;
    td_u32 i = 0;
    td_u32 count = 0;
    td_u32 mask_random = DEFAULT_MASK_CODE;
    td_u32 count_len = data_len;
    crypto_hal_func_enter();
    if (data == TD_NULL) {
        crypto_log_err("data is NULL\n");
        return;
    }

    /* secure enhancement */
    check_sum_assert(ram_section, data, data_len);

    /* 1. get the mask random number */
    mask_random = pke_reg_read(PKE_DRAM_MASK);

    td_u32 start_dram_addr = PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;
    if ((count_len % CRYPTO_WORD_WIDTH) != 0) {
        val = pke_reg_read(start_dram_addr + (count_len - (count_len % CRYPTO_WORD_WIDTH)));
        val ^= mask_random;
        for (count = 0; count < (count_len % CRYPTO_WORD_WIDTH); count++) {
            *(data + count) = val >> (CRYPTO_BITS_IN_BYTE * ((count_len % CRYPTO_WORD_WIDTH) - 1 - count)) & 0xFF;
        }
        val_enhance_chk(count, count_len % CRYPTO_WORD_WIDTH);
        count_len = count_len - (count_len % CRYPTO_WORD_WIDTH);
    }
    for (i = count_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        val = pke_reg_read(start_dram_addr + i - CRYPTO_WORD_WIDTH);
        val ^= mask_random;
        *(data + count + 0) = (val >> 24) & 0xFF; /* shift 24 bits */
        *(data + count + 1) = (val >> 16) & 0xFF; /* shift 16 bits */
        *(data + count + 2) = (val >> 8) & 0xFF;  /* offset 2, shift 8 bits */
        *(data + count + 3) = (val) & 0xFF;       /* offset 3 */
        count += CRYPTO_WORD_WIDTH;
    }
    val_enhance_chk(count, count_len);

#ifdef SEC_ENHANCE
    /* secure enhancement */
    count = 0;
    count_len = data_len;
    if ((count_len % CRYPTO_WORD_WIDTH) != 0) {
        val = 0;
        for (count = 0; count < (count_len % CRYPTO_WORD_WIDTH); count++) {
            val |= *(data + count) << (CRYPTO_BITS_IN_BYTE * (CRYPTO_WORD_WIDTH - 1 - count));
        }
        val ^= mask_random;
        val_enhance_chk(val, pke_reg_read(start_dram_addr + (count_len - (count_len % CRYPTO_WORD_WIDTH))));
        count_len = count_len - (count_len % CRYPTO_WORD_WIDTH);
    }

    for (i = count_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        val = (*(data + count + 0) << 24) + // 24 bits left-shift according to algorithmn
            (*(data + count + 1) << 16) + // 16 bits left-shift according to algorithmn
            (*(data + count + 2) << CRYPTO_BITS_IN_BYTE) + // 2 is byte index: 0, 1, 2, 3
            *(data + count + 3); // 3 is byte index: 0, 1, 2, 3
        val ^= mask_random;
        val_enhance_chk(pke_reg_read(start_dram_addr + i - CRYPTO_WORD_WIDTH), val);
        count += CRYPTO_WORD_WIDTH;
    }
    val_enhance_chk(count, count_len);
#endif

    crypto_dump_buffer("data", data, data_len);
    crypto_hal_func_exit();
    return;
}

td_s32 hal_pke_clean_ram(void)
{
    td_s32 ret = TD_FAILURE;
    crypto_hal_func_enter();

    /* 1. wait for PKE free, no need to wait. */
    /* Fotr that, if wait_done success, here will also success, if wait_done failed, here will also failed, but will
    cost a long time to wait failure result.  */

    pke_dram_clr dram_clr = {.u32 = PKE_NON_SPECIAL_VAL};
    dram_clr.u32 = pke_reg_read(PKE_DRAM_CLR);
    dram_clr.bits.dram_clr = 0x1;
    pke_reg_write(PKE_DRAM_CLR, dram_clr.u32);

    /* 2. wait for PKE free */
    ret = hal_pke_check_free();
    crypto_hal_func_exit();
    return ret;
}

td_s32 hal_pke_set_mode(pke_mode mode, td_u32 single_instr, const rom_lib *batch_instr,
    pke_data_work_len work_len CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 batch_instr_addr = 0;
    td_u32 batch_instr_num = 0;
    crypto_hal_func_enter();
    /* secure enhancement */
    check_sum_assert(mode, single_instr, batch_instr, work_len);

    hal_crypto_pke_check_param((mode == PKE_SINGLE_INSTR0 || mode == PKE_SINGLE_INSTR1) && single_instr == 0);
    hal_crypto_pke_check_param(mode == PKE_BATCH_INSTR && batch_instr == TD_NULL);

    if (batch_instr != TD_NULL) {
        /* avoid dynamic MICRO to be used in rom code. */
        batch_instr_addr = batch_instr->instr_addr;
        batch_instr_num = batch_instr->instr_num * CRYPTO_WORD_WIDTH;

        crypto_log_dbg("batch_instr_addr = %d\r\n", (batch_instr_addr - PKE_ROM_LIB_START_ADDR) / CRYPTO_WORD_WIDTH);
        crypto_log_dbg("batch_instr_len = %d\r\n", batch_instr->instr_num);
        crypto_log_dbg("batch_instr_0 = 0x%x\r\n", crypto_reg_read(batch_instr_addr));
    }

    /* 1. set work_len, work_len = celing(width/64bit) */
    pke_reg_write(PKE_WORK_LEN, work_len);
    /* call back for secure enhancement */
    reg_callback_chk(PKE_WORK_LEN, work_len);

    /* 2. check pke instr ready */
    ret = check_instr_rdy(mode);
    crypto_chk_func_return(check_instr_rdy, ret);

    /* 3. set calculate instruction */
    if (mode == PKE_SINGLE_INSTR0) {
        pke_reg_write(PKE_INSTR0, single_instr);
        /* call back for secure enhancement */
        reg_callback_chk(PKE_INSTR0, single_instr);
    } else if (mode == PKE_SINGLE_INSTR1) {
        pke_reg_write(PKE_INSTR1, single_instr);
        /* call back for secure enhancement */
        reg_callback_chk(PKE_INSTR1, single_instr);
    } else if (mode == PKE_BATCH_INSTR) {
        pke_reg_write(PKE_INSTR_ADDR_LOW, batch_instr_addr);
        /* call back for secure enhancement */
        reg_callback_chk(PKE_INSTR_ADDR_LOW, batch_instr_addr);
        /* WARN: need to set. */
        pke_reg_write(PKE_INSTR_ADDR_HIG, 0);
        /* call back for secure enhancement */
        reg_callback_chk(PKE_INSTR_ADDR_HIG, 0);
        /* here the PKE_INSTR_LEN should be set in byte, and should be aligned to 4 bytes. And eatch instr
        will occupy 4 bytes. */
        pke_reg_write(PKE_INSTR_LEN, batch_instr_num);
        /* call back for secure enhancement */
        reg_callback_chk(PKE_INSTR_LEN, batch_instr_num);
    }

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_pke_start(pke_mode mode CIPHER_CHECK_WORD)
{
    crypto_hal_func_enter();
    /* secure enhancement */
    check_sum_assert(mode);

    switch (mode) {
        case PKE_SINGLE_INSTR0: {
            hal_pke_start0();
            break;
        }
        case PKE_SINGLE_INSTR1: {
            hal_pke_start1();
            break;
        }
        case PKE_BATCH_INSTR: {
            hal_pke_batch_start();
            break;
        }
        default:
            crypto_log_err("error, pke_instr_start_mode invaild!\n");
            return TD_FAILURE;
    }
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_pke_wait_done(void)
{
    td_s32 ret = TD_FAILURE;
    crypto_hal_func_enter();

    /* wait ready */
    if (g_pke_wait_event.wait_func != TD_NULL) {
        ret = hal_pke_wait_free_int();
    } else {
        ret = hal_pke_wait_free();
    }
    if (ret != TD_SUCCESS) {
        return ret;
    }

    hal_pke_calc_int_disable();

    if (hal_pke_check_robust_warn() == TD_SUCCESS) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_ROBUST_WARNING);
    }

    ret = hal_pke_error_code();
    crypto_chk_func_return(hal_pke_error_code, ret);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_pke_get_align_val(const td_u32 len, td_u32 *aligned_len)
{
    crypto_hal_func_enter();
    hal_crypto_pke_check_param(aligned_len == TD_NULL);

    if (len <= DRV_PKE_LEN_192) {
        *aligned_len = DRV_PKE_LEN_192;
    } else if (len <= DRV_PKE_LEN_256) {
        *aligned_len = DRV_PKE_LEN_256;
    } else if (len <= DRV_PKE_LEN_384) {
        *aligned_len = DRV_PKE_LEN_384;
    } else if (len <= DRV_PKE_LEN_512) {
        *aligned_len = DRV_PKE_LEN_512;
    } else if (len <= DRV_PKE_LEN_576) {
        *aligned_len = DRV_PKE_LEN_576;
    } else if (len <= DRV_PKE_LEN_1024) {
        *aligned_len = DRV_PKE_LEN_1024;
    } else if (len <= DRV_PKE_LEN_1536) {
        *aligned_len = DRV_PKE_LEN_1536;
    } else if (len <= DRV_PKE_LEN_2048) {
        *aligned_len = DRV_PKE_LEN_2048;
    } else if (len <= DRV_PKE_LEN_3072) {
        *aligned_len = DRV_PKE_LEN_3072;
    } else if (len <= DRV_PKE_LEN_4096) {
        *aligned_len = DRV_PKE_LEN_4096;
    } else {
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_pke_set_mont_para(td_u32 low_bit, td_u32 high_bit CIPHER_CHECK_WORD)
{
    crypto_hal_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), low_bit, high_bit);

    /* set low 32bit data */
    pke_reg_write(PKE_MONT_PARA0, low_bit);
    /* call back for secure enhancement */
    reg_callback_chk(PKE_MONT_PARA0, low_bit);

    /* set high 32bit data */
    pke_reg_write(PKE_MONT_PARA1, high_bit);
    /* call back for secure enhancement */
    reg_callback_chk(PKE_MONT_PARA1, high_bit);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

#ifdef CONFIG_PKE_SUPPORT_CURVE
td_s32 hal_pke_set_curve_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    td_s32 ret = TD_FAILURE;
    td_u32 aligned_len = ecc_curve->ksize;
    crypto_hal_func_enter();
    hal_crypto_pke_check_param(ecc_curve->ecc_type != DRV_PKE_ECC_TYPE_RFC7748 &&
        ecc_curve->ecc_type != DRV_PKE_ECC_TYPE_RFC7748_448);

    if (ecc_curve->ksize == DRV_PKE_LEN_448) {
        aligned_len = DRV_PKE_LEN_448;
    } else {
        ret = hal_pke_get_align_val(ecc_curve->ksize, &aligned_len);
        crypto_chk_return((ret != TD_SUCCESS), ret, "hal_pke_get_align_val failed!\n");
    }
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_n, ecc_curve->n, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_p, ecc_curve->p, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_rrn, init_param->rrn, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_rrp, init_param->rrp, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_const_0, init_param->const_0, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_const_1, init_param->const_1, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_mont_a24, init_param->mont_a, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_mont_1_p, init_param->mont_1_p, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(curve_addr_mont_1_n, init_param->mont_1_n, ecc_curve->ksize, aligned_len));

    crypto_hal_func_exit();
    return TD_SUCCESS;
}
#endif

#ifdef CONFIG_PKE_SUPPORT_EDWARD
td_s32 hal_pke_set_ed_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    td_s32 ret = TD_FAILURE;
    td_u32 aligned_len = ecc_curve->ksize;
    hal_crypto_pke_check_param(ecc_curve->ecc_type != DRV_PKE_ECC_TYPE_RFC8032);

    ret = hal_pke_get_align_val(ecc_curve->ksize, &aligned_len);
    crypto_chk_return((ret != TD_SUCCESS), ret, "hal_pke_get_align_val failed!\n");
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_n, ecc_curve->n, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_p, ecc_curve->p, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_rrn, init_param->rrn, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_rrp, init_param->rrp, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_const_0, init_param->const_0, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_const_1, init_param->const_1, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_mont_d, init_param->mont_a, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_sqrt_m1, init_param->mont_b, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_mont_1_p, init_param->mont_1_p, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ed_addr_mont_1_n, init_param->mont_1_n, ecc_curve->ksize, aligned_len));
    return TD_SUCCESS;
}
#endif

td_s32 hal_pke_set_ecc_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    td_s32 ret = TD_FAILURE;
    td_u32 aligned_len = ecc_curve->ksize;
    crypto_hal_func_enter();
    hal_crypto_pke_check_param(ecc_curve->ecc_type >= DRV_PKE_ECC_TYPE_MAX ||
        ecc_curve->ecc_type < DRV_PKE_ECC_TYPE_RFC5639_P256 || ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_RFC7748 ||
        ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_RFC7748_448 || ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_RFC8032);

    ret = hal_pke_get_align_val(ecc_curve->ksize, &aligned_len);
    crypto_chk_return((ret != TD_SUCCESS), ret, "hal_pke_get_align_val failed!\n");
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_n, ecc_curve->n, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_p, ecc_curve->p, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_rrn, init_param->rrn, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_rrp, init_param->rrp, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_const_0, init_param->const_0, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_const_1, init_param->const_1, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_mont_a, init_param->mont_a, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_mont_b, init_param->mont_b, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_mont_1_p, init_param->mont_1_p, ecc_curve->ksize, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_mont_1_n, init_param->mont_1_n, ecc_curve->ksize, aligned_len));
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_pke_set_init_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    td_s32 ret = TD_FAILURE;
    crypto_hal_func_enter();

    switch (ecc_curve->ecc_type) {
#ifdef CONFIG_PKE_SUPPORT_CURVE
        case DRV_PKE_ECC_TYPE_RFC7748:
        case DRV_PKE_ECC_TYPE_RFC7748_448:
            ret = hal_pke_set_curve_param(init_param, ecc_curve);
            break;
#endif
#ifdef CONFIG_PKE_SUPPORT_EDWARD
        case DRV_PKE_ECC_TYPE_RFC8032:
            ret = hal_pke_set_ed_param(init_param, ecc_curve);
            break;
#endif
        case DRV_PKE_ECC_TYPE_RFC5639_P256:
        case DRV_PKE_ECC_TYPE_RFC5639_P384:
        case DRV_PKE_ECC_TYPE_RFC5639_P512:
        case DRV_PKE_ECC_TYPE_FIPS_P192R:
        case DRV_PKE_ECC_TYPE_FIPS_P224R:
        case DRV_PKE_ECC_TYPE_FIPS_P256R:
        case DRV_PKE_ECC_TYPE_FIPS_P384R:
        case DRV_PKE_ECC_TYPE_FIPS_P521R:
        case DRV_PKE_ECC_TYPE_SM2:
            ret = hal_pke_set_ecc_param(init_param, ecc_curve);
            break;
        default:
            ret = PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
            break;
    }

    crypto_hal_func_exit();
    return ret;
}