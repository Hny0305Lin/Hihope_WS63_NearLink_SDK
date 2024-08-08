/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm2 verify function implement.
 *
 * Create: 2023-05-19
*/

#include "drv_common_pke.h"
#include "crypto_drv_common.h"
#include "ecc_ec_fp.h"

CRYPTO_STATIC td_s32 inner_sm2_verify_param_check(const drv_pke_ecc_point *pub_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig);

CRYPTO_STATIC td_s32 inner_sm2_verify_param_check(const drv_pke_ecc_point *pub_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig)
{
    drv_crypto_pke_check_param(pub_key == TD_NULL || pub_key->x == TD_NULL || pub_key->y == TD_NULL ||
        pub_key->length == 0);
    drv_crypto_pke_check_param(hash == TD_NULL || hash->data == TD_NULL || hash->length == 0);
    drv_crypto_pke_check_param(sig == TD_NULL || sig->r == TD_NULL || sig->s == TD_NULL);
    drv_crypto_pke_check_param(pub_key->length != DRV_PKE_LEN_256);
    drv_crypto_pke_check_param(sig->length != DRV_PKE_LEN_256);
    /* increase security strength check. */
    drv_crypto_pke_check_param(hash->length < DRV_PKE_LEN_256);
    return TD_SUCCESS;
}

td_s32 crypto_drv_pke_common_sm2_verify(const drv_pke_ecc_point *pub_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    td_u32 local_cnt = local_step(0);
    union {
        td_u8 t[DRV_PKE_LEN_256];   /* step 2, 2.1, 3.2 */
        td_u8 rx[DRV_PKE_LEN_256];  /* step 4 */
    } pke_data_1;
    td_u8 v[DRV_PKE_LEN_256] = {0};
    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data pp = {0};
    drv_pke_data cc = {0};
    drv_pke_ecc_point rr = {0};

    crypto_drv_func_enter();

    ret = inner_sm2_verify_param_check(pub_key, hash, sig);
    crypto_chk_func_return(inner_sm2_verify_param_check, ret);
    /* Need to truncate digest if it is too long. */
    const drv_pke_ecc_curve *sm2 = crypto_drv_pke_common_get_ecc_curve(DRV_PKE_ECC_TYPE_SM2);
    crypto_chk_return(sm2 == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");
    drv_pke_data used_hash = {.data = hash->data, .length = crypto_min(hash->length, sm2->ksize)};

    /* Step 1: check whether r, s is in [1, n-1] */
    ret = crypto_drv_pke_common_range_check(sig->r, sm2->n, sm2->ksize);
    crypto_chk_func_return(crypto_drv_pke_common_range_check, ret);
    ret = crypto_drv_pke_common_range_check(sig->s, sm2->n, sm2->ksize);
    crypto_chk_func_return(crypto_drv_pke_common_range_check, ret);

    /* Step 1.1: set initial parameters into DRAM. */
    ret = crypto_drv_pke_common_init_param(sm2);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);
    /* Step 1.2: set montgomery parameter. */
    ret = update_modulus(sec_arg_add_cs(sm2->n, sm2->ksize));
    crypto_chk_func_return(update_modulus, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 2: calculate t = (r + s) mod n. */
    ret = TD_FAILURE;
    aa = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = sig->r};
    bb = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = sig->s};
    pp = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = (td_u8 *)sm2->n};
    cc = (drv_pke_data) {.length = DRV_PKE_LEN_256, .data = pke_data_1.t};
    ret = ecc_ecfn_add_mod(sec_arg_add_cs(&aa, &bb, &pp, &cc));
    crypto_chk_func_return(ecc_ecfn_add_mod, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 2.1: t = 0 ? */
    if (crypto_drv_pke_common_is_zero(pke_data_1.t, DRV_PKE_LEN_256) == TD_TRUE) {
        return TD_FAILURE;
    }
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 3: calcualte R = k * G = s * G + t * Qu */
    /* Step 3.1: set montgomery parameter. */
    ret = update_modulus(sec_arg_add_cs(sm2->p, sm2->ksize));
    crypto_chk_func_return(update_modulus, ret);
    local_cnt += LOCAL_STEP_AUTH;
    /* Step 3.2: use multi_multiplication point to calculate. */
    rr = (drv_pke_ecc_point) {.x = pke_data_1.rx, .y = TD_NULL, .length = DRV_PKE_LEN_256};
    ret = ecc_ecfp_mul_add(sec_arg_add_cs(sm2, &bb, &cc, pub_key, &rr));
    crypto_chk_func_return(ecc_ecfp_mul_add, ret);

    /* Step 4: claculate rx = e + Rx mod n. */
    /* Step 4.1: set montgomery parameter. */
    ret = update_modulus(sec_arg_add_cs(sm2->n, sm2->ksize));
    crypto_chk_func_return(update_modulus, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 4.2: start claculate rx = e + Rx mod n */
    ret = TD_FAILURE;
    bb = (drv_pke_data) {.data = pke_data_1.rx, .length = DRV_PKE_LEN_256};
    cc = (drv_pke_data) {.data = v, .length = DRV_PKE_LEN_256};
    ret = ecc_ecfn_add_mod(sec_arg_add_cs(&used_hash, &bb, &pp, &cc));
    crypto_chk_func_return(ecc_ecfn_add_mod, ret);
    local_cnt += LOCAL_STEP_AUTH;

    /* Step 5: rx = r ? */
    if (memcmp_enhance(v, sig->r, DRV_PKE_LEN_256) == TD_SUCCESS &&
       memcmp_enhance(sig->r, v, DRV_PKE_LEN_256) == TD_SUCCESS) {
        local_cnt += LOCAL_STEP_AUTH;
    } else {
        crypto_dump_buffer("expect sig->r", sig->r, DRV_PKE_LEN_256);
        crypto_dump_buffer("infect v", v, DRV_PKE_LEN_256);
        crypto_log_err("compare signature failed!\n");
        return TD_FAILURE;
    }

    /* Step 6: clean ram */

    val_enhance_chk(local_cnt, local_step(7)); // 7 steps total
    crypto_drv_func_exit();
    return TD_SUCCESS;
}