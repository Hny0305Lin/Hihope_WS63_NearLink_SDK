/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides partition manage api. \n
 *
 * History: \n
 * 2022-09-01, Create file. \n
 */
#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include "errcode.h"
#include "partition_resource_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_partition_api Partition
 * @ingroup  middleware_utils
 * @{
 */

/**
 * @if Eng
 * @brief  Partition storage location type.
 * @else
 * @brief  分区位置类型
 * @endif
 */
typedef enum partition_type {
    PARTITION_BY_ADDRESS,  /*!< @if Eng Partition stored on an memory address.
                                  @else   分区存储在内存地址上 @endif */
    PARTITION_BY_PATH,     /*!< @if Eng Partition stored on filesystem.
                                  @else   分区存储在文件系统上 @endif */
    PARTITION_TYPE_COUNT   /*!< @if Eng type count, not a valid type.
                                  @else   类型数量，不作为类型使用 @endif */
} partition_type_t;

/**
 * @if Eng
 * @brief  Partition information.
 * @else
 * @brief  分区信息结构
 * @endif
 */
typedef struct partition_information {
    partition_type_t type; /*!< @if Eng Partition storage location type.
                                  @else   分区存储位置类型 @endif */
    union {
        struct {
            uint32_t addr; /*!< @if Eng Address of partition if partition stored on an memory address.
                                @else   如果分区存储在内存地址上，addr存储分区的起始地址 @endif */
            uint32_t size; /*!< @if Eng Byte length of partition if partition stored on flash or ram.
                                @else   如果分区存储在内存地址上，size存储分区的字节长度 @endif */
        } addr_info;       /*!< @if Eng The address info of the partition.
                                @else   分区的地址信息 @endif */
        char *file_path;   /*!< @if Eng File path if image partition on filesystem.
                                @else   如果分区存在文件系统上，file_path标识存储分区所在路径 @endif */
    } part_info;           /*!< @if Eng the union of partition information(address or file path).
                                @else   保存分区信息（地址或文件路径）的共同体 @endif */
} partition_information_t;

/**
 * @if Eng
 * @brief  Initialize the partition management module.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  分区模块初始化
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @endif
 */
errcode_t uapi_partition_init(void);

/**
 * @if Eng
 * @brief  Obtaining Partition Information.
 * @par Description: Obtains information about a partition specified by partition_id.
 * The partition information includes the start address and byte length of the partition.
 * @param  [in] partition_id Id of specified partition. see @ref partition_ids_t
 * @param  [out] info Used to save the information when obtained successfully. see @ref partition_information_t
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li partition_resource_id.h
 * @else
 * @brief  获取分区信息
 * @par 说明: 获取partition_id指定的分区信息
 * @param  [in] partition_id 指定的分区的ID。 参考 @ref partition_ids_t
 * @param  [out] info 用来保存获取到的分区信息。 参考 @ref partition_information_t
 * @retval ERRCODE_SUCC         成功返回#ERRCODE_SUCC
 * @retval Others               失败返回#ERRCODE_FAIL或其他值
 * @par 依赖:
 * @li partition_resource_id.h
 * @endif
 */
errcode_t uapi_partition_get_info(partition_ids_t partition_id, partition_information_t *info);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
