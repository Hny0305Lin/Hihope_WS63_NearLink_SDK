/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: RSA algorithm emplement.
 *
 * Create: 2022-08-18
*/

#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "crypto_osal_adapt.h"
#include "crypto_drv_common.h"
#include "drv_common_pke.h"
#include "rsa.h"

#define pke_data_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, PKE_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), "%s is NULL\n", #ptr)
#define DRV_PKE_CAL_LEN_1536    192
#define DRV_PKE_CAL_LEN_320     40
#define MAX_OFFSET_OF_VALID_BIT 2
#define LOOP_NUMBER_4096    8
#define LOOP_NUMBER_3072    10
#define LOOP_NUMBER_2048    7
#define LOOP_NUMBER_1536    9
#define LOOP_NUMBER_1024    6
#define LOOP_NUMBER_192     6
#define LOOP_NUMBER_256     4
#define LOOP_NUMBER_320     6
#define LOOP_NUMBER_384     7
#define LOOP_NUMBER_512     5
#define MAX_RRN_SUPPORT_TYPE    10

typedef enum {
    RSA_MOD_ADD,
    RSA_MOD_SUB,
    RSA_MOD_MUL
} rsa_mod_mode;

typedef struct {
    td_u32 aligned_len;
    const rom_lib *batch_instr;
    td_u32 loop_number;
} rrn_loop_table;

const rrn_loop_table g_rrn_loop_table[MAX_RRN_SUPPORT_TYPE] = {
    {DRV_PKE_LEN_3072, &instr_rsa_rrn_add_3072, LOOP_NUMBER_3072},
    {DRV_PKE_CAL_LEN_1536, &instr_rsa_rrn_add_3072, LOOP_NUMBER_1536},
    {DRV_PKE_LEN_192, &instr_rsa_rrn_add_3072, LOOP_NUMBER_192},
    {DRV_PKE_LEN_256, &instr_rsa_rrn_add, LOOP_NUMBER_256},
    {DRV_PKE_CAL_LEN_320, &instr_rsa_rrn_add_320, LOOP_NUMBER_320},
    {DRV_PKE_LEN_384, &instr_rsa_rrn_add_3072, LOOP_NUMBER_384},
    {DRV_PKE_LEN_512, &instr_rsa_rrn_add, LOOP_NUMBER_512},
    {DRV_PKE_LEN_1024, &instr_rsa_rrn_add, LOOP_NUMBER_1024},
    {DRV_PKE_LEN_2048, &instr_rsa_rrn_add, LOOP_NUMBER_2048},
    {DRV_PKE_LEN_4096, &instr_rsa_rrn_add, LOOP_NUMBER_4096},
};

/*
 * rsa inner API start
*/
static void get_valid_bit(const td_u8 *n, const td_u32 key_size, td_u32 *valid_bit_index)
{
    for (*valid_bit_index = 0; *valid_bit_index < key_size; (*valid_bit_index)++) {
        if (n[*valid_bit_index] != 0x00) {
            break;
        }
    }
    return;
}

static td_s32 get_loop_number(td_u32 aligned_len, td_u32 *loop_number)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u32 work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* Step3: prepare data for loop calculate. */
    for (; i < MAX_RRN_SUPPORT_TYPE; i++) {
        if (aligned_len == g_rrn_loop_table[i].aligned_len) {
            ret = crypto_drv_pke_common_batch_instr_process(g_rrn_loop_table[i].batch_instr, work_len);
            crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
            *loop_number = g_rrn_loop_table[i].loop_number;
            return TD_SUCCESS;
        }
    }
    crypto_log_err("invalid modulur\n");
    return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
}
/*
 * rsa inner API end
*/

td_s32 rsa_rrn(const td_u8 *n, const td_u32 key_size, const td_u32 aligned_len, td_u8 *rrn)
{
    td_s32 ret = TD_FAILURE;
    td_u32 cnt = 0;
    td_u32 i = 0;
    /* WARN: the aligned_len maybe larger than key_size. */
    td_u32 work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    td_u8 bit_2[DRV_PKE_LEN_4096];

    /* Step 1: get data initial data rr = 2^(bit_len - 1). */
    memset_enhance_chk_return(ret, bit_2, DRV_PKE_LEN_4096, 0x00, DRV_PKE_LEN_4096);
    bit_2[0] = HALF_BYTE_VALUE;

    /* Step 2: set data to PKE DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_n, n, key_size, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_rr, bit_2, aligned_len, aligned_len));
    crypto_dump_buffer("2^(bit_len - 1)", bit_2, aligned_len);

    /* Step3: prepare data for loop calculate. */
    ret = get_loop_number(aligned_len, &cnt);
    crypto_chk_func_return(get_loop_number, ret);

    /* Step 4.2: start calculate square multiplication modulur. */
    for (i = 0; i < cnt; i++) {
        ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_rrn_mul, work_len);
        crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    }

    /* Step 4: if rrn is not null, get data out from DRAM. */
    if (rrn != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(rsa_addr_rr, rrn, aligned_len));
    }

    return ret;
}

td_s32 update_rsa_modulus(const td_u8 *n, const td_u32 n_len, const td_u32 aligned_len CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u8 rrn[DRV_PKE_LEN_4096] = {0};
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), n, n_len);

    drv_pke_data mod_n = {.data = (td_u8 *)n, .length = n_len};
    /* 1. get new montgomery parameters and set it into register. */
    ret = crypto_drv_pke_common_set_mont_param(&mod_n);
    crypto_chk_func_return(crypto_drv_pke_common_set_mont_param, ret);
    /* 2. calculate to get rrn value. */
    ret = rsa_rrn(n, n_len, aligned_len, rrn);
    crypto_chk_func_return(rsa_rrn, ret);
    /* 3. set rrn into PKE DRAM rsa_addr_rr. */
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_rr, rrn, aligned_len, aligned_len));
    crypto_dump_buffer("rrn", rrn, aligned_len);

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

