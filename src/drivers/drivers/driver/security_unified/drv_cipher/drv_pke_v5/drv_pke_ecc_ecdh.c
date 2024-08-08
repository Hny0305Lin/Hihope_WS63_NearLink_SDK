/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ecc ecdh function implement
 *
 * Create: 2023-05-23
*/

#include "crypto_osal_adapt.h"
#include "drv_common.h"
#include "drv_common_pke.h"
#include "ecc_ec_fp.h"
#include "curve_ec_fp.h"
#include "drv_pke.h"

CRYPTO_STATIC td_s32 pke_ecc_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key CIPHER_CHECK_WORD);

CRYPTO_STATIC td_s32 pke_ecc_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key CIPHER_CHECK_WORD)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u32 local_cnt = local_step(0);
    td_u8 temp[ECC_KEY_LEN_IN_BYTE];
    const drv_pke_ecc_curve *ecc;
    drv_pke_ecc_point rr = {0};

    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), curve_type, input_pub_key, input_priv_key,
        output_shared_key);
    crypto_chk_return((curve_type == DRV_PKE_ECC_TYPE_RFC8032), PKE_COMPAT_ERRNO(ERROR_UNSUPPORT),
        "unsupport ecc type\n");
    drv_crypto_pke_check_param(input_pub_key == TD_NULL);
    drv_crypto_pke_check_param(input_pub_key->x == TD_NULL);
    drv_crypto_pke_check_param(curve_type != DRV_PKE_ECC_TYPE_RFC7748 &&
                               curve_type != DRV_PKE_ECC_TYPE_RFC7748_448 && input_pub_key->y == TD_NULL);
    drv_crypto_pke_check_param(input_priv_key == TD_NULL);
    drv_crypto_pke_check_param(input_priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(output_shared_key == TD_NULL);
    drv_crypto_pke_check_param(output_shared_key->data == TD_NULL);
    ecc = crypto_drv_pke_common_get_ecc_curve(curve_type);
    crypto_chk_return(ecc == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");
    drv_crypto_pke_check_param(ecc->ksize != input_priv_key->length);
    drv_crypto_pke_check_param(ecc->ksize != input_pub_key->length);
    drv_crypto_pke_check_param(ecc->ksize != output_shared_key->length);

    /* output_shared_key = input_priv_key *  input_pub_key */
    ret = memset_enhance(temp, ECC_KEY_LEN_IN_BYTE, 0x00, ECC_KEY_LEN_IN_BYTE);
    crypto_chk_return((ret != TD_SUCCESS), ret, "memset_enhance failed, ret = 0x%x\n", ret);
    if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
        rr = (drv_pke_ecc_point) {.length = ecc->ksize, .x = output_shared_key->data, .y = TD_NULL};
        ret = curve_ecfp_mul_dot(sec_arg_add_cs(ecc, input_priv_key, input_pub_key, &rr));
        crypto_chk_func_return(curve_ecfp_mul_dot, ret);
    } else {
        rr = (drv_pke_ecc_point) {.length = ecc->ksize, .x = output_shared_key->data, .y = temp};
        ret = TD_FAILURE;
        ret = ecc_ecfp_mul_naf(sec_arg_add_cs(ecc, input_priv_key, input_pub_key, &rr));
        crypto_chk_func_return(ecc_ecfp_mul_naf, ret);
    }
    local_cnt += LOCAL_STEP_AUTH;           // local_cnt = 1

    val_enhance_chk(local_cnt, local_step(1));  // 1 steps total

    return TD_SUCCESS;
}

td_s32 drv_cipher_pke_ecc_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key)
{
    td_s32 ret = TD_FAILURE;
    ret = crypto_drv_pke_common_resume();
    crypto_chk_func_return(crypto_drv_pke_common_resume, ret);
    ret = pke_ecc_gen_ecdh_key(sec_arg_add_cs(curve_type, input_pub_key, input_priv_key, output_shared_key));
    crypto_drv_pke_common_suspend();
    return ret;
}