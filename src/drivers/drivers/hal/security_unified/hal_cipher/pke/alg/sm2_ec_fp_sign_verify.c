/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm2 common algorithm implementation
 *
 * Create: 2023-06-05
*/

#include "ecc_ec_fp.h"
#include "crypto_drv_common.h"
#include "hal_pke_reg.h"
#include "hal_pke.h"
#include "drv_common_pke.h"

td_s32 ecc_ecfn_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD)
{
    td_s32 ret = TD_FAILURE;
    td_u32 work_len = p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    crypto_drv_func_enter();
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), a, b, p, c);

    /* Step 1: set data into DRAM. */
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_r, a->data, a->length, p->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_s, b->data, b->length, p->length));
    hal_pke_set_ram(sec_arg_add_cs(ecc_addr_m, p->data, p->length, p->length));

    /* Step 2: start calculate. */
    ret = crypto_drv_pke_common_batch_instr_process(&instr_sm2_verify_t_3, work_len);
    crypto_chk_func_return(crypto_drv_pke_common_batch_instr_process, ret);

    /* Step 3: get data out of DRAM. */
    hal_pke_get_ram(sec_arg_add_cs(ecc_addr_t, c->data, c->length));

    crypto_drv_func_exit();
    return TD_SUCCESS;
}
