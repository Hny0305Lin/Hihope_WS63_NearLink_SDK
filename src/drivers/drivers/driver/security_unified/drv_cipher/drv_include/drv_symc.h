/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver symc header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_SYMC_H
#define DRV_SYMC_H

#include "crypto_type.h"
#include "crypto_symc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 drv_cipher_symc_init(td_void);

td_s32 drv_cipher_symc_deinit(td_void);

td_s32 drv_cipher_symc_create(td_handle *symc_handle, const crypto_symc_attr *symc_attr);

td_s32 drv_cipher_symc_destroy(td_handle symc_handle);

td_s32 drv_cipher_symc_get_module_info(crypto_symc_module_info *module_info);

td_s32 drv_cipher_symc_get_proc_info(td_handle symc_handle, crypto_symc_proc_info *proc_symc_info);

td_s32 drv_cipher_symc_set_config(td_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl);

td_s32 drv_cipher_symc_get_config(td_handle symc_handle, crypto_symc_ctrl_t *symc_ctrl);

td_s32 drv_cipher_symc_attach(td_handle symc_handle, td_handle keyslot_handle);

td_s32 drv_cipher_symc_get_keyslot_handle(td_handle symc_handle, td_handle *keyslot_handle);

td_s32 drv_cipher_dma_copy(const crypto_buf_attr *src_buf, const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 drv_cipher_symc_encrypt(td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 drv_cipher_symc_decrypt(td_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 drv_cipher_symc_encrypt_multi(td_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl,
    const crypto_symc_pack *src_buf_pack, const crypto_symc_pack *dst_buf_pack, td_u32 pack_num);

td_s32 drv_cipher_symc_decrypt_multi(td_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl,
    const crypto_symc_pack *src_buf_pack, const crypto_symc_pack *dst_buf_pack, td_u32 pack_num);

td_s32 drv_cipher_symc_cenc_decrypt(td_handle symc_handle, const crypto_symc_cenc_param *cenc_param,
    const crypto_buf_attr *src_buf, const crypto_buf_attr *dst_buf, td_u32 length);

td_s32 drv_cipher_symc_get_tag(td_handle symc_handle, td_u8 *tag, td_u32 tag_length);

td_s32 drv_cipher_mac_start(td_handle *symc_handle, const crypto_symc_mac_attr *mac_attr);

td_s32 drv_cipher_mac_update(td_handle symc_handle, const crypto_buf_attr *src_buf, td_u32 length);

td_s32 drv_cipher_mac_finish(td_handle symc_handle, td_u8 *mac, td_u32 *mac_length);

td_s32 drv_cipher_symc_set_key(td_handle symc_handle, td_u8 *key, td_u32 key_len);

td_void drv_cipher_debug_handle(td_handle symc_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
