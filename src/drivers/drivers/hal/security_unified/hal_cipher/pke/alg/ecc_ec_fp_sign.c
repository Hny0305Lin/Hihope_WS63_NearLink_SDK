/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc sign algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "drv_trng.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

td_s32 ecc_ecfn_sign_s(const drv_pke_data *k_inv, const drv_pke_data *e, const drv_pke_data *d,
    const drv_pke_data *r, const drv_pke_data *n, const drv_pke_data *s CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), k_inv, e, d, r, n, s);

    td_u32 work_len = n->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    /* 1. check free, no need here, for the former calculation will definitely wait for PKE not busy. */

    /* Step 1: set data into PKE DRAM. the ecc_addr_rrn and ecc_addr_const_1 have been set in intial parameters. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_r, r->data, r->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_e, e->data, e->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_d, d->data, d->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_s, k_inv->data, k_inv->length, n->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_n, n->data, n->length, n->length));

    /* Step 2. start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_ecfn_sign_s_12, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: get data from PKE DRAM */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_s, s->data, s->length));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}