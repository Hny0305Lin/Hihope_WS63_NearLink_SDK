/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the ecc drv common algorithm head file. The data will transfor by the func param and will be set into
 * DRAM in the algorithms.
 *
 * Create: 2022-08-15
*/

#ifndef ECC_EC_FP_H
#define ECC_EC_FP_H

#include "crypto_pke_struct.h"
#include "drv_pke_inner.h"
#include "crypto_osal_lib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* The whole DRAM is 4KB, and there are 128 blocks, each block is 256bits. For ecc, it needs to support 521bits, so
there should be 3 block DRAM used for calculate. */

/* * struct of ecc point */
typedef struct {
    td_u8 *x;   /* X coordinates of the point in Jacobian coordinate system. */
    td_u8 *y;   /* Y coordinates of the point in Jacobian coordinate system. */
    td_u8 *z;   /* Z coordinates of the point in Jacobian coordinate system. */
    td_u32 length;
} pke_ecc_point_jac;

/**
 * @brief Get final calculate result. trans (ecc_addr_cx, ecc_addr_cy, ecc_addr_cz) from jac to aff.
 * aff_x = jac_x * ((jac_z)^-2); aff_y = jac_y * ((jac_z)^-3)
 *
 * @param work_len aligned_len work_len = celing(bitWidth/64).
 * @param mod_p the consistent of modulus n, n = p * q.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_jac_to_aff(const drv_pke_data *mod_p);

td_s32 ecc_ecfp_demontgomery_data_jac_z(const drv_pke_data *z, td_u32 work_len);

/**
 * @brief trans coordinate system from affine to jacobin, before call this API, you should have set mont_1_p and const_0
 * into ecc_addr_mont_1_p and ecc_addr_const_0.
 * (ecc_addr_px, ecc_addr_py) -> (ecc_addr_cx, ecc_addr_cy, ecc_addr_cz).
 *
 * @param in the input point in affine system.
 * @param mod_p the modulur.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_aff_to_jac(const drv_pke_ecc_point *in, const drv_pke_data *mod_p, pke_ecc_point_jac *out);

/**
 * @brief montgomery the point coordinate in affine coordinate system.
 *
 * @param in intput point in affine coordinate system.
 * @param mod_p modulus.
 * @param out output point in affine coordinate system.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_montgomery_data_aff_backup(const drv_pke_ecc_point *in, const drv_pke_data *mod_p,
    const drv_pke_ecc_point *out, const uintptr_t check_word);

/**
 * @brief montgomery the point coordinate in affine coordinate system. Before call this API, you should have set curve
 * initial parameters into the DRAM.
 *
 * @param in intput point in affine coordinate system.
 * @param mod_p modulus.
 * @param out output point in affine coordinate system.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_montgomery_data_aff(const drv_pke_ecc_point *in, const drv_pke_data *mod_p,
    const drv_pke_ecc_point *out CIPHER_CHECK_WORD);

/**
 * @brief demontgomery data, (ecc_addr_cx, ecc_addr_cy) -> (ecc_addr_cx, ecc_addr_cy). Before calculate demont, you
 * should have set const_1 to ecc_addr_const_1.
 *
 * @param r store the result.
 * @param work_len work_len = celing(aligned_len/64).
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_demontgomery_data_aff(const drv_pke_ecc_point *r, td_u32 work_len);

/**
 * @brief R = k * G, point multiplication.
 *
 * @param ecc input ecc curve param.
 * @param k the scalar.
 * @param p the input point.
 * @param r the output point.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_mul_naf(const drv_pke_ecc_curve *ecc, const drv_pke_data *k,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *r CIPHER_CHECK_WORD);

/**
 * @brief R = k * G, point multiplication. Before call this API, you should have set curve initial parameters, and set
 * point coordinate into the DRAM.
 *
 * @param work_len work_len = align_len / ALIGNED_TO_WORK_LEN_IN_BYTE.
 * @param k the scalar.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_mul_naf_cal(td_u32 work_len, const drv_pke_data *k);

/**
 * @brief R = u1 * G + u2 * Q. Before call this API, you should have set curve initial parameters into DRAM,
 * which must include ecc_addr_mont_a, ecc_addr_mont_1_p, ecc_addr_const_1, ecc_addr_const_0.
 *
 * @param ecc input ecc curve param.
 * @param u1 the scalar.
 * @param u2 the scalar.
 * @param q the input point(public key), which is in affine coordinate system.
 * @param r the output point. result is in (ecc_addr_cx, ecc_addr_cy, ecc_addr_cz).
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_mul_add(const drv_pke_ecc_curve *ecc, const drv_pke_data *u1, const drv_pke_data *u2,
    const drv_pke_ecc_point *q, const drv_pke_ecc_point *r CIPHER_CHECK_WORD);

/**
 * @brief c = ~a mod n, ecc_addr_s = 1/ecc_addr_k. Before call this API, you should have set curve initial parameters
 * into DRAM, which must include ecc_addr_rrn, ecc_addr_mont_1_n.
 *
 * @param a the input data.
 * @param n the modulus.
 * @param c the output data.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfn_inv(const drv_pke_data *a, const drv_pke_data *n, const drv_pke_data *c CIPHER_CHECK_WORD);
/************************************************** inner calculate API start************************************/
/**
 * @brief Get the random key object range in [1, n-1]
 *
 * @param n range.
 * @param size the byte length of number n.
 * @param rand the output random data.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 get_random_key(const td_u8 *n, const td_u32 size, td_u8 *rand CIPHER_CHECK_WORD);

/**
 * @brief update modulus into ecc_addr_m, and set new montgomery parameter into reg. Before call this API, you should
 * have set initial parameters into PKE DRAM.
 *
 * @param n the new modulus
 * @param n_len the byte length of new modulus.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 update_modulus(const td_u8 *n, const td_u32 n_len CIPHER_CHECK_WORD);

/**
 * @brief calculate the s value of signature. s = k_inv * (e + d * r) mod n.
 * montgomery multiplication modulurs and twice modulo reduction to demontgomery, and make sure s, r in [1, n-1]
 * Before call this API, you should have set curve initial parameters into DRAM,
 * which must include ecc_addr_rrn, ecc_addr_const_1.
 *
 * @param k_inv the result of 1/k.
 * @param e the input hash result of message.
 * @param d the input private key.
 * @param r the r value of signature.
 * @param n the modulus.
 * @param s the output data s of signature.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfn_sign_s(const drv_pke_data *k_inv, const drv_pke_data *e, const drv_pke_data *d,
    const drv_pke_data *r, const drv_pke_data *n, const drv_pke_data *s CIPHER_CHECK_WORD);

/**
 * @brief calculate the u1 = s_inv * e mod n & u2 = s_inv * r mod n.
 * montgomery multiplication modulurs and twice modulo reduction to demontgomery.
 * Before call this API, you should have set curve initial parameters into DRAM,
 * which must include ecc_addr_rrn, ecc_addr_const_1.
 *
 * @param s_inv the input modular inverse of s value.
 * @param e the input hash result of message.
 * @param r the r value of signature.
 * @param n the modulus.
 * @param u1 the output value u1 = s_inv * e.
 * @param u2 the output value u2 = s_inv * r mod n.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfn_verify_u(const drv_pke_data *s_inv, const drv_pke_data *e, const drv_pke_data *r,
    const drv_pke_data *n, const drv_pke_data *u1, const drv_pke_data *u2 CIPHER_CHECK_WORD);

/**
 * @brief verify whether the v == r.
 *
 * @param ecc the input ecc curve parameter.
 * @param sig the signature to be verified.
 * @param out_v the calculated verify value.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfn_verify_v(const drv_pke_ecc_curve *ecc, const drv_pke_ecc_sig *sig, td_u8 *out_v);

/**
 * @brief the standard method to check whether the point is on the curve.
 *
 * @param ecc the input ecc curve parameter.
 * @param pub_key the input public key to be checked.
 * @param is_on_curve the output result, true: is on the curve, false: is not on the curve.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfp_point_valid_standard(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve CIPHER_CHECK_WORD);

/**
 * @brief calculate modular addition on prime n domain. Here use instr_sm2_verify_t_3 as common add_mod calculate to
 * calculate both ecc and sm2 signature value r.
 *
 * @param a the first data.
 * @param b the second data.
 * @param p the module.
 * @param c the output result.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 ecc_ecfn_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD);

/**
 * @brief calculate s for sm2 signature. s = (1 + d) * (k - r *d) mod n. and the output is the demontgomeried r & s.
 *
 * @param k the input data k.
 * @param d the input private key.
 * @param n the order of G point of the curve.
 * @param r the input r of signature and the output r(demontgomeried) of signature.
 * @param s the result s(demontgomeried) of the signature.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 sm2_ecfn_sign_s(const drv_pke_data *k, const drv_pke_data *d, const drv_pke_data *n,
    const drv_pke_data *r, const drv_pke_data *s);

td_s32 sm2_ecfn_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c, uintptr_t const check_word);

/**
 * @brief calculate r` = e + Rx mod n, and check whether r` -r == 0?
 *
 * @param rx the input Rx.
 * @param hash the input hash value.
 * @param r the input r of signature.
 * @param n the input modulo.
 * @param out the output value of r` - r.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 sm2_ecfn_verify_r(const drv_pke_data *rx, const drv_pke_data *hash, const drv_pke_data *r,
    const drv_pke_data *n, const drv_pke_data *out);
/**
 * @brief calculate hash for sm2.
 *
 * @param param the other data that is used for sm2 hash calculate.
 * @param msg the input message that need to be calculate hash.
 * @param hash the hash result.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 sign_verify_hash(const sm2_sign_verify_hash_pack *param, const drv_pke_msg *msg,
    drv_pke_data *hash CIPHER_CHECK_WORD);
/************************************************** inner calculate API end************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif