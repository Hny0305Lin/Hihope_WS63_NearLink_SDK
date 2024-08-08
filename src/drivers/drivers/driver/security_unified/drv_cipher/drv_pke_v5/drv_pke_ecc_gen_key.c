/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc generate key function implement.
 *
 * Create: 2023-05-23
*/

#include "crypto_osal_adapt.h"
#include "drv_common.h"
#include "drv_trng.h"
#include "drv_common_pke.h"
#include "ecc_ec_fp.h"
#include "ed_ec_fp.h"
#include "curve_ec_fp.h"
#include "drv_pke.h"

CRYPTO_STATIC td_s32 pke_ecc_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key CIPHER_CHECK_WORD);

CRYPTO_STATIC td_s32 pke_ecc_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key CIPHER_CHECK_WORD)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u8 temp[ECC_KEY_LEN_IN_BYTE];
    const drv_pke_ecc_curve *ecc;
    td_u32 local_cnt = local_step(0);
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), curve_type, input_priv_key, output_priv_key,
        output_pub_key);
    drv_crypto_pke_check_param(output_priv_key == TD_NULL);
    drv_crypto_pke_check_param(output_priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(output_pub_key == TD_NULL);

    ecc = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ecc == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    drv_crypto_pke_check_param(input_priv_key != TD_NULL && ecc->ksize != input_priv_key->length);
    drv_crypto_pke_check_param(ecc->ksize != output_priv_key->length);
    drv_crypto_pke_check_param(ecc->ksize != output_pub_key->length);

    drv_crypto_pke_check_param(curve_type != DRV_PKE_ECC_TYPE_RFC8032 && output_pub_key->x == TD_NULL);
    drv_crypto_pke_check_param(curve_type != DRV_PKE_ECC_TYPE_RFC7748 && curve_type != DRV_PKE_ECC_TYPE_RFC7748_448 &&
        output_pub_key->y == TD_NULL);

    drv_pke_ecc_point g_point = gen_pke_point(ecc->ksize, (td_u8 *)ecc->gx, (td_u8 *)ecc->gy);

    if ((input_priv_key != TD_NULL) && (input_priv_key->data != TD_NULL)) {
        if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448 ||
            curve_type == DRV_PKE_ECC_TYPE_RFC8032) {
            local_cnt += LOCAL_STEP_AUTH;
        } else {
            ret = TD_FAILURE;
            /* for that there only have 1 < k < n-1 effective points, while k == n, it's infinite point.
               check 1<=input_priv_key<=n-1 */
            ret = crypto_drv_pke_common_range_check(input_priv_key->data, ecc->n, ecc->ksize);
            if (ret == TD_SUCCESS) {
                local_cnt += LOCAL_STEP_AUTH;
            } else {
                crypto_print_func_err(crypto_drv_pke_common_range_check, ret);
                return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
            }
        }

        ret = memcpy_enhance(output_priv_key->data, output_priv_key->length, input_priv_key->data, ecc->ksize);
        crypto_chk_return((ret != TD_SUCCESS), ret, "memcpy_enhance failed, ret = 0x%x\n", ret);
    } else {
        /* if not given private key, then generate a random private key. */
        /* Generate key: 1 <= key <= n-1 */
        /* WARN: here only limit the key of RFC5639 in [1, n-1], but not for RFC7748 & RFC8032, why? */
        if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448 ||
            curve_type == DRV_PKE_ECC_TYPE_RFC8032) {
            /* for curve25519, the cofactor h is not 0x1, so that the private key is not simplely limited in [1, n-1] */
            ret = memset_enhance(temp, ECC_KEY_LEN_IN_BYTE, 0x00, ECC_KEY_LEN_IN_BYTE);
            crypto_chk_return((ret != TD_SUCCESS), ret, "memset_enhance failed, ret = 0x%x\n", ret);

            ret = drv_cipher_trng_get_multi_random(ecc->ksize, temp);
            crypto_chk_func_return(drv_cipher_trng_get_random, ret);

            ret = TD_FAILURE;
            ret = memcpy_enhance(output_priv_key->data, output_priv_key->length, temp, ecc->ksize);
            crypto_chk_func_return(memcpy_enhance, ret);
        } else {
            ret = TD_FAILURE;
            ret = get_random_key(sec_arg_add_cs(ecc->n, ecc->ksize, output_priv_key->data));
            crypto_chk_func_return(get_random_key, ret);
        }

        local_cnt += LOCAL_STEP_AUTH;           // local_cnt = 1
    }

    /* PA = dA * G */
    ret = TD_FAILURE;
    if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
        /* Step 1: set curve initial parameters(a24, etc.) into PKE DRAM for later calculation. */
        ret = curve_ecfp_mul_dot(sec_arg_add_cs(ecc, output_priv_key, &g_point, output_pub_key));
        crypto_chk_func_return(curve_ecfp_mul_dot, ret);
    } else if (curve_type == DRV_PKE_ECC_TYPE_RFC8032) {
        ret = ed_secret_to_public(ecc, output_priv_key, output_pub_key);
        crypto_chk_func_return(ed_secret_to_public, ret);
    } else {
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(ecc, output_priv_key, &g_point, output_pub_key));
        crypto_chk_func_return(ecc_ecfp_mul_naf, ret);
    }
    local_cnt += LOCAL_STEP_AUTH;

    val_enhance_chk(local_cnt, local_step(2));  /* 2 steps total */

    return TD_SUCCESS;
}

td_s32 drv_cipher_pke_ecc_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = pke_ecc_gen_key(sec_arg_add_cs(curve_type, input_priv_key, output_priv_key, output_pub_key));
    crypto_drv_pke_common_suspend();
    return ret;
}