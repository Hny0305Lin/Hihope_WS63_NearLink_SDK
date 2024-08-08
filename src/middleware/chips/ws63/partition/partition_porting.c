/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Partition manage module -- seperate configuration for each product.
 */

#include <stdint.h>
#include <stddef.h>
#include "partition.h"
#include "partition_porting.h"

#define PARTITION_START_ADDRESS 0x200380
#define uapi_array_size(_array)  (sizeof(_array) / sizeof((_array)[0]))

static partition_path_map_t g_image_path[] = {};
uintptr_t partition_get_addr(void)
{
    return PARTITION_START_ADDRESS;
}

uint32_t partition_get_path_map(partition_path_map_t **path_map)
{
    *path_map = g_image_path;
    return uapi_array_size(g_image_path);
}
