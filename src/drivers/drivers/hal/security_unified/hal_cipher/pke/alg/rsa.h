/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: RSA algorithm header file.
 *
 * Create: 2022-08-18
*/

#ifndef RSA_H
#define RSA_H

#include "crypto_pke_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief rrn = 2^(2*n_bit_len) mod n.
 *
 * @param n for rsa n = p * q, for ecc it's prime the order of the base point G.
 * @param key_size rsa key length in byte or ecc curve key length in byte.
 * @param rrn the output rrn data. for RSA there is no need to get this data out to reset it into rsa_addr_rr, but for
 * ecc, it's necessary, for there are two different address. which byte length is the same with n.
 * @param aligned_len aligned length for calculate.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 rsa_rrn(const td_u8 *n, const td_u32 key_size, const td_u32 aligned_len, td_u8 *rrn);

td_s32 update_rsa_modulus(const td_u8 *n, const td_u32 n_len, const td_u32 aligned_len CIPHER_CHECK_WORD);

/**
 * @brief regular multiplication. c = a * b.
 *
 * @param a input data.
 * @param b input data.
 * @param c output data.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 rsa_regular_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c);

/**
 * @brief c = a mod p.
 *
 * @param a the input data, which bit length shouldn't more than 2 * p->bit_length.
 * @param p the modulus.
 * @param c the output data.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 rsa_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c CIPHER_CHECK_WORD);

/**
 * @brief c = (a + b) mod p
 */
td_s32 rsa_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD);

/**
 * @brief c = (a - b) mod p. a could be smaller than b.
 */
td_s32 rsa_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD);

/**
 * @brief c = ~a mod p. where a * c mod p = 1 mod p.
 */
td_s32 rsa_inv_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c CIPHER_CHECK_WORD);

/**
 * @brief c = (a * b) mod p use montgomery modular multiplication. before call this API, you should have call rsa_rrn
 * API to set rrn into rsa_addr_rr DRAM.
 */
td_s32 rsa_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c CIPHER_CHECK_WORD);

/**
 * @brief out = in ^ k mod n.
 *
 * @param n the input modulo.
 * @param k the exponent.
 * @param in the input data.
 * @param out the output data.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 rsa_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out CIPHER_CHECK_WORD);

    #ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