/* bit c len will no more than bit_a_len + bit_b_len */
td_s32 rsa_regular_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 aligned_len = 0;
    td_u32 work_len = 0;
    td_u8 *N = TD_NULL;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, b, c);
    drv_crypto_pke_check_param(a == TD_NULL || a->data == TD_NULL);
    drv_crypto_pke_check_param(b == TD_NULL || b->data == TD_NULL);
    drv_crypto_pke_check_param(c == TD_NULL || c->data == TD_NULL);
    drv_crypto_pke_check_param(a->length > DRV_PKE_LEN_2048);
    drv_crypto_pke_check_param(b->length > DRV_PKE_LEN_2048);
    drv_crypto_pke_check_param(c->length < (a->length + b->length));
    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    ret = hal_pke_get_align_val(crypto_max(a->length, b->length), &aligned_len);
    crypto_chk_func_return(hal_pke_get_align_val, ret);
    aligned_len = 2 * aligned_len;
    work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;

    N = crypto_malloc(aligned_len);
    crypto_chk_return((N == TD_NULL), TD_FAILURE, "no enough memory!\n");
    /* set modulus' whole bits as 1. */
    ret = memset_enhance(N, aligned_len, 0xFF, aligned_len);  /* 0xFF: set the whole modulus ram to 1. */
    crypto_chk_goto((ret != TD_SUCCESS), __exit, "memset_enhance failed!\n");

    /* 1. check free */
    ret = hal_pke_check_free();
    crypto_chk_goto((ret != TD_SUCCESS), __exit, "pke module is busy now!\n");

    /* 2. set data to DRAM */
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_n, N, aligned_len, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t0, a->data, a->length, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t1, b->data, b->length, aligned_len));

    /* 3. set montgomery mod multiplication parameter 0x1. */
    pke_reg_write(PKE_MONT_PARA0, 0x1);
    pke_reg_write(PKE_MONT_PARA1, 0x0);
    /* call back for secure enhancement */
    reg_callback_chk(PKE_MONT_PARA0, 0x1);
    reg_callback_chk(PKE_MONT_PARA0, 0x0);

    /* 4. start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_regular_mul, work_len);
    crypto_chk_goto((ret != TD_SUCCESS), __exit, "crypto_drv_pke_common_batch_instr_process failed, ret = 0x%x", ret);

    /* 5. get result data from DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(rsa_addr_s, c->data, c->length));

__exit:
    if (N != TD_NULL) {
        crypto_free(N);
        N = TD_NULL;
    }

    crypto_drv_func_exit();
    return ret;
}

td_s32 rsa_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = 0;
    td_u32 aligned_len = 0;
    td_u8 const_1[CRYPTO_WORD_WIDTH] = {0};
    const_1[CRYPTO_WORD_WIDTH - 1] = 0x1;   /* set const value. */
    td_u8 bit_2[CRYPTO_WORD_WIDTH] = {0};
    td_u32 a_valid_index = 0;
    td_u32 a_valid_length = 0;
    td_u32 p_valid_index = 0;
    td_u32 p_valid_length = 0;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, p, c);
    drv_crypto_pke_check_param(a == TD_NULL || a->data == TD_NULL);
    drv_crypto_pke_check_param(p == TD_NULL || p->data == TD_NULL);
    drv_crypto_pke_check_param(c == TD_NULL || c->data == TD_NULL);
    drv_crypto_pke_check_param(p->length > DRV_PKE_LEN_4096);
    /* the modulur couldn't be zero. */
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(p->data, p->length) == TD_TRUE);
    /* the modulur couldn't be even data. */
    drv_crypto_pke_check_param(p->data[p->length - 1] % 2 == 0);    /* 2: to check if even. */

    /* the parameter shouldn't be too large than the modulur. */
    get_valid_bit(a->data, a->length, &a_valid_index);
    get_valid_bit(p->data, p->length, &p_valid_index);
    a_valid_length = a->length - a_valid_index;
    p_valid_length = p->length - p_valid_index;
    drv_crypto_pke_check_param(a_valid_length > 2 * p_valid_length);
    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    /* Note: For 521 bit mode, the actual size should be 576 */
    ret = hal_pke_get_align_val(p_valid_length, &aligned_len);
    crypto_chk_func_return(hal_pke_get_align_val, ret);
    /* the time for calculate is 2^(valid_bit_index * 8) * 15 * 25(4,40), when valid_bit_index == 3, the time >= 10s;
        when valid_bit_index == 4, the time > 10000s, which will call timeout. */
    crypto_chk_return((aligned_len - p_valid_length > MAX_OFFSET_OF_VALID_BIT),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "the modulur is too small\n");
    /* the length should be enough to save the result. */
    drv_crypto_pke_check_param(c->length < aligned_len);

    ret = memset_enhance(bit_2, CRYPTO_WORD_WIDTH, 0x00, CRYPTO_WORD_WIDTH);
    crypto_chk_func_return(memset_enhance, ret);
    /* Step 1. set data into DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_n, p->data + p_valid_index, p_valid_length, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_const_1, const_1, CRYPTO_WORD_WIDTH, aligned_len));
    /* set high bit into rsa_addr_t1, set low bit into rsa_addr_t0. */
    if (a_valid_length < aligned_len) {
        hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t0, a->data + a_valid_index, a_valid_length, aligned_len));
        hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t1, bit_2, CRYPTO_WORD_WIDTH, aligned_len));
    } else {
        hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t0, a->data + a->length - aligned_len, aligned_len, aligned_len));
        hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t1, a->data + a_valid_index, a_valid_length - aligned_len,
            aligned_len));
    }
    /* set mont_param */
    ret = update_rsa_modulus(sec_arg_add_cs(p->data + p_valid_index, p_valid_length, aligned_len));
    crypto_chk_func_return(update_rsa_modulus, ret);

    /* Step 2. start calculation. */
    work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_mod, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3. get result. */
    (void)memset_enhance(c->data, c->length, 0x00, c->length);
    hal_pke_get_ram(sec_arg_add_cs(rsa_addr_a, c->data + c->length - aligned_len, aligned_len));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

static td_s32 pke_rsa_mod_param_check(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    drv_crypto_pke_check_param(a == TD_NULL);
    drv_crypto_pke_check_param(a->data == TD_NULL);
    drv_crypto_pke_check_param(b == TD_NULL);
    drv_crypto_pke_check_param(b->data == TD_NULL);
    drv_crypto_pke_check_param(p == TD_NULL);
    drv_crypto_pke_check_param(p->data == TD_NULL);
    drv_crypto_pke_check_param(c == TD_NULL);
    drv_crypto_pke_check_param(c->data == TD_NULL);
    drv_crypto_pke_check_param(p->length < a->length);
    drv_crypto_pke_check_param(p->length < b->length);
    drv_crypto_pke_check_param(p->length > DRV_PKE_LEN_4096);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(p->data, p->length) == TD_TRUE);

    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    return TD_SUCCESS;
}

td_s32 pke_rsa_mod_common_process(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    td_u32 *aligned_len, rsa_mod_mode mode)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = 0;
    td_u32 a_valid_index = 0;
    td_u32 b_valid_index = 0;
    td_u32 p_valid_index = 0;

    /* the parameter shouldn't be too large than the modulur. */
    get_valid_bit(a->data, a->length, &a_valid_index);
    get_valid_bit(b->data, b->length, &b_valid_index);
    get_valid_bit(p->data, p->length, &p_valid_index);
    *aligned_len = crypto_max(crypto_max(a->length - a_valid_index, b->length - b_valid_index),
        p->length - p_valid_index);
    /* Note: For 521 bit mode, the actual size should be 576 */
    ret = hal_pke_get_align_val(*aligned_len, aligned_len);
    crypto_chk_func_return(hal_pke_get_align_val, ret);
    /* the time for calculate is 2^(valid_bit_index * 8) * 15 * 25(4,40), when valid_bit_index == 3, the time >= 10s;
        when valid_bit_index == 4, the time > 10000s, which will call timeout. */
    crypto_chk_return((*aligned_len - (p->length - p_valid_index) > MAX_OFFSET_OF_VALID_BIT),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "the modulur is too small\n");

    /* Step 1. wait for PKE free */
    ret = hal_pke_check_free();
    crypto_chk_func_return(hal_pke_check_free, ret);

    /* Step 2. set data into DRAM */
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_n, p->data + p_valid_index, p->length - p_valid_index, *aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t0, a->data + a_valid_index, a->length - a_valid_index, *aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t1, b->data + b_valid_index, b->length - b_valid_index, *aligned_len));

    /* Step 3. start calculation processing. */
    work_len = *aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    if (mode == RSA_MOD_ADD) {
        ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_mod_add, work_len);
    } else if (mode == RSA_MOD_SUB) {
        ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_mod_sub, work_len);
    }
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    return ret;
}

td_s32 rsa_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    /* Note: For 521 bit mode, the actual size should be 576 */
    td_u32 aligned_len = 0;
    td_u32 work_len = 0;
    td_u32 out_instr = 0;
    td_u8 tmp_out[DRV_PKE_LEN_4096] = {0};
    instr_data_ram data_ram = {0};
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, b, p, c);
    ret = pke_rsa_mod_param_check(a, b, p, c);
    crypto_chk_func_return(pke_rsa_mod_param_check, ret);

    ret = pke_rsa_mod_common_process(a, b, p, &aligned_len, RSA_MOD_ADD);
    crypto_chk_func_return(pke_rsa_mod_common_process, ret);

    /* the length should be enough to save the result. */
    drv_crypto_pke_check_param(c->length < aligned_len);
    (void)memset_enhance(c->data, c->length, 0x00, c->length);
    (void)memset_enhance(tmp_out, DRV_PKE_LEN_4096, 0x00, DRV_PKE_LEN_4096);
    work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* Step 5. for complete reduction. */
    data_ram = (instr_data_ram) {.ram_section_c = rsa_addr_s, .ram_section_a = rsa_addr_s, .ram_section_b = rsa_addr_n,
        .ram_section_tp = rsa_addr_tp};
    do {
        /* Step 4. get result from DRAM */
        hal_pke_get_ram(sec_arg_add_cs(rsa_addr_s, c->data + c->length - aligned_len, aligned_len));
        ret = crypto_drv_pke_common_get_single_instr(CMD_SUB_MOD, &data_ram, &out_instr);
        crypto_chk_func_return(crypto_drv_pke_common_get_single_instr, ret);
        ret = crypto_drv_pke_common_single_instr_process(out_instr, work_len);
        crypto_chk_func_return(crypto_drv_pke_common_single_instr_process, ret);
        hal_pke_get_ram(sec_arg_add_cs(rsa_addr_s, tmp_out + c->length - aligned_len, aligned_len));
        crypto_dump_buffer("tmp_out", tmp_out, c->length);
        crypto_dump_buffer("c->data", c->data, c->length);
    } while (memcmp_enhance(c->data, tmp_out, c->length) != TD_SUCCESS);

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 rsa_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    /* Note: For 521 bit mode, the actual size should be 576 */
    td_u32 aligned_len = 0;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, b, p, c);
    ret = pke_rsa_mod_param_check(a, b, p, c);
    crypto_chk_func_return(pke_rsa_mod_param_check, ret);

    ret = pke_rsa_mod_common_process(a, b, p, &aligned_len, RSA_MOD_SUB);
    crypto_chk_func_return(pke_rsa_mod_common_process, ret);

    /* 4. get result from DRAM */
    /* the length should be enough to save the result. */
    drv_crypto_pke_check_param(c->length < aligned_len);
    (void)memset_enhance(c->data, c->length, 0x00, c->length);
    hal_pke_get_ram(sec_arg_add_cs(rsa_addr_s, c->data + c->length - aligned_len, aligned_len));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 rsa_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    /* Note: For 521 bit mode, the actual size should be 576 */
    td_u32 aligned_len = 0;
    td_u32 work_len = 0;
    td_u8 const_1[CRYPTO_WORD_WIDTH] = {0};
    const_1[CRYPTO_WORD_WIDTH - 1] = 0x1;   /* set const value. */
    td_u32 a_valid_index = 0;
    td_u32 b_valid_index = 0;
    td_u32 p_valid_index = 0;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, b, p, c);
    ret = pke_rsa_mod_param_check(a, b, p, c);
    crypto_chk_func_return(pke_rsa_mod_param_check, ret);
    /* the modulur couldn't be even data. */
    drv_crypto_pke_check_param(p->data[p->length - 1] % 2 == 0);    /* 2: to check if even. */

    /* the parameter shouldn't be too large than the modulur. */
    get_valid_bit(a->data, a->length, &a_valid_index);
    get_valid_bit(b->data, b->length, &b_valid_index);
    get_valid_bit(p->data, p->length, &p_valid_index);
    aligned_len = crypto_max(crypto_max(a->length - a_valid_index, b->length - b_valid_index),
        p->length - p_valid_index);
    ret = hal_pke_get_align_val(aligned_len, &aligned_len);
    crypto_chk_func_return(hal_pke_get_align_val, ret);
    /* the time for calculate is 2^(valid_bit_index * 8) * 15 * 25(4,40), when valid_bit_index == 3, the time >= 10s;
        when valid_bit_index == 4, the time > 10000s, which will call timeout. */
    crypto_chk_return((aligned_len - (p->length - p_valid_index) > MAX_OFFSET_OF_VALID_BIT),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "the modulur is too small\n");
    /* the length should be enough to save the result. */
    drv_crypto_pke_check_param(c->length < aligned_len);

    /* 1. wait for PKE free */
    ret = hal_pke_check_free();
    crypto_chk_func_return(hal_pke_check_free, ret);

    /* 2. set mont_param register. */
    ret = update_rsa_modulus(sec_arg_add_cs(p->data + p_valid_index, p->length - p_valid_index, aligned_len));
    crypto_chk_func_return(update_rsa_modulus, ret);

    /* 3. set data into DRAM */
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_n, p->data + p_valid_index, p->length - p_valid_index, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t0, a->data + a_valid_index, a->length - a_valid_index, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_t1, b->data + b_valid_index, b->length - b_valid_index, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_const_1, const_1, CRYPTO_WORD_WIDTH, aligned_len));

    /* 4. start calculation processing. */
    work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_mod_mul, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 4. get result from DRAM */
    (void)memset_enhance(c->data, c->length, 0x00, c->length);
    hal_pke_get_ram(sec_arg_add_cs(rsa_addr_s, c->data + c->length - aligned_len, aligned_len));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

