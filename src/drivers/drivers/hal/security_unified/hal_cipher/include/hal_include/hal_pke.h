/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Head file of pke HAL API and logical algorithm API
 *
 * Create: 2022-08-09
*/

#ifndef HAL_PEK_H
#define HAL_PEK_H

#include "crypto_osal_lib.h"
#include "crypto_pke_struct.h"
#include "hal_common.h"
#include "rom_lib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    PKE_SINGLE_INSTR0 = 0x0,
    PKE_SINGLE_INSTR1 = 0x1,
    PKE_BATCH_INSTR = 0x2
} pke_mode;

typedef enum {
    PKE_192_BIT_WORK_LEN = 0x3,
    PKE_224_BIT_WORK_LEN = 0x4,
    PKE_256_BIT_WORK_LEN = 0x4,
    PKE_384_BIT_WORK_LEN = 0x6,
    PKE_BRAINPOOL_512_BIT_WORK_LEN = 0x8,
    PKE_512_BIT_WORK_LEN = 0x9,
    PKE_2048_BIT_WORK_LEN = 0x20,
    PKE_3072_BIT_WORK_LEN = 0x30,
    PKE_4096_BIT_WORK_LEN = 0x40,
} pke_data_work_len;

extern td_u32 g_pke_initialize;

/************************************************** hal outter API start************************************/
td_s32 hal_pke_init(void);
td_s32 hal_pke_deinit(void);

td_s32 hal_pke_lock(void);
void hal_pke_unlock(void);

void hal_pke_enable_noise(void);
void hal_pke_disable_noise(void);

/**
 * @brief set mask random into registerfor set_ram and get_ram.
 *
 * @return td_s32
 */
td_s32 hal_pke_pre_process(void);

void hal_pke_set_wait_event(crypto_wait_t *wait, const crypto_wait_timeout_interruptible wait_func,
    const td_u32 timeout_ms, const td_bool wait_done);

/**
 * @brief check PKE busy, if success, then do next steps.
 *
 * @return td_s32 TD_SUCCESS or other.
 */
td_s32 hal_pke_check_free(void);

/**
 * @brief set data to PKE DRAM.
 *
 * @param ram_section the address of offset block in DRAM for data to write into.
 * @param data the data value.
 * @param data_len  the effective length of data in byte.
 * @param aligned_len the aligned length of data in byte when participate in calculate. In old IP, this is the klen,
 * such as 32(256bit), 48(384bit), 64(512bit), 68(521bit), etc. This API mainly used in RSA calculate, for ecc, the
 * input curve parameters are aligned, no need to process.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_void
 */
td_void hal_pke_set_ram(td_u32 ram_section, const td_u8 *data, td_u32 data_len, td_u32 aligned_len CIPHER_CHECK_WORD);

/**
 * @brief get result data in PKE DRAM
 *
 * @param ram_section the address of offset block in DRAM for data to get out.
 * @param data the data value.
 * @param data_len the effective length of data in byte.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 */
td_void hal_pke_get_ram(td_u32 ram_section, td_u8 *data, td_u32 data_len CIPHER_CHECK_WORD);

/**
 * @brief clear the data in DRAM as random value. Only need to clear at the last calculate.
 *
 * @return td_s32
 */
td_s32 hal_pke_clean_ram(void);

/**
 * @brief set calculate mode, work_len and instr to register.
 *
 * @param mode calculation mode, see pke_mode.
 * @param single_instr If the mode is PKE_SINGLE_INSTR0 or PKE_SINGLE_INSTR1, then set this. Otherwise, set it 0.
 * @param batch_instr If the mode is PKE_BATCH_INSTR, then set this. Otherwise, set it TD_NULL.
 * @param work_len work_len=ceiling(width/64), width is the data aligned bit width length,
 * or work_len=ceiling(aligned_len/8).
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 hal_pke_set_mode(pke_mode mode, td_u32 single_instr, const rom_lib *batch_instr,
    pke_data_work_len work_len CIPHER_CHECK_WORD);

/**
 * @brief start PKE calculate
 *
 * @param mode SINGLE_INSTR or BATCH_INSTR
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 hal_pke_start(pke_mode mode CIPHER_CHECK_WORD);

/**
 * @brief PKE wait calculation finish
 *
 * @return td_s32
 */
td_s32 hal_pke_wait_done(void);

/**
 * @brief Get the align val object of PKE, which depending on the chip version.
 *
 * @param len the input data len.
 * @param aligned_len the aligned length used to calculate.
 */
td_s32 hal_pke_get_align_val(const td_u32 len, td_u32 *aligned_len);

/**
 * @brief Set the montgomery parameter for modular multiplication.
 *
 * @param low_bit the low 32bit data.
 * @param high_bit the high 32bit data.
 * @param CIPHER_CHECK_WORD ifdef SEC_ENHANCE, the value is XOR result of parameters, otherwise it doesn't exist.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 hal_pke_set_mont_para(td_u32 low_bit, td_u32 high_bit CIPHER_CHECK_WORD);

/**
 * @brief set the initial parameters into DRAM.
 *
 * @param init_param the initial parameters.
 * @param ecc_curve the ecc curve type.
 * @return td_s32 TD_SUCCESS or others.
 */
td_s32 hal_pke_set_init_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve);
/************************************************** hal outter API end************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif