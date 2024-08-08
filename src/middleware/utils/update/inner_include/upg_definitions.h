/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: struct definitions for UPG
 */

#ifndef UPG_DEFINITIONS_H
#define UPG_DEFINITIONS_H

#include <stdint.h>
#include "partition_resource_id.h"
#include "upg.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define UPG_IMAGE_ID_NV                 0xCB9E063C
#define UPG_IMAGE_ID_RES_INDEX          0xCB9E0826
#define UPG_IMAGE_ID_RES_DATA           0xCB9E0832
#define UPG_IMAGE_ID_KEY_AREA           0xCB8D154E
#define UPG_IMAGE_ID_FOTA_INFO_AREA     0xCB8D154E
#define UPG_IMAGE_HRADER_MAGIC          0x464F5451

#define UPG_FIRMWARE_MAX_NUM            20
#define UPG_FLAG_RETYR_TIMES            3

#define VERIFY_BUFF_LEN                 0x1000

#define ENCRYPTION_FLAG_ENABLED         0x3C7896E1  /* 加密打开 */
#define DECOMPRESS_FLAG_ZIP             0x3C7896E1  /* 压缩镜像 */
#define DECOMPRESS_FLAG_DIFF            0x44494646  /* 差分镜像 */

#define UPG_HEAD_MAGIC                  0x55AA55AA
#define UPG_END_MAGIC                   0xAA55AA55

typedef struct upg_image_partition_ids_map {
    uint32_t image_id;
    partition_ids_t item_id;
} upg_image_partition_ids_map_t;

typedef enum upg_image_status {
    UPG_IMAGE_STATUS_NOT_STARTED,       /* 镜像未开始处理 */
    UPG_IMAGE_STATUS_STARTED,           /* 镜像已开始处理 */
    UPG_IMAGE_STATUS_RETRY,             /* 镜像处理失败，需要重试 */
    UPG_IMAGE_STATUS_FINISHED,          /* 镜像已处理完成(镜像更新成功或更新失败但重试已达到3次) */
    UPG_IMAGE_STATUS_INVALID,
} upg_image_status_t;

typedef enum upg_image_status_switch {
    UPG_IMAGE_STATUS_SWITCH_TO_STARTED,     /* 切换到开始处理状态(当前处理的flag置为0x0F) */
    UPG_IMAGE_STATUS_SWITCH_TO_RETRY,       /* 切换到重试状态(当前处理的flag置为0x00) */
    UPG_IMAGE_STATUS_SWITCH_TO_FINISHED,    /* 切换到处理完成状态(所有3个flag都置为0x00) */
    UPG_IMAGE_STATUS_SWITCH_INVALID,
} upg_image_status_switch_t;

typedef enum upg_resource_operation {
    UPG_RESOURCE_ADD_FILE,
    UPG_RESOURCE_AMEND_FILE,
    UPG_RESOURCE_DELETE_FILE,
    UPG_RESOURCE_REMOVE_DIR
} upg_resource_operation_t;

typedef struct fota_upgrade_flag_area {
    uint32_t head_magic;
    uint32_t head_before_offset;
    uint32_t package_length;
    uint32_t firmware_num;
    uint8_t  firmware_flag[UPG_FIRMWARE_MAX_NUM][UPG_FLAG_RETYR_TIMES];
    uint8_t  nv_flag[UPG_FLAG_RETYR_TIMES];
    uint8_t  ver_change_flag; /* 版本是否变化标记，0x0:无变化 0xFF：有变化 */
    uint32_t update_result;
    uint32_t nv_data_offset;
    uint32_t nv_data_len;
    uint32_t nv_hash_offset;
    uint32_t nv_hash_len;
    uint32_t complete_flag;
    uint32_t head_end_magic;
} fota_upgrade_flag_area_t;

typedef struct upg_package_info {
    uint32_t total_new_fw_size;
    uint32_t finished_fw_size;
} upg_package_info_t;

typedef struct upg_image_collections {
    uint32_t *img_ids;
    uint32_t img_ids_cnt;
} upg_image_collections_t;

typedef struct upg_resource_index {
    char file_path[128];
    uint32_t offset;
    uint32_t file_len;
    uint32_t operation_type;
} upg_resource_node_t;

typedef struct upg_resource_head {
    uint32_t file_num;
    upg_resource_node_t file_node[0];
} upg_resource_index_t;

typedef struct upg_storage_ctx {
    bool                     inited;
    uint8_t                  rsv[3]; /* reserved 3 Bytes */
    upg_status_t             upg_status;
    upg_result_t             temporary_result;
    upg_func_t               func_list;
    uint32_t                 packge_len;
#if (UPG_CFG_PROCESS_NOTIFY_SUPPORT == YES)
    uapi_upg_progress_cb progress_cb;
#endif
    upg_package_info_t   package_info;
} upg_storage_ctx_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* UPG_DEFINITIONS_H */
