/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: efuse driver wrap
 *
 * Create: 2023-04-13
 */
#ifndef EFUSE_WRAP_H
#define EFUSE_WRAP_H

#include <stdint.h>

#define EFUSE_IDX_NRW 0x0
#define EFUSE_IDX_RO  0x1
#define EFUSE_IDX_WO  0x2
#define EFUSE_IDX_RW  0x3

typedef enum {
    EFUSE_CHIP_ID = 0,
    EFUSE_DIE_ID = 1,
    EFUSE_MCU_VER_ID = 2,
    EFUSE_FLASHBOOT_VER_ID = 3,
    EFUSE_PARAMS_VER_ID = 4,
    EFUSE_HASH_ROOT_PUBLIC_KEY_ID = 5,
    EFUSE_MSID_ID = 6,
    EFUSE_SEC_VERIFY_ENABLE = 7,
    EFUSE_IDX_MAX,
} efuse_idx;

uint32_t efuse_read_item(efuse_idx efuse_id, uint8_t *data, uint16_t data_len);

#endif