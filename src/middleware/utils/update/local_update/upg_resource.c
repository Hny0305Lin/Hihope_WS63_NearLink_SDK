/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG upgrade functions source file
 */

#include <stdint.h>
#include <stddef.h>
#include "securec.h"
#include "common_def.h"
#include "upg_definitions.h"
#include "errcode.h"
#include "upg_lzmadec.h"
#include "upg_common.h"
#include "upg_common_porting.h"
#include "upg_alloc.h"
#include "upg_porting.h"
#include "upg_config.h"
#include "upg_debug.h"
#if (UPG_CFG_SUPPORT_RESOURCES_FILE == YES)
#include "dfx_file_operation.h"

STATIC errcode_t upg_resource_file_full(const upg_lzma_decode2_data_t *data, upg_resource_node_t *file_info)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t file_len = file_info->file_len;
    uint32_t read_offset = data->in_offset + file_info->offset;
    uint32_t file_offset = 0;

    int32_t write_fd = dfx_file_open_for_write((const char *)file_info->file_path);
    if (write_fd < 0) {
        return ERRCODE_UPG_FILE_OPEN_FAIL;
    }

    while (file_len > 0) {
        uint32_t tmp_len = file_len > UPG_FLASH_PAGE_SIZE ? UPG_FLASH_PAGE_SIZE : file_len;
        (void)memset_s(data->buf.outbuf, UPG_FLASH_PAGE_SIZE, 0, UPG_FLASH_PAGE_SIZE);
        ret = upg_read_fota_pkg_data(read_offset + file_offset, data->buf.outbuf, &tmp_len);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        if (dfx_file_write_fd(write_fd, file_offset, data->buf.outbuf, tmp_len) != (int32_t)tmp_len) {
            ret = ERRCODE_UPG_FILE_WRITE_FAIL;
            break;
        }
        file_len -= tmp_len;
        file_offset += tmp_len;
    }
    upg_calculate_and_notify_process((uint32_t)file_offset);

    dfx_file_fsync(write_fd);
    dfx_file_close(write_fd);
    return ret;
}

STATIC errcode_t upg_resource_file_delete(const char *file_path)
{
    (void)dfx_file_delete(file_path);
    return ERRCODE_SUCC;
}

STATIC errcode_t upg_resource_file_rmdir(const char *path)
{
    (void)dfx_file_rmdir(path);
    return ERRCODE_SUCC;
}

STATIC errcode_t upg_resource_data_task(CLzmaDec *p, upg_lzma_decode2_data_t *data,
                                        const upg_image_header_t *image, upg_resource_node_t *file_info)
{
    if (file_info->operation_type != UPG_RESOURCE_ADD_FILE && file_info->operation_type != UPG_RESOURCE_AMEND_FILE) {
        return ERRCODE_SUCC;
    }

    if (file_info->file_len == 0) {
        return ERRCODE_SUCC;
    }

    if (image->decompress_flag == DECOMPRESS_FLAG_ZIP) {
        upg_log_info("[UPG] decompress resource file\r\n");
        return upg_resource_file_decode(p, data, file_info);
    }

    upg_log_info("[UPG] full resource file\r\n");
    return upg_resource_file_full(data, file_info);
}

STATIC errcode_t upg_resource_index_task(upg_resource_node_t *file_info)
{
    if (file_info->operation_type == UPG_RESOURCE_DELETE_FILE) {
        upg_log_info("[UPG] delete resource file\r\n");
        return upg_resource_file_delete(file_info->file_path);
    } else if (file_info->operation_type == UPG_RESOURCE_REMOVE_DIR) {
        upg_log_info("[UPG] remove resource dir\r\n");
        return upg_resource_file_rmdir(file_info->file_path);
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t upg_resource_process_node(CLzmaDec *p, upg_lzma_decode2_data_t *data,
                                           const upg_image_header_t *image, bool is_index)
{
    errcode_t ret = ERRCODE_FAIL;
    upg_resource_index_t resource_index = {0};
    uint32_t read_len = sizeof(upg_resource_index_t);

    int32_t index_fd = dfx_file_open_for_read(upg_get_res_file_index_path());
    if (index_fd < 0) {
        return ERRCODE_UPG_FILE_OPEN_FAIL;
    }

    if (dfx_file_read_fd(index_fd, 0, (uint8_t *)&resource_index, read_len, true) != (int32_t)read_len) {
        dfx_file_close(index_fd);
        return ERRCODE_UPG_FILE_READ_FAIL;
    }

    upg_resource_node_t *file_info = (upg_resource_node_t *)upg_malloc(sizeof(upg_resource_node_t));
    if (file_info == NULL) {
        dfx_file_close(index_fd);
        return ERRCODE_MALLOC;
    }

    read_len = sizeof(upg_resource_node_t);
    for (uint32_t i = 0; i < resource_index.file_num; i++) {
        memset_s(file_info, sizeof(upg_resource_node_t), 0, sizeof(upg_resource_node_t));
        uint32_t offset = sizeof(upg_resource_index_t) + (sizeof(upg_resource_node_t) * i);

        if (dfx_file_read_fd(index_fd, offset, (uint8_t *)file_info, read_len, true) != (int32_t)read_len) {
            ret = ERRCODE_UPG_FILE_READ_FAIL;
            break;
        }

        if (is_index) {
            ret = upg_resource_index_task(file_info);
        } else {
            ret = upg_resource_data_task(p, data, image, file_info);
        }
        if (ret != ERRCODE_SUCC) {
            break;
        }
    }

    upg_free(file_info);
    dfx_file_close(index_fd);
    return ret;
}

errcode_t uapi_upg_resource_index_process(const upg_image_header_t *image)
{
    errcode_t ret;
    upg_log_info("[UPG] resource index decompress_flag: 0x%x\r\n", image->decompress_flag);
    if (image->decompress_flag == DECOMPRESS_FLAG_ZIP) {
        /* 压缩升级 */
        upg_log_info("[UPG] decompress upg\r\n");
        ret = uapi_upg_compress_image_update(image);
    } else {
        /* 全镜像升级 */
        upg_log_info("[UPG] full upg\r\n");
        ret = uapi_upg_full_image_update(image);
    }

    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return upg_resource_process_node(NULL, NULL, image, true);
}

errcode_t uapi_upg_resource_data_process(const upg_image_header_t *image)
{
    errcode_t ret;
    CLzmaDec p;
    upg_lzma_decode2_data_t val_tmp = { 0 };

    if (image->decompress_flag == DECOMPRESS_FLAG_ZIP) {
        uint32_t read_offset = image->image_offset;
        uint8_t header[LZMA_HEAD_ALIGN_SIZE]; /* lzma解压头长度 */
        uint32_t read_len = (uint32_t)sizeof(header);

        ret = upg_copy_pkg_image_data(image, 0, &read_len, header);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        read_offset += read_len;

        ret = upg_lzma_init(&p, &val_tmp, header, sizeof(header));
        if (ret != ERRCODE_SUCC) {
            return ret;
        }

        val_tmp.image_id = image->image_id;
        val_tmp.in_offset = read_offset;
        val_tmp.out_offset = 0;
        val_tmp.compress_len = image->image_len - (uint32_t)sizeof(header);
    } else {
        val_tmp.in_offset = image->image_offset;
        val_tmp.buf.outbuf = upg_malloc(UPG_FLASH_PAGE_SIZE);
        if (val_tmp.buf.outbuf == NULL) {
            return ERRCODE_MALLOC;
        }
    }

    ret = upg_resource_process_node(&p, &val_tmp, image, false);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_resource_process_node fail ret = 0x%x\r\n", ret);
    }

    if (image->decompress_flag == DECOMPRESS_FLAG_ZIP) {
        upg_lzma_deinit(&p, &val_tmp);
    } else {
        upg_free(val_tmp.buf.outbuf);
    }

    return ret;
}
#endif /* (UPG_CFG_SUPPORT_RESOURCES_FILE == YES) */
