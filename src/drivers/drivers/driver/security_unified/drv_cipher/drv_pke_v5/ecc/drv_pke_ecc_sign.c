/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc sign function implement.
 *
 * Create: 2023-05-19
*/

#include "drv_common_pke.h"
#include "crypto_drv_common.h"
#include "ecc_ec_fp.h"

td_s32 crypto_drv_pke_common_ecc_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    td_s32 ret = TD_FAILURE;
    td_u32 i = 0;
    td_u32 k = 0;
    const drv_pke_ecc_curve *ecc;
    td_u32 local_cnt = local_step(0);
    drv_pke_data used_hash = {0};

    drv_crypto_pke_check_param(curve_type == DRV_PKE_ECC_TYPE_SM2 || curve_type == DRV_PKE_ECC_TYPE_RFC7748 ||
        curve_type == DRV_PKE_ECC_TYPE_RFC7748_448 || curve_type == DRV_PKE_ECC_TYPE_RFC8032);
    drv_crypto_pke_check_param(priv_key == TD_NULL || priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(hash == TD_NULL || hash->data == TD_NULL || hash->length == 0);
    drv_crypto_pke_check_param(sig == TD_NULL || sig->r == TD_NULL || sig->s == TD_NULL);
    ecc = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ecc == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    drv_crypto_pke_check_param(ecc->ksize != priv_key->length);
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

    union {
        td_u8 k[ECC_KEY_LEN_IN_BYTE];               /* step 1~2 and step 6 */
        td_u8 mkni[ECC_KEY_LEN_IN_BYTE];            /* step 6 */
    } pke_data_1;
    union {
        td_u8 rx[ECC_KEY_LEN_IN_BYTE];              /* step 2~3 */
    } pke_data_2;
    union {
        td_u8 ry[ECC_KEY_LEN_IN_BYTE];              /* step 2 */
        td_u8 const_0[ECC_KEY_LEN_IN_BYTE];         /* step 3 */
    } pke_data_3;

    drv_pke_data aa = {0};
    drv_pke_data bb = {0};
    drv_pke_data pp = {0};
    drv_pke_data cc = {0};
    drv_pke_data kk = {.data = pke_data_1.k, .length = ecc->ksize};
    drv_pke_data kk_inv = {.data = pke_data_1.mkni, .length = ecc->ksize};
    drv_pke_data mod_n = {.data = (td_u8 *)ecc->n, .length = ecc->ksize};
    drv_pke_ecc_point gg = {.x = (td_u8 *)ecc->gx, .y = (td_u8 *)ecc->gy, .length = ecc->ksize};
    drv_pke_ecc_point rr = {.x = pke_data_2.rx, .y = pke_data_3.ry, .length = ecc->ksize};

    for (i = 0, k = 0; i < PKE_MAX_TIMES; i++) {
        /* Step 1: get random k, 1 <= k <= n-1; */
        ret = get_random_key(sec_arg_add_cs(ecc->n, ecc->ksize, pke_data_1.k));
        crypto_chk_func_return(get_random_key, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 2: R(x1, y1) = k * G, outPut point is in the affine coordinate system. */
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(ecc, &kk, &gg, &rr));
        crypto_chk_func_return(ecc_ecfp_mul_naf, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 3: r = (rx + 0) mod n */
        /* Step 3.1 copy n to address 0 to switch the modulur to n and set new montgomery parameter. */
        ret = update_modulus(sec_arg_add_cs(ecc->n, ecc->ksize));
        crypto_chk_func_return(update_modulus, ret);
        /* Step 3.2 use n as modulus to calculate r. And here will set n into address 0 */
        ret = memset_enhance(pke_data_3.const_0, ECC_KEY_LEN_IN_BYTE, 0x00, ECC_KEY_LEN_IN_BYTE);
        crypto_chk_func_return(memset_enhance, ret);
        aa = (drv_pke_data) {.data = pke_data_3.const_0, .length = ecc->ksize};
        bb = (drv_pke_data) {.data = pke_data_2.rx, .length = ecc->ksize};
        pp = (drv_pke_data) {.data = (td_u8 *)ecc->n, .length = ecc->ksize};
        cc = (drv_pke_data) {.data = (td_u8 *)sig->r, .length = ecc->ksize};
        ret = ecc_ecfn_add_mod(sec_arg_add_cs(&aa, &bb, &pp, &cc));
        crypto_chk_func_return(ecc_ecfn_add_mod, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 4: check r = 0 ? */
        if (crypto_drv_pke_common_is_zero(sig->r, ecc->ksize) == TD_TRUE) {
            // if 0, continue generate k, and calculate r
            continue;
        }

        /* Step 5: e = hash(M), here the input is hash of message. */

        /* Step 6: calculate 1/k */
        ret = ecc_ecfn_inv(sec_arg_add_cs(&kk, &pp, &kk_inv));
        crypto_chk_func_return(ecc_ecfn_inv, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 7: calculate s = k_inv * (e + d * r) mod n. */
        bb = (drv_pke_data) {.data = sig->s, .length = ecc->ksize};
        ret = ecc_ecfn_sign_s(sec_arg_add_cs(&kk_inv, &used_hash, priv_key, &cc, &mod_n, &bb));
        crypto_chk_func_return(ecc_ecfn_sign_s, ret);
        local_cnt += LOCAL_STEP_AUTH;

        /* Step 8: check s = 0? */
        if (crypto_drv_pke_common_is_zero(sig->s, ecc->ksize) != TD_TRUE) {
            // if not 0, finish loop.
            break;
        }
        k++;
    }

    if (i != 0) {
        crypto_log_trace("Generate r & s times: %d", i);
    }

    if (i >= PKE_MAX_TIMES) {
        crypto_log_err("Generate r & s timeout!\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    val_enhance_chk(local_cnt, local_step((i + 1) * 3 + (k + 1) * 2)); // (i+1)*3 for r, (k+1)*2 for s steps total
    return TD_SUCCESS;
}