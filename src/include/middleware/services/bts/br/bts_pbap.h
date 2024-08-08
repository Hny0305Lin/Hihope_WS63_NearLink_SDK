/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT PBAP module.
 */

/**
 * @defgroup bluetooth_bts_pbap_pce PBAP PCE API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_PBAP_H
#define BTS_PBAP_H

#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Length of PBAP mask.
 * @else
 * @brief PBAP掩码长度。
 * @endif
 */
#define PBAP_MASK_LEN 8

/**
 * @if Eng
 * @brief Enum of PBAP vCard format.
 * @else
 * @brief PBAP vCard格式取值。
 * @endif
 */
typedef enum {
    PBAP_FORMAT_VCARD21 = 0x00, /*!< @if Eng PBAP vCard Format V2.1 @else PBAP vCard格式v2.1 @endif */
    PBAP_FORMAT_VCARD30,        /*!< @if Eng PBAP vCard Format V3.0 @else PBAP vCard格式v3.0 @endif */
} pbap_format_vcard_t;

/**
 * @if Eng
 * @brief Enum of PBAP storage type.
 * @else
 * @brief PBAP存储位置。
 * @endif
 */
typedef enum {
    PBAP_REPOSITORY_LOCAL = 0x01, /*!< @if Eng PBAP Phone book repository telecom
                                       @else PBAP电话簿存储位置在手机 @endif */
    PBAP_REPOSITORY_SIM, /*!< @if Eng PBAP Phone book repository SIM1/telecom @else PBAP电话簿存储位置在SIM卡 @endif */
} pbap_repositories_t;

/**
 * @if Eng
 * @brief Enum of PBAP order type.
 * @else
 * @brief PBAP排序类型。
 * @endif
 */
typedef enum {
    PBAP_ORDER_INDEXED = 0x00, /*!< @if Eng PBAP order Indexed @else PBAP排序顺序为索引序 @endif */
    PBAP_ORDER_ALPHANUMERIC,   /*!< @if Eng PBAP order Alphabetical @else PBAP排序顺序为字典序 @endif */
    PBAP_ORDER_PHONETIC,       /*!< @if Eng PBAP order Phonetical @else PBAP排序顺序为语音序 @endif */
} pbap_order_type_t;

/**
 * @if Eng
 * @brief Enum of search properties.
 * @else
 * @brief PBAP搜索属性。
 * @endif
 */
typedef enum {
    PBAP_SEARCH_PROPERTY_NAME = 0x00, /*!< @if Eng PBAP Search Property Name @else PBAP搜索属性为名字 @endif */
    PBAP_SEARCH_PROPERTY_NUMBER,      /*!< @if Eng PBAP Search Property Number @else PBAP搜索属性为号码 @endif */
    PBAP_SEARCH_PROPERTY_SOUND,       /*!< @if Eng PBAP Search Property Sound @else PBAP搜索属性为读音 @endif */
} pbap_search_property_t;

/**
 * @if Eng
 * @brief Enum of property mask type.
 * @else
 * @brief PBAP属性掩码取值。
 * @endif
 */
