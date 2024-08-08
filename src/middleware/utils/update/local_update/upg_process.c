/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG process management functions source file
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "securec.h"
#include "common_def.h"
#include "partition.h"
#include "upg_definitions.h"
#include "errcode.h"
#include "upg_common.h"
#include "upg_common_porting.h"
#include "upg_alloc.h"
#include "upg_porting.h"
#include "upg_config.h"
#include "upg_debug.h"

#if ((UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM == YES) || (UPG_CFG_SUPPORT_RESOURCES_FILE == YES))
#include "fcntl.h"
#include "sys/stat.h"
#include "sys/vfs.h"
#endif

#define NOT_START_FLAG 0xFF

STATIC errcode_t upg_get_package_info(fota_upgrade_flag_area_t *upg_flag,
                                      const upg_package_header_t *pkg_header, upg_package_info_t *pkg_info)
{
    errcode_t ret;
    upg_image_hash_node_t *img_hash_table = NULL;
    int32_t image_index;
    int32_t firmware_index;
    uint32_t image_num;

    pkg_info->total_new_fw_size = 0;
    pkg_info->finished_fw_size = 0;

    ret = upg_get_pkg_image_hash_table(pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("[UPG] upg_get_pkg_image_hash_table fail\r\n");
        return ret;
    }

    image_num = pkg_header->info_area.image_num;

    for (image_index = 0, firmware_index = 0; image_index < (int32_t)image_num; image_index++, firmware_index++) {
        if (img_hash_table[image_index].image_id == UPG_IMAGE_ID_NV) {
            firmware_index--;
            continue;
        }

        upg_image_header_t *img_header = NULL;
        ret = upg_get_pkg_image_header((const upg_image_hash_node_t *)&(img_hash_table[image_index]), &img_header);
        if (ret != ERRCODE_SUCC || img_header == NULL) {
            upg_log_err("[UPG] upg_get_pkg_image_header fail\r\n");
            upg_free(img_hash_table);
            return ret;
        }

        pkg_info->total_new_fw_size += img_header->new_image_len;

        upg_image_status_t status;
        status = upg_get_image_update_status(upg_flag, (uint32_t)firmware_index, img_hash_table[image_index].image_id);
        if (status == UPG_IMAGE_STATUS_FINISHED) {
            pkg_info->finished_fw_size += img_header->new_image_len;
        }
        upg_free(img_header);
    }
    upg_free(img_hash_table);
    upg_log_info("[UPG] package info total= 0x%x, finished = 0x%x\r\n",
        pkg_info->total_new_fw_size, pkg_info->finished_fw_size);

    return ret;
}

#if defined(UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE) && defined(YES) && (UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE == YES)
__attribute__((weak)) errcode_t upg_erase_whole_image(const upg_image_header_t *img_header)
{
    unused(img_header);
    return ERRCODE_SUCC;
}
#endif
/* 执行指定固件的更新任务 */
STATIC errcode_t upg_perform_image_task(const upg_image_header_t *img_header)
{
    errcode_t ret;
    if (img_header->image_id != UPG_IMAGE_ID_RES_INDEX && img_header->image_id != UPG_IMAGE_ID_RES_DATA) {
        upg_log_info("[UPG] image decompress_flag: 0x%x\r\n", img_header->decompress_flag);
        if (img_header->decompress_flag == DECOMPRESS_FLAG_ZIP) {
            /* 压缩升级 */
            upg_log_info("[UPG] decompress upg\r\n");
#if defined(UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE) && defined(YES) && (UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE == YES)
            ret = upg_erase_whole_image(img_header);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
#endif
            ret = uapi_upg_compress_image_update(img_header);
        } else if (img_header->decompress_flag == DECOMPRESS_FLAG_DIFF) {
            /* 差分升级 */
            upg_log_info("[UPG] diff upg\r\n");
            ret = uapi_upg_diff_image_update(img_header);
        } else {
            /* 全镜像升级 */
            upg_log_info("[UPG] full upg\r\n");
#if defined(UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE) && defined(YES) && (UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE == YES)
            ret = upg_erase_whole_image(img_header);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
#endif
            ret = uapi_upg_full_image_update(img_header);
        }
#if (UPG_CFG_SUPPORT_RESOURCES_FILE == YES)
    } else if (img_header->image_id == UPG_IMAGE_ID_RES_INDEX) {
        ret = uapi_upg_resource_index_process(img_header);
    } else {
        ret = uapi_upg_resource_data_process(img_header);
#else
    } else {
        ret = ERRCODE_UPG_NOT_SUPPORTED;
#endif
    }

    return ret;
}

STATIC errcode_t upg_perform_upgrade_task(const upg_image_header_t *img_header,
                                          const upg_image_hash_node_t *img_hash_table, uint32_t firmware_index,
                                          upg_image_status_t status, fota_upgrade_flag_area_t *upg_flag)
{
    errcode_t ret;
    upg_image_status_switch_t switch_status;

    if (status == UPG_IMAGE_STATUS_NOT_STARTED) {
        ret = upg_flash_erase_metadata_pages();
        if (ret != ERRCODE_SUCC) {
            upg_log_err("[UPG] upg_flash_erase_metadata_pages fail.\r\n");
            goto end;
        }
    }

#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
    /* 校验Image, 如果校验失败，再次尝试 */
    ret = uapi_upg_verify_file_image(img_header, img_hash_table->image_hash, SHA_256_LENGTH,
                                     status == UPG_IMAGE_STATUS_NOT_STARTED);
    if (ret != ERRCODE_SUCC) {
        goto end;
    }
#else
    unused(img_hash_table);
#endif

    ret = upg_perform_image_task(img_header);
end:
    if (ret == ERRCODE_SUCC) {
        /* 校验并升级成功，设置升级标记为FINISH */
        switch_status = UPG_IMAGE_STATUS_SWITCH_TO_FINISHED;
#if (UPG_CFG_ANTI_ROLLBACK_SUPPORT == YES)
        (void)upg_anti_rollback_version_update(img_header);
#endif
    } else {
        /* 校验或升级不成功，设置升级标记为RETRY */
        switch_status = UPG_IMAGE_STATUS_SWITCH_TO_RETRY;
    }

    upg_log_info("[UPG] switch status = 0x%x\r\n", (uint32_t)switch_status);
    errcode_t status_ret = upg_set_firmware_update_status(upg_flag, firmware_index, switch_status);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return status_ret;
}

/* 执行NV镜像的处理任务 */
STATIC errcode_t upg_perform_nv_task(const upg_image_header_t *img_header,
                                     uint32_t image_header_offset, fota_upgrade_flag_area_t *upg_flag)
{
    errcode_t ret;

    if (img_header->image_id != UPG_IMAGE_ID_NV) {
        return ERRCODE_UPG_INVALID_IMAGE_ID;
    }

    upg_flag->nv_data_offset = img_header->image_offset;
    upg_flag->nv_data_len = upg_aligned(img_header->image_len, 16); /* 16-byte alignment */
    upg_flag->nv_hash_offset = image_header_offset + offsetof(upg_image_header_t, image_hash);
    upg_flag->nv_hash_len = SHA_256_LENGTH;

    uint32_t fota_flag_addr = 0;
    ret = upg_get_upgrade_flag_flash_start_addr(&fota_flag_addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t nv_addr = fota_flag_addr + offsetof(fota_upgrade_flag_area_t, nv_data_offset);
    uint32_t nv_info_len = 4 * (uint32_t)sizeof(uint32_t); /* write 4 fields of u32 */
    ret = upg_flash_write(nv_addr, nv_info_len, (uint8_t *)&(upg_flag->nv_data_offset), false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return upg_set_firmware_update_status(upg_flag, UPG_IMAGE_ID_NV, UPG_IMAGE_STATUS_SWITCH_TO_STARTED);
}

STATIC errcode_t upg_process_update_image_tasks(fota_upgrade_flag_area_t *upg_flag,
                                                uint32_t image_num, const upg_image_hash_node_t *hash_table)
{
    upg_image_header_t *img_header = NULL;
    int32_t img_idx;
    int32_t fw_idx;
    errcode_t ret = ERRCODE_SUCC;

    for (img_idx = 0, fw_idx = 0; img_idx < (int32_t)image_num; img_idx++, fw_idx++) {
        if (hash_table[img_idx].image_id == UPG_IMAGE_ID_NV) {
            fw_idx--;
        }

        if (!upg_img_in_set(hash_table[img_idx].image_id)) {
            continue;
        }

        upg_image_status_t status =
            upg_get_image_update_status(upg_flag, (uint32_t)fw_idx, hash_table[img_idx].image_id);
        /* 如果该Image已经完成，处理下一个image */
        if (status == UPG_IMAGE_STATUS_FINISHED) {
            upg_log_info("[UPG] The image has finished. image_id = 0x%x\r\n", hash_table[img_idx].image_id);
            continue;
        }

        /* 获取Image Header */
        ret = upg_get_pkg_image_header(&(hash_table[img_idx]), &img_header);
        if (ret != ERRCODE_SUCC || img_header == NULL) {
            upg_log_err("[UPG] upg_get_pkg_image_header fail.\r\n");
            goto ret_free;
        }

        /* 执行Image的更新 */
        if (img_header->image_id != UPG_IMAGE_ID_NV) {
            /* 设置升级标记为STARTED */
            ret = upg_set_firmware_update_status(upg_flag, (uint32_t)fw_idx, UPG_IMAGE_STATUS_SWITCH_TO_STARTED);
            if (ret != ERRCODE_SUCC) {
                upg_log_err("[UPG] upg_set_firmware_update_status fail\r\n");
                goto ret_free;
            }
            upg_log_info("[UPG] start perform update image : 0x%x\r\n", img_header->image_id);
            ret = upg_perform_upgrade_task(img_header, &(hash_table[img_idx]), (uint32_t)fw_idx, status, upg_flag);
            upg_log_info("[UPG] perform update image over. ret = 0x%x\r\n", ret);

            if (ret != ERRCODE_SUCC) {
                goto ret_free;
            }
        } else {
            upg_log_info("[UPG] start perform NV image : 0x%x\r\n", img_header->image_id);
            ret = upg_perform_nv_task(img_header, hash_table[img_idx].image_addr, upg_flag);
            upg_log_info("[UPG] perform NV image over. ret = 0x%x\r\n", ret);
        }

        upg_free(img_header);
        img_header = NULL;
        upg_watchdog_kick();
    }
ret_free:
    if (img_header) {
        upg_free(img_header);
    }
    return ret;
}

STATIC errcode_t upg_process_update(fota_upgrade_flag_area_t *upg_flag, const upg_package_header_t *pkg_header)
{
    errcode_t ret_val = ERRCODE_SUCC;
    upg_image_hash_node_t *img_hash_table = NULL;

    uint32_t image_num = pkg_header->info_area.image_num;
    upg_log_info("[UPG] update image number = 0x%x\r\n", image_num);
    upg_log_info("[UPG] update firmware number = 0x%x\r\n", upg_flag->firmware_num);

    /* 升级标记中的firmware数量与升级包中的image数量不一致 */
    if ((upg_flag->firmware_num != image_num && upg_flag->firmware_num != image_num - 1) ||
        (upg_flag->firmware_num > UPG_FIRMWARE_MAX_NUM)) {
        return ERRCODE_UPG_WRONG_IMAGE_NUM;
    }

    ret_val = upg_get_pkg_image_hash_table((const upg_package_header_t *)pkg_header, &img_hash_table);
    if (ret_val != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("[UPG] upg_get_pkg_image_hash_table fail\r\n");
        return ret_val;
    }

    ret_val = upg_process_update_image_tasks(upg_flag, image_num, (const upg_image_hash_node_t *)img_hash_table);
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_process_update_image_tasks fail, ret = 0x%x\r\n", ret_val);
    }
    upg_free(img_hash_table);
    return ret_val;
}

#if (UPG_CFG_VERIFICATION_SUPPORT == YES) && (UPG_CFG_INTEGRITY_VERIFICATION_ONLY == NO)
bool upg_check_first_entry(const fota_upgrade_flag_area_t *upg_flag_info)
{
    uint8_t check_flag[UPG_FLAG_RETYR_TIMES] = {NOT_START_FLAG, NOT_START_FLAG, NOT_START_FLAG};
    for (uint32_t i = 0; i < upg_flag_info->firmware_num; i++) {
        if (memcmp(upg_flag_info->firmware_flag[i], check_flag, UPG_FLAG_RETYR_TIMES) != 0) {
            return false;
        }
    }
    return true;
}
#endif

/* 开始升级 */
errcode_t uapi_upg_start(void)
{
    fota_upgrade_flag_area_t *upg_flag_info = NULL;
    upg_package_header_t     *pkg_header = NULL;
    errcode_t                 ret;
    uint32_t                  img_num = 0;
    upg_package_info_t       *pkg_info = NULL;
    bool                      direct_finish = true;

    if (upg_is_inited() == false) {
        ret = ERRCODE_UPG_NOT_INIT;
        goto end;
    }

    ret = upg_alloc_and_get_upgrade_flag(&upg_flag_info);
    if (ret != ERRCODE_SUCC || upg_flag_info == NULL) {
        goto end;
    }

    /* 判断升级区有没有升级包 */
    if (!(upg_flag_info->head_magic == UPG_HEAD_MAGIC &&
        upg_flag_info->head_end_magic == UPG_END_MAGIC && upg_flag_info->complete_flag != 0)) {
        /* 不需要升级直接返回 */
        upg_log_err("[UPG] Not need to upgrade...\r\n");
        ret = ERRCODE_UPG_NOT_NEED_TO_UPDATE;
        goto end;
    }

    ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        upg_log_err("[UPG] upg_get_package_header fail\r\n");
        goto end;
    }

#if (UPG_CFG_VERIFICATION_SUPPORT == YES) && (UPG_CFG_INTEGRITY_VERIFICATION_ONLY == NO)
    /* 升级包的整包校验 */
    if (upg_check_first_entry((const fota_upgrade_flag_area_t *)upg_flag_info)) {
        ret = uapi_upg_verify_file((const upg_package_header_t *)pkg_header);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
#endif

    pkg_info = &upg_get_ctx()->package_info;
    ret = upg_get_package_info(upg_flag_info, (const upg_package_header_t *)pkg_header, pkg_info);
    if (ret != ERRCODE_SUCC) {
        upg_set_temporary_result(UPG_RESULT_VERIFY_HEAD_FAILED);
        goto end;
    }

    ret = upg_process_update(upg_flag_info, (const upg_package_header_t *)pkg_header);
    direct_finish = false;
    img_num = pkg_header->info_area.image_num;
end:
    /* 更新complete_flag */
    upg_set_complete_flag(img_num, ret, direct_finish);
    upg_free(upg_flag_info);
    upg_free(pkg_header);
    return ret;
}

/* 注册升级进度通知回调函数 */
errcode_t uapi_upg_register_progress_callback(uapi_upg_progress_cb func)
{
#if (UPG_CFG_PROCESS_NOTIFY_SUPPORT == YES)
    upg_get_ctx()->progress_cb = func;
    return ERRCODE_SUCC;
#else
    unused(func);
    return ERRCODE_UPG_NOT_SUPPORTED;
#endif
}

/* 计算升级进度并通知上层 */
void upg_calculate_and_notify_process(uint32_t current_size)
{
#if (UPG_CFG_PROCESS_NOTIFY_SUPPORT == YES)
    static uint32_t last_percent = 0;
    uint32_t percent = 0;
    upg_package_info_t *pkg_info = &upg_get_ctx()->package_info;

    if (upg_get_ctx()->progress_cb != NULL) {
        pkg_info->finished_fw_size += current_size;
        if (pkg_info->total_new_fw_size != 0) {
            percent = pkg_info->finished_fw_size * 100 / pkg_info->total_new_fw_size; /* 100: percent */
        }
        if (percent != last_percent) {
            upg_get_ctx()->progress_cb(percent);
            last_percent = percent;
        }
    }
#else
    unused(current_size);
#endif
}

#if ((UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM == YES) || (UPG_CFG_SUPPORT_RESOURCES_FILE == YES))
STATIC errcode_t upg_write_new_image_data_on_fs(const char *file_path, uint32_t write_offset,
                                                uint8_t *buffer, uint32_t *write_len)
{
    uint16_t real_len = 0;
    FILE *wr_fd = NULL;

    if (write_offset == 0) {
        wr_fd = fopen(file_path, "wb");
    } else {
        wr_fd = fopen(file_path, "rb+");
    }
    if (wr_fd == NULL) {
        upg_log_err("[UPG] open %s fail!\r\n", file_path);
        return ERRCODE_UPG_FILE_OPEN_FAIL;
    }

    errcode_t ret = fseek(wr_fd, write_offset, SEEK_SET);
    if (ret != 0) {
        ret = ERRCODE_UPG_FILE_SEEK_FAIL;
        upg_log_err("[UPG] seek file fail!\r\n");
        goto end;
    }

    uint16_t left_len = *write_len;
    while (left_len > 0) {
        /* 一次未写完，可多次读取 */
        uint16_t tmp = fwrite(buffer + real_len, 1, left_len, wr_fd);
        if (tmp == 0) {
            /* 写入失败，中断写入操作 */
            if (ferror(wr_fd)) {
                ret = ERRCODE_UPG_FILE_WRITE_FAIL;
                upg_log_err("[UPG] write file fail!\r\n");
                goto end;
            }
        }
        left_len -= tmp;
        real_len += tmp;
    }

end:
    *write_len = real_len;
    (void)fclose(wr_fd);
    return ret;
}
#endif

/*
 * 将buffer中的数据写入指定image_id的镜像所在的地址上
 * write_offset 相对镜像起始地址的偏移
 * buffer       写入数据的buffer指针
 * write_len    输入buffer的长度，输出实际写入的数据长度
 * image_id     镜像的ID
 */
errcode_t upg_write_new_image_data(uint32_t write_offset, uint8_t *buffer, uint32_t *write_len, uint32_t image_id,
    bool do_erase)
{
    errcode_t ret = ERRCODE_SUCC;
    partition_information_t image_info = {0};

    if (image_id == PARAMS_PARTITION_IMAGE_ID) {
        /* 参数区地址信息 */
        image_info.type = PARTITION_BY_ADDRESS;
        image_info.part_info.addr_info.addr = PARAMS_PARTITION_START_ADDR;
        image_info.part_info.addr_info.size = PARAMS_PARTITION_LENGTH;
#if (UPG_CFG_SUPPORT_RESOURCES_FILE == YES)
    } else if (image_id == UPG_IMAGE_ID_RES_INDEX) {
        image_info.type = PARTITION_BY_PATH;
        image_info.part_info.file_path = upg_get_res_file_index_path();
#endif
    } else {
        ret = upg_get_image_info(image_id, &image_info);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    if (image_info.type == PARTITION_BY_ADDRESS) {
        ret = upg_flash_write(
            image_info.part_info.addr_info.addr + write_offset, *write_len, (uint8_t *)buffer, do_erase); /* 写前擦除 */
    } else { /* 分区类型：PARTITION_BY_PATH */
#if ((UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM == YES) || (UPG_CFG_SUPPORT_RESOURCES_FILE == YES))
        ret = upg_write_new_image_data_on_fs(
            (const char *)image_info.part_info.file_path, write_offset, buffer, write_len);
#else
        ret = ERRCODE_IMAGE_CONFIG_NOT_FOUND;
#endif
    }
    return ret;
}