static td_s32 pke_rsa_inv_mod_param_check(const drv_pke_data *a, const drv_pke_data *p,
    const drv_pke_data *c)
{
    drv_crypto_pke_check_param(a == TD_NULL);
    drv_crypto_pke_check_param(a->data == TD_NULL);
    drv_crypto_pke_check_param(p == TD_NULL);
    drv_crypto_pke_check_param(p->data == TD_NULL);
    drv_crypto_pke_check_param(c == TD_NULL);
    drv_crypto_pke_check_param(c->data == TD_NULL);
    drv_crypto_pke_check_param(p->length != a->length);
    drv_crypto_pke_check_param(p->length > DRV_PKE_LEN_4096);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(p->data, p->length) == TD_TRUE);
    /* the modulur couldn't be even data. */
    drv_crypto_pke_check_param(p->data[p->length - 1] % 2 == 0);    /* 2: to check if even. */

    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    return TD_SUCCESS;
}

td_s32 rsa_inv_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u8 p_2[DRV_PKE_LEN_4096] = {0};
    drv_pke_data p_2_data;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, p, c);
    ret = pke_rsa_inv_mod_param_check(a, p, c);
    crypto_chk_func_return(pke_rsa_inv_mod_param_check, ret);

    /* Step 1. calculate p-2. */
    ret = memcpy_enhance(p_2, DRV_PKE_LEN_4096, p->data, p->length);
    crypto_chk_return((ret != TD_SUCCESS), ret, "memcyp_ss failed!\n");
    crypto_drv_pke_common_array_sub_value(p_2, p->length, 2);
    p_2_data = gen_pke_data(p->length, p_2);

    /* Step 2. calculate by exp_mod. */
    ret = rsa_exp_mod(sec_arg_add_cs(p, &p_2_data, a, c));
    crypto_chk_func_return(rsa_exp_mod, ret);

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

static td_s32 pke_rsa_exp_mod_param_check(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out)
{
    drv_crypto_pke_check_param(n == TD_NULL);
    drv_crypto_pke_check_param(n->data == TD_NULL);
    drv_crypto_pke_check_param(k == TD_NULL);
    drv_crypto_pke_check_param(k->data == TD_NULL);
    drv_crypto_pke_check_param(in == TD_NULL);
    drv_crypto_pke_check_param(in->data == TD_NULL);
    drv_crypto_pke_check_param(out == TD_NULL);
    drv_crypto_pke_check_param(out->data == TD_NULL);
    /* here the length should also can be 256/384/512/576 */
    drv_crypto_pke_check_param(n->length > DRV_PKE_LEN_4096);
    drv_crypto_pke_check_param(crypto_drv_pke_common_is_zero(n->data, n->length) == TD_TRUE);
    drv_crypto_pke_check_param(k->length > DRV_PKE_LEN_4096);
    drv_crypto_pke_check_param(in->length > DRV_PKE_LEN_4096);
    /* the modulur couldn't be even data. */
    drv_crypto_pke_check_param(n->data[n->length - 1] % 2 == 0);    /* 2: to check if even. */

    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    return TD_SUCCESS;
}

td_s32 rsa_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_s32 j = 0;
    td_u32 aligned_len = 0;
    td_u32 work_len = 0;
    td_u8 const_1[CRYPTO_WORD_WIDTH] = {0};
    const_1[CRYPTO_WORD_WIDTH - 1] = 0x1;
    const rom_lib *rom_lib_list[4] = {
        &instr_rsa_exp_00, &instr_rsa_exp_01, &instr_rsa_exp_10, &instr_rsa_exp_11
    };
    td_u8 bit_2 = 0;
    td_bool start_flag = TD_FALSE;
    td_u32 n_valid_index = 0;
    td_u32 k_valid_index = 0;
    td_u32 in_valid_index = 0;

    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), n, k, in, out);
    ret = pke_rsa_exp_mod_param_check(n, k, in, out);
    crypto_chk_func_return(pke_rsa_exp_mod_param_check, ret);

    /* the parameter shouldn't be too large than the modulur. */
    get_valid_bit(n->data, n->length, &n_valid_index);
    get_valid_bit(k->data, k->length, &k_valid_index);
    get_valid_bit(in->data, in->length, &in_valid_index);
    aligned_len = crypto_max(crypto_max(k->length - k_valid_index, in->length - in_valid_index),
        n->length - n_valid_index);
    ret = hal_pke_get_align_val(aligned_len, &aligned_len);
    crypto_chk_func_return(hal_pke_get_align_val, ret);
    /* the time for calculate is 2^(valid_bit_index * 8) * 15 * 25(4,40), when valid_bit_index == 3, the time >= 10s;
        when valid_bit_index == 4, the time > 10000s, which will call timeout. */
    crypto_chk_return((aligned_len - (n->length - n_valid_index) > MAX_OFFSET_OF_VALID_BIT),
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "the modulur is too small\n");
    /* the length should be enough to save the result. */
    drv_crypto_pke_check_param(out->length < aligned_len);

    ret = update_rsa_modulus(sec_arg_add_cs(n->data + n_valid_index, n->length - n_valid_index, aligned_len));
    crypto_chk_func_return(update_rsa_modulus, ret);

    /* step 1: data montgomery and copy. */
    /* 1. set data into DRAM. */
    work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_n, n->data + n_valid_index, n->length - n_valid_index, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_a, in->data + in_valid_index, in->length - in_valid_index, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(rsa_addr_const_1, const_1, CRYPTO_WORD_WIDTH, aligned_len));
    /* 2. start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_exp_pre_6, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /*
     * 1) k->data is one byte array, k->length is the byte number.
     * 2) For each byte in k->data, get bit2 = byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
     * 3) If there is one bit2 is not zero, then all the next bit2(contain this time) will call
        crypto_drv_pke_common_batch_instr_process(), the first param depends on bit2's value.
     */
    for (i = 0; i < k->length; i++) {
        for (j = 3; j >= 0; j--) {                  // 3: get byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
            bit_2 = (k->data[i] >> ((td_u32)j * 2)) & 0x3;   // 2, 0x3: get bit2
            if ((start_flag == TD_FALSE) && (bit_2 != 0)) {
                start_flag = TD_TRUE;
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_list[bit_2], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
            } else if (start_flag == TD_TRUE) {
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_list[bit_2], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
            }
        }
    }

    /* step 3: data demontgomery and twice modulus reduction. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_rsa_exp_post_3, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* step 4: get data out. */
    (void)memset_enhance(out->data, out->length, 0x00, out->length);
    hal_pke_get_ram(sec_arg_add_cs(rsa_addr_s, out->data + out->length - aligned_len, aligned_len));

    crypto_drv_func_exit();
    return ret;
}
