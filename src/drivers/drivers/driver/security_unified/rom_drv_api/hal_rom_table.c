/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Corresponds ROM functions to their implementation ROM addresses.
 *
 * Create: 2023-05-17
*/

#include "crypto_osal_adapt.h"
#include "crypto_hash_struct.h"
#include "hal_pke.h"
#include "hal_hash.h"
#include "hal_cipher_trng.h"
#include "crypto_security.h"
#include "security_rom_table.h"

/*
 * ROM PKE
*/
td_s32 hal_pke_init(void)
{
    return ((td_s32(*)(void))(HAL_PKE_INIT))();
}

td_s32 hal_pke_deinit(void)
{
    return ((td_s32(*)(void))(HAL_PKE_DEINIT))();
}

td_s32 hal_pke_lock(void)
{
    return ((td_s32(*)(void))(HAL_PKE_LOCK))();
}

void hal_pke_unlock(void)
{
    return ((void(*)(void))(HAL_PKE_UNLOCK))();
}

void hal_pke_enable_noise(void)
{
    return ((void(*)(void))(HAL_PKE_ENABLE_NOISE))();
}

void hal_pke_disable_noise(void)
{
    return ((void(*)(void))(HAL_PKE_DISABLE_NOISE))();
}

td_s32 hal_pke_pre_process(void)
{
    return ((td_s32(*)(void))(HAL_PKE_PRE_PROCESS))();
}

td_s32 hal_pke_check_free(void)
{
    return ((td_s32(*)(void))(HAL_PKE_CHECK_FREE))();
}

td_void hal_pke_set_ram(td_u32 ram_section, const td_u8 *data, td_u32 data_len, td_u32 aligned_len CIPHER_CHECK_WORD)
{
    return ((td_void(*)(td_u32 ram_section, const td_u8 *data, td_u32 data_len, td_u32 aligned_len CIPHER_CHECK_WORD))
        (HAL_PKE_SET_RAM))(ram_section, data, data_len, aligned_len CRYPTO_ROM_CHECK_WORD);
}

td_void hal_pke_get_ram(td_u32 ram_section, td_u8 *data, td_u32 data_len CIPHER_CHECK_WORD)
{
    return ((td_void(*)(td_u32 ram_section, td_u8 *data, td_u32 data_len CIPHER_CHECK_WORD))(HAL_PKE_GET_RAM))
        (ram_section, data, data_len CRYPTO_ROM_CHECK_WORD);
}

td_s32 hal_pke_clean_ram(void)
{
    return ((td_s32(*)(void))(HAL_PKE_CLEAN_RAM))();
}

td_s32 hal_pke_set_mode(pke_mode mode, td_u32 single_instr, const rom_lib *batch_instr,
    pke_data_work_len work_len CIPHER_CHECK_WORD)
{
    return ((td_s32(*)(pke_mode mode, td_u32 single_instr, const rom_lib *batch_instr,
        pke_data_work_len work_len CIPHER_CHECK_WORD))(HAL_PKE_SET_MODE))(mode, single_instr, batch_instr,
        work_len CRYPTO_ROM_CHECK_WORD);
}

td_s32 hal_pke_start(pke_mode mode CIPHER_CHECK_WORD)
{
    return ((td_s32(*)(pke_mode mode CIPHER_CHECK_WORD))(HAL_PKE_START))(mode CRYPTO_ROM_CHECK_WORD);
}

td_s32 hal_pke_wait_done(void)
{
    return ((td_s32(*)(void))(HAL_PKE_WAIT_DONE))();
}

td_s32 hal_pke_get_align_val(const td_u32 len, td_u32 *aligned_len)
{
    return ((td_s32(*)(const td_u32 len, td_u32 *aligned_len))(HAL_PKE_GET_ALIGN_VAL))(len, aligned_len);
}

td_s32 hal_pke_set_mont_para(td_u32 low_bit, td_u32 high_bit CIPHER_CHECK_WORD)
{
    return ((td_s32(*)(td_u32 low_bit, td_u32 high_bit CIPHER_CHECK_WORD))(HAL_PKE_SET_MONT_PARA))(low_bit,
        high_bit CRYPTO_ROM_CHECK_WORD);
}

td_s32 hal_pke_set_ecc_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    return ((td_s32(*)(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve))
        (HAL_PKE_SET_ECC_PARAM))(init_param, ecc_curve);
}

td_s32 hal_pke_set_init_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    return ((td_s32(*)(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve))
        (HAL_PKE_SET_INIT_PARAM))(init_param, ecc_curve);
}

/*
* ROM TRNG
*/
td_s32 hal_cipher_trng_init(td_void)
{
    return ((td_s32(*)(td_void))(HAL_CIPHER_TRNG_INIT))();
}

td_s32 hal_cipher_trng_get_random(td_u32 *randnum)
{
    return ((td_s32(*)(td_u32 *randnum))(HAL_CIPHER_TRNG_GET_RANDOM))(randnum);
}

/*
 * ROM HASH
 */
td_s32 hal_cipher_hash_init(td_void)
{
    return ((td_s32(*)(td_void))(HAL_CIPHER_HASH_INIT))();
}

td_s32 hal_hash_lock(td_u32 chn_num)
{
    return ((td_s32(*)(td_u32 chn_num))(HAL_HASH_LOCK))(chn_num);
}

td_s32 hal_hash_unlock(td_u32 chn_num)
{
    return ((td_s32(*)(td_u32 chn_num))(HAL_HASH_UNLOCK))(chn_num);
}

td_s32 hal_cipher_hash_config(td_u32 chn_num, crypto_hash_type hash_type, const td_u32 *state)
{
    return ((td_s32(*)(td_u32 chn_num, crypto_hash_type hash_type, const td_u32 *state))(HAL_CIPHER_HASH_CONFIG))
        (chn_num, hash_type, state);
}

td_s32 hal_cipher_hash_add_in_node(td_u32 chn_num, td_phys_addr_t data_phys, td_u32 data_len,
    in_node_type_e in_node_type)
{
    return ((td_s32(*)(td_u32 chn_num, td_phys_addr_t data_phys, td_u32 data_len, in_node_type_e in_node_type))
        (HAL_CIPHER_HASH_ADD_IN_NODE))(chn_num, data_phys, data_len, in_node_type);
}

td_s32 hal_cipher_hash_start(td_u32 chn_num, td_bool is_wait)
{
    return ((td_s32(*)(td_u32 chn_num, td_bool is_wait))(HAL_CIPHER_HASH_START))(chn_num, is_wait);
}

td_s32 hal_cipher_hash_wait_done(td_u32 chn_num, td_u32 *state, td_u32 state_size)
{
    return ((td_s32(*)(td_u32 chn_num, td_u32 *state, td_u32 state_size))(HAL_CIPHER_HASH_WAIT_DONE))
        (chn_num, state, state_size);
}