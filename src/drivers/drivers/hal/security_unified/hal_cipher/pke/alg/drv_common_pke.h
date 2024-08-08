/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: the common function head file of drv_layer.
 *
 * Create: 2022-10-27
*/

#ifndef DRV_COMMON_PKE_H
#define DRV_COMMON_PKE_H

#include "crypto_pke_struct.h"
#include "hal_pke.h"
#include "rom_lib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/************************************************** pke common MICRO start************************************/
#define JAC_TO_AFF_INSTR_NUM    6
#define POINT_NAF_INSTR_NUM     3

#define LOCAL_STEP_CNT_INIT                    0x96C3785A
#define LOCAL_STEP_AUTH                        11
#define local_step(cnt)                        (LOCAL_STEP_CNT_INIT + LOCAL_STEP_AUTH * (cnt))

#define ED25519_MAX_HASH_ELEMENTS               3
#ifndef DRV_PKE_SM2_LEN_IN_BYTES
#define DRV_PKE_SM2_LEN_IN_BYTES                32
#endif
#define HALF_BYTE_VALUE 128
#define BYTE_VALUE 256

#define NEED_PAD_ZERO       1
#define NO_NEED_PAD_ZERO    0

#define gen_pke_data(len, value) ((drv_pke_data) {.length = (len), .data = (value)})
#define gen_pke_point(len, xvalue, yvalue) ((drv_pke_ecc_point) {.length = (len), .x = (xvalue), .y = (yvalue)})

#define PKE_COMPAT_ERRNO(err_code)      DRV_COMPAT_ERRNO(ERROR_MODULE_PKE, err_code)

#if (defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 2 || CRYPTO_LOG_LEVEL == 3 || CRYPTO_LOG_LEVEL == 4))
#define crypto_dump_buffer(buffer_name, buffer, len) crypto_dump_data(buffer_name, buffer, len)
#else
#define crypto_dump_buffer(fmt, args...)
#endif
/************************************************** pke common MICRO end************************************/

/************************************************ pke common global variable start************************************/
typedef struct {
    td_u32 ram_section_a;   /* input data a ram section. */
    td_u32 ram_section_b;   /* input data b ram section. */
    td_u32 ram_section_c;   /* out data ram section. */
    td_u32 ram_section_tp;  /* intermediate parameter ram section. */
} instr_data_ram;
extern pke_default_parameters *g_ecc_params;

extern td_u32 *g_ecc_num;
/************************************************ pke common global variable end************************************/

/************************************************** pke common API start************************************/
const drv_pke_ecc_curve *crypto_drv_pke_common_get_ecc_curve(drv_pke_ecc_curve_type curve_type);

/* right shift any bit you want, the shift_bit should be [1, 8], and the k should be bigendian data. */
void crypto_drv_pke_common_arry_right_shift_value(td_u8 *k, td_u32 k_len, td_u32 shift_bit);

/* add or minus one. */
void crypto_drv_pke_common_array_add_plus_minus_one(td_u8 *k, td_u32 k_len, td_s32 value);

td_s32 crypto_drv_pke_common_get_binary_data(const td_u8 *mod_p, const td_u32 mod_p_len, td_u8 *mod_p_bin,
    td_u32 *mod_p_bin_len, td_u32 flag);

/**
 * @brief convert normal scalar value to non-adjacent-form. the output data from k0 to ki is from low bit to high bit.
 * which need to be reverse, or use from ki(high bit).
 *
 * @param k the normal scalar
 * @param k_len the byte length of k.
 * @param k_naf the non-adjacent-form of k. the output data from k0 to ki is from low bit to high bit.
 * which need to be reverse, or use from ki(high bit).
 * @param k_naf_len the length of k_naf, k_naf_len = k_len * 8 or k_naf_len = k_len * 8 + 1;
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_convert_normal_scalar_to_naf(const td_u8 *k, const td_u32 k_len, td_s32 *k_naf,
    td_u32 *k_naf_len);

/* config data for montgomery calculation. */
td_s32 crypto_drv_pke_common_montgomery_data_config(td_u8 *rr, td_u32 rr_len, td_u8 *rrp, td_u32 rrp_len);

/* whether the data is whole zero. */
td_bool crypto_drv_pke_common_is_zero(const td_u8 *val, td_u32 klen);

/**
 * @brief minus the specified value.
 * @param k the input data.
 * @param k_len the byte length of the input k.
 * @param value the specified value that is to be subtracted.
 */
td_void crypto_drv_pke_common_array_sub_value(td_u8 *k, td_u32 k_len, td_s32 value);

/**
 * @brief littleendian and bigendian transform to get the right value.
 * @param output_endian output value.
 * @param input_endian input value.
 * @param byte_len the byte length of the value to be processed.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_little_big_endian_trans(td_u8 *output_endian, const td_u8 *input_endian, td_u32 byte_len);

td_s32 crypto_drv_pke_common_byte_stream_to_int_array(const td_u8 *byte_stream, const td_u32 stream_length,
    td_u32 *int_array, td_u32 array_length);

td_void crypto_drv_pke_common_normal_add(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c);

/**
 * @brief Get the montgomery param object and set it into register.
 * @param p input prime.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_set_mont_param(const drv_pke_data *p);

/**
 * @brief Get the montgomery param object and set it into register.
 * @param p input prime.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_set_ecc_mont_param(const drv_pke_data *p);
/**
 * @brief batch instruction calculation, which include set_mode->start->wait_done.
 * @param batch_instr the batch instruction that need to process.
 * @param work_len work_len = aligned_len / 8.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_batch_instr_process(const rom_lib *batch_instr, td_u32 work_len);

/**
 * @brief get instr for single instr process.
 * @param instr_cmd instr mode CMD_MUL_MOD | CMD_ADD_MOD | CMD_SUB_MOD.
 * @param data_ram instr parameter ram section.
 * @param out_instr the generated instruction.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_get_single_instr(td_u32 instr_cmd, const instr_data_ram *data_ram, td_u32 *out_instr);
/**
 * @brief single instruction calculation, which include set_mode->start->wait_done.
 * @param single_instr the single instruction that need to process.
 * @param work_len work_len = aligned_len / 8.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_single_instr_process(td_u32 single_instr, td_u32 work_len);

#ifdef PKE_V5_DEBUG
/**
 * @brief batch instruction calculate in single process, which include set_mode->start->wait_done.
 * @param signle_instr_list the single instruction lists that need to process.
 * @param work_len work_len = aligned_len / 8.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_batch_instr_single_process_test(const rom_lib *signle_instr_list, td_u32 work_len);
#endif

/**
 * @brief Jacobin to Affine coordinate transfer calculation.
 * @param batch_instr the batch instructions array, which include a series of instructions to be used for calculation.
 * It include 6 instructions: pre_process, j2a_exp_00, j2a_exp_01, j2a_exp_10, j2a_exp_11.
 * @param batch_instr_num the batch instructions num.
 * @param mod_p the modulur used for calculation.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_jac_to_aff_cal(const rom_lib *batch_instr, const td_u32 batch_instr_num,
    const drv_pke_data *mod_p);

/**
 * @brief main process of NAF(non-adjacent-form) point multiplication. Q = k * G. Before call this function, you should
 * have set input data into the right PKE DRAM address.
 * @param batch_instr the batch instructions array, which include a series of instructions to be used for calculation.
 * It include 6 instructions: pre_process, j2a_exp_00, j2a_exp_01, j2a_exp_10, j2a_exp_11.
 * @param batch_instr_num the batch instructions num.
 * @param k the invariant that participate in the calculation.
 * @param work_len work_len = align_len / ALIGNED_TO_WORK_LEN_IN_BYTE.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_point_mul_naf(const rom_lib *batch_instr, const td_u32 batch_instr_num,
    const drv_pke_data *k, td_u32 work_len);

/**
 * @brief calculate hash for sm2 and ed25519.
 * @param arr the input multi-buffer that need to be calculate hash.
 * @param arr_len the buffer number that need to be calculate hash.
 * @param hash_type the hash type user want.
 * @param hash the ouput hash result.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_calc_hash(const drv_pke_data* arr, const td_u32 arr_len, const drv_pke_hash_type hash_type,
    drv_pke_data *hash);

/**
 * @brief lock, enable noise, and set mask random.
 * @return td_s32
 */
td_s32 crypto_drv_pke_common_resume(void);

/**
 * @brief disable noise and unlock.
 */
void crypto_drv_pke_common_suspend(void);

/* check whether y >= p, if yes, return failure. */
td_s32 crypto_drv_pke_common_limit_value_check(const td_u8 *in_buf, const td_u8 *limit_value, const td_u32 limit_len);

td_s32 crypto_drv_pke_common_range_check(const td_u8 *in_buf, const td_u8 *limit_value, const td_u32 limit_len);

/**
 * @brief According to curve type set init parameters into DRAM.
 *
 * @param ecc Curve parameter.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 crypto_drv_pke_common_init_param(const drv_pke_ecc_curve *ecc);

/************************************************** pke common API end************************************/
td_s32 crypto_drv_pke_common_ecc_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 crypto_drv_pke_common_ecc_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 crypto_drv_pke_common_sm2_sign(const drv_pke_data *priv_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig);

td_s32 crypto_drv_pke_common_sm2_verify(const drv_pke_ecc_point *pub_key, const drv_pke_data *hash,
    const drv_pke_ecc_sig *sig);

td_s32 crypto_drv_pke_common_sm2_public_encrypt(const drv_pke_ecc_point *pub_key, const drv_pke_data *plain_text,
    const drv_pke_data *cipher_text);

td_s32 crypto_drv_pke_common_sm2_private_decrypt(const drv_pke_data *priv_key, const drv_pke_data *cipher_text,
    const drv_pke_data *plain_text);

td_s32 crypto_drv_pke_common_sm2_dsa_hash(const drv_pke_data *sm2_id, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_data *hash);

td_s32 normal_sub(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* DRV_COMMON_PKE_H */