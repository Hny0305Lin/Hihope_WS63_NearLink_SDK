/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hash romable functions header file.
 *
 * Create: 2024-01-25
*/

#ifndef HASH_ROMABLE_H
#define HASH_ROMABLE_H

#include "crypto_hash_struct.h"

int drv_rom_hash(crypto_hash_type hash_type, const unsigned char *data, unsigned int data_length,
    unsigned char *hash, unsigned int hash_length, unsigned int hash_chn);

#endif  /* HASH_ROMABLE_H */
