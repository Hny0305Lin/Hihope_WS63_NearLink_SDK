/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc common algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

/* (ecc_addr_cx, ecc_addr_cy, ecc_addr_cz) -> (ecc_addr_px, ecc_addr_py) */
td_s32 ecc_ecfp_jac_to_aff(const drv_pke_data *mod_p)
{
    td_s32 ret = TD_FAILURE;

    /* Step 1: prepare instructions to use */
    rom_lib batch_instr_block[JAC_TO_AFF_INSTR_NUM] = {instr_ecfp_j2a_pre_5, instr_ecfp_j2a_exp_00,
        instr_ecfp_j2a_exp_01, instr_ecfp_j2a_exp_10, instr_ecfp_j2a_exp_11,
        instr_ecfp_j2a_post_4};

    /* Step 2: start transfer calculation. */
    ret = crypto_drv_pke_common_jac_to_aff_cal(batch_instr_block, JAC_TO_AFF_INSTR_NUM, mod_p);
    crypto_chk_func_return(crypto_drv_pke_common_jac_to_aff_cal, ret);

    return ret;
}

/* use when the process only have ecc related data address. */
td_s32 ecc_ecfp_montgomery_data_aff(const drv_pke_ecc_point *in, const drv_pke_data *mod_p,
    const drv_pke_ecc_point *out CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();
    /* secure enhancement */
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), in, mod_p, out);

    drv_crypto_pke_check_param(in == TD_NULL || in->x == TD_NULL || in->y == TD_NULL);
    drv_crypto_pke_check_param(mod_p == TD_NULL || mod_p->data == TD_NULL);
    /* the out should be enough to save result. */
    drv_crypto_pke_check_param(out != TD_NULL && out->x != TD_NULL &&
                               out->y != TD_NULL && mod_p->length != out->length);

    td_u32 in_aligned_len = 0;
    td_u32 out_aligned_len = 0;
    td_u32 work_len = 0;

    /* P = P * rrp mod p to Montgomery data, point mul_mud */
    /* 1. wait for PKE free */
    ret = hal_pke_check_free();
    crypto_chk_func_return(hal_pke_check_free, ret);

    /* 2. set data to DRAM */
    hal_pke_get_align_val(in->length, &in_aligned_len);
    if (out != TD_NULL) {
        hal_pke_get_align_val(out->length, &out_aligned_len);
        in_aligned_len = (out_aligned_len >= in_aligned_len) ? out_aligned_len : in_aligned_len;
    }
    work_len = in_aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_px, in->x, in->length, in_aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_py, in->y, in->length, in_aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, mod_p->data, mod_p->length, in_aligned_len));

    /* 3. start calculate */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_mont_p_2, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 6. get result from DRAM */
    if (out != TD_NULL && out->x != TD_NULL && out->y != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_px, out->x, out->length));
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_py, out->y, out->length));
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

/* (ecc_addr_cx, ecc_addr_cy) -> (ecc_addr_cx, ecc_addr_cy) */
td_s32 ecc_ecfp_demontgomery_data_aff(const drv_pke_ecc_point *r, td_u32 work_len)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    /* Step 1: start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_demont_c_6, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 2: get data from PKE DRAM */
    if (r != TD_NULL && r->x != TD_NULL && r->y != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cx, r->x, r->length));
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cy, r->y, r->length));
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 update_modulus(const td_u8 *n, const td_u32 n_len CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), n, n_len);

    drv_pke_data mod_n = {.data = (td_u8 *)n, .length = n_len};
    td_u32 work_len = n_len / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* 1. set new modulus and const_0(by initial parameters set) into PKE DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_n, n, n_len, n_len));

    /* 2. start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_prime_n_1, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* 5. get new montgomery parameters and set it into register. */
    ret = crypto_drv_pke_common_set_ecc_mont_param(&mod_n);
    crypto_chk_func_return(crypto_drv_pke_common_set_ecc_mont_param, ret);

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 ecc_ecfn_inv(const drv_pke_data *a, const drv_pke_data *n, const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u8 n_2[DRV_PKE_LEN_576];
    td_u32 aligned_len = 0;
    td_u32 work_len = 0;
    td_u32 i = 0;
    td_s32 j = 0;
    const rom_lib *rom_lib_list[4] = {
        &instr_ecfn_inv_exp_00, &instr_ecfn_inv_exp_01, &instr_ecfn_inv_exp_10, &instr_ecfn_inv_exp_11
    };
    td_u8 bit_2 = 0;
    td_bool start_flag = TD_FALSE;

    crypto_drv_func_enter();
    crypto_timer_start(TIMER_ID_3, __func__);

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, n, c);
    drv_crypto_pke_check_param(c->length != n->length);

    aligned_len = crypto_max(a->length, n->length);
    ret = hal_pke_get_align_val(aligned_len, &aligned_len);
    crypto_chk_func_return(hal_pke_get_align_val, ret);
    work_len  = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* Step 1. former 5 process. data montgomery and copy */
    /* 1.set data into DRAM, ecc_addr_rrn and ecc_addr_mont_1_n will be set in initial parameters. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_k, a->data, a->length, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, n->data, n->length, aligned_len));
    /* 2. start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfn_inv_pre_5, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    crypto_timer_end(TIMER_ID_3, "Step 1");

    /* Step 2. start inv calculate. */
    /* expand (n-2)'s bit length to 2n, and calculate from high bit */
    /* get n - 2 */
    ret = memcpy_enhance(n_2, DRV_PKE_LEN_576, n->data, n->length);
    crypto_chk_return((ret != TD_SUCCESS), ret, "memcyp_ss failed!\n");

    crypto_drv_pke_common_array_sub_value(n_2, n->length, 2);
    crypto_timer_end(TIMER_ID_3, "Step 2");

    /* calculate by binary data, from high address to low address. the result store in ecc_addr_s */
    /*
     * 1) n_2 is one byte array, n->length is the byte number.
     * 2) For each byte in n_2, get bit2 = byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
     * 3) If there is one bit2 is not zero, then all the next bit2(contain this time) will call
        crypto_drv_pke_common_batch_instr_process(), the first param depends on bit2's value.
     */
    for (i = 0; i < n->length; i++) {
        for (j = 3; j >= 0; j--) {                  // 3: get byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
            bit_2 = (n_2[i] >> ((td_u32)j * 2)) & 0x3;   // 2, 0x3: get bit2
            if ((start_flag == TD_FALSE) && (bit_2 != 0)) {
                start_flag = TD_TRUE;
                crypto_timer_end(TIMER_ID_3, "Step 3 process");
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_list[bit_2], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
            } else if (start_flag == TD_TRUE) {
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_list[bit_2], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
                crypto_timer_end(TIMER_ID_3, "Step 3 process");
            }
        }
    }

    /* get data out from DRAM ecc_addr_s, the result is montgomeried. */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_s, c->data, c->length));

    crypto_timer_end(TIMER_ID_3, "Step 4");

    crypto_drv_func_exit();
    return TD_SUCCESS;
}