typedef enum {
    PBAP_PROPERTY_MASK_VERSION = 0x00, /*!< @if Eng PBAP Property VERSION @else PBAP属性 vCard版本 @endif */
    PBAP_PROPERTY_MASK_FN,             /*!< @if Eng PBAP Property FN @else PBAP属性 格式化名称 @endif */
    PBAP_PROPERTY_MASK_N,              /*!< @if Eng PBAP Property N @else PBAP属性 名称的结构化表示 @endif */
    PBAP_PROPERTY_MASK_PHOTO,          /*!< @if Eng PBAP Property PHOTO @else PBAP属性 相关图像或照片 @endif */
    PBAP_PROPERTY_MASK_BDAY,           /*!< @if Eng PBAP Property BDAY @else PBAP属性 生日 @endif */
    PBAP_PROPERTY_MASK_ADR,            /*!< @if Eng PBAP Property ADR @else PBAP属性 收货地址 @endif */
    PBAP_PROPERTY_MASK_LABEL,          /*!< @if Eng PBAP Property LABEL @else PBAP属性 标签 @endif */
    PBAP_PROPERTY_MASK_TEL,            /*!< @if Eng PBAP Property TEL @else PBAP属性 电话号码 @endif */
    PBAP_PROPERTY_MASK_EMAIL,          /*!< @if Eng PBAP Property EMAIL @else PBAP属性 电子邮件地址 @endif */
    PBAP_PROPERTY_MASK_MAILER,         /*!< @if Eng PBAP Property MAILER @else PBAP属性 电子邮件 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_TZ,    /*!< @if Eng PBAP Property TZ @else PBAP属性 时区 @endif */
    PBAP_PROPERTY_MASK_GEO,            /*!< @if Eng PBAP Property GEO @else PBAP属性 地理位置 @endif */
    PBAP_PROPERTY_MASK_TITLE,          /*!< @if Eng PBAP Property TITLE @else PBAP属性 工作 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_ROLE,  /*!< @if Eng PBAP Property ROLE @else PBAP属性 组织内的角色 @endif */
    PBAP_PROPERTY_MASK_LOGO,           /*!< @if Eng PBAP Property LOGO @else PBAP属性 组织徽标 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_AGENT, /*!< @if Eng PBAP Property AGENT @else PBAP属性 代表人员的vCard @endif */
    PBAP_PROPERTY_MASK_ORG,            /*!< @if Eng PBAP Property ORG @else PBAP属性 组织名称 @endif */
    PBAP_PROPERTY_MASK_NOTE,           /*!< @if Eng PBAP Property NOTE @else PBAP属性 注释 @endif */
    PBAP_PROPERTY_MASK_REV,            /*!< @if Eng PBAP Property REV @else PBAP属性 修订版本 @endif */
    PBAP_PROPERTY_MASK_SOUND,          /*!< @if Eng PBAP Property SOUND @else PBAP属性 姓名读音 @endif */
    PBAP_PROPERTY_MASK_URL,            /*!< @if Eng PBAP Property URL @else PBAP属性 统一资源定位器 @endif */
    PBAP_PROPERTY_MASK_UID,            /*!< @if Eng PBAP Property UID @else PBAP属性 唯一标识 @endif */
    PBAP_PROPERTY_MASK_KEY,            /*!< @if Eng PBAP Property KEY @else PBAP属性 公共加密密钥 @endif */
    PBAP_PROPERTY_MASK_NICKNAME,       /*!< @if Eng PBAP Property NICKNAME @else PBAP属性 昵称 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_RTY_MASK_CATEGORIES,  /*!< @if Eng PBAP Property CATEGORIES @else PBAP属性 类别 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_PROID,                /*!< @if Eng PBAP Property PROID @else PBAP属性 产品ID @endif */
    PBAP_PROPERTY_MASK_CLASS,                         /*!< @if Eng PBAP Property CLASS @else PBAP属性 CLASS信息 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_RTY_MASK_SORT_STRING, /*!< @if Eng PBAP Property SORT-STRING @else PBAP属性
                                                         用于排序操作的字符串 @endif */
    PBAP_PROPERTY_MASK_X_IRMC_CALL_DATETIME, /*!< @if Eng PBAP Property X-IRMC-CALL-DATETIME
                                                  @else PBAP属性 时间戳 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_X_BT_SPEEDDIALKEY, /*!< @if Eng PBAP Property X-BT-SPEEDDIALKEY @else PBAP属性
                                                      快速拨号快捷方式 @endif */
    PBAP_PROPERTY_MASK_RTY_MASK_X_BT_UCI, /*!< @if Eng PBAP Property X-BT-UCI @else PBAP属性 统一主叫方标识符 @endif */
    PBAP_PROPERTY_MASK_X_BT_UID, /*!< @if Eng PBAP Property X-BT-UID @else PBAP属性 蓝牙联系人唯一标识符 @endif */
    PBAP_PROPERTY_MASK_FILTER = 0x27, /*!< @if Eng PBAP Property Proprietary Filter @else PBAP属性 私有过滤器 @endif */
    PBAP_PROPERTY_MASK_INVALID = 0xFF,
} pbap_property_mask_t;

