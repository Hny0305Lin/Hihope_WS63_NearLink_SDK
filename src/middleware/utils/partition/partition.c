/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Partition manage module -- implementation \n
 * Author:
 * History: \n
 * 2022-09-01, Create file. \n
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "common_def.h"
#include "partition_porting.h"
#include "partition_info.h"

static partition_t g_partition_info;

partition_t *partition_get_global_info(void)
{
    return &g_partition_info;
}

errcode_t uapi_partition_init(void)
{
    params_area_t *params_head = (params_area_t *)partition_get_addr();
    if (params_head->image_id != PARTITION_IMAGE_ID) {
        return ERRCODE_PARTITION_INIT_ERR;
    }
    param_area_data_t *partition = (param_area_data_t *)(partition_get_addr() + params_head->param_item_offset);
    uint8_t *ids = (uint8_t *)(partition_get_addr() + PARTITION_ITEM_ID_OFFSET);

    partition_t *partition_info = partition_get_global_info();
    partition_info->image_id = params_head->image_id;
    partition_info->struct_ver = params_head->struct_ver;
    partition_info->params_area_ver = params_head->params_area_ver;
    partition_info->partition_cnt = params_head->param_item_count;

    if (partition_info->partition_cnt < PARTITION_MAX_CNT) {
        partition_printf("part cnt- %d < max cnt- %d\n", partition_info->partition_cnt, PARTITION_MAX_CNT);
        return ERRCODE_PARTITION_INIT_ERR;
    }

    for (uint8_t i = 0; i < PARTITION_MAX_CNT; i++) {
        partition_info->partition_tbl[i].addr = partition[i].addr;
        partition_info->partition_tbl[i].size = partition[i].size;
        partition_info->partition_tbl[i].id = ids[i];
    }

    return ERRCODE_SUCC;
}

STATIC bool partition_find_by_addr(uint8_t id, uint8_t *tbl_index)
{
    partition_t *partition_info = partition_get_global_info();
    for (uint8_t i = 0; i < partition_info->partition_cnt; i++) {
        partition_printf("tbl id -%d, find id -%d\n", partition_info->partition_tbl[i].id, id);
        if (partition_info->partition_tbl[i].id == id) {
            *tbl_index = i;
            return true;
        }
    }
    return false;
}

STATIC char *partition_find_by_path(uint32_t item_id)
{
    partition_path_map_t *path_map = NULL;
    uint32_t path_num = partition_get_path_map(&path_map);
    if (path_num == 0 || path_map == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < path_num; i++) {
        if (path_map[i].item_id == item_id) {
            return path_map[i].file_path;
        }
    }
    return NULL;
}

errcode_t uapi_partition_get_info(partition_ids_t partition_id, partition_information_t *info)
{
    uint8_t tbl_index = 0;
    if (info == NULL) {
        return ERRCODE_PARTITION_INVALID_PARAMS;
    }

    if (partition_find_by_addr(partition_id, &tbl_index)) {
        partition_t *partition_info = partition_get_global_info();
        info->type = PARTITION_BY_ADDRESS;
        info->part_info.addr_info.addr = partition_info->partition_tbl[tbl_index].addr;
        info->part_info.addr_info.size = partition_info->partition_tbl[tbl_index].size;
        return ERRCODE_SUCC;
    }

    char *path = partition_find_by_path(partition_id);
    if (path != NULL) {
        info->type = PARTITION_BY_PATH;
        info->part_info.file_path = path;
        return ERRCODE_SUCC;
    }
    return ERRCODE_PARTITION_CONFIG_NOT_FOUND;
}
