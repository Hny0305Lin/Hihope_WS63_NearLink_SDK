/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides ported functions related to partitions.
 */
#ifndef PARTITION_PORTING_H
#define PARTITION_PORTING_H

#include "partition_resource_id.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint32_t item_id;
    char *file_path;
} partition_path_map_t;

/**
 * @if Eng
 * @brief  Obtaining the start address of the partition table on the flash memory.
 * @retval PARTITION_START_ADDRESS.
 * @else
 * @brief  获取分区表在flash上的起始地址
 * @retval PARTITION_START_ADDRESS.
 * @endif
 */
uintptr_t partition_get_addr(void);

/**
 * @if Eng
 * @brief  Obtaining the mapping information between partition IDs and paths.
 * @param  [out] path_map Used to store the mappings between partition ID and path.
 * @retval Number of mappings between partition IDs and paths
 * @else
 * @brief  获取分区ID和路径的映射关系
 * @param  [out] path_map 用来保存分区ID与路径映射的关系
 * @retval 分区ID与路径映射的个数
 * @endif
 */
uint32_t partition_get_path_map(partition_path_map_t **path_map);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