/**
 * @if Eng
 * @brief Struct of phonebook parameters.
 * @else
 * @brief 电话簿参数。
 * @endif
 */
typedef struct {
    unsigned char property_selector[PBAP_MASK_LEN]; /*!< @if Eng property selector @else  属性选择器 @endif */
    unsigned char vcard_selector[PBAP_MASK_LEN];    /*!< @if Eng vCard selector @else  vCard选择器 @endif */
    unsigned short max_list_count;                  /*!< @if Eng max list count @else  最大列表计数 @endif */
    unsigned short list_offset;                     /*!< @if Eng list start Offset @else  列表起始偏移量 @endif */
    unsigned char *name;                            /*!< @if Eng name @else  名称 @endif */
    unsigned char format;                           /*!< @if Eng format @else  格式 @endif */
    unsigned char reset_new_missed_calls;  /*!< @if Eng reset new missed calls @else  重置新的未接呼叫 @endif */
    unsigned char vcard_selector_operator; /*!< @if Eng vCard selector operator @else  vCard选择器运算符 @endif */
} pbap_pce_pull_phone_book_param_t;

/**
 * @if Eng
 * @brief Struct of vCard list parameters.
 * @else
 * @brief vCard列表对象参数。
 * @endif
 */
typedef struct {
    unsigned char vcard_selector[PBAP_MASK_LEN]; /*!< @if Eng vCard selector @else  vCard选择器 @endif */
    unsigned short max_list_count;               /*!< @if Eng max list count @else  最大列表计数 @endif */
    unsigned short list_offset;                  /*!< @if Eng list start Offset @else  列表起始偏移量 @endif */
    unsigned char *name;                         /*!< @if Eng name @else  名称 @endif */
    unsigned char *search_value;                 /*!< @if Eng search value @else  搜索值 @endif */
    unsigned char search_property;               /*!< @if Eng search property @else  搜索属性 @endif */
    unsigned char order;                         /*!< @if Eng order @else  顺序 @endif */
    unsigned char reset_new_missed_calls;  /*!< @if Eng reset new missed calls @else  重置新的未接呼叫 @endif */
    unsigned char vcard_selector_operator; /*!< @if Eng vCard selector operator @else  vCard选择器运算符 @endif */
} pbap_pce_pull_vcard_listing_param_t;

/**
 * @if Eng
 * @brief Struct of vCard parameters.
 * @else
 * @brief vCard参数。
 * @endif
 */
typedef struct {
    unsigned char property_selector[PBAP_MASK_LEN]; /*!< @if Eng property selector @else  属性选择器 @endif */
    unsigned char *name;                            /*!< @if Eng name @else  名称 @endif */
    unsigned char format;                           /*!< @if Eng format @else  格式 @endif */
} pbap_pce_pull_vcard_entry_param_t;

/**
 * @if Eng
 * @brief Offset of PBAP error code.
 * @else
 * @brief PBAP错误码偏移。
 * @endif
 */
#define PBAP_ERROR_OFFSET 0x100

/**
 * @if Eng
 * @brief Enum of pbap error codes.
 * @else
 * @brief PBAP错误码。
 * @endif
 */
typedef enum {
    PBAP_ERROR_OK = PBAP_ERROR_OFFSET, /*!< @if Eng PBAP error code OK @else  PBAP错误码 正常 @endif */
    PBAP_ERROR_BAD_REQUEST,            /*!< @if Eng PBAP error code Bad Request @else  PBAP错误码 错误请求 @endif */
    PBAP_ERROR_NOT_IMPLEMENTED,        /*!< @if Eng PBAP error code Not implemented @else  PBAP错误码 未执行 @endif */
    PBAP_ERROR_UNAUTHORIZED,           /*!< @if Eng PBAP error code Unauthorized @else  PBAP错误码 未授权 @endif */
    PBAP_ERROR_PRECONDITION_FAILED, /*!< @if Eng PBAP error code Precondition Failed
                                         @else  PBAP错误码 预置条件失败 @endif */
    PBAP_ERROR_NOT_FOUND,           /*!< @if Eng PBAP error code Not Found @else  PBAP错误码 未找到 @endif */
    PBAP_ERROR_NOT_ACCEPTABLE, /*!< @if Eng PBAP error code Not Acceptable @else  PBAP错误码 不可接受 @endif */
    PBAP_ERROR_SERVICE_UNAVAILABLE, /*!< @if Eng PBAP error code Service unavailable
                                         @else  PBAP错误码 服务不可用 @endif */
    PBAP_ERROR_FORBIDDEN,           /*!< @if Eng PBAP error code Forbidden @else  PBAP错误码 禁止 @endif */
} pbap_error_codes_t;

