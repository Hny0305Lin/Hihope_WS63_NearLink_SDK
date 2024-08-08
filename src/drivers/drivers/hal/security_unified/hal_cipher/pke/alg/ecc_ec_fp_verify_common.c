/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: verify common algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

CRYPTO_STATIC td_s32 point_multi_mul_sim(const drv_pke_ecc_curve *ecc, const drv_pke_data *u1,
    const drv_pke_data *u2 CIPHER_CHECK_WORD);

CRYPTO_STATIC td_s32 point_multi_mul_sim(const drv_pke_ecc_curve *ecc, const drv_pke_data *u1,
    const drv_pke_data *u2 CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_s32 i = 0;
    td_s32 j = 0;
    td_u32 work_len = (td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, u1, u2);
    td_u8 u1_bit = 0;
    td_u8 u2_bit = 0;
    td_u8 bit_2 = 0;
    td_bool first_flag = TD_TRUE;
    const rom_lib *rom_lib_first_list[4] = {    // 4: select by bit_2
        TD_NULL, &instr_ecfp_cpy_p2c_3, &instr_ecfp_cpy_g2c_3, &instr_ecfp_cpy_a2c_3
    };
    const rom_lib *rom_lib_second_list[4] = {   // 4: select by bit_2
        &instr_ecfp_mul_c_double_22, &instr_ecfp_mul_p_22_18, &instr_ecfp_mul_g_22_18, &instr_ecfp_mul_jj_22_23
    };

    crypto_timer_start(TIMER_ID_4, __func__);
    crypto_chk_return(u1->length != u2->length, TD_FAILURE, "u1->length != u2->length\n");

    /*
     * 1) u1->data and u2->data are two byte arrays, their length u1->length and u2->length is the sample.
     * 2) For each bit in u1->data and u2->data, get bit_2 = u1_bit | u2_bit in order.
     * 3) If there is one bit_2 is not zero, then all the next bit_2(contain this time) will call
        crypto_drv_pke_common_batch_instr_process(), the first param depends on bit_2's value.
     */
    for (i = 0; i < (td_s32)u1->length; i++) {
        if (i >= (td_s32)u1->length || i >= (td_s32)u2->length) {
            break;
        }
        for (j = CRYPTO_BITS_IN_BYTE - 1; j >= 0; j--) {
            u1_bit = (u1->data[i] >> (td_u32)j) & 1;
            u2_bit = (u2->data[i] >> (td_u32)j) & 1;
            bit_2 = u1_bit << 1 | u2_bit;       // bit_2 = u1_bit | u2_bit
            if (first_flag == TD_TRUE) {
                if (bit_2 == 0) {
                    continue;
                }
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_first_list[bit_2], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
                first_flag = TD_FALSE;
                crypto_timer_end(TIMER_ID_4, "process");
            } else {
                ret = crypto_drv_pke_common_batch_instr_process(rom_lib_second_list[bit_2], work_len);
                crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_drv_pke_common_batch_instr_process failed\n");
                crypto_timer_end(TIMER_ID_4, "process");
            }
        }
    }

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 ecc_ecfp_mul_add(const drv_pke_ecc_curve *ecc, const drv_pke_data *u1, const drv_pke_data *u2,
    const drv_pke_ecc_point *q, const drv_pke_ecc_point *r CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 aligned_len = 0;
    crypto_drv_func_enter();
    crypto_timer_start(TIMER_ID_2, __func__);

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), ecc, u1, u2, q, r);
    drv_crypto_pke_check_param(ecc == TD_NULL || ecc->gx == TD_NULL || ecc->gy == TD_NULL);
    drv_crypto_pke_check_param(ecc->p == TD_NULL);
    drv_crypto_pke_check_param(u1 == TD_NULL || u1->data == TD_NULL);
    drv_crypto_pke_check_param(u2 == TD_NULL || u2->data == TD_NULL);
    drv_crypto_pke_check_param(q == TD_NULL || q->x == TD_NULL || q->y == TD_NULL);
    drv_crypto_pke_check_param(r != TD_NULL && r->length != ecc->ksize);

    if (g_pke_initialize != TD_TRUE) {
        crypto_log_err("PKE not initialized\n");
        return PKE_COMPAT_ERRNO(ERROR_NOT_INIT);
    }

    /* for ecc curve the aligned_len is determined by ksize. */
    drv_pke_ecc_point g = {.x = (td_u8 *)ecc->gx, .y = (td_u8 *)ecc->gy, .length = ecc->ksize};
    ret = hal_pke_get_align_val(ecc->ksize, &aligned_len);

    td_u8 g_x_jac[DRV_PKE_LEN_576] = {0};
    td_u8 g_y_jac[DRV_PKE_LEN_576] = {0};
    td_u8 q_x_jac[DRV_PKE_LEN_576] = {0};
    td_u8 q_y_jac[DRV_PKE_LEN_576] = {0};
    /* get const value 0, and montgomerized p */
    drv_pke_data pp = {0};
    drv_pke_ecc_point g_jac = {.x = g_x_jac, .y = g_y_jac, .length = ecc->ksize};
    drv_pke_ecc_point q_jac = {.x = q_x_jac, .y = q_y_jac, .length = ecc->ksize};

    /* Step 0: set curve initial parameters into DRAM. */
    ret = crypto_drv_pke_common_init_param(ecc);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);
    crypto_timer_end(TIMER_ID_2, "Step 0");

    /* Step 1: montgomery the input point. */
    pp = (drv_pke_data) {.length = ecc->ksize, .data = (td_u8 *)ecc->p};
    ret = ecc_ecfp_montgomery_data_aff(sec_arg_add_cs(&g, &pp, &g_jac));
    crypto_chk_func_return(ecc_ecfp_montgomery_data_aff, ret);
    ret = ecc_ecfp_montgomery_data_aff(sec_arg_add_cs(q, &pp, &q_jac));
    crypto_chk_func_return(ecc_ecfp_montgomery_data_aff, ret);
    crypto_timer_end(TIMER_ID_2, "Step 1");

    /* Step 2: A = P + Q, from affine to jacobin. */
    /* 2.1 set data into PKE DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_px, q_jac.x, q_jac.length, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_py, q_jac.y, q_jac.length, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_gx, g_jac.x, g_jac.length, aligned_len));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_gy, g_jac.y, g_jac.length, aligned_len));

    /* point add A = G + Q. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfp_add_ja_verify_18,
        aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    crypto_timer_end(TIMER_ID_2, "Step 2");

    /* Step 3: simultaneous calculate multi-multiplication. */
    ret = point_multi_mul_sim(sec_arg_add_cs(ecc, u1, u2));
    crypto_chk_func_return(point_multi_mul_sim, ret);
    crypto_timer_end(TIMER_ID_2, "Step 3");

    /* Step 4: trans Jacobin to Affine. */
    ret = ecc_ecfp_jac_to_aff(&pp);
    crypto_chk_func_return(ecc_ecfp_jac_to_aff, ret);
    crypto_timer_end(TIMER_ID_2, "Step 4");

    /* Step 5: demontgomery, just process, not get data out of the DRAM. */
    ret = ecc_ecfp_demontgomery_data_aff(TD_NULL, aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE);
    crypto_chk_func_return(ecc_ecfp_demontgomery_data_aff, ret);
    crypto_timer_end(TIMER_ID_2, "Step 5");

    /* Step 6: get result out from PKE data RAM. only need to get r.x for compare. */
    if (r != TD_NULL && r->x != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cx, r->x, r->length));
    }
    if (r != TD_NULL && r->y != TD_NULL) {
        hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cy, r->y, r->length));
    }
    crypto_timer_end(TIMER_ID_2, "Step 6");

    crypto_drv_func_exit();
    return TD_SUCCESS;
}
