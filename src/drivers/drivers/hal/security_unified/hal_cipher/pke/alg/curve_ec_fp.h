/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the curve drv common algorithm head file. The data will transfor by the func param and will be
 * set into DRAM in the algorithms.
 *
 * Create: 2022-10-27
*/

#ifndef CURVE_EC_FP_H
#define CURVE_EC_FP_H

#include "crypto_pke_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief decode the normal k value. In RFC7748, the k value and point on curve are all encoded.
 *
 * @param output_key the decoded k value, which can be used in calculation directly.
 * @param input_key the input encoded k value.
 * @param klen the byte length of k value.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 decode_priv_key(td_u8 *output_key, const td_u8 *input_key, td_u32 klen);

/**
 * @brief decode the x coordinate of the point. In RFC7748, the k value and point on curve are all encoded.
 *
 * @param output_u_x the decoded x coordinate of the point, which can be used in calculation directly.
 * @param input_u_x the input encoded x coordinate of the point.
 * @param ulen the byte length of x coordinate of the point.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 decode_point_x(td_u8 *output_u_x, const td_u8 *input_u_x, td_u32 ulen);

/**
 * @brief encode the x coordinate of the point. In RFC7748, the k value and point on curve are all encoded.
 *
 * @param output_u_x the encoded x coordinate of the point, which will be transfered to user.
 * @param input_u_x the input x coordinate of the point.
 * @param ulen the byte length of x coordinate of the point.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 encode_point_x(td_u8 *output_u_x, const td_u8 *input_u_x, td_u32 ulen);

/**
 * @brief Rx = k * ux. calculate point multiplication for curve25519 and curve448.
 * before call this API you should have decoded the k value and the x coordinate of the point.
 * after call this API, if you will transfer the rx to user, you should encode the rx by yourself.
 *
 * @param ecc the ecc curve param.
 * @param k the input decoded normal k value.
 * @param ux the intput decoded x coordinate of the point.
 * @param rx the output without encoded x coordinate of the point.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 curve_ecfp_mul(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_data *ux,
    const drv_pke_data *rx);

/**
 * @brief rx = k * px. calculate point multiplication for curve25519 and curve448.
 *
 * @param ecc The ecc curve param.
 * @param k The input decoded normal k value.
 * @param p The intput decoded x coordinate of the point.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 curve_ecfp_mul_dot(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r CIPHER_CHECK_WORD);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CURVE_EC_FP_H */