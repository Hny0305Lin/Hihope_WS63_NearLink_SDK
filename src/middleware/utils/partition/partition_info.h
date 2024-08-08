/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Partition information \n
 * Author:
 * History: \n
 * 2022-09-01, Create file. \n
 */
#ifndef PARTITION_INFO_H
#define PARTITION_INFO_H

#include <stdint.h>
#include "partition.h"
#include "partition_resource_id.h"
#include "errcode.h"

#ifdef CONFIG_PARTITION_SUPPORT_DEBUG
#include <stdio.h>
#define partition_printf(fmt, arg...) printf("[PARTITION] "fmt, ##arg)
#else
#define partition_printf(fmt, arg...)
#endif

#define PARTITION_ITEM_ID_OFFSET 15

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief  Description for each part.
 */
typedef struct {
    uint32_t addr;
    uint32_t dir : 1;
    uint32_t rsv : 31;
    uint32_t id  : 8;
    uint32_t size : 24;
    uint32_t extra;
} partition_info_t;

/**
 * @brief  Partition struct in ram.
 */
typedef struct {
    uint32_t image_id;
    uint32_t struct_ver;
    uint32_t params_area_ver;
    uint32_t partition_cnt;
    partition_info_t partition_tbl[PARTITION_MAX_CNT];
} partition_t;

/**
 * @brief  Partition struct in flash.
 */
typedef struct {
    uint32_t addr;
    uint32_t size;
} param_area_data_t;

typedef struct {
    uint32_t image_id;
    uint32_t struct_ver;
    uint32_t params_area_ver;
    uint16_t param_item_offset;
    uint8_t param_item_count;
} params_area_t;

partition_t *partition_get_global_info(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* PARTITION_INFO_H */
