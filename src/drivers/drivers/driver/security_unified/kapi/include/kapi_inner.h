/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: security_unified kernel system init API header file.
 *
 * Create: 2023-05-26
*/

#ifndef KAPI_INNER_H
#define KAPI_INNER_H

#include "crypto_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 kapi_cipher_hash_env_init(td_void);

td_s32 kapi_cipher_hash_env_deinit(td_void);

td_s32 kapi_cipher_symc_env_init(td_void);

td_s32 kapi_cipher_symc_env_deinit(td_void);

td_s32 kapi_cipher_trng_env_init(td_void);

td_s32 kapi_cipher_trng_env_deinit(td_void);

td_s32 kapi_cipher_pke_env_init(td_void);

td_s32 kapi_cipher_pke_env_deinit(td_void);

td_s32 kapi_km_env_init(td_void);

td_s32 kapi_km_env_deinit(td_void);

td_s32 kapi_env_init(td_void);

td_void kapi_env_deinit(td_void);

td_void inner_kapi_trng_lock(td_void);

td_void inner_kapi_trng_unlock(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif