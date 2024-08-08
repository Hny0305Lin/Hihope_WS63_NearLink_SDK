/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG storage functions source file
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "securec.h"
#include "upg_config.h"
#if (UPG_CFG_SUPPORT_FILE_SYSTEM == YES)
#include "fcntl.h"
#include "sys/stat.h"
#include "sys/vfs.h"
#endif /* (UPG_CFG_SUPPORT_FILE_SYSTEM == YES) */
#include "errcode.h"
#include "common_def.h"
#include "partition.h"
#include "upg_definitions.h"
#include "upg_common.h"
#include "upg_common_porting.h"
#include "upg_alloc.h"
#include "upg_verify.h"
#include "upg_porting.h"
#include "upg_debug.h"

STATIC errcode_t upg_package_storage_write(uint32_t offset, const uint8_t *buff, uint16_t len, uint32_t pkg_len);

STATIC errcode_t upg_prepare_erase(uint32_t package_len)
{
    errcode_t ret;
    uint32_t status_start_addr;
    uint32_t status_size;
    unused(package_len);
    ret = upg_get_progress_status_start_addr(&status_start_addr, &status_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if (UPG_CFG_SUPPORT_FILE_SYSTEM == NO)
    partition_information_t info;
    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = upg_flash_erase(info.part_info.addr_info.addr, package_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
    ret = upg_flash_erase(status_start_addr, status_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ret;
}
/*
 * 存储升级包前对Flash的准备工作（包括擦除升级区、初始化升级标记中的head_before_offset和Head_magic）
 * prepare_info 分区准备信息，如升级包总长度
 */
errcode_t uapi_upg_prepare(upg_prepare_info_t *prepare_info)
{
    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    errcode_t ret;
    uint32_t flag_start_addr;
    if (prepare_info == NULL || prepare_info->package_len == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

#if (UPG_CFG_SUPPORT_FILE_SYSTEM == NO)
    if (prepare_info->package_len > uapi_upg_get_storage_size()) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }
#endif

    upg_storage_ctx_t *ctx = upg_get_ctx();
    ctx->packge_len = prepare_info->package_len;

    do {
        /* 擦除FOTA区 */
        ret = upg_prepare_erase(prepare_info->package_len);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        /* 初始化head_before_offset */
        ret = upg_get_upgrade_flag_flash_start_addr(&flag_start_addr);
        if (ret != ERRCODE_SUCC) {
            break;
        }
        uint32_t flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_before_offset);
        /* head_before_offset为FOTA区起始地址相对APP分区的偏移地址 */
        uint32_t head_before_offset = 0;
        ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&head_before_offset, false);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        /* 初始化 package_length */
        flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, package_length);
        ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&prepare_info->package_len, false);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        /* 初始化 Head_magic */
        uint32_t head_magic = UPG_HEAD_MAGIC;
        flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_magic);
        ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&head_magic, false);
        if (ret != ERRCODE_SUCC) {
            break;
        }
    } while (0);
    return ret;
}

/* 重置升级标记，准备升级 */
errcode_t uapi_upg_reset_upgrade_flag(void)
{
    errcode_t ret = 0;
    uint32_t flag_start_addr;
    uint32_t upg_flag_address = 0;
    uint32_t status_size;

    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    ret = upg_get_progress_status_start_addr(&upg_flag_address, &status_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = upg_get_upgrade_flag_flash_start_addr(&flag_start_addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 擦除标记区前，读取package_len */
    uint32_t package_len = 0;
    uint32_t flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, package_length);
    ret = upg_flash_read(flash_offset, sizeof(uint32_t), (uint8_t *)&package_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 擦除标记区 */
    ret = upg_flash_erase(upg_flag_address, status_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 初始化 head_before_offset */
    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_before_offset);
    uint32_t head_before_offset = 0;
    ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&head_before_offset, false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 初始化 Head_magic */
    uint32_t head_magic = UPG_HEAD_MAGIC;
    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_magic);
    ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&head_magic, false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 回写package_length */
    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, package_length);
    return upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&package_len, false);
}

STATIC errcode_t upg_check_buff(const uint8_t *buff, uint16_t len)
{
    if (buff == NULL) {
        return ERRCODE_UPG_NULL_POINTER;
    }
    if (len == 0) {
        return ERRCODE_UPG_INVALID_BUFF_LEN;
    }

    return ERRCODE_SUCC;
}

/*
 * 升级包存入本地存储空间
 * offset     要存入本地存储空间的位置（相对起始位置的偏移）
 * buff       要存入的数据包数据指针
 * len        要存入的数据包长度（4字节对齐）
 */
STATIC errcode_t upg_write_package(uint32_t offset, const uint8_t *buff, uint16_t len)
{
    if (!upg_is_inited()) {
        return ERRCODE_UPG_NOT_INIT;
    }

    errcode_t ret = upg_check_buff(buff, len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    upg_storage_ctx_t *ctx = upg_get_ctx();
    if (ctx->packge_len == 0) {
        return ERRCODE_UPG_NOT_PREPARED;
    }

    return upg_package_storage_write(offset, buff, len, ctx->packge_len);
}

/*
 * 升级包存入本地存储空间,异步接口
 * offset     要存入本地存储空间的位置（相对起始位置的偏移）
 * buff       要存入的数据包数据指针
 * len        要存入的数据包长度（4字节对齐）
 * callback   写入完成的回调函数。
 */
errcode_t uapi_upg_write_package_async(uint32_t offset, const uint8_t *buff, uint16_t len,
                                       uapi_upg_write_done_cb callback)
{
    errcode_t ret = upg_write_package(offset, buff, len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (callback != NULL) {
        callback(ret);
    }
    return ret;
}

/*
 * 升级包存入本地存储空间,同步接口
 * offset     要存入本地存储空间的位置（相对起始位置的偏移）
 * buff       要存入的数据包数据指针
 * len        要存入的数据包长度（4字节对齐）
 */
errcode_t uapi_upg_write_package_sync(uint32_t offset, const uint8_t *buff, uint16_t len)
{
    return upg_write_package(offset, buff, len);
}

/*
 * 从本地存储空间读取升级包数据
 * offset     要读取的本地存储空间的位置（相对与升级包起始位置的偏移）
 * buff       存放数据包的空间指针
 * len        要读取的数据包长度（4字节对齐）
 */
errcode_t uapi_upg_read_package(uint32_t offset, uint8_t *buff, uint32_t len)
{
    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    errcode_t ret = upg_check_buff(buff, (uint16_t)len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return upg_read_fota_pkg_data(offset, buff, (uint32_t *)&len);
}

STATIC errcode_t upg_get_firmware_number_in_package(const upg_package_header_t *pkg_header, uint32_t *firmware_num)
{
    errcode_t ret;
    upg_image_hash_node_t *img_hash_table = NULL;

    uint32_t image_num = pkg_header->info_area.image_num;
    uint32_t fw_num = 0;

    if (image_num > UPG_FIRMWARE_MAX_NUM) {
        return ERRCODE_UPG_WRONG_IMAGE_NUM;
    }

    ret = upg_get_pkg_image_hash_table((const upg_package_header_t *)pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("[UPG] upg_get_pkg_image_hash_table fail\r\n");
        return ret;
    }

    for (uint32_t i = 0; i < image_num; i++) {
        if (img_hash_table[i].image_id != UPG_IMAGE_ID_NV) {
            fw_num++;
        }
    }
    *firmware_num = fw_num;
    upg_free(img_hash_table);
    return ret;
}

/*
 * 申请开始本地升级
 * firmware_num 升级包中firmware数量
 */
STATIC errcode_t upg_upgrade_request(uint32_t firmware_num)
{
    uint32_t flag_start_address = 0;
    errcode_t ret = upg_get_upgrade_flag_flash_start_addr(&flag_start_address);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 写入firmware_num */
    uint32_t flash_offset = (uint32_t)flag_start_address + offsetof(fota_upgrade_flag_area_t, firmware_num);
    ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)(&firmware_num), false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 写入Head_end_magic */
    uint32_t head_end_magic = UPG_END_MAGIC;
    flash_offset = (uint32_t)flag_start_address + offsetof(fota_upgrade_flag_area_t, head_end_magic);
    ret = upg_flash_write(flash_offset, sizeof(uint32_t), (uint8_t *)&(head_end_magic), false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_upg_request_upgrade(bool reset)
{
    errcode_t ret;
    upg_package_header_t *pkg_header = NULL;
    uint32_t firmware_num = 0;

    if (upg_is_inited() == false) {
        return ERRCODE_UPG_NOT_INIT;
    }

    ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        upg_log_err("[UPG] upg_get_package_header fail\r\n");
        return ret;
    }

#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
    ret = uapi_upg_verify_file((const upg_package_header_t *)pkg_header);
    if (ret != ERRCODE_SUCC) {
        upg_free(pkg_header);
        upg_log_err("[UPG] uapi_upg_verify_file fail, ret = 0x%x\r\n", ret);
        return ret;
    }
#endif

    ret = upg_get_firmware_number_in_package((const upg_package_header_t *)pkg_header, &firmware_num);
    if (ret != ERRCODE_SUCC) {
        upg_free(pkg_header);
        upg_log_err("[UPG] upg_get_firmware_number_in_package fail, ret = 0x%x\r\n", ret);
        return ret;
    }

    upg_free(pkg_header);

    ret = upg_upgrade_request(firmware_num);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_upgrade_request fail. ret = 0x%x\r\n", ret);
        return ret;
    }
    /* app中下载后直接开始升级流程，升级流程结束后重启 */
    if (reset) {
        /* IOT设备重启 */
        upg_reboot();
    }
    return ERRCODE_SUCC;
}

#if (UPG_CFG_SUPPORT_FILE_SYSTEM == YES)
STATIC errcode_t upg_package_storage_write(uint32_t offset, const uint8_t *buff, uint16_t len, uint32_t pkg_len)
{
    uint16_t write_len = 0;
    static FILE *wr_fd = NULL;
    static uint32_t wr_pos = 0;
    errcode_t ret = ERRCODE_SUCC;

    if (wr_fd == NULL && (upg_get_pkg_file_path() != NULL)) {
        /* 第一次调用，打开文件 */
        wr_fd = fopen(upg_get_pkg_file_path(), "wb");
        if (wr_fd == NULL) {
            return ERRCODE_UPG_FILE_OPEN_FAIL;
        }
    }

    if (offset != wr_pos) {
        ret = ERRCODE_UPG_INVALID_OFFSET;
        goto end;
    }

    uint16_t left_len = (uint16_t)((wr_pos + len > pkg_len) ? pkg_len - wr_pos : len);
    while (left_len > 0) {
        /* 一次未写完，可多次读取 */
        uint16_t tmp = (uint16_t)fwrite(buff + write_len, 1, left_len, wr_fd);
        if (tmp == 0) {
            /* 写入失败，中断写入操作 */
            upg_log_err("[UPG] set file error : %d\r\n", ferror(wr_fd));
            ret = ERRCODE_UPG_FILE_WRITE_FAIL;
            goto end;
        }
        left_len -= tmp;
        write_len += tmp;
    }
    wr_pos = (uint32_t)ftell(wr_fd);
end:
    if (ret != ERRCODE_SUCC || wr_pos >= pkg_len) {
        /* 失败或者全部写完后，关闭文件 */
        fclose(wr_fd);
        wr_fd = NULL;
        wr_pos = 0;
    }
    return ret;
}
#else
STATIC errcode_t upg_package_storage_write(uint32_t offset, const uint8_t *buff, uint16_t len, uint32_t pkg_len)
{
    uint32_t start_addr = 0;
    uint32_t size = 0;
    errcode_t ret = upg_get_fota_partiton_area_addr(&start_addr, &size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    size -= UPG_META_DATA_LENGTH + UPG_UPGRADE_FLAG_LENGTH;
    if (offset >= size || offset + len > size || offset >= pkg_len || offset + len > pkg_len) {
        return ERRCODE_UPG_NO_ENOUGH_SPACE;
    }

    return upg_flash_write(start_addr + offset, len, buff, false); /* prepare过程已擦除整个fota区，此处不需写前擦 */
}
#endif

upg_status_t uapi_upg_get_status(void)
{
    return upg_get_ctx()->upg_status;
}