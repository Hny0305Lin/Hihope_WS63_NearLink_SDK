/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.\n
 * Description: NV STORAGE INTERFACE \n
 *
 * History: \n
 * 2022-02-25, fix comments. \n
 * 2018-08-08, Create file. \n
 */

#ifndef NV_H
#define NV_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "key_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_nv_api NV
 * @ingroup middleware_utils
 * @{
 */

/**
 * @if Eng
 * @brief  nv storage callback.
 * @else
 * @brief  nv存储回调函数。
 * @endif
 */
typedef void (*nv_storage_completed_callback)(errcode_t result);

/**
 * @if Eng
 * @brief  nv changed notify callback.
 * @else
 * @brief  nv值更改回调函数。
 * @endif
 */
typedef void (*nv_changed_notify_func)(uint16_t key);

/**
 * @if Eng
 * @brief  nv key Properties.
 * @else
 * @brief  NV属性
 * @endif
 */
typedef struct {
    bool permanent;     /*!< @if Eng Indicates whether the key is a permanent key.
                             @else   是否为永久NV @endif */
    bool encrypted;     /*!< @if Eng Indicates whether to store ciphertext.
                             @else   是否为密文存储 @endif */
    bool non_upgrade;   /*!< @if Eng Indicates whether upgradeable.
                             @else   是否不可升级 @endif */
    uint8_t reserve;    /*!< @if Eng Reserved field for the future
                             @else   保留字段 @endif */
} nv_key_attr_t;

/**
 * @if Eng
 * @brief  nv storage Status.
 * @else
 * @brief  nv存储状态
 * @endif
 */
typedef struct {
    uint32_t total_space;       /*!< @if Eng Total NV space of the current core
                                     @else   当前核的总NV空间 @endif */
    uint32_t used_space;        /*!< @if Eng Used NV space of the current core
                                     @else   当前核已使用的NV空间 @endif */
    uint32_t reclaimable_space; /*!< @if Eng NV recyclable space of the current core,
                                             can be reused after erasing
                                     @else   当前核的NV可回收空间 @endif */
    uint32_t corrupted_space;   /*!< @if Eng NV corrupted space of the current core,
                                             It's a valid data,but the data is abnormal.
                                             It can be reused after erasing
                                     @else   当前核已损坏了的NV空间 @endif */
    uint32_t max_key_space;     /*!< @if Eng Maximum single NV space that can be stored
                                     @else   可存储的最大单NV项空间 @endif */
} nv_store_status_t;

/**
 * @if Eng
 * @brief  region of of key_id value.
 * @else
 * @brief  key_id取值区域
 * @endif
 */
typedef enum {
    KEY_ID_REGION0,        /*!< @if Eng The region 0 of value range of key_id: [0x0001,0x1000).
                                @else   key_id的取值区域0：[0x0001,0x1000)。 @endif */
    KEY_ID_REGION1,        /*!< @if Eng The region 1 of value range of key_id: [0x1000,0x2000).
                                @else   key_id的取值区域1：[0x1000,0x2000)。 @endif */
    KEY_ID_REGION2,        /*!< @if Eng The region 2 of value range of key_id: [0x2000,0x3000).
                                @else   key_id的取值区域2：[0x2000,0x3000)。 @endif */
    KEY_ID_REGION3,        /*!< @if Eng The region 3 of value range of key_id: [0x3000,0x4000).
                                @else   key_id的取值区域3：[0x3000,0x4000)。 @endif */
    KEY_ID_REGION4,        /*!< @if Eng The region 4 of value range of key_id: [0x4000,0x5000).
                                @else   key_id的取值区域4：[0x4000,0x5000)。 @endif */
    KEY_ID_REGION5,        /*!< @if Eng The region 5 of value range of key_id: [0x5000,0x6000).
                                @else   key_id的取值区域5：[0x5000,0x6000)。 @endif */
    KEY_ID_REGION6,        /*!< @if Eng The region 6 of value range of key_id: [0x6000,0x7000).
                                @else   key_id的取值区域6：[0x6000,0x7000)。 @endif */
    KEY_ID_REGION7,        /*!< @if Eng The region 7 of value range of key_id: [0x7000,0x8000).
                                @else   key_id的取值区域7：[0x7000,0x8000)。 @endif */
    KEY_ID_REGION8,        /*!< @if Eng The region 8 of value range of key_id: [0x8000,0x9000).
                                @else   key_id的取值区域8：[0x8000,0x9000)。 @endif */
    KEY_ID_REGION9,        /*!< @if Eng The region 9 of value range of key_id: [0x9000,0xA000).
                                @else   key_id的取值区域9：[0x9000,0xA000)。 @endif */
    KEY_ID_REGION10,       /*!< @if Eng The region 10 of value range of key_id: [0xA000,0xB000).
                                @else   key_id的取值区域10：[0xA000,0xB000)。 @endif */
    KEY_ID_REGION11,       /*!< @if Eng The region 11 of value range of key_id: [0xB000,0xC000).
                                @else   key_id的取值区域11：[0xB000,0xC000)。 @endif */
    KEY_ID_REGION12,       /*!< @if Eng The region 12 of value range of key_id: [0xC000,0xD000).
                                @else   key_id的取值区域12：[0xC000,0xD000)。 @endif */
    KEY_ID_REGION13,       /*!< @if Eng The region 13 of value range of key_id: [0xD000,0xE000).
                                @else   key_id的取值区域13：[0xD000,0xE000)。 @endif */
    KEY_ID_REGION14,       /*!< @if Eng The region 14 of value range of key_id: [0xE000,0xF000).
                                @else   key_id的取值区域14：[0xE000,0xF000)。 @endif */
    KEY_ID_REGION15,       /*!< @if Eng The region 15 of value range of key_id: [0xF000,0xFFFF].
                                @else   key_id的取值区域15：[0xF000,0xFFFF]。 @endif */
    KEY_ID_REGION_MAX_NUM, /*!< @if Eng The Number of the regions.
                                @else   key_id的取值区域数量 @endif */
} nv_key_id_region_t;

/**
 * @if Eng
 * @brief  Mode selection of Restoring factory settings.The value true indicates that the data is to be restored.
 * @else
 * @brief  恢复出厂区域配置，true代表要恢复
 * @endif
 */
typedef struct {
    bool region_mode[KEY_ID_REGION_MAX_NUM];     /*!< @if Eng Recovery region flag configuration
                                                         @else   恢复出厂区域标志配置 @endif */
} nv_restore_mode_t;

/**
 * @if Eng
 * @brief  Backup region flag configuration.The value true indicates that the data is backed up.
 * @else
 * @brief  备份区域配置，true代表要备份
 * @endif
 */
typedef struct {
    bool region_mode[KEY_ID_REGION_MAX_NUM];     /*!< @if Eng Backup region flag configuration
                                                         @else   备份区域标志配置 @endif */
} nv_backup_mode_t;

/**
 * @if Eng
 * @brief Init the nv module, it must be called before using nv function.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief 初始化nv模块，必须在使用nv函数之前调用。
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
void uapi_nv_init(void);

/**
 * @if Eng
 * @brief  Store a key value pair.
 * @par Description: Writes NV data item. The default attribute is Normal and no callback function is available.
 * @param  [in]  key The key ID to which the kvalue is associated.
 * @param  [in]  kvalue The value to store.
 * @param  [in]  kvalue_length Length of kvalue in bytes.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  写入NV数据项
 * @par 说明: 默认属性为Normal，没有回调函数。
 * @param  [in]  key 要写入的NV项的key ID，用于索引
 * @param  [in]  kvalue 指向要写入的NV项的值的指针
 * @param  [in]  kvalue_length 写入数据的长度（以字节为单位）
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length);


/**
 * @if Eng
 * @brief  set a value associated with a specific key.
 * @par Description: Write the NV data associated with a particular key.
 * @attention The encryption attribute and permanent attribute of a key cannot be modified,
 *            and the kvalue of the permanent attribute cannot be modified.
 * @param  [in]  key The key to which the kvalue is associated.
 * @param  [in]  kvalue The value to store.
 * @param  [in]  kvalue_length Length of kvalue in bytes.
 * @param  [in]  attr Set the storage properties of the key.
 * @param  [in]  func Callback function invoked after kvalue is written to the flash memory.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  写入NV数据项，并根据业务需求配置属性及回调函数
 * @par 说明: 写入NV数据项，并根据业务需求配置属性及回调函数
 * @attention NV的加密属性和永久属性不能修改，永久属性的kvalue不能修改
 * @param  [in]  key 要写入的NV项的key ID，用于索引
 * @param  [in]  kvalue 指向要写入的NV项的值的指针
 * @param  [in]  kvalue_length 写入数据的长度（以字节为单位）
 * @param  [in]  attr 要配置的NV项的属性
 * @param  [in]  func kvalue写入flash后调用的回调函数
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr,
                                  nv_storage_completed_callback func);

/**
 * @if Eng
 * @brief  Get a value associated with a specific key.
 * @par Description: Reads NV data.
 * @attention By default, the key attribute value is not obtained.
 * @param  [in]  key The key to which the kvalue is associated.
 * @param  [in]  kvalue_max_length The maximum length, in bytes,
 *                               that is allowed to be copied in the kvalue buffer if the key is found.
 * @param  [out] kvalue_length Length of kvalue in bytes.
 * @param  [out] kvalue The value to store.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  读取指定NV数据项的值
 * @par 说明: 读取指定NV数据项的值
 * @attention 默认情况下，不获取NV属性值
 * @param  [in]  key 要读取的NV项的key ID，用于索引
 * @param  [in]  kvalue_max_length 允许存储数据的最大长度（以字节为单位）
 * @param  [out] kvalue_length 实际读取到的数据长度
 * @param  [out] kvalue 指向保存读取数据的buffer的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue);

/**
 * @if Eng
 * @brief  Get a value associated with a specific key.
 * @par Description: Reads NV data and obtains the attribute value of the key.
 * @param  [in]  key The key to which the kvalue is associated.
 * @param  [in]  kvalue_max_length The maximum length, in bytes,
 *                               that is allowed to be copied in the kvalue buffer if the key is found.
 * @param  [out] kvalue_length Length of kvalue in bytes.
 * @param  [out] kvalue The value to store.
 * @param  [out] attr Attribute value of the key.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  读取指定NV数据项的值，同时获取key的属性值
 * @par 说明: 读取指定NV数据项的值，同时获取key的属性值
 * @param  [in]  key 要读取的NV项的key ID，用于索引。
 * @param  [in]  kvalue_max_length 允许存储数据的最大长度（以字节为单位）
 * @param  [out] kvalue_length 实际读取到的数据长度
 * @param  [out] kvalue 指向保存读取数据的buffer的指针
 * @param  [out] attr 获取到的NV项的属性
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_read_with_attr(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue,
                                 nv_key_attr_t *attr);

/**
 * @if Eng
 * @brief  Get the space usage of NV storage.
 * @par Description: Query the NV space status.
 * @param  [out] status pointer to NV status .
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  获取NV存储的空间使用情况
 * @par 说明: 查询NV空间状态
 * @param  [out] status 指向保存NV状态数据的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_get_store_status(nv_store_status_t *status);

/**
 * @if Eng
 * @brief  NV backup.
 * @par Description: NV backup.
 * @param  [in] backup_mode pointer to NV backup region flag.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  执行NV备份
 * @par 说明: 执行NV备份
 * @param  [in] backup_mode  指向保存NV备份区域选择的指针
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_backup(const nv_backup_mode_t *backup_mode);

/**
 * @if Eng
 * @brief Set NV all restore flag.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief 设置NV全量恢复标志
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_set_restore_mode_all(void);

/**
 * @if Eng
 * @brief  Set NV partitial restore flag.
 * @par Description: Set NV partitial restore flag.
 * @param  [in] restore_mode pointer to NV restore region flag.
 * @retval EXT_ERR_SUCCESS     Success.
 * @retval Other               ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  设置NV部分恢复标志
 * @par 说明: 设置NV部分恢复标志
 * @param  [in] restore_mode   指向保存NV各区域是否恢复标志的指针
 * @retval ERRCODE_SUCC        成功返回#ERRCODE_SUCC
 * @retval Other               失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_set_restore_mode_partitial(const nv_restore_mode_t *restore_mode);


/**
 * @if Eng
 * @brief  Ensure NV data from ram flush to flash.
 * @par Description: Only when nv open the CONFIG NV SUPPORT ASYNCHRONOUS STORE valid.
 * @retval EXT_ERR_SUCCESS     Success.
 * @retval Other               ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  确保NV数据从Ram上同步到Falsh
 * @par 说明: 仅NV支持异步存储时调用有效。
 * @retval ERRCODE_SUCC        成功返回#ERRCODE_SUCC
 * @retval Other               失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_flush(void);

/**
 * @if Eng
 * @brief  register callback function of nv key value changed notification.
 * @param  min_key Minimum key-ID supported by registration callback.
 * @param  max_key Maximum key-ID supported by registration callback.
 * @param  func callback function.
 * @retval EXT_ERR_SUCCESS   Success.
 * @retval Other             ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li nv.h
 * @else
 * @brief  注册nv键值改变通知的回调函数
 * @param  min_key 注册回调支持的最小key-ID
 * @param  max_key 注册回调支持的最大key-ID
 * @param  func 回调函数.
 * @retval ERRCODE_SUCC      成功返回#ERRCODE_SUCC
 * @retval Other             失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li nv.h
 * @endif
 */
errcode_t uapi_nv_register_change_notify_proc(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_H */