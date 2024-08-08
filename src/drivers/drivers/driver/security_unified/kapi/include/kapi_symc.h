/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: symmetric algorithm kernel API header file.
 *
 * Create: 2023-05-26
*/

#ifndef KAPI_SYMC_H
#define KAPI_SYMC_H

#include "crypto_type.h"
#include "crypto_symc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define KAPI_SYMC_MODULE_ID             0x01

td_s32 kapi_cipher_symc_init(td_void);

td_s32 kapi_cipher_symc_deinit(td_void);

td_s32 kapi_cipher_symc_create(td_handle *kapi_symc_handle, const crypto_symc_attr *symc_attr);

td_s32 kapi_cipher_symc_destroy(td_handle kapi_symc_handle);

td_s32 kapi_cipher_symc_set_config(td_handle kapi_symc_handle, const crypto_symc_ctrl_t *symc_ctrl);

td_s32 kapi_cipher_symc_get_config(td_handle kapi_symc_handle, crypto_symc_ctrl_t *symc_ctrl);

td_s32 kapi_cipher_symc_attach(td_handle kapi_symc_handle, td_handle keyslot_handle);

td_s32 kapi_cipher_symc_get_keyslot_handle(td_handle kapi_symc_handle, td_handle *keyslot_handle);

td_s32 kapi_cipher_symc_detach(td_handle kapi_symc_handle, td_handle keyslot_handle);

td_s32 kapi_cipher_symc_set_key(td_handle kapi_symc_handle, td_u8 *key, td_u32 key_len);

td_s32 kapi_cipher_symc_encrypt(td_handle kapi_symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 kapi_cipher_symc_decrypt(td_handle kapi_symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 kapi_cipher_symc_encrypt_multi(td_handle kapi_symc_handle, const crypto_symc_ctrl_t *symc_ctrl,
    const crypto_symc_pack *src_buf_pack, const crypto_symc_pack *dst_buf_pack, td_u32 pack_num);

td_s32 kapi_cipher_symc_decrypt_multi(td_handle kapi_symc_handle, const crypto_symc_ctrl_t *symc_ctrl,
    const crypto_symc_pack *src_buf_pack, const crypto_symc_pack *dst_buf_pack, td_u32 pack_num);

td_s32 kapi_cipher_symc_cenc_decrypt(td_handle kapi_symc_handle, const crypto_symc_cenc_param *cenc_param,
    const crypto_buf_attr *src_buf, const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 kapi_cipher_dma_copy(const crypto_buf_attr *src_buf, const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 kapi_cipher_symc_get_tag(td_handle kapi_symc_handle, td_u8 *tag, td_u32 tag_length);

td_s32 kapi_cipher_mac_start(td_handle *kapi_symc_handle, const crypto_symc_mac_attr *mac_attr);

td_s32 kapi_cipher_mac_update(td_handle kapi_symc_handle, const crypto_buf_attr *src_buf, td_u32 length);

td_s32 kapi_cipher_mac_finish(td_handle kapi_symc_handle, td_u8 *mac, td_u32 *mac_length);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif