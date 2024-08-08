/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ed25519 related functions head file.
 *
 * Create: 2022-10-31
*/

#ifndef ED_EC_FP_H
#define ED_EC_FP_H

#include "crypto_pke_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief R = k * G, point multiplication.
 * @param ecc input ecc curve param.
 * @param k the scalar.
 * @param p the input point.
 * @param r the output point.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ed_ecfp_mul_naf(const drv_pke_ecc_curve *ecc, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r CIPHER_CHECK_WORD);

/**
 * @brief R = k * G, point multiplication. ed inner function, while the G is the default point G(gx, gy). And before
 * call this function, the curve init parameters should have been set into PKE DRAM.
 * @param work_len work_len = align_len / ALIGNED_TO_WORK_LEN_IN_BYTE.
 * @param k the scalar.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ed_ecfp_mul_naf_cal(td_u32 work_len,  const drv_pke_data *k);

/**
 * @brief R = k * u, point multiplication with point compress.
 * @param k the scalar.
 * @param u the input point.
 * @param p the modulur.
 * @param r the output point.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 point_mul_compress(const drv_pke_data *k, const drv_pke_ecc_point *u, const drv_pke_data *p,
    const drv_pke_ecc_point *r);

/**
 * @brief Qu = d * G, get public key from private key.
 * @param ecc the curve parameter.
 * @param priv_key the input private key.
 * @param pub_key the output public key.
 */
td_s32 ed_secret_to_public(const drv_pke_ecc_curve *ecc, const drv_pke_data *priv_key,
    const drv_pke_ecc_point *pub_key);

/**
 * @brief calculate the sha512 result of three segment buffer and modulo the result by q.
 * @param q the modulus.
 * @param in1 the first segment.
 * @param in2 the second segment.
 * @param in3 the third segment.
 * @param out the result.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 sha512_modq(const drv_pke_data *q, const drv_pke_data *in1, const drv_pke_data *in2,
    const drv_pke_data *in3, const drv_pke_data *out);

/**
 * @brief multi-multiplication to calculate the signature s. s = (r + h * sk) mod n.
 * @param sk the first 32 bytes of the decoded private key.
 * @param r the result of sha512 modulo L, r = sha512(prefix || msg) mod n.
 * @param h the result of sha512 modulo L, h = sha512(R || A || msg) mod n. R and A is the encoded rG and pubkey.
 * @param n the group order of G.
 * @param s the signature result.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ed_ecfn_sign_s(const drv_pke_data *sk, const drv_pke_data *r, const drv_pke_data *h,
    const drv_pke_data *n, const drv_pke_data *s);

/**
 * @brief calculate sB - hA for ed verify. the result is in ed_addr_cx, ed_addr_cy and ed_addr_cz, which is in Jacobin
 * coordinate.
 * @attention Need to set rrp and use p to update montgomery parameter first.
 * @param ecc the input ed curve parameters.
 * @param s the decoded signature value.
 * @param h the result of sha512 modulo L, h = sha512(R || A || msg) mod n. R and A is the encoded rG and pubkey.
 * @param a the decoded public key.
 * @param r the output result.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ed_mul_sub(const drv_pke_ecc_curve *ecc, const drv_pke_data *s, const drv_pke_data *h,
    const drv_pke_ecc_point *a, const drv_pke_ecc_point *r);

/**
 * @brief private key expand function. from 32 byte private key get 64 byte output. (a, prefix) = sha512(secret).
 * @param secret the input private key.
 * @param a the first 32 byte of the sha512(secret).
 * @param prefix the last 32 byte of the sha512(secret).
 * @param len the byte length of the input private key.
 * @return td_s32
 */
td_s32 secret_expand(const td_u8 *secret, td_u8 *a, td_u8 *prefix, const td_u32 len);

/**
 * @brief point decompress, from little endian data get real point by curve equation.
 * @attention Need to set rrp and use p to update montgomery parameter first.
 * @param enc_pkey the encoded point value.
 * @param p the module.
 * @param pkey the the decoded point.
 * @return td_s32
 */
td_s32 point_decompress(const drv_pke_data *enc_pkey, const drv_pke_data *p, const drv_pke_ecc_point *pkey);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* ED_EC_FP_H */
