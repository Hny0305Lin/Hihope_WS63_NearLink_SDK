/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver klad header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_KLAD_H
#define DRV_KLAD_H

#include "td_type.h"
#include "hal_klad.h"
#include "hal_rkp.h"
#include "crypto_km_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 drv_klad_create(td_handle *klad_handle);

td_s32 drv_klad_destroy(td_handle klad_handle);

td_s32 drv_klad_attach(td_handle klad_handle, crypto_klad_dest klad_dest, td_handle keyslot_handle);

td_s32 drv_klad_detach(td_handle klad_handle, crypto_klad_dest klad_dest, td_handle keyslot_handle);

td_s32 drv_klad_set_attr(td_handle klad_handle, const crypto_klad_attr *klad_attr);

td_s32 drv_klad_get_attr(td_handle klad_handle, crypto_klad_attr *attr);

td_s32 drv_klad_set_clear_key(td_handle klad_handle, const crypto_klad_clear_key *clear_key);

td_s32 drv_klad_set_effective_key(td_handle klad_handle, const crypto_klad_effective_key *content_key);

td_s32 drv_kdf_update(crypto_kdf_otp_key otp_key, crypto_kdf_update_alg alg);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif