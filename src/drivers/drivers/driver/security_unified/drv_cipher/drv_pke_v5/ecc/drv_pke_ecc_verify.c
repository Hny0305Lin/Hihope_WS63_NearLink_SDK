/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: for secure boot.
 *
 * Create: 2023-02-23
*/

#include "drv_common_pke.h"
#include "crypto_drv_common.h"
#include "ecc_ec_fp.h"

td_s32 crypto_drv_pke_common_ecc_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    td_u32 local_cnt = local_step(0);
    const drv_pke_ecc_curve *ecc;
    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data pp = {0};
    drv_pke_data cc = {0};
    drv_pke_data dd = {0};
    union {
        td_u8 s_inv[DRV_PKE_LEN_576];
        td_u8 cy[DRV_PKE_LEN_576];
    } pke_data_1;
    td_u8 u1[DRV_PKE_LEN_576];
    td_u8 u2[DRV_PKE_LEN_576];
    td_u8 *v = pke_data_1.cy;
    drv_pke_data used_hash = {0};

    crypto_drv_func_enter();

    drv_crypto_pke_check_param(curve_type == DRV_PKE_ECC_TYPE_SM2 || curve_type == DRV_PKE_ECC_TYPE_RFC7748 ||
                               curve_type == DRV_PKE_ECC_TYPE_RFC7748_448 || curve_type == DRV_PKE_ECC_TYPE_RFC8032);
    drv_crypto_pke_check_param(pub_key == TD_NULL || pub_key->x == TD_NULL ||
        pub_key->y == TD_NULL || pub_key->length == 0);
    drv_crypto_pke_check_param(hash == TD_NULL || hash->data == TD_NULL || hash->length == 0);
    drv_crypto_pke_check_param(sig == TD_NULL || sig->r == TD_NULL || sig->s == TD_NULL);
    ecc = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ecc == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    drv_crypto_pke_check_param(ecc->ksize != pub_key->length);
    drv_crypto_pke_check_param(ecc->ksize != sig->length);
    /* increase security strength check. for nist-fips-224, it's the same security strength with SHA224 */
    drv_crypto_pke_check_param((hash->length < ecc->ksize) &&
        !(hash->length == HASH_SIZE_SHA_224 && ecc->ksize == DRV_PKE_LEN_224) &&
        !(hash->length == HASH_SIZE_SHA_512 && (ecc->ksize == DRV_PKE_LEN_521 || ecc->ksize == DRV_PKE_LEN_576)));
    /* Need to truncate digest if it is too long. */
    if (curve_type == DRV_PKE_ECC_TYPE_FIPS_P224R) {
        used_hash = (drv_pke_data) {.data = hash->data, .length = crypto_min(hash->length, HASH_SIZE_SHA_224)};
    } else {
        used_hash = (drv_pke_data) {.data = hash->data, .length = crypto_min(hash->length, ecc->ksize)};
    }

    /* Step 1: check whether r is [1, n-1] or s is [1, n-1]. */
    crypto_timer_start(TIMER_ID_0, __func__);
    ret = crypto_drv_pke_common_range_check(sig->r, ecc->n, ecc->ksize);
    crypto_chk_func_return(crypto_drv_pke_common_range_check, ret);
    local_cnt += LOCAL_STEP_AUTH;
    ret = crypto_drv_pke_common_range_check(sig->s, ecc->n, ecc->ksize);
    crypto_chk_func_return(crypto_drv_pke_common_range_check, ret);
    local_cnt += LOCAL_STEP_AUTH;
    crypto_timer_end(TIMER_ID_0, "Step 1 crypto_drv_pke_common_range_check");

    /* Step 1.1: set initial parameters into DRAM. */
    ret = crypto_drv_pke_common_init_param(ecc);
    crypto_chk_func_return(crypto_drv_pke_common_init_param, ret);
    crypto_timer_end(TIMER_ID_0, "Step 1 hal_pke_set_init_param");

    /* Step 1.2: set montgomery data into register at first. */
    pp = (drv_pke_data) {.data = (td_u8 *)ecc->n, .length = ecc->ksize};
    ret = crypto_drv_pke_common_set_ecc_mont_param(&pp);
    crypto_chk_func_return(crypto_drv_pke_common_set_ecc_mont_param, ret);
    local_cnt += LOCAL_STEP_AUTH;
    crypto_timer_end(TIMER_ID_0, "Step 1 crypto_drv_pke_common_set_ecc_mont_param");

    /* Step 2: calculate s_inv */
    aa = (drv_pke_data) {.data = sig->s, .length = sig->length};
    pp = (drv_pke_data) {.data = (td_u8 *)ecc->n, .length = ecc->ksize};
    cc = (drv_pke_data) {.data = pke_data_1.s_inv, .length = ecc->ksize};
    ret = ecc_ecfn_inv(sec_arg_add_cs(&aa, &pp, &cc));
    crypto_chk_func_return(ecc_ecfn_inv, ret);
    local_cnt += LOCAL_STEP_AUTH;

    crypto_timer_end(TIMER_ID_0, "Step 2 ecc_ecfn_inv");
    /* Step 3: calculate u1 = s_inv * e mod n, u2 = s_inv * r mod n */
    /* montgomery multiplication modulurs and twice modulo reduction. */
    aa = (drv_pke_data) {.data = sig->r, .length = sig->length};
    bb = (drv_pke_data) {.data = u1, .length = ecc->ksize};
    dd = (drv_pke_data) {.data = u2, .length = ecc->ksize};
    ret = ecc_ecfn_verify_u(sec_arg_add_cs(&cc, &used_hash, &aa, &pp, &bb, &dd));
    crypto_chk_func_return(ecc_ecfn_verify_u, ret);
    local_cnt += LOCAL_STEP_AUTH;
    crypto_timer_end(TIMER_ID_0, "Step 3 ecc_ecfn_verify_u");

    /* Step 3.2: update montgomery parameters. */
    pp = (drv_pke_data) {.data = (td_u8 *)ecc->p, .length = ecc->ksize};
    ret = crypto_drv_pke_common_set_ecc_mont_param(&pp);
    crypto_chk_func_return(crypto_drv_pke_common_set_ecc_mont_param, ret);

    crypto_timer_end(TIMER_ID_0, "Step 3 crypto_drv_pke_common_set_ecc_mont_param");
    /* Step 4: calaulate R = u1 * G + u2 * Q, which is in finite field fp, so it needs to set new modulur p. */
    ret = ecc_ecfp_mul_add(sec_arg_add_cs(ecc, &bb, &dd, pub_key, TD_NULL));
    crypto_chk_func_return(ecc_ecfp_mul_add, ret);
    local_cnt += LOCAL_STEP_AUTH;

    crypto_timer_end(TIMER_ID_0, "Step 4 ecc_ecfp_mul_add");
    /* Step 5: out.v == sig.r ? */
    ret = ecc_ecfn_verify_v(ecc, sig, v);
    crypto_chk_func_return(ecc_ecfn_verify_v, ret);
    local_cnt += LOCAL_STEP_AUTH;

    crypto_timer_end(TIMER_ID_0, "Step 5 ecc_ecfn_verify_v");
    /* Step 6: clear data in the DRAM. */

    /* secure enhancement */
    val_enhance_chk(local_cnt, local_step(7)); /* 7 steps total */

    crypto_timer_print_all();
    crypto_drv_func_exit();
    return TD_SUCCESS;
}