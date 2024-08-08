/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.\n
 * Description: UPG Interface Header \n
 *
 * History: \n
 * 2022-09-01, Create file. \n
 */

#ifndef UPG_INTERFACE_H
#define UPG_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "upg_definitions_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_utils_update_api Update
 * @ingroup  middleware_utils
 * @{
 */

/**
 * @if Eng
 * @brief  SHA 256 length.
 * @else
 * @brief  哈希长度。
 * @endif
 */
#define SHA_256_LENGTH  32

/**
 * @if Eng
 * @brief  Indicates the length of the chip ID.
 * @else
 * @brief  芯片ID长度。
 * @endif
 */
#define DIE_ID_LEN      16

/**
 * @if Eng
 * @brief  Key protection length.
 * @else
 * @brief  加密秘钥长度。
 * @endif
 */
#define PROTECT_KEY_LEN 16

/**
 * @if Eng
 * @brief  IV length.
 * @else
 * @brief  IV 长度。
 * @endif
 */
#define IV_LEN          16

/**
 * @if Eng
 * @brief  partition type enumeration.
 * @else
 * @brief  分区枚举类型。
 * @endif
 */
typedef enum upg_result {
    UPG_RESULT_UPDATE_SUCCESS,           /*!< @if Eng The upgrade is successful.
                                              @else   升级成功。 @endif */
    UPG_RESULT_VERIFY_HEAD_FAILED,       /*!< @if Eng Failed to verify the upgrade header.
                                              @else   校验升级包头失败。 @endif */
    UPG_RESULT_VERIFY_HASH_TABLE_FAILED, /*!< @if Eng Failed to verify the hash table.
                                              @else   校验Hash Table失败。 @endif */
    UPG_RESULT_VERIFY_IMAGE_FAILED,      /*!< @if Eng Failed to verify the image.
                                              @else   校验镜像失败。 @endif */
    UPG_RESULT_VERIFY_OLD_IMAGE_FAILED,  /*!< @if Eng Failed to verify the old image.
                                              @else   校验旧镜像失败。 @endif */
    UPG_RESULT_DECOMPRESS_IMAGE_FAILED,  /*!< @if Eng Decompression failed.
                                              @else   解压缩失败。 @endif */
    UPG_RESULT_DECRYPT_IMAGE_FAILED,     /*!< @if Eng Decryption failed.
                                              @else   解密失败。 @endif */
    UPG_RESULT_RECRYPT_IMAGE_FAILED,     /*!< @if Eng Re-encryption failed.
                                              @else   重加密失败。 @endif */
    UPG_RESULT_DIFF_IMAGE_FAILED,        /*!< @if Eng Differential restoration failed.
                                              @else   差分恢复失败。 @endif */
    UPG_RESULT_UPDATE_IMAGE_FAILED,      /*!< @if Eng Failed to update the image to the flash memory.
                                              @else   更新镜像到Flash失败。 @endif */
    UPG_RESULT_PROCESS_NV_FAILED,        /*!< @if Eng Failed to process the NV image.
                                              @else   处理NV镜像失败。 @endif */
    UPG_RESULT_VERIFY_VERSION_FAILED,    /*!< @if Eng Failed to verify the anti-rollback number.
                                              @else   防回滚校验失败。 @endif */
    UPG_RESULT_IMAGE_ID_FAILED,          /*!< @if Eng Failed to verify the Image ID.
                                              @else   镜像ID校验失败。 @endif */
    UPG_RESULT_RETRY_ALL_FAILED,          /*!< @if Eng All failed to retry to update the image.
                                              @else   所有升级尝试均失败。 @endif */
    UPG_RESULT_MAX,                      /*!< @if Eng result_max.
                                              @else   升级结果最大值。 @endif */
} upg_result_t;

/**
 * @if Eng
 * @brief  update status type enumeration.
 * @else
 * @brief  升级状态枚举类型。
 * @endif
 */
typedef enum upg_status {
    UPG_STATUS_SUCC,     /*!< @if Eng The upgrade is successful.
                              @else   升级成功。 @endif */
    UPG_STATUS_FAIL,     /*!< @if Eng The upgrade is failed.
                              @else   升级失败。 @endif */
    UPG_STATUS_UPDATING, /*!< @if Eng The upgrade in progress.
                              @else   正在进行升级。 @endif */
    UPG_STATUS_NONE      /*!< @if Eng Non-upgrade status.
                              @else   非升级状态。 @endif */
} upg_status_t;

/**
 * @if Eng
 * @brief  upg key area data type.
 * @else
 * @brief  升级key区域数据类型。
 * @endif
 */
typedef struct upg_key_area_data {
    uint32_t image_id;            /*!< @if Eng Identity of FOTA Key Area.
                                       @else   FOTA key区域的标识。 @endif */
    uint32_t struct_version;      /*!< @if Eng The Structure Version of FOTA Key Area.
                                       @else   FOTA key区域结构版本。 @endif */
    uint32_t struct_length;       /*!< @if Eng The Structure Length.
                                       @else   结构长度。 @endif */
    uint32_t signature_length;    /*!< @if Eng The signature Length.
                                       @else   签名长度。 @endif */
    uint32_t key_owner_id;        /*!< @if Eng OwnerID of the FOTA external public key.
                                       @else   FOTA二级公钥的所有者ID。 @endif */
    uint32_t key_id;              /*!< @if Eng Key ID of FOTA_External_Public_Key.
                                       @else   FOTA二级公共密钥的密钥ID。 @endif */
    uint32_t key_alg;             /*!< @if Eng Algorithm of the external public key.
                                       @else   二级公钥算法。 @endif */
    uint32_t ecc_curve_type;      /*!< @if Eng ecc curve type.
                                       @else   ecc 曲线类型。 @endif */
    uint32_t key_length;          /*!< @if Eng The length of FOTA_External_Public_Key.
                                       @else   FOTA二级公共密钥的长度。 @endif */
    uint32_t fota_key_version_ext;/*!< @if Eng Version of FOTA_External_Public_Key.
                                       @else   FOTA二级公共密钥的版本。 @endif */
    uint32_t mask_fota_key_version_ext;/*!< @if Eng Mask of FOTA_Key_Version_Ext.
                                            @else   FOTA密钥版本扩展的掩码。 @endif */
    uint32_t msid_ext;            /*!< @if Eng Market segment ID.
                                       @else   细分市场ID。 @endif */
    uint32_t mask_msid_ext;       /*!< @if Eng Mask of MSID.
                                       @else   MSID掩码。 @endif */
    uint32_t maintenance_mode;    /*!< @if Eng Maintenance Mode.
                                       @else   维护模式。 @endif */
    uint8_t die_id[DIE_ID_LEN];   /*!< @if Eng Chipset DIE ID, valid when maintenance mode is enabled.
                                       @else   芯片组芯片ID，在启用维护模式时有效。 @endif */
    uint32_t fota_info_addr;      /*!< @if Eng Address of FOTA Info.
                                       @else   FOTA info区域的偏移地址。 @endif */
    uint8_t reserved[KEY_AREA_RESERVED_LEN];         /*!< @if Eng reserve bytes for aligned.
                                                          @else   为字节对齐而保留的字段。 @endif */
    uint8_t fota_external_public_key[PUBLIC_KEY_LEN];/*!< @if Eng FOTA_External_Public_Key.
                                                          @else   FOTA二级公钥。 @endif */
    uint8_t sig_fota_key_area[SIG_LEN];/*!< @if Eng Signature of the FOTA Key Area.
                                            @else   FOTA key区域签名。 @endif */
} upg_key_area_data_t;

/**
 * @if Eng
 * @brief  upg fota info data type.
 * @else
 * @brief  升级fota信息数据类型。
 * @endif
 */
typedef struct upg_fota_info_data {
    uint32_t image_id;             /*!< @if Eng Identity of FOTA Info Area.
                                        @else   FOTA信息区标识。 @endif */
    uint32_t struct_version;       /*!< @if Eng The Structure Version of FOTA info Area.
                                        @else   FOTA key区域结构版本。 @endif */
    uint32_t struct_length;        /*!< @if Eng The Structure Length.
                                        @else   结构长度。 @endif */
    uint32_t signature_length;     /*!< @if Eng The signature Length.
                                        @else   签名长度。 @endif */
    uint32_t fota_version_ext;     /*!< @if Eng Version of FOTA Info Area.
                                        @else   FOTA信息区的版本。 @endif */
    uint32_t mask_fota_version_ext;/*!< @if Eng Version of FOTA_External_Public_Key.
                                        @else   FOTA二级公共密钥的版本。 @endif */
    uint32_t msid_ext;             /*!< @if Eng Market segment ID.
                                        @else   细分市场ID。 @endif */
    uint32_t mask_msid_ext;        /*!< @if Eng Mask of MSID.
                                        @else   MSID掩码。 @endif */
    uint32_t image_hash_table_addr;/*!< @if Eng Address of Image_Hash_Table In FOTA Package.
                                        @else   FOTA包中镜像哈希表的地址。 @endif */
    uint32_t image_hash_table_length;/*!< @if Eng Length of Image_Hash_Table.
                                          @else   镜像哈希表的长度。 @endif */
    uint8_t image_hash_table_hash[SHA_256_LENGTH];/*!< @if Eng Hash of Image_Hash_Table.
                                                       @else   镜像哈希表的哈希。 @endif */
    uint32_t image_num;            /*!< @if Eng The total numbers of FOTA images.
                                        @else   FOTA镜像的总数。 @endif */
    uint32_t hardware_id;          /*!< @if Eng Hardware ID.
                                        @else   硬件ID。 @endif */
    uint8_t user_defined[INFO_AREA_USER_LEN];      /*!< @if Eng Reserve 48 bytes for user customization.
                                                        @else   预留字节供用户自定义使用。 @endif */
    uint8_t sign_fota_info[SIG_LEN];/*!< @if Eng Signature of FOTA Info.
                                         @else   FOTA信息签名。 @endif */
} upg_fota_info_data_t;

/**
 * @if Eng
 * @brief  upg image hash node type.
 * @else
 * @brief  升级镜像哈希节点类型。
 * @endif
 */
typedef struct upg_image_hash_node {
    uint32_t image_id;                  /*!< @if Eng Identity of FOTA Key Area.
                                             @else   FOTA key区域的标识。 @endif */
    uint32_t image_addr;                /*!< @if Eng Offset of Image Header.
                                             @else   镜像头偏移量。 @endif */
    uint32_t image_length;              /*!< @if Eng Length of Image.
                                             @else   镜像长度。 @endif */
    uint8_t image_hash[SHA_256_LENGTH]; /*!< @if Eng Hash Of Image Header.
                                             @else   镜像头的哈希。 @endif */
} upg_image_hash_node_t;

/**
 * @if Eng
 * @brief  upg package header type.
 * @else
 * @brief  升级包头类型
 * @endif
 */
typedef struct upg_package_header {
    upg_key_area_data_t  key_area;/*!< @if Eng upg key area data.
                                       @else   升级密钥区域数据。 @endif */
    upg_fota_info_data_t info_area;/*!< @if Eng fota info data.
                                        @else   FOTA info区数据。 @endif */
} upg_package_header_t;

/**
 * @if Eng
 * @brief  upg image header type.
 * @else
 * @brief  升级包镜像头类型
 * @endif
 */
typedef struct upg_image_header {
    uint32_t header_magic; /*!< @if Eng header magic.
                                @else   镜像头魔术字。 @endif */
    uint32_t image_id;     /*!< @if Eng Identity of FOTA Key Area.
                                @else   FOTA key区域的标识。 @endif */
    uint32_t image_offset; /*!< @if Eng Offset of update Image data.
                                @else   要更新的镜像数据的偏移地址。 @endif */
    uint32_t image_len;    /*!< @if Eng Length of update image data, actual data length, NOT including padding.
                                @else   更新的镜像数据的长度（实际数据长度，不包括填充字段）。 @endif */
    uint8_t image_hash[SHA_256_LENGTH]; /*!< @if Eng Hash of update image data.
                                             @else   更新镜像数据的哈希。 @endif */
    uint32_t old_image_len; /*!< @if Eng Length of old image.
                                 @else   旧镜像长度。 @endif */
    uint8_t old_image_hash[SHA_256_LENGTH]; /*!< @if Eng Hash value of old image.
                                                 @else   旧镜像的哈希值。 @endif */
    uint32_t new_image_len;             /*!< @if Eng length of new image.
                                             @else   新镜像长度。 @endif */
    uint32_t version_ext;               /*!< @if Eng Version of the new image.
                                             @else   新镜像版本。 @endif */
    uint32_t version_mask;              /*!< @if Eng The version mask.
                                             @else   版本掩码。 @endif */
    uint32_t decompress_flag;           /*!< @if Eng Decompress flag.
                                             @else   解压标志。 @endif */
    uint32_t re_enc_flag;               /*!< @if Eng Re-Encryption flag.
                                             @else   重新加密标志。 @endif */
    uint32_t root_key_type;             /*!< @if Eng Root key which is used to encrypt the image.
                                             @else   用于加密镜像的密钥。 @endif */
    uint8_t enc_pk_l1[PROTECT_KEY_LEN]; /*!< @if Eng The level 1 encrypted protection key
                                           for decrypting update image.
                                             @else   用于解密更新镜像的一级加密保护密钥。 @endif */
    uint8_t enc_pk_l2[PROTECT_KEY_LEN]; /*!< @if Eng level 2 encrypted protection key
                                         for decrypting update image.
                                             @else   用于解密更新镜像的二级加密保护密钥。 @endif */
    uint8_t iv[IV_LEN];       /*!< @if Eng The IV for decrypt update image.
                                   @else   用于解密升级镜像的IV。 @endif */
    uint8_t padding[4];       /*!< @if Eng reserve bytes for 16-byte aligned.
                                   @else   保留字段，以保证整个结构16字节对齐。 @endif */
} upg_image_header_t;

/**
 * @if Eng
 * @brief  The callback function after writting type.
 * @else
 * @brief  写类型后的回调函数。
 * @endif
 */
typedef void (*uapi_upg_write_done_cb)(errcode_t result);

/**
 * @if Eng
 * @brief  The progress notification callback function type.
 * @else
 * @brief  进度通知回调函数类型。
 * @endif
 */
typedef void (*uapi_upg_progress_cb)(uint32_t percent);

/**
 * @if Eng
 * @brief  The function for verifying user-defined field type.
 * @else
 * @brief  自定义字段类型校验函数。
 * @endif
 */
typedef errcode_t (*uapi_upg_user_defined_check)(uint8_t *user_info, uint32_t info_len, uintptr_t param);

/**
 * @if Eng
 * @brief  upg malloc function.
 * @else
 * @brief  升级使用内存分配函数。
 * @endif
 */
typedef void *(*upg_func_malloc)(const uint32_t size);

/**
 * @if Eng
 * @brief  upg free mandatory function.
 * @else
 * @brief  升级释放内存函数。
 * @endif
 */
typedef void (*upg_func_free)(void *ptr);

/**
 * @if Eng
 * @brief  upg serial putc mandatory function.
 * @else
 * @brief  upg串口输出函数。
 * @endif
 */
typedef void (*upg_func_serial_putc)(const char c);

/**
 * @if Eng
 * @brief  struct of optional function.
 * @else
 * @brief  可选函数结构。
 * @endif
 */
typedef struct upg_func {
    upg_func_malloc malloc;            /*!< @if Eng upg malloc function.(Mandatory Function)
                                            @else   升级使用内存分配函数（必选函数）。 @endif */
    upg_func_free free;                /*!< @if Eng upg free mandatory function.(Mandatory Function)
                                            @else   升级释放内存函数 （必选函数）。 @endif */
    upg_func_serial_putc serial_putc;  /*!< @if Eng upg serial putc mandatory function.（Optional Functions）
                                            @else   upg串口输出函数。 @endif */
} upg_func_t;

/**
 * @if Eng
 * @brief  struct of upg prepare info.
 * @else
 * @brief  升级准备信息结构。
 * @endif
 */
typedef struct upg_prepare_info {
    uint32_t package_len;   /*!< @if Eng package length.
                                 @else   升级包的大小。 @endif */
} upg_prepare_info_t;

/**
 * @if Eng
 * @brief  Initializing the update module.
 * @par Description: Initializing the update module.
 * @param  [in]  func_list Register functions list for update code usage. see @ref upg_func_t
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  初始化升级模块。
 * @par 说明: 初始化升级模块。
 * @param  [in]  func_list 升级模块使用的注册函数列表。 参考 @ref upg_func_t
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_init(const upg_func_t *func_list);

/**
 * @if Eng
 * @brief  Start local update.
 * @par Description: Start local update.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  开始本地升级。
 * @par 说明: 开始本地升级。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_start(void);

/**
 * @if Eng
 * @brief  Register the progress notification callback function.
 * @par Description: Register the progress notification callback function.
 * @param  [in]  func The progress notification callback function. see @ref uapi_upg_progress_cb
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  注册升级进度通知回调函数。
 * @par 说明: 注册升级进度通知回调函数。
 * @param  [in]  func 进度通知的回调函数。 参考 @ref uapi_upg_progress_cb
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_register_progress_callback(uapi_upg_progress_cb func);

/**
 * @if Eng
 * @brief  Obtain the update result.
 * @par Description: Obtain the update result.
 * @param  [out]  result Address for saving the update result. see @ref upg_result_t
 * @param  [out]  last_image_index Address for saving the last image index.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  获取升级结果。
 * @par 说明: 获取升级结果。
 * @param  [out]  result 保存获取的升级结果。 参考 @ref upg_result_t
 * @param  [out]  last_image_index 保存获取的最后一个处理的升级镜像的序号。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_get_result(upg_result_t *result, uint32_t *last_image_index);

/**
 * @if Eng
 * @brief  Obtain the update status.
 * @par Description: Obtain the update status.
 * @retval see @ref upg_status_t.
 * @else
 * @brief  获取升级状态。
 * @par 说明: 获取升级状态。
 * @retval 参考 @ref upg_status_t
 * @endif
 */
upg_status_t uapi_upg_get_status(void);

/**
 * @if Eng
 * @brief  Preparing the local storage for storing the upgrade package.
 * @par Description: This function is blocked and waits for the return value after execution.
 * @param  [in]  prepare_info The pointer of prepare info. see @ref upg_prepare_info_t
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  用来保存升级包的本地存储器的准备工作。
 * @par 说明: 该函数阻塞等待执行完返回。
 * @param  [in]  prepare_info 准备信息的指针。 参考 @ref upg_prepare_info_t
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_prepare(upg_prepare_info_t *prepare_info);

/**
 * @if Eng
 * @brief  Reset upgrade flag
 * @par Description: This function is blocked and waits for the return value after execution.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  重置升级标记
 * @par 说明: 该函数阻塞等待执行完返回。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_reset_upgrade_flag(void);

/**
 * @if Eng
 * @brief  Write the update package data to the local storage.
 * @par Description: This function is asynchronous and returns immediately.
 *                   After the actual operation is complete,
 *                   the callback function is invoked.
 * @attention The next packet can be written only after the callback function
 *            is called and the write success is returned.
 *            Before that, this interface cannot be called again to write the next packet.
 * @param  [in]  offset Offset from the start of the package.
 * @param  [in]  buff Buffer for storing package section data.
 *                    The callback function for completing preparations.
 * @param  [in]  len The length of package section data.
 * @param  [in]  callback The callback function after writting. see @ref uapi_upg_write_done_cb
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  将升级包数据写入本地存储器。
 * @par 说明: 该函数异步执行立即返回，实际操作完成后调用回调函数。
 * @attention 当前接口的回调函数被调用且返回写成功才能继续写下一个数据包，在此之前禁止再次调用该接口写下一个数据包。
 * @param  [in]  offset 相对升级包开头的偏移。
 * @param  [in]  buff 存放升级包数据的buffer。
 * @param  [in]  len 升级包数据buffer的长度。
 * @param  [in]  callback 写入完成的回调函数。 参考 @ref uapi_upg_write_done_cb
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_write_package_async(uint32_t offset, const uint8_t *buff, uint16_t len,
                                       uapi_upg_write_done_cb callback);

/**
 * @if Eng
 * @brief  Write the update package data to the local storage.
 * @par Description: This function is synchronous and returns immediately.
 * @param  [in]  offset Offset from the start of the package.
 * @param  [in]  buff Buffer for storing package section data.
 *                    The callback function for completing preparations.
 * @param  [in]  len The length of package section data.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  将升级包数据写入本地存储器。
 * @par 说明: 该函数同步执行立即返回。
 * @param  [in]  offset 相对升级包开头的偏移。
 * @param  [in]  buff 存放升级包数据的buffer。
 * @param  [in]  len 升级包数据buffer的长度。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_write_package_sync(uint32_t offset, const uint8_t *buff, uint16_t len);

/**
 * @if Eng
 * @brief  Read the package data from the local storage.
 * @par Description: Read the package data from the local storage.
 * @param  [in]  offset Offset from the start of the package.
 * @param  [in]  buff Buffer for storing package section data.
 *                    The callback function for completing preparations.
 * @param  [in]  len The length of package section data.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  从本地存储器读取升级包数据。
 * @par 说明: 从本地存储器读取升级包数据。
 * @param  [in]  offset 相对升级包开头的偏移。
 * @param  [in]  buff 存放升级包数据的buffer。
 * @param  [in]  len 升级包数据的长度buffer。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_read_package(uint32_t offset, uint8_t *buff, uint32_t len);

/**
 * @if Eng
 * @brief  Obtain the size for the package.
 * @par Description: Obtain the size for the package.
 * @retval 0            Obtain the size failed.
 * @retval Others       The size obtained.
 * @else
 * @brief  获取可存放升级包的空间大小。
 * @par 说明: 获取可存放升级包的空间大小。
 * @retval 0            失败返回0。
 * @retval Others       其他值：成功返回空间大小。
 * @endif
 */
uint32_t uapi_upg_get_storage_size(void);

/**
 * @if Eng
 * @brief  Request local update.
 * @par Description: Request local update.
 * @param  [in]  reset Whether to restart the system after the function process is complete.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  申请开始进行本地升级。
 * @par 说明: 申请开始进行本地升级。
 * @param  [in]  reset 申请流程结束后是否重启系统。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_request_upgrade(bool reset);

/**
 * @if Eng
 * @brief  verify the Header of the update package.
 * @par Description: verify the Header of the update package.
 * @param  [in]  pkg_header Pointer to the package header. see @ref upg_package_header_t
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  校验升级包头结构。
 * @par 说明: 校验升级包头结构。
 * @param  [in]  pkg_header 指向升级包头结构的指针。 参考 @ref upg_package_header_t
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_verify_file_head(const upg_package_header_t *pkg_header);

/**
 * @if Eng
 * @brief  verify the image in the upgrade package.
 * @par Description: verify_old applies to differential update, used to determine whether the old image
                     in use is the same as the old image used for differential mirroring.
 * @param  [in]  img_header Pointer to the image header. see @ref upg_image_header_t
 * @param  [in]  hash The hash of image.
 *                    The callback function for completing preparations.
 * @param  [in]  hash_len The length of hash(unit: byte).
 * @param  [in]  verify_old Whether to verify the old image.
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  校验升级包中的升级镜像。
 * @par 说明: verify_old一般适用于差分升级场景下，用于判断当前使用的旧镜像与做差分镜像时的旧镜像是否一致。
 * @param  [in]  img_header 指向升级包中升级镜像头结构的指针。 参考 @ref upg_image_header_t
 * @param  [in]  hash 升级镜像的HASH值。
 * @param  [in]  hash_len HASH的长度(单位：字节)。
 * @param  [in]  verify_old 是否校验旧镜像。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_verify_file_image(const upg_image_header_t *img_header,
                                     const uint8_t *hash, uint32_t hash_len, bool verify_old);

/**
 * @if Eng
 * @brief  verify the whole update package.
 * @par Description: verify the whole update package.
 * @param  [in]  pkg_header Pointer to the package header. see @ref upg_package_header_t
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  校验整个升级包。
 * @par 说明: 校验整个升级包。
 * @param  [in]  pkg_header 指向升级包头结构的指针。 参考 @ref upg_package_header_t
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
errcode_t uapi_upg_verify_file(const upg_package_header_t *pkg_header);

/**
 * @if Eng
 * @brief  Register a user-defined field verification function.
 * @par Description: After the user-defined verification function is registered.
 *                   When the uapi_upg_verify_file_head and uapi_upg_verify_file functions are invoked,
 *                   the verification function is invoked.
 * @param  [in]  func The function for verifying user-defined field. see @ref uapi_upg_user_defined_check
 * @param  [in]  param Registration parameters .
 * @retval ERRCODE_SUCC            Success.
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  注册用户自定义字段的校验函数。
 * @par 说明: 注册后，调用uapi_upg_verify_file_head和uapi_upg_verify_file函数时，校验函数会被调用到。
 * @param  [in]  func 用于校验用户自定义字段的校验函数。 参考 @ref uapi_upg_user_defined_check
 * @param  [in]  param 注册参数。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC。
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值。
 * @endif
 */
void uapi_upg_register_user_defined_verify_func(uapi_upg_user_defined_check func, uintptr_t param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_INTERFACE_H */