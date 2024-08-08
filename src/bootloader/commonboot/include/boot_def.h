/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_DEF_H
#define BOOT_DEF_H

#include "common_def.h"

#define SZ_1KB 1024
#ifndef CONFIG_LOADERBOOT_SUPPORT_DYNAMIC_PACKET_SIZE
#define READ_SIZE SZ_1KB
#else
#define READ_SIZE CONFIG_FLASH_WRITE_SIZE
#endif
#define SZ_1MB (SZ_1KB * SZ_1KB)
#define SZ_4KB 4096

#define BITS_PER_BYTE               8
#define HEXADECIMAL                 16
#define DECIMAL                     10

#endif