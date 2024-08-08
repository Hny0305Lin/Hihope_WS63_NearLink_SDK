/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: key manager kernel API header file.
 *
 * Create: 2023-05-26
*/

#ifndef KAPI_KM_H
#define KAPI_KM_H

#include "crypto_km_struct.h"

#define KAPI_KEYSLOT_MODULE_ID              0x03
#define KAPI_KLAD_MODULE_ID                 0x04

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 kapi_km_deinit(td_void);

/* Keyslot. */
td_s32 kapi_keyslot_create(td_handle *kapi_keyslot_handle, crypto_keyslot_type keyslot_type);
td_s32 kapi_keyslot_destroy(td_handle kapi_keyslot_handle);

/* Klad. */
td_s32 kapi_klad_create(td_handle *kapi_klad_handle);
td_s32 kapi_klad_destroy(td_handle kapi_klad_handle);

td_s32 kapi_klad_attach(td_handle kapi_klad_handle, crypto_klad_dest klad_type,
    td_handle kapi_keyslot_handle);
td_s32 kapi_klad_detach(td_handle kapi_klad_handle, crypto_klad_dest klad_type,
    td_handle kapi_keyslot_handle);

td_s32 kapi_klad_set_attr(td_handle kapi_klad_handle, const crypto_klad_attr *attr);
td_s32 kapi_klad_get_attr(td_handle kapi_klad_handle, crypto_klad_attr *attr);

td_s32 kapi_klad_set_effective_key(td_handle kapi_klad_handle, const crypto_klad_effective_key *effective_key);

td_s32 kapi_klad_set_clear_key(td_handle kapi_klad_handle, const crypto_klad_clear_key *key);

td_s32 kapi_kdf_update(crypto_kdf_otp_key otp_key, crypto_kdf_update_alg alg);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif