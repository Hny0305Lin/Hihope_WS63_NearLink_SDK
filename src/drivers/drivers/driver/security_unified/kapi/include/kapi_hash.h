/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hash kernel API header file.
 *
 * Create: 2023-05-26
*/

#ifndef KAPI_HASH_H
#define KAPI_HASH_H

#include "crypto_type.h"
#include "crypto_hash_struct.h"
#include "crypto_kdf_struct.h"
#include "crypto_drv_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define KAPI_HASH_MODULE_ID         0x02

typedef struct {
    long tid;
    td_bool is_open;
    td_bool is_long_term;
    td_handle drv_hash_handle;
    crypto_hash_clone_ctx hash_clone_ctx;
} crypto_kapi_hash_ctx;

typedef struct {
    td_u32 pid;
    crypto_owner owner;
    crypto_kapi_hash_ctx hash_ctx_list[CRYPTO_HASH_VIRT_CHN_NUM];
    crypto_mutex hash_ctx_mutex[CRYPTO_HASH_VIRT_CHN_NUM];
    td_u32 ctx_num;
    td_u32 init_counter;
    td_bool is_used;
} crypto_kapi_hash_process;

td_s32 kapi_cipher_hash_init(td_void);

td_s32 kapi_cipher_hash_deinit(td_void);

td_s32 kapi_cipher_hash_start(td_handle *kapi_hash_handle, const crypto_hash_attr *hash_attr);

td_s32 kapi_cipher_hash_update(td_handle kapi_hash_handle,  const crypto_buf_attr *src_buf, const td_u32 len);

td_s32 kapi_cipher_hash_finish(td_handle kapi_hash_handle, td_u8 *out, td_u32 *out_len);

td_s32 kapi_cipher_hash_get(td_handle kapi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx);

td_s32 kapi_cipher_hash_set(td_handle kapi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx);

td_s32 kapi_cipher_hash_destroy(td_handle kapi_hash_handle);

td_s32 kapi_cipher_pbkdf2(const crypto_kdf_pbkdf2_param *param, td_u8 *out, const td_u32 out_len);

td_s32 kapi_cipher_hkdf_extract(crypto_hkdf_extract_t *extract_param, td_u8 *prk, td_u32 *prk_length);

td_s32 kapi_cipher_hkdf_expand(const crypto_hkdf_expand_t *expand_param, td_u8 *okm, td_u32 okm_length);

td_s32 kapi_cipher_hkdf(crypto_hkdf_t *hkdf_param, td_u8 *okm, td_u32 okm_length);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif