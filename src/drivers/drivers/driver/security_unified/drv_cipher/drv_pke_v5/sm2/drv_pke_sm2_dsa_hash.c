/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm2 hash calculate function implement.
 *
 * Create: 2023-05-19
*/

#include "drv_common_pke.h"
#include "crypto_drv_common.h"
#include "ecc_ec_fp.h"

#define PKE_HI_U16_MAX              0xFFFF
#define ENTLA_LEN                   2
#define ID_BYTE_BITS                8

/*
 * inner function API start
*/
CRYPTO_STATIC td_s32 inner_sign_verify_hash(const sm2_sign_verify_hash_pack *param, const drv_pke_msg *msg,
    drv_pke_data *hash CIPHER_CHECK_WORD);

CRYPTO_STATIC td_s32 inner_sign_verify_hash(const sm2_sign_verify_hash_pack *param, const drv_pke_msg *msg,
    drv_pke_data *hash CIPHER_CHECK_WORD)
{
    check_sum_inspect(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), param, msg, hash);
    drv_crypto_pke_check_param(param == TD_NULL);
    drv_crypto_pke_check_param(param->px == TD_NULL);
    drv_crypto_pke_check_param(param->py == TD_NULL);
    drv_crypto_pke_check_param(param->id == TD_NULL);
    drv_crypto_pke_check_param(msg == TD_NULL);
    drv_crypto_pke_check_param(msg->data == TD_NULL);
    drv_crypto_pke_check_param(hash == TD_NULL);
    drv_crypto_pke_check_param(hash->data == TD_NULL);
    const drv_pke_ecc_curve *sm2 = crypto_drv_pke_common_get_ecc_curve(DRV_PKE_ECC_TYPE_SM2);
    crypto_chk_return(sm2 == TD_NULL, PKE_COMPAT_ERRNO(ERROR_UNSUPPORT), "unsupport alg\n");

    volatile td_s32 ret = TD_FAILURE;
    td_u8 entla[ENTLA_LEN];
    td_u8 h[DRV_PKE_SM2_LEN_IN_BYTES];
    drv_pke_data hh = {DRV_PKE_SM2_LEN_IN_BYTES, h};

    if (param->id_len > PKE_HI_U16_MAX / ID_BYTE_BITS) {
        crypto_log_err("ID length in bit cannot present in 2 Bytes.\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    entla[0] = (td_u8)((param->id_len * ID_BYTE_BITS) >> ID_BYTE_BITS);
    entla[1] = (td_u8)((param->id_len * ID_BYTE_BITS));

    /* *** ZA=H256(ENTLA||IDA||a||b||xG||yG||xA||yA) *** */
    const drv_pke_data arr[] = {
        {ENTLA_LEN, entla},
        {param->id_len, param->id},
        {DRV_PKE_SM2_LEN_IN_BYTES, (td_u8 *)sm2->a},
        {DRV_PKE_SM2_LEN_IN_BYTES, (td_u8 *)sm2->b},
        {DRV_PKE_SM2_LEN_IN_BYTES, (td_u8 *)sm2->gx},
        {DRV_PKE_SM2_LEN_IN_BYTES, (td_u8 *)sm2->gy},
        {DRV_PKE_SM2_LEN_IN_BYTES, (td_u8 *)param->px},
        {DRV_PKE_SM2_LEN_IN_BYTES, (td_u8 *)param->py}
    };
    ret = TD_FAILURE;
    ret = crypto_drv_pke_common_calc_hash(arr, sizeof(arr) / sizeof(drv_pke_data), DRV_PKE_HASH_TYPE_SM3, &hh);
    crypto_chk_func_return(crypto_drv_pke_common_calc_hash, ret);

    /* *** e=H256(ZA||M) *** */
    const drv_pke_data arr2[] = {
        {DRV_PKE_SM2_LEN_IN_BYTES, h},
        {msg->length, msg->data}
    };
    ret = crypto_drv_pke_common_calc_hash(arr2, sizeof(arr2) / sizeof(drv_pke_data), DRV_PKE_HASH_TYPE_SM3, hash);
    crypto_chk_func_return(crypto_drv_pke_common_calc_hash, ret);

    return TD_SUCCESS;
}
/*
 * inner function API end
*/

td_s32 crypto_drv_pke_common_sm2_dsa_hash(const drv_pke_data *sm2_id, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_data *hash)
{
    volatile td_s32 ret = TD_FAILURE;
    sm2_sign_verify_hash_pack hash_param = {0};
    drv_pke_data aa = {0};
    crypto_drv_func_enter();

    drv_crypto_pke_check_param(sm2_id == TD_NULL);
    drv_crypto_pke_check_param(sm2_id->data == TD_NULL);
    drv_crypto_pke_check_param(pub_key == TD_NULL);
    drv_crypto_pke_check_param(pub_key->x == TD_NULL);
    drv_crypto_pke_check_param(pub_key->y == TD_NULL);
    drv_crypto_pke_check_param(msg == TD_NULL);
    drv_crypto_pke_check_param(msg->data == TD_NULL);
    drv_crypto_pke_check_param(hash == TD_NULL);
    drv_crypto_pke_check_param(hash->data == TD_NULL);

    if (hash->length != DRV_PKE_SM2_LEN_IN_BYTES || pub_key->length != DRV_PKE_SM2_LEN_IN_BYTES) {
        crypto_log_err("Invalid hash or key size input!\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    /* e = Hash */
    hash_param = (sm2_sign_verify_hash_pack) {.px = pub_key->x, .py = pub_key->y, .id = sm2_id->data,
                                             .id_len = sm2_id->length};
    aa = (drv_pke_data) {.length = DRV_PKE_SM2_LEN_IN_BYTES, .data = hash->data};
    ret = TD_FAILURE;
    ret = inner_sign_verify_hash(sec_arg_add_cs(&hash_param, msg, &aa));
    crypto_chk_func_return(inner_sign_verify_hash, ret);

    crypto_drv_func_exit();
    return ret;
}