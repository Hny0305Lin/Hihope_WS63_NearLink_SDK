/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: The head file of sec_random.c
 */
#ifndef SEC_RANDOM_H
#define SEC_RANDOM_H

#include "chip_definitions.h"

#if !CHIP_WS63 && !CHIP_WS53
#define HASH_DIGEST_LENGTH 32  // hash digest length

#define DEFAULT_CONST     "HRAND CONNECTIVITY CHIP"
#define DEFAULT_CONST_LEN 23
#define POOL_LEN          HASH_DIGEST_LENGTH

typedef struct {
    uint8_t v[POOL_LEN];
    uint8_t c[POOL_LEN];
} hrand_ctx;

void init_rand(void);
uint32_t sec_rand(void);   // Secure random number
uint32_t nsec_rand(void);  // Unsafe random number
void sec_rand_bytes(uint8_t *output, uint32_t output_len);
#endif
#endif
