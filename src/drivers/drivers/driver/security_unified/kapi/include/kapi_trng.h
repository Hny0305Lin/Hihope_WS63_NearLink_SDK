/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: trng kernel API header file.
 *
 * Create: 2023-05-26
*/

#ifndef KAPI_TRNG_H
#define KAPI_TRNG_H

#include "crypto_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 kapi_cipher_trng_get_random(td_u32 *randnum);

td_s32 kapi_cipher_trng_get_multi_random(td_u32 size, td_u8 *randnum);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif