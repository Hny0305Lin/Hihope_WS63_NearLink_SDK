/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc verify algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

td_s32 ecc_ecfn_verify_u(const drv_pke_data *s_inv, const drv_pke_data *e, const drv_pke_data *r,
    const drv_pke_data *n, const drv_pke_data *u1, const drv_pke_data *u2 CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), s_inv, e, r, n, u1, u2);

    td_u32 work_len = n->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    crypto_timer_start(TIMER_ID_1, __func__);

    /* 1. check free, no need here, for the former calculation will definitely wait for PKE not busy. */

    /* Step 1: set data into PKE DRAM. the ecc_addr_rrn and ecc_addr_const_1 is set in initial parameters. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_e, e->data, e->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_r, r->data, r->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_s, s_inv->data, s_inv->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, n->data, n->length, n->length));
    crypto_timer_end(TIMER_ID_1, "Step 1");

    /* Step 2: start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfn_verify_u_10, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);
    crypto_timer_end(TIMER_ID_1, "Step 2");

    /* Step 3: get result out of PKE DRAM */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_u1, u1->data, u1->length));
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_u2, u2->data, u2->length));
    crypto_timer_end(TIMER_ID_1, "Step 3");

    crypto_drv_func_exit();
    return TD_SUCCESS;
}

td_s32 ecc_ecfn_verify_v(const drv_pke_ecc_curve *ecc, const drv_pke_ecc_sig *sig, td_u8 *out_v)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = (td_u32)ecc->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE;
    td_u8 cz[DRV_PKE_LEN_576];
    crypto_drv_func_enter();

    /* Step 1: set n as new modulur and set const_0 to ecc_addr_const_0.
    where the ecc_addr_const_0 is set in initial parameters. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, ecc->n, ecc->ksize, ecc->ksize));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_r, sig->r, sig->length, ecc->ksize));

    /* Step 2: calculate out.v = (Rx + 0) mod n and check out.v - sig.r == 0 ? */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfn_verify_v_4, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: check whether R = u1G + u2Q is infinite point by check whether Rz_Jac is zero. */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cz, cz, ecc->ksize));
    ret = crypto_drv_pke_common_is_zero(cz, ecc->ksize);
    crypto_chk_return((ret == TD_TRUE), TD_FAILURE, "the result is infinite point\r\n");

    /* Step 4: check whether r == v ?. follow this step, we couldn't get the out_v value, here the out_v is zero. */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_cy, out_v, ecc->ksize));
    ret = crypto_drv_pke_common_is_zero(out_v, ecc->ksize);
    crypto_chk_return((ret != TD_TRUE), TD_FAILURE, "v != r, verify failed\r\n");

    crypto_drv_func_exit();
    return TD_SUCCESS;
}
