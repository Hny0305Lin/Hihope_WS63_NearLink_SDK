/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver keyslot header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_KEYSLOT_H
#define DRV_KEYSLOT_H

#include "td_type.h"
#include "hal_keyslot.h"
#include "crypto_km_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 drv_keyslot_init(td_void);

td_s32 drv_keyslot_deinit(td_void);

td_s32 drv_keyslot_create(td_handle *keyslot_handle, crypto_keyslot_type keyslot_type);

td_s32 drv_keyslot_destroy(td_handle keyslot_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif