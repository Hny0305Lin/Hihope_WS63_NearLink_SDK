/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG common functions header file
 */

#ifndef UPG_COMMON_H
#define UPG_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"
#include "partition.h"
#include "upg.h"
#include "upg_definitions.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static inline uint32_t upg_aligned(uint32_t len, uint32_t align)
{
    return ((uint32_t)(len) + ((align) - 1)) & ~((align) - 1);
}

/**** upg_commom ****/
upg_storage_ctx_t *upg_get_ctx(void);

/*
 * 获取在当前Flash上指定固件镜像的地址信息。该地址为flash上的相对地址，是相对flash基地址的偏移
 * image_id      固件的镜像ID
 * start_address 返回该镜像的起始地址
 * size          返回该镜像区域的大小
 */
errcode_t upg_get_partition_info(uint32_t image_id, uint32_t *start_address, uint32_t *size);

/*
 * 获取指定固件镜像的地址信息。
 * image_id      固件的镜像ID
 * image_info    返回该镜像的信息
 */
errcode_t upg_get_image_info(uint32_t image_id, partition_information_t *image_info);

/*
 * 读取升级包中的数据到buffer中
 * read_offset 相对升级包起始的偏移
 * buffer 读取数据buffer指针
 * read_len 输入buffer的长度，输出实际读到的数据长度
 */
errcode_t upg_read_fota_pkg_data(uint32_t read_offset, uint8_t *buffer, uint32_t *read_len);

/* 获取升级标记结构到RAM中 */
errcode_t upg_alloc_and_get_upgrade_flag(fota_upgrade_flag_area_t **upg_flag);

/*
 * 获取升级包包头结构指针
 * pkg_header 返回升级包头结构指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *            (如果采用直接访问flash的方式，返回升级包头所在的flash地址)
 */
errcode_t upg_get_package_header(upg_package_header_t **pkg_header);

/*
 * 获取镜像哈希表结构指针
 * pkg_header 升级包头结构指针
 * img_hash_table 返回对应的升级镜像HASH表头指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *                (如果采用直接访问flash的方式，返回升级镜像哈希表所在的flash地址)
 */
errcode_t upg_get_pkg_image_hash_table(const upg_package_header_t *pkg_header,
                                       upg_image_hash_node_t **img_hash_table);

/*
 * 获取升级镜像的头结构指针
 * img_hash_table 升级镜像HASH表节点指针
 * img_header 返回对应的升级镜像头指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *            (如果采用直接访问flash的方式，返回镜像头所在的flash地址)
 */
errcode_t upg_get_pkg_image_header(const upg_image_hash_node_t *img_hash_table, upg_image_header_t **img_header);

/*
 * 获取升级镜像的数据指针
 * img_header 升级镜像头结构指针
 * data_offset 相对升级镜像数据开头的偏移
 * data_len 输出要获取数据的长度，输出实际获取到的数据长度
 * img_data 返回升级镜像数据的指针，指针指向的空间在函数内分配，需要使用者使用完后调用upg_free释放。
 *          (如果采用直接访问flash的方式，返回数据所在的flash地址)
 */
errcode_t upg_get_pkg_image_data(const upg_image_header_t *img_header,
                                 uint32_t data_offset, uint32_t *data_len, uint8_t **img_data);

/*
 * 拷贝升级镜像指定范围的数据至buffer中
 * img_header 升级镜像头结构指针
 * data_offset 相对升级镜像数据开头的偏移
 * data_len 输入要拷贝的数据的长度，输出实际拷贝的数据长度
 * img_data 保存数据的buffer指针，buffer的空间需要使用者分配
 */
errcode_t upg_copy_pkg_image_data(const upg_image_header_t *img_header,
                                  uint32_t data_offset, uint32_t *data_len, uint8_t *img_data);

/*
 * 将buffer中的数据写入指定image_id的镜像所在的地址上
 * write_offset 相对镜像起始地址的偏移
 * buffer       写入数据的buffer指针
 * write_len    输入buffer的长度，输出实际写入的数据长度
 * image_id     镜像的ID
 * do_erase     是否执行写前擦
 */
errcode_t upg_write_new_image_data(uint32_t write_offset, uint8_t *buffer, uint32_t *write_len, uint32_t image_id,
    bool do_erase);

/*
 * 从指定image_id的镜像所在的地址上读取数据到buffer中
 * write_offset 相对镜像起始地址的偏移
 * buffer       存储数据的buffer指针
 * write_len    buffer的长度
 * image_id     镜像的ID
 */
errcode_t upg_read_old_image_data(uint32_t read_offset, uint8_t *buffer, uint32_t *read_len, uint32_t image_id);

/*
 * 获取升级包中的升级镜像的升级标记状态(NOT_STARTED/STARTED/RETRY/FINISHED)
 * firmware_index为升级包中的固件（除NV之外的镜像）的序号
 * 如升级包中包含 固件0、固件1、固件2、NV、固件3，其中固件3的序号为3而不是4
 * 如为NV镜像，firmware_index参数可忽略
 */
upg_image_status_t upg_get_image_update_status(fota_upgrade_flag_area_t *upg_flag,
                                               uint32_t firmware_index, uint32_t image_id);

/*
 * 设置指定固件（不包含NV）的升级标记(STARTED/RETRY/FINISHED)
 * firmware_index为升级包中的固件（除NV之外的镜像）的序号
 * 如升级包中包含 固件0、固件1、固件2、NV、固件3，其中固件3的序号为3而不是4
 */
errcode_t upg_set_firmware_update_status(fota_upgrade_flag_area_t *upg_flag,
                                         uint32_t firmware_index, upg_image_status_switch_t switch_status);
errcode_t upg_set_nv_update_flag(fota_upgrade_flag_area_t *upg_flag, upg_image_status_t status);

/* 擦除metadata数据区 */
errcode_t upg_flash_erase_metadata_pages(void);

/* 设置升级结果(临时保存) */
void upg_set_temporary_result(upg_result_t result);

/* 获取临时保存的升级结果 */
upg_result_t upg_get_temporary_result(void);

/* 检查是否所有镜像都已完成升级（包括升级失败但是已尝试最大次数） */
bool upg_check_image_update_complete(const fota_upgrade_flag_area_t *upg_flag, uint32_t image_num);

/* 将升级结果保存至Flash升级标记区 */
void upg_set_update_result(upg_result_t result);

/* 设置升级完成标记 */
void upg_set_complete_flag(uint32_t image_num, errcode_t result, bool direct_finish);

/* 获取注册函数列表 */
upg_func_t *upg_get_func_list(void);

/* update模块是否已经初始化 */
bool upg_is_inited(void);

/* 镜像是否在当前程序支持升级 */
bool upg_img_in_set(uint32_t img_id);

/**** upg_process ****/
/* 计算升级进度并通知上层 */
void upg_calculate_and_notify_process(uint32_t current_size);


/**** upg_upgrade ****/
/* FOTA压缩升级 */
errcode_t uapi_upg_compress_image_update(const upg_image_header_t *image);

/* FOTA差分升级 */
errcode_t uapi_upg_diff_image_update(const upg_image_header_t *image);

/* FOTA全镜像升级 */
errcode_t uapi_upg_full_image_update(const upg_image_header_t *image);

/* 防回滚号校验 */
errcode_t upg_anti_rollback_version_verify(
    const upg_package_header_t *pkg_header, const upg_image_header_t *img_header);

/* 更新防回滚版本号 */
errcode_t upg_anti_rollback_version_update(const upg_image_header_t *img_header);

/* 判断升级包是否为第一次启动后验签 */
#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
bool upg_check_first_entry(const fota_upgrade_flag_area_t *upg_flag_info);
#endif

/**** upg_resource ****/
/* 处理资源文件 */
errcode_t uapi_upg_resource_index_process(const upg_image_header_t *image);
errcode_t uapi_upg_resource_data_process(const upg_image_header_t *image);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* UPG_COMMON_H */