/**
 * @if Eng
 * @brief Callback function for connection state change.
 * @par Description: When registered, this callback reports connection state change to upper layer application.
 * @param  [in]  bd_addr  The address of the remote device. See @ref bd_addr_t
 * @param  [in]  state    Connection state. See {@link profile_connect_state_t}
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @else
 * @brief 连接状态变化回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将连接状态变化传递给上层应用。
 * @param  [in]  bd_addr  远端设备地址。见 @ref bd_addr_t
 * @param  [in]  state    连接状态。见 {@link profile_connect_state_t}
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @endif
 */
typedef void (*pbap_pce_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);

/**
 * @if Eng
 * @brief Callback function for phonebook size.
 * @par Description: When registered, this callback reports phonebook size to upper layer application.
 * @param  [in]  bd_addr  The address of the remote device. See @ref bd_addr_t
 * @param  [in]  size     Phonebook size.
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @else
 * @brief 电话簿大小回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将电话簿大小传递给上层应用。
 * @param  [in]  bd_addr  远端设备地址。见 @ref bd_addr_t
 * @param  [in]  size     电话簿大小。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @endif
 */
typedef void (*pbap_pce_phone_book_size_callback)(const bd_addr_t *bd_addr, unsigned short size);

/**
 * @if Eng
 * @brief Callback function for transfer completion.
 * @par Description: When registered, this callback reports transfer completion to upper layer application.
 * @param  [in]  bd_addr  The address of the remote device. See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @else
 * @brief 传输完成回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将传输完成事件上报给应用。
 * @param  [in]  bd_addr  远端设备地址。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @endif
 */
typedef void (*pbap_pce_trans_complete_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Callback function for data reception.
 * @par Description: When registered, this callback reports received data to upper layer application.
 * @param  [in]  bd_addr  The address of the remote device. See @ref bd_addr_t
 * @param  [in]  data     Received data.
 * @param  [in]  len      Length of data.
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @else
 * @brief 接收到数据回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将接收到的数据上报给应用。
 * @param  [in]  bd_addr  远端设备地址。见 @ref bd_addr_t
 * @param  [in]  data     接收到的数据。
 * @param  [in]  len      数据长度。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @endif
 */
typedef void (*pbap_pce_receive_data_callback)(const bd_addr_t *bd_addr, const unsigned char *data, int len);

/**
 * @if Eng
 * @brief Callback function for error code.
 * @par Description: When registered, this callback reports error code to upper layer application.
 * @param  [in]  bd_addr  The address of the remote device. See @ref bd_addr_t
 * @param  [in]  code     Error code.
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @else
 * @brief 上报错误码回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将错误码上报给应用。
 * @param  [in]  bd_addr  远端设备地址。见 @ref bd_addr_t
 * @param  [in]  code     要上报的错误码。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @endif
 */
typedef void (*pbap_pce_error_code_callback)(const bd_addr_t *bd_addr, int code);

/**
 * @if Eng
 * @brief Struct of PBAP callback functions.
 * @else
 * @brief PBAP回调函数接口定义。
 * @endif
 */
typedef struct {
    pbap_pce_conn_state_changed_callback state_changed_cb;
    pbap_pce_phone_book_size_callback phone_book_sized_cb;
    pbap_pce_trans_complete_callback trans_complete_cb;
    pbap_pce_receive_data_callback receive_data_cb;
    pbap_pce_error_code_callback error_code_cb;
} pbap_pce_callbacks_t;

/**
 * @if Eng
 * @brief Use this funtion to connect to PSE.
 * @par Description: Use this funtion to connect to PSE.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 建立与PSE的连接。
 * @par 说明: 建立与PSE的连接。
 * @param  [in]  bd_addr  待连接的设备地址。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to disconnect from PSE.
 * @par Description: Use this funtion to disconnect from PSE.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 建立与PSE的连接。
 * @par 说明: 建立与PSE的连接。
 * @param  [in]  bd_addr  待断开连接的设备地址。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to get connection state of PBAP client.
 * @par Description: Use this funtion to get connection state of PBAP client.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @retval Return connecting/connected/disconnecting/disconnected. See @ref profile_connect_state_t.
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 查询PBAP client连接状态。
 * @par 说明: 查询PBAP client连接状态。
 * @param  [in]  bd_addr  待查询的设备地址。见 @ref bd_addr_t
 * @retval 返回值为正在连接，已连接，正在断开连接，已断开连接。参考 @ref profile_connect_state_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_get_device_connect_state(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to pull phonebook.
 * @par Description: Use this funtion to pull phonebook.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @param  [in]  param    Phonebook parameters. See @ref pbap_pce_pull_phone_book_param_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 下载电话簿对象。
 * @par 说明: 下载电话簿对象。
 * @param  [in]  bd_addr  设备地址。见 @ref bd_addr_t
 * @param  [in]  param    电话簿参数。见 @ref pbap_pce_pull_phone_book_param_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_pull_phone_book(const bd_addr_t *bd_addr, const pbap_pce_pull_phone_book_param_t *param);

/**
 * @if Eng
 * @brief Use this funtion to set current directory.
 * @par Description: Use this funtion to set current directory.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @param  [in]  name     Directory name.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 设置虚拟文件夹体系结构中的当前文件夹。
 * @par 说明: 设置虚拟文件夹体系结构中的当前文件夹。
 * @param  [in]  bd_addr  设备地址。见 @ref bd_addr_t
 * @param  [in]  name     文件夹名称。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_set_phone_book(const bd_addr_t *bd_addr, const unsigned char *name);

/**
 * @if Eng
 * @brief Use this funtion to search in the phonebook list.
 * @par Description: Use this funtion to search in the phonebook list.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @param  [in]  param    Phonebook list parameters. See @ref pbap_pce_pull_vcard_listing_param_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 检索PSE的电话簿列表对象。
 * @par 说明: 检索PSE的电话簿列表对象。
 * @param  [in]  bd_addr  设备地址。见 @ref bd_addr_t
 * @param  [in]  param    电话簿列表对象参数。见 @ref pbap_pce_pull_vcard_listing_param_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_pull_vcard_listing(const bd_addr_t *bd_addr, const pbap_pce_pull_vcard_listing_param_t *param);

/**
 * @if Eng
 * @brief Use this funtion to search for a specific vCard.
 * @par Description: Use this funtion to search for a specific vCard.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @param  [in]  param    vCard parameters. See @ref pbap_pce_pull_vcard_entry_param_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 检索特定的vCard。
 * @par 说明: 检索特定的vCard。
 * @param  [in]  bd_addr  设备地址。见 @ref bd_addr_t
 * @param  [in]  param    vCard对象参数。见 @ref pbap_pce_pull_vcard_entry_param_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_pull_vcard_entry(const bd_addr_t *bd_addr, const pbap_pce_pull_vcard_entry_param_t *param);

/**
 * @if Eng
 * @brief Use this funtion to stop transferring.
 * @par Description: Use this funtion to stop transferring.
 * @param  [in]  bd_addr  Address of the device. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @else
 * @brief 停止传输。
 * @par 说明: 停止传输。
 * @param  [in]  bd_addr  设备地址。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @endif
 */
int pbap_pce_stop_trans(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Register callback functions for PBAP.
 * @par Description: Register callback functions for PBAP.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  func  A poniter of the Callback funcs.See @ref pbap_pce_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @else
 * @brief 注册PBAP回调函数。
 * @par 说明: 注册上层应用的回调。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  func  指向回调函数接口定义的指针。见 @ref pbap_pce_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_pbap.h
 * @see pbap_pce_callbacks_t
 * @endif
 */
int pbap_pce_register_callbacks(pbap_pce_callbacks_t *func);

#ifdef __cplusplus
}
#endif
#endif /* end of bts_pbap.h */
