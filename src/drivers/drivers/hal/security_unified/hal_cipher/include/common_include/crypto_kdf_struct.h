/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: crypto kdf struct header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef CRYPTO_KDF_STRUCT_H
#define CRYPTO_KDF_STRUCT_H

#include "crypto_common_struct.h"
#include "crypto_hash_struct.h"

typedef struct {
    crypto_hash_type hash_type;
    td_u8 *password;
    td_u32 plen;
    td_u8 *salt;
    td_u32 slen;
    td_u16 count;
} crypto_kdf_pbkdf2_param;

#endif