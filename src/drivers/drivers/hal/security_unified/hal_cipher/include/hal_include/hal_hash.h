/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal hash header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_HASH_H
#define HAL_HASH_H

#include "hal_common.h"
#include "crypto_hash_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 hal_cipher_hash_init(td_void);

td_s32 hal_cipher_hash_deinit(td_void);

td_s32 hal_hash_lock(td_u32 chn_num);

td_s32 hal_hash_unlock(td_u32 chn_num);

td_s32 hal_cipher_hash_config(td_u32 chn_num, crypto_hash_type hash_type, const td_u32 *state);

td_s32 hal_cipher_hash_attach(td_u32 chn_num, td_u32 keyslot_chn_num);

td_s32 hal_cipher_hash_add_in_node(td_u32 chn_num, td_phys_addr_t data_phys, td_u32 data_len,
    in_node_type_e in_node_type);

td_s32 hal_cipher_hash_start(td_u32 chn_num, td_bool is_wait);

td_s32 hal_cipher_hash_start_calc(td_u32 chn_num, td_u8 *data, td_u32 length);

td_s32 hal_cipher_hash_wait_done(td_u32 chn_num, td_u32 *state, td_u32 state_size);

td_s32 hal_cipher_hash_register_wait_func(td_u32 chn_num, td_void *wait,
    crypto_wait_timeout_interruptible wait_func, td_u32 timeout_ms);

td_s32 hal_cipher_hash_done_notify(td_u32 chn_num);

td_u32 hal_cipher_hash_done_try(td_u32 chn_num);

td_void hal_hash_debug(td_void);

td_void hal_hash_debug_chn(td_u32 chn_num);

td_s32 hal_cipher_hash_wait_func_config(td_u32 chn_num, td_bool is_enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif