/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG common functions source file
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "upg_config.h"
#if (UPG_CFG_SUPPORT_FILE_SYSTEM == YES)
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/vfs.h"
#include "dfx_file_operation.h"
#endif /* (UPG_CFG_SUPPORT_FILE_SYSTEM == YES) */
#include "upg_common_porting.h"
#include "errcode.h"
#include "securec.h"
#include "partition.h"
#include "upg_alloc.h"
#include "upg_otp_reg.h"
#include "upg.h"
#include "upg_porting.h"
#include "upg_config.h"
#include "upg_debug.h"
#include "upg_common.h"

#define NOT_START_FLAG      0xFF
#define STARTED_FLAG        0x0F
#define FINISHED_FLAG       0x00
#define UPG_FINISH_HALF_FLAG 0xFFFF
#define UPG_FINISH_ALL_FLAG 0
#define UPG_ABNORMAL_FLAG 0x5a5a5a5a
#define PER_FILE_STORAGE_MAX_SIZE   4294967295

STATIC upg_storage_ctx_t g_upg_ctx = {0};
upg_storage_ctx_t *upg_get_ctx(void)
{
    return &g_upg_ctx;
}

/* 获取升级标记结构到RAM中 */
errcode_t upg_alloc_and_get_upgrade_flag(fota_upgrade_flag_area_t **upg_flag)
{
    uint32_t start_addr = 0;
    errcode_t ret_val;

    ret_val = upg_get_upgrade_flag_flash_start_addr(&start_addr);
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_get_upgrade_flag_flash_start_addr fail\r\n");
        return ret_val;
    }

    *upg_flag = (fota_upgrade_flag_area_t*)upg_malloc(sizeof(fota_upgrade_flag_area_t));
    if (*upg_flag == NULL) {
        upg_log_err("[UPG] upg_alloc_and_get_upgrade_flag upg_malloc fail\r\n");
        return ERRCODE_MALLOC;
    }

    ret_val = upg_flash_read(start_addr, sizeof(fota_upgrade_flag_area_t), (uint8_t *)(*upg_flag));
    if (ret_val != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_alloc_and_get_upgrade_flag read flash fail\r\n");
        upg_free(*upg_flag);
        *upg_flag = NULL;
        return ret_val;
    }

    return ERRCODE_SUCC;
}

/*
 * 获取在当前Flash上指定固件镜像的地址信息。该地址为flash上的相对地址，是相对flash基地址的偏移
 * image_id      固件的镜像ID
 * start_address 返回该镜像的起始地址
 * size          返回该镜像区域的大小
 */
errcode_t upg_get_partition_info(uint32_t image_id, uint32_t *start_address, uint32_t *size)
{
    errcode_t ret_val = ERRCODE_SUCC;
    partition_information_t image_info = {0};

    if (image_id == PARAMS_PARTITION_IMAGE_ID) {
        /* 参数区地址信息 */
        image_info.type = PARTITION_BY_ADDRESS;
        image_info.part_info.addr_info.addr = PARAMS_PARTITION_START_ADDR;
        image_info.part_info.addr_info.size = PARAMS_PARTITION_LENGTH;
#if UPG_CFG_SUPPORT_RESOURCES_FILE == YES
    } else if (image_id == UPG_IMAGE_ID_RES_INDEX) {
        image_info.type = PARTITION_BY_PATH;
#endif
    } else {
        ret_val = upg_get_image_info(image_id, &image_info);
        if (ret_val != ERRCODE_SUCC) {
            return ret_val;
        }
    }

    if (image_info.type == PARTITION_BY_ADDRESS) {
        *start_address = image_info.part_info.addr_info.addr;
        *size = image_info.part_info.addr_info.size;
    } else { /* PARTITION_BY_PATH */
        *start_address = 0;
        *size = uapi_upg_get_storage_size();
    }

    return ret_val;
}

STATIC errcode_t upg_img_id_convert_to_partition_id(uint32_t image_id, partition_ids_t *item_id)
{
    upg_image_partition_ids_map_t *map = NULL;
    uint32_t cnt = upg_get_ids_map(&map);
    if (cnt == 0 || map == NULL || item_id == NULL) {
        return ERRCODE_FAIL;
    }
    for (uint32_t i = 0; i < cnt; i++) {
        if (map[i].image_id == image_id) {
            *item_id = map[i].item_id;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t upg_get_image_info(uint32_t image_id, partition_information_t *image_info)
{
    partition_ids_t item_id;
    if (image_info == NULL || upg_img_id_convert_to_partition_id(image_id, &item_id) != ERRCODE_SUCC) {
        return ERRCODE_PARTITION_INVALID_PARAMS;
    }

    return uapi_partition_get_info(item_id, image_info);
}

#if (UPG_CFG_SUPPORT_FILE_SYSTEM == NO)
/*
 * 读取升级包中的数据到buffer中
 * read_offset 相对升级包开头的偏移
 * buffer      读取数据buffer指针
 * read_len    输入buffer的长度，输出实际读到的数据长度
 */
errcode_t upg_read_fota_pkg_data(uint32_t read_offset, uint8_t *buffer, uint32_t *read_len)
{
    errcode_t ret_val;
    uint32_t start_addr = 0;
    uint32_t size = 0;
    uint32_t actual_len;

    ret_val = upg_get_fota_partiton_area_addr(&start_addr, &size);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    if (read_offset >= size || *read_len == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    actual_len = ((read_offset + *read_len) > size) ? (size - read_offset) : *read_len;
    start_addr += read_offset;

    ret_val = upg_flash_read(start_addr, actual_len, (uint8_t *)buffer);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *read_len = actual_len;
    return ERRCODE_SUCC;
}

STATIC errcode_t upg_package_get_storage_max_size(uint32_t *size)
{
    uint32_t start_addr = 0;
    uint32_t fota_size = 0;
    errcode_t ret = upg_get_fota_partiton_area_addr(&start_addr, &fota_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    fota_size -= UPG_META_DATA_LENGTH + UPG_UPGRADE_FLAG_LENGTH;
    *size = fota_size;
    return ERRCODE_SUCC;
}
#else
errcode_t upg_read_fota_pkg_data(uint32_t read_offset, uint8_t *buffer, uint32_t *read_len)
{
    uint32_t len = 0;
    struct stat stat_buff = {0};
    errcode_t ret_code = ERRCODE_SUCC;
    uint32_t ret = (uint32_t)stat(upg_get_pkg_file_path(), &stat_buff);
    if (ret != 0 || stat_buff.st_size == 0) {
        return ERRCODE_UPG_EMPTY_FILE;
    }

    /* create ota image file */
    int32_t rd_fd = open(upg_get_pkg_file_path(), O_RDONLY);
    if (rd_fd < 0) {
        return ERRCODE_UPG_FILE_OPEN_FAIL;
    }

    if ((long int)read_offset >= (long int)stat_buff.st_size) {
        upg_log_err("[UPG] The read_offset is more than file size!\r\n");
        ret_code = ERRCODE_UPG_FILE_READ_FAIL;
        goto end;
    }

    long int left_len = ((long int)(read_offset + *read_len) > stat_buff.st_size) ?
                        stat_buff.st_size - (long int)read_offset : (long int)(*read_len);
    ret = (uint32_t)lseek(rd_fd, read_offset, SEEK_SET);
    if (ret != read_offset) {
        ret_code = ERRCODE_UPG_FILE_SEEK_FAIL;
        goto end;
    }

    while (left_len > 0) {
        int32_t tmp = read(rd_fd, buffer + len, (size_t)left_len);
        if (tmp <= 0) {
            upg_log_err("[UPG] read_fota_pkg_data failed\r\n");
            ret_code = ERRCODE_UPG_FILE_READ_FAIL;
            goto end;
        }
        left_len -= tmp;
        len += (uint32_t)tmp;
    }

end:
    *read_len = len;
    (void)close(rd_fd);
    return ret_code;
}

STATIC errcode_t upg_package_get_storage_max_size(uint32_t *size)
{
    struct statfs sfs;
    int result;
    uint64_t free_size;
    (void)memset_s(&sfs, sizeof(sfs), 0, sizeof(sfs));

    result = statfs(upg_get_pkg_file_dir(), &sfs);
    if (result != 0 || sfs.f_type == 0) {
        upg_log_err("[UPG] statfs failed! Invalid argument!\r\n");
        return ERRCODE_FAIL;
    }

    free_size = (uint64_t)sfs.f_bsize * sfs.f_bfree;
    if (free_size <= PER_FILE_STORAGE_MAX_SIZE) {
        *size = (uint32_t)free_size;
    } else {
        *size = PER_FILE_STORAGE_MAX_SIZE;
    }
    return ERRCODE_SUCC;
}
#endif

/*
 * 获取本地存储空间可存储升级包的最大空间
 * 返回最大空间
 */
uint32_t uapi_upg_get_storage_size(void)
{
    if (upg_is_inited() == false) {
        return 0;
    }

    uint32_t size;
    if (upg_package_get_storage_max_size(&size) != ERRCODE_SUCC) {
        return 0;
    }
    return size;
}

/*
 * 获取升级包包头结构指针
 * pkg_header 返回升级包头结构指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *            (如果采用直接访问flash的方式，返回升级包头所在的flash地址)
 */
errcode_t upg_get_package_header(upg_package_header_t **pkg_header)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == NO)
    errcode_t ret;
    uint32_t actual_len = (uint32_t)sizeof(upg_package_header_t);

    *pkg_header = upg_malloc(sizeof(upg_package_header_t));
    if (*pkg_header == NULL) {
        return ERRCODE_MALLOC;
    }

    ret = upg_read_fota_pkg_data(0, (uint8_t *)(*pkg_header), &actual_len);
    if (ret != ERRCODE_SUCC || actual_len != sizeof(upg_package_header_t)) {
        upg_free(*pkg_header);
        *pkg_header = NULL;
        return ret;
    }
#else
    uint32_t start_addr = 0;
    uint32_t size = 0;
    errcode_t ret;
    ret = upg_get_fota_partiton_area_addr(&start_addr, &size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    *pkg_header = (upg_package_header_t*)(uintptr_t)(start_addr + upg_get_flash_base_addr());
#endif /* #if UPG_CFG_DIRECT_FLASH_ACCESS */

    return ERRCODE_SUCC;
}

/*
 * 获取镜像哈希表结构指针
 * pkg_header 升级包头结构指针
 * img_hash_table 返回对应的升级镜像HASH表头指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *                (如果采用直接访问flash的方式，返回升级镜像哈希表所在的flash地址)
 */
errcode_t upg_get_pkg_image_hash_table(const upg_package_header_t *pkg_header,
                                       upg_image_hash_node_t **img_hash_table)
{
    uint32_t offset;
    errcode_t ret_val;

    if (pkg_header->info_area.image_hash_table_addr == 0) {
        offset = (uint32_t)sizeof(upg_package_header_t);
    } else {
        offset = pkg_header->info_area.image_hash_table_addr;
    }

#if (UPG_CFG_DIRECT_FLASH_ACCESS == NO)
    /* 由于ImageHashTable为了16字节对齐有填充字段，此处的长度不能使用image_num * sizeof(upg_image_hash_node_t)) */
    uint32_t actual_len = pkg_header->info_area.image_hash_table_length;

    *img_hash_table = upg_malloc(actual_len);
    if (*img_hash_table == NULL) {
        return ERRCODE_MALLOC;
    }

    ret_val = upg_read_fota_pkg_data(offset, (uint8_t *)(*img_hash_table), &actual_len);
    if (ret_val != ERRCODE_SUCC || actual_len != pkg_header->info_area.image_hash_table_length) {
        upg_free(*img_hash_table);
        *img_hash_table = NULL;
        return ret_val;
    }

#else
    uint32_t start_addr = 0;
    uint32_t size = 0;

    ret_val = upg_get_fota_partiton_area_addr(&start_addr, &size);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    start_addr += offset;

    *img_hash_table = (upg_image_hash_node_t*)(uintptr_t)(start_addr + upg_get_flash_base_addr());
#endif /* #if UPG_CFG_DIRECT_FLASH_ACCESS */

    return ERRCODE_SUCC;
}

/*
 * 获取升级镜像的头结构指针
 * img_hash_table 升级镜像HASH表节点指针
 * img_header 返回对应的升级镜像头指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *            (如果采用直接访问flash的方式，返回镜像头所在的flash地址)
 */
errcode_t upg_get_pkg_image_header(const upg_image_hash_node_t *img_hash_table, upg_image_header_t **img_header)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == NO)
    errcode_t ret_val;
    uint32_t actual_len = (uint32_t)sizeof(upg_image_header_t);

    *img_header = upg_malloc(sizeof(upg_image_header_t));
    if (*img_header == NULL) {
        return ERRCODE_MALLOC;
    }

    ret_val = upg_read_fota_pkg_data(img_hash_table->image_addr, (uint8_t *)(*img_header), &actual_len);
    if (ret_val != ERRCODE_SUCC || actual_len != sizeof(upg_image_header_t)) {
        upg_free(*img_header);
        *img_header = NULL;
        return ret_val;
    }
#else
    uint32_t start_addr = 0;
    uint32_t size = 0;
    errcode_t ret_val;
    ret_val = upg_get_fota_partiton_area_addr(&start_addr, &size);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    start_addr += img_hash_table->image_addr;

    *img_header = (upg_image_header_t*)(uintptr_t)(start_addr + upg_get_flash_base_addr());
#endif /* #if UPG_CFG_DIRECT_FLASH_ACCESS */

    return ERRCODE_SUCC;
}

/*
 * 获取升级镜像的数据指针
 * img_header 升级镜像头结构指针
 * data_offset 相对升级镜像数据开头的偏移
 * data_len 输入要获取数据的长度，输出实际获取到的数据长度
 * img_data 返回升级镜像数据的指针
            如果未采用直接访问flash方式（即UPG_CFG_DIRECT_FLASH_ACCESS==NO），返回的指针指向的空间在函数内分配，使用者使用完后
                必须调用upg_free释放。
 *          如果采用直接访问flash的方式（即UPG_CFG_DIRECT_FLASH_ACCESS==YES），返回指针为数据所在的flash地址，使用者无需释放。
 */
errcode_t upg_get_pkg_image_data(const upg_image_header_t *img_header,
                                 uint32_t data_offset, uint32_t *data_len, uint8_t **img_data)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == NO)
    errcode_t ret_val;
    *img_data = upg_malloc(*data_len);
    if (*img_data == NULL) {
        return ERRCODE_MALLOC;
    }

    ret_val = upg_copy_pkg_image_data(img_header, data_offset, data_len, *img_data);
    if (ret_val != ERRCODE_SUCC) {
        upg_free(*img_data);
        *img_data = NULL;
        return ret_val;
    }
#else
    uint32_t start_addr = 0;
    uint32_t size = 0;
    uint32_t actual_len;
    errcode_t ret_val;
    uint32_t aligned_image_len = upg_aligned(img_header->image_len, 16); /* 16-byte alignment */

    ret_val = upg_get_fota_partiton_area_addr(&start_addr, &size);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    if (data_offset >= aligned_image_len || img_data == NULL ||
        data_len == NULL || *data_len == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    actual_len = (data_offset + *data_len > aligned_image_len) ? (aligned_image_len - data_offset) : *data_len;

    start_addr += img_header->image_offset;
    *img_data = (uint8_t*)(uintptr_t)(start_addr + data_offset + upg_get_flash_base_addr());
    *data_len = actual_len;
#endif /* #if UPG_CFG_DIRECT_FLASH_ACCESS */
    return ERRCODE_SUCC;
}

/*
 * 拷贝升级镜像指定范围的数据至buffer中
 * img_header 升级镜像头结构指针
 * data_offset 相对升级镜像数据开头的偏移
 * data_len 输出要拷贝的数据的长度，输出实际拷贝的数据长度
 * img_data 保存数据的buffer指针，buffer的空间需要使用者分配
 */
errcode_t upg_copy_pkg_image_data(const upg_image_header_t *img_header,
                                  uint32_t data_offset, uint32_t *data_len, uint8_t *img_data)
{
    errcode_t ret_val;
    uint32_t aligned_image_len = upg_aligned(img_header->image_len, 16); /* 16-byte alignment */
    uint32_t actual_len;

    if (data_offset >= aligned_image_len || img_data == NULL ||
        data_len == NULL || *data_len == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    actual_len = (data_offset + *data_len > aligned_image_len) ? (aligned_image_len - data_offset) : *data_len;

    ret_val = upg_read_fota_pkg_data(img_header->image_offset + data_offset, img_data, &actual_len);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *data_len = actual_len;
    return ERRCODE_SUCC;
}

#if UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM == YES
STATIC errcode_t upg_read_old_image_data_from_fs(const char *file_path, uint32_t read_offset,
                                                 uint8_t *buffer, uint32_t *read_len)
{
    uint16_t len = 0;
    struct stat stat_buff = {0};
    uint32_t ret = stat(file_path, &stat_buff);
    if (ret != 0 || stat_buff.st_size == 0) {
        return ERRCODE_UPG_EMPTY_FILE;
    }

    /* create ota image file */
    FILE *rd_fd = fopen(file_path, "rb");
    if (rd_fd == NULL) {
        upg_log_err("[UPG] open %s fail!\r\n", file_path);
        return ERRCODE_UPG_FILE_OPEN_FAIL;
    }

    if ((long int)read_offset > stat_buff.st_size) {
        upg_log_err("[UPG] The read_offset is more than file size!\r\n");
        ret = ERRCODE_UPG_FILE_READ_FAIL;
        goto end;
    }

    long int left_len = ((long int)(read_offset + *read_len) > stat_buff.st_size) ?
                        stat_buff.st_size - (long int)read_offset : (long int)(*read_len);
    ret = fseek(rd_fd, read_offset, SEEK_SET);
    if (ret != 0) {
        upg_log_err("[UPG] seek file fail!\r\n");
        ret = ERRCODE_UPG_FILE_SEEK_FAIL;
        goto end;
    }

    while (left_len > 0) {
        uint16_t tmp_read = fread(buffer + len, 1, left_len, rd_fd);
        if (tmp_read == 0) {
            if (ferror(rd_fd)) {
                upg_log_err("[UPG] read file fail!\r\n");
                ret = ERRCODE_UPG_FILE_READ_FAIL;
                goto end;
            }
        }
        left_len -= tmp_read;
        len += tmp_read;
    }

end:
    *read_len = len;
    (void)fclose(rd_fd);
    return ret;
}
#endif

/*
 * 从指定image_id的镜像所在的地址上读取数据到buffer中
 * write_offset 相对镜像起始地址的偏移
 * buffer       存储数据的buffer指针
 * write_len    buffer的长度
 * image_id     镜像的ID
 */
errcode_t upg_read_old_image_data(uint32_t read_offset, uint8_t *buffer, uint32_t *read_len, uint32_t image_id)
{
    errcode_t ret_val;
    partition_information_t image_info = {0};

    if (image_id == PARAMS_PARTITION_IMAGE_ID) {
        /* 参数区地址信息 */
        image_info.type = PARTITION_BY_ADDRESS;
        image_info.part_info.addr_info.size = PARAMS_PARTITION_LENGTH;
        image_info.part_info.addr_info.addr = PARAMS_PARTITION_START_ADDR;
    } else {
        ret_val = upg_get_image_info(image_id, &image_info);
        if (ret_val != ERRCODE_SUCC) {
            return ret_val;
        }
    }

    if (image_info.type == PARTITION_BY_ADDRESS) {
        uint32_t app_address = image_info.part_info.addr_info.addr;
        ret_val = upg_flash_read(app_address + read_offset, *read_len, (uint8_t *)buffer);
    } else { /* 分区类型：PARTITION_BY_PATH */
#if UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM == YES
        ret_val = upg_read_old_image_data_from_fs(
            (const char *)image_info.part_info.file_path, read_offset, buffer, read_len);
#else
        ret_val = ERRCODE_UPG_INVALID_IMAGE_ID;
#endif
    }
    return ret_val;
}

STATIC errcode_t upg_get_firmware_flag_address(uint32_t firmware_index, uint32_t current_loop, uint32_t *flag_addr)
{
    uint32_t fota_flag_addr = 0;
    uint32_t addr;

    errcode_t ret = upg_get_upgrade_flag_flash_start_addr(&fota_flag_addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (firmware_index == UPG_IMAGE_ID_NV) {
        addr = fota_flag_addr + offsetof(fota_upgrade_flag_area_t, nv_flag);
        addr += current_loop * (uint32_t)sizeof(uint8_t);
    } else {
        addr = fota_flag_addr + offsetof(fota_upgrade_flag_area_t, firmware_flag);
        addr += (firmware_index * UPG_FLAG_RETYR_TIMES) + (current_loop * (uint32_t)sizeof(uint8_t));
    }

    *flag_addr = addr;
    return ERRCODE_SUCC;
}

/*
 * 设置指定固件的升级标记
 * firmware_index为升级包中的固件（除NV之外的镜像）的序号
 * 如升级包中包含 固件0、固件1、固件2、NV、固件3，其中固件3的序号为3而不是4
 */
errcode_t upg_set_firmware_update_status(fota_upgrade_flag_area_t *upg_flag,
                                         uint32_t firmware_index, upg_image_status_switch_t switch_status)
{
    uint32_t write_len = 0;
    uint32_t flag_addr = 0;
    uint8_t *firmware_flag;
    uint32_t i;

    if (firmware_index == UPG_IMAGE_ID_NV) {
        firmware_flag = upg_flag->nv_flag;
    } else {
        firmware_flag = upg_flag->firmware_flag[firmware_index];
    }

    /* 获取第一个非0的flag, 即第i次重试的flag */
    for (i = 0; i < UPG_FLAG_RETYR_TIMES; i++) {
        if (firmware_flag[i] != 0) {
            break;
        }
    }

    if (i >= UPG_FLAG_RETYR_TIMES) {
        return ERRCODE_SUCC;
    }

    errcode_t ret = upg_get_firmware_flag_address(firmware_index, i, &flag_addr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if ((switch_status == UPG_IMAGE_STATUS_SWITCH_TO_STARTED) && (firmware_flag[i] == NOT_START_FLAG)) {
        /* 当前flag为NOT_START, 要切换成STARTED */
        write_len = 1;
        firmware_flag[i] = STARTED_FLAG;
    } else if ((switch_status == UPG_IMAGE_STATUS_SWITCH_TO_RETRY) && (firmware_flag[i] == STARTED_FLAG)) {
        /* 当前flag为STARTED, 要切换成RETRY, 即当前flag置为0 */
        write_len = 1;
        firmware_flag[i] = FINISHED_FLAG;
    } else if (switch_status == UPG_IMAGE_STATUS_SWITCH_TO_FINISHED) {
        /* 要切换成FINISH，即全部flag置为0 */
        write_len = UPG_FLAG_RETYR_TIMES - i;
        (void)memset_s(&firmware_flag[i], write_len, FINISHED_FLAG, write_len);
    } else {
        /* 其他情况不做处理 */
        return ERRCODE_SUCC;
    }

    ret = upg_flash_write(flag_addr, write_len, (uint8_t *)&(firmware_flag[i]), false);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_flash_write upgrader flag fail. ret = 0x%x\r\n", ret);
        return ret;
    }

    if (i >= UPG_FLAG_RETYR_TIMES - 1 && firmware_flag[i] == FINISHED_FLAG) {
        upg_log_err("[UPG] retry times all failed\r\n");
        upg_set_temporary_result(UPG_RESULT_RETRY_ALL_FAILED);
        return ERRCODE_UPG_RETRY_ALL_FAIL;
    }
    return ERRCODE_SUCC;
}

/*
 * 获取升级包中的升级镜像的升级标记状态(NOT_STARTED/STARTED/RETRY/FINISHED)
 * firmware_index为升级包中的固件（除NV之外的镜像）的序号
 * 如升级包中包含 固件0、固件1、固件2、NV、固件3，其中固件3的序号为3而不是4
 * 如为NV镜像，firmware_index参数可忽略
 */
upg_image_status_t upg_get_image_update_status(fota_upgrade_flag_area_t *upg_flag,
                                               uint32_t firmware_index, uint32_t image_id)
{
    uint8_t *firmware_flag;
    uint8_t finished_flag[UPG_FLAG_RETYR_TIMES] = { FINISHED_FLAG, FINISHED_FLAG, FINISHED_FLAG };
    uint8_t init_flag[UPG_FLAG_RETYR_TIMES] = { NOT_START_FLAG, NOT_START_FLAG, NOT_START_FLAG };
    uint32_t i;

    if (image_id == UPG_IMAGE_ID_NV) {
        firmware_flag = upg_flag->nv_flag;
    } else {
        firmware_flag = upg_flag->firmware_flag[firmware_index];
    }

    if (memcmp(firmware_flag, finished_flag, UPG_FLAG_RETYR_TIMES) == 0) {
        /* 3个flag都是0x00, 处理完成状态 */
        return UPG_IMAGE_STATUS_FINISHED;
    } else if (memcmp(firmware_flag, init_flag, UPG_FLAG_RETYR_TIMES) == 0) {
        /* 3个flag都是0xFF, 未开始处理状态 */
        return UPG_IMAGE_STATUS_NOT_STARTED;
    } else {
        /* 获取第一个非0的flag，即当前正在处理的第i次重试的flag */
        for (i = 0; i < UPG_FLAG_RETYR_TIMES; i++) {
            if (firmware_flag[i] != 0) {
                break;
            }
        }

        if (firmware_flag[i] == STARTED_FLAG) {
            return UPG_IMAGE_STATUS_STARTED;
        } else if (firmware_flag[i] == NOT_START_FLAG) {
            return UPG_IMAGE_STATUS_RETRY;
        } else {
            return UPG_IMAGE_STATUS_INVALID;
        }
    }
}

/* 擦除metadata数据区 */
errcode_t upg_flash_erase_metadata_pages(void)
{
    return ERRCODE_SUCC;
}

/* 设置升级结果(临时保存) */
void upg_set_temporary_result(upg_result_t result)
{
    upg_get_ctx()->temporary_result = result;
}

/* 获取临时保存的升级结果 */
upg_result_t upg_get_temporary_result(void)
{
    return upg_get_ctx()->temporary_result;
}

/* 将升级结果保存至Flash升级标记区 */
void upg_set_update_result(upg_result_t result)
{
    uint32_t fota_flag_addr = 0;
    uint32_t result_tmp = (uint32_t)result;
    errcode_t ret = upg_get_upgrade_flag_flash_start_addr(&fota_flag_addr);
    if (ret != ERRCODE_SUCC) {
        return;
    }

    uint32_t result_addr = fota_flag_addr + offsetof(fota_upgrade_flag_area_t, update_result);
    ret = upg_flash_write(result_addr, sizeof(uint32_t), (uint8_t *)&(result_tmp), false);
    if (ret != ERRCODE_SUCC) {
        return;
    }
    return;
}

/* 检查是否所有镜像都已完成升级（包括升级失败但是已尝试最大次数） */
bool upg_check_image_update_complete(const fota_upgrade_flag_area_t *upg_flag, uint32_t image_num)
{
    uint32_t firmware_index;
    const uint8_t *firmware_flag;
    uint8_t finish_flag[UPG_FLAG_RETYR_TIMES] = {FINISHED_FLAG, FINISHED_FLAG, FINISHED_FLAG};

    for (firmware_index = 0; firmware_index < upg_flag->firmware_num; firmware_index++) {
        firmware_flag = upg_flag->firmware_flag[firmware_index];
        /* 存在升级包未完成升级 */
        if (memcmp(firmware_flag, finish_flag, UPG_FLAG_RETYR_TIMES) != 0) {
            return false;
        }
    }

    /* 存在nv升级包，并且nv升级准备工作未完成 */
    if (upg_flag->firmware_num != image_num && upg_flag->nv_flag[0] != STARTED_FLAG) { /* index 0:start flag */
        return false;
    }
    return true;
}

STATIC void upg_write_complete_flag(uint32_t fota_flag_addr, uint32_t complete_flag)
{
    if (complete_flag != UPG_ABNORMAL_FLAG) {
        uint32_t flag_addr = fota_flag_addr + offsetof(fota_upgrade_flag_area_t, complete_flag);
        upg_log_info("[UPG] write complete: 0x%x\r\n", complete_flag);
        if (upg_flash_write(flag_addr, sizeof(uint32_t), (uint8_t *)&(complete_flag), false) != ERRCODE_SUCC) {
            return;
        }
    }

#if (UPG_CFG_SUPPORT_RESOURCES_FILE == YES)
    if (complete_flag == 0) {
        dfx_file_delete(upg_get_res_file_index_path());
    }
#endif
}
/* 设置升级完成标记 */
void upg_set_complete_flag(uint32_t image_num, errcode_t result, bool direct_finish)
{
    uint32_t fota_flag_addr = 0;
    uint32_t complete_flag = UPG_ABNORMAL_FLAG;
    bool image_complete = false;

    errcode_t ret = upg_get_upgrade_flag_flash_start_addr(&fota_flag_addr);
    if (ret != ERRCODE_SUCC) {
        return;
    }

    fota_upgrade_flag_area_t upg_flag;
    ret = upg_flash_read(fota_flag_addr, sizeof(fota_upgrade_flag_area_t), (uint8_t *)(&upg_flag));
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg_flash_read flag fail\r\n");
        return;
    }

    do {
        /*
         * 1. 当前程序中升级流程返回失败，直接结束升级流程（如整包校验失败），
         *    则升级流程失败，结束升级，将flag置成 UPG_FINISH_ALL_FLAG；
         */
        if (direct_finish && (upg_flag.complete_flag != UPG_FINISH_ALL_FLAG)) {
            complete_flag = UPG_FINISH_ALL_FLAG;
            /* 保存升级结果至升级标记区内 */
            upg_set_update_result(upg_get_temporary_result());
            break;
        }

        /*
         * 2. 当前程序中升级流程返回成功，
         *    即当前程序中支持升级的所有镜像成功升级，但存在还需要升级的镜像，则将flag置成 UPG_FINISH_HALF_FLAG；
         */
        image_complete = upg_check_image_update_complete(&upg_flag, image_num);
        if (result == ERRCODE_SUCC && !image_complete) {
            if (upg_flag.complete_flag != UPG_FINISH_HALF_FLAG) {
                complete_flag = UPG_FINISH_HALF_FLAG;
            }
        } else if (image_complete && (upg_flag.complete_flag != UPG_FINISH_ALL_FLAG)) {
            /*
             * 3. 若升级包中所有镜像成功升级，将flag置成 UPG_FINISH_ALL_FLAG；
             */
            complete_flag = UPG_FINISH_ALL_FLAG;
            if (result == ERRCODE_SUCC) {
                upg_set_temporary_result(UPG_RESULT_UPDATE_SUCCESS);
            }
            /* 保存升级结果至升级标记区内 */
            upg_set_update_result(upg_get_temporary_result());
        } else if (upg_get_temporary_result() == UPG_RESULT_RETRY_ALL_FAILED &&
            (upg_flag.complete_flag != UPG_FINISH_ALL_FLAG)) {
            /*
             * 4. 当前程序中升级流程返回失败，但最后一个已处理的镜像的3个flag全0，即已经重试最大次数，仍然失败，
             *    则升级流程失败，结束升级，将flag置成 UPG_FINISH_ALL_FLAG；
             */
            complete_flag = UPG_FINISH_ALL_FLAG;
            upg_set_update_result(upg_get_temporary_result());
        }
    } while (0);

    /* 升级标记需要修改 */
    upg_write_complete_flag(fota_flag_addr, complete_flag);
}

/* 获取升级结果 */
errcode_t uapi_upg_get_result(upg_result_t *result, uint32_t *last_image_index)
{
    if (result == NULL || last_image_index == NULL) {
        return ERRCODE_UPG_NULL_POINTER;
    }
    fota_upgrade_flag_area_t *upg_flag_info;
    errcode_t ret = upg_alloc_and_get_upgrade_flag(&upg_flag_info);
    if (ret != ERRCODE_SUCC || upg_flag_info == NULL) {
        return ret;
    }

    *result = upg_flag_info->update_result;
    *last_image_index = UINT32_MAX;
    upg_free(upg_flag_info);
    return ERRCODE_SUCC;
}

/* 获取注册函数列表 */
upg_func_t *upg_get_func_list(void)
{
    return &(g_upg_ctx.func_list);
}

bool upg_is_inited(void)
{
    return g_upg_ctx.inited;
}

STATIC upg_status_t upg_get_status(void)
{
    uint8_t temp = 0;
    upg_status_t status = UPG_STATUS_NONE;
    uint32_t flag_addr = 0;
    fota_upgrade_flag_area_t *upg_flag_info = NULL;
    if (upg_alloc_and_get_upgrade_flag(&upg_flag_info) != ERRCODE_SUCC) {
        return status;
    }

    do {
        if (upg_flag_info->head_magic != UPG_HEAD_MAGIC || upg_flag_info->head_end_magic != UPG_END_MAGIC) {
            status = UPG_STATUS_NONE;
            break;
        }

        if (upg_flag_info->complete_flag != 0) {
            status = UPG_STATUS_UPDATING;
            break;
        }

        if (upg_flag_info->ver_change_flag == 0) {
            status = UPG_STATUS_NONE;
            break;
        }

        if (upg_flag_info->update_result == UPG_RESULT_UPDATE_SUCCESS) {
            status = UPG_STATUS_SUCC;
        } else {
            status = UPG_STATUS_FAIL;
        }
        if (upg_get_upgrade_flag_flash_start_addr(&flag_addr) != ERRCODE_SUCC) {
            status = UPG_STATUS_NONE;
            break;
        }

        flag_addr += offsetof(fota_upgrade_flag_area_t, ver_change_flag);
        if (upg_flash_write(flag_addr, sizeof(uint8_t), &temp, false) != ERRCODE_SUCC) {
            status = UPG_STATUS_NONE;
        }
    } while (0);

    upg_free(upg_flag_info);
    return status;
}

errcode_t uapi_upg_init(const upg_func_t *func_list)
{
    if (upg_is_inited()) {
        return ERRCODE_UPG_ALREADY_INIT;
    }

    if (func_list != NULL) {
        g_upg_ctx.func_list.malloc = func_list->malloc;
        g_upg_ctx.func_list.free = func_list->free;
        g_upg_ctx.func_list.serial_putc = func_list->serial_putc;
    }
    g_upg_ctx.temporary_result = UPG_RESULT_MAX;
    g_upg_ctx.packge_len = 0;
    g_upg_ctx.inited = true;
    g_upg_ctx.upg_status = upg_get_status();
    return ERRCODE_SUCC;
}

#if (UPG_CFG_ANTI_ROLLBACK_SUPPORT == YES)
errcode_t upg_anti_rollback_version_verify(
    const upg_package_header_t *pkg_header, const upg_image_header_t *img_header)
{
    // NV及资源文件不做防回滚
    if (img_header->image_id == UPG_IMAGE_ID_NV || img_header->image_id == UPG_IMAGE_ID_RES_INDEX ||
        img_header->image_id == UPG_IMAGE_ID_RES_DATA) {
        return ERRCODE_SUCC;
    }
    uint32_t image_id = img_header->image_id;
    uint32_t key_ver = pkg_header->key_area.fota_key_version_ext;
    uint32_t code_ver = img_header->version_ext;
    uint32_t otp_ver;

    errcode_t ret = upg_get_board_rollback_version(image_id, &otp_ver);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t board_key_mask;
    uint32_t board_code_mask;

    ret = upg_get_board_version_mask(image_id, &board_key_mask, &board_code_mask);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (((key_ver & board_key_mask) < (otp_ver & board_key_mask)) ||
        ((code_ver & board_code_mask) < (otp_ver & board_code_mask))) {
        return ERRCODE_FAIL;
    }
    upg_log_info("[UPG] upg verify: anti rollback version_verify ok. image_id = 0x%x\r\n", img_header->image_id);
    return ERRCODE_SUCC;
}

errcode_t upg_anti_rollback_version_update(const upg_image_header_t *img_header)
{
    // NV及资源文件不做防回滚
    if (img_header->image_id == UPG_IMAGE_ID_NV || img_header->image_id == UPG_IMAGE_ID_RES_INDEX ||
        img_header->image_id == UPG_IMAGE_ID_RES_DATA) {
        return ERRCODE_SUCC;
    }

    uint32_t image_id = img_header->image_id;
    uint32_t key_ver = 0;
    uint32_t board_key_mask = 0;
    uint32_t code_ver = 0;
    uint32_t board_code_mask = 0;
    uint32_t new_ver;
    uint32_t old_ver;

    errcode_t ret = upg_get_board_rollback_version(image_id, &old_ver);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    // 获取更新后的板端的掩码
    ret = upg_get_board_version_mask(image_id, &board_key_mask, &board_code_mask);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
     // 获取更新后的板端镜像版本
    ret = upg_get_board_version(image_id, &key_ver, &code_ver);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if ((code_ver & board_code_mask) < (old_ver & board_code_mask) ||
        (key_ver & board_key_mask) < (old_ver & board_key_mask)) {
        return ERRCODE_FAIL;
    }

    new_ver = (code_ver & board_code_mask) | (key_ver & board_key_mask);
    return upg_set_board_rollback_version(image_id, &new_ver);
}
#endif
