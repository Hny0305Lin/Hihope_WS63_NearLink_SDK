/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT HFP AG module.
 */

/**
 * @defgroup bluetooth_bts_hfp_ag HFP AG API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_HFP_AG_H
#define BTS_HFP_AG_H

#include <stdbool.h>
#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Max value of volume.
 * @else
 * @brief 最大音量值范围。
 * @endif
 */
#define HFP_AG_MAX_VOLUME 15

/**
 * @if Eng
 * @brief Max length of phone number.
 * @else
 * @brief 最大手机号码长度。
 * @endif
 */
#define HFP_AG_PHONENUM_MAX_DIGITS 32

/**
 * @if Eng
 * @brief Max length of operator.
 * @else
 * @brief 最大运营商长度。
 * @endif
 */
#define HFP_AG_COPS_OPERATOR_MAXLEN 16

/**
 * @if Eng
 * @brief Max length of battery charge.
 * @else
 * @brief 最大电量值范围。
 * @endif
 */
#define HFP_AG_MAX_BATTERY_CHARGE 5


/**
 * @if Eng
 * @brief Max length of signal.
 * @else
 * @brief 最大信号强度范围。
 * @endif
 */
#define HFP_AG_MAX_SIGNAL 5

/**
 * @if Eng
 * @brief Enum of codec info.
 * @else
 * @brief 定义codec编码信息。CVSD是默认编码格式
 * @endif
 */
typedef enum {
    HFP_AG_CODEC_ID_CVSD = 0x01, /*!< @if Eng HFP AG codec type of CVSD @else HFP AG 编码格式为CVSD @endif */
    HFP_AG_CODEC_ID_MSBC,        /*!< @if Eng HFP AG codec type of MSBC @else HFP AG 编码格式为MSBC @endif */
} hfp_ag_codec_id_t;

/**
 * @if Eng
 * @brief The value of service indicator.
 * @else
 * @brief 服务指示器的值
 * @endif
 */
typedef enum {
    HFP_AG_SERVICE_UNAVAILABLE = 0,
    HFP_AG_SERVICE_AVAILABLE = 1,
} hfp_ag_service_indi_t;

/**
 * @if Eng
 * @brief The value of call indicator.
 * @else
 * @brief 呼叫指示器的值
 * @endif
 */
typedef enum {
    HFP_AG_CALL_NO_ACTIVE = 0,
    HFP_AG_CALL_ACTIVE = 1,
} hfp_ag_call_indi_t;

/**
 * @if Eng
 * @brief The value of callsetup indicator.
 * @else
 * @brief 呼叫建立指示器的值
 * @endif
 */
typedef enum {
    HFP_AG_CALLSETUP_NO = 0,
    HFP_AG_CALLSETUP_INCOMING = 1,
    HFP_AG_CALLSETUP_OUTGOING = 2,
    HFP_AG_CALLSETUP_OUTALERT = 3,
} hfp_ag_callsetup_indi_t;

/**
 * @if Eng
 * @brief The value of callheld indicator.
 * @else
 * @brief 呼叫保持指示器的值
 * @endif
 */
typedef enum {
    HFP_AG_CALLHELD_NO = 0,
    HFP_AG_CALLHELD_ACTIVE_HOLD = 1,
    HFP_AG_CALLHELD_ONHOLD = 2,
} hfp_ag_callheld_indi_t;


/**
 * @if Eng
 * @brief The value of service indicator.
 * @else
 * @brief 漫游指示器的值
 * @endif
 */
typedef enum {
    HFP_AG_ROAM_NO_ACTIVE = 0,
    HFP_AG_ROAM_ACTIVE = 1,
} hfp_ag_roam_indi_t;

/**
 * @if Eng
 * @brief Enum of Phone call state type.
 * @else
 * @brief 电话呼叫状态类型。
 * @endif
 */
typedef enum {
    HFP_AG_CALL_STATE_ACTIVE = 0, /*!< @if Eng HFP phone calling state of active @else正在通话 @endif */
    HFP_AG_CALL_STATE_HELD,       /*!< @if Eng HFP phone calling state of hold @else保持通话 @endif */
    HFP_AG_CALL_STATE_DIALING,    /*!< @if Eng HFP phone calling state of dialing @else正在拨号 @endif */
    HFP_AG_CALL_STATE_ALERTING,   /*!< @if Eng HFP phone calling state of alerting @else正在响铃 @endif */
    HFP_AG_CALL_STATE_INCOMING,   /*!< @if Eng HFP phone calling state of incoming @else呼叫接入 @endif */
    HFP_AG_CALL_STATE_WAITING,    /*!< @if Eng HFP phone calling state of waiting @else呼叫等待 @endif */
} hfp_ag_call_state_t;

/**
 * @if Eng
 * @brief Enum of network event type.
 * @else
 * @brief 网络事件类型。
 * @endif
 */
typedef enum {
    HFP_AG_NETWORK_EVENT_RMT_IS_BUSY = 0x01,        /*!< @if Eng Remote busy @else 远端忙事件 @endif */
    HFP_AG_NETWORK_EVENT_ALERTING_RM = 0x02,        /*!< @if Eng Alerting remote @else 响铃远端事件 @endif */
    HFP_AG_NETWORK_EVENT_INCOMING_CALL = 0x03,      /*!< @if Eng Incoming call @else 来电事件 @endif */
    HFP_AG_NETWORK_EVENT_RMT_ANSWER_CALL = 0x04,    /*!< @if Eng Remote answer call @else 远端接听事件 @endif */
    HFP_AG_NETWORK_EVENT_SVC_UNAVAILABLE = 0x05,    /*!< @if Eng Service unavailable @else 服务不可用事件 @endif */
    HFP_AG_NETWORK_EVENT_SVC_AVAILABLE = 0x06,      /*!< @if Eng Service available @else 服务可用事件 @endif */
    HFP_AG_NETWORK_EVENT_SIGNAL_STRENGTH = 0x07,    /*!< @if Eng Signal strength @else 信号强度事件 @endif */
    HFP_AG_NETWORK_EVENT_ROAMING_RESET = 0x08,      /*!< @if Eng Roaming reset @else 漫游重置事件 @endif */
    HFP_AG_NETWORK_EVENT_ROAMING_ACTIVE = 0x09,      /*!< @if Eng Roaming active @else 漫游活跃事件 @endif */
} hfp_ag_network_event_t;

/**
 * @if Eng
 * @brief Enum of hfp type.
 * @else
 * @brief hfp类型。
 * @endif
 */
typedef enum {
    HFP_AG_TYPE_ALL_CALLS = 0x01,            /*!< @if Eng (Release) all the existing calls @else 所有存在的通话 @endif */
    HFP_AG_TYPE_INCOMING_CALL = 0x02,        /*!< @if Eng (Reject or accept) the incoming call @else 来电 @endif */
    HFP_AG_TYPE_HELDINCOMING_CALL = 0x03,    /*!< @if Eng (Reject or accept) the Held incoming call
                                                  @else 保持状态的来电 @endif */
    HFP_AG_TYPE_OUTGOING_CALL = 0x04,        /*!< @if Eng (Release) the outgoing call @else 呼叫中的通话 @endif */
    HFP_AG_TYPE_ONGOING_CALL = 0x05,         /*!< @if Eng (Release) the ongoing call @else 进行中的通话 @endif */
} hfp_ag_type_t;

/**
 * @if Eng
 * @brief Enum of hfp type.
 * @else
 * @brief AG 挂断类型。
 * @endif
 */
typedef enum {
    HFP_AG_CANCELED_ALLCALL   = 0x01,    /*!< @if Eng AG released all calls or GSM Service Unavailable
                                              @else AG释放所有通话或者GSM服务不可用 @endif */
    HFP_AG_CANCELED_CALLSETUP = 0x02,    /*!< @if Eng AG or GSM Release the Incoming Call or Outgoing Call
                                              @else AG或者GSM释放来电中的通话或者呼叫中的通话 @endif */
    HFP_AG_CANCELED_LASTCALL  = 0x03,    /*!< @if Eng AG or GSM Release Last Call in Call
                                              @else AG或者GSM释放最后一次通话 @endif */
    HFP_AG_CANCELED_CALLHELD  = 0x04,    /*!< @if Eng GSM Release only held calls @else GSM释放保持中的通话 @endif */
} hfp_ag_action_t;

/**
 * @if Eng
 * @brief Enum of last or not.
 * @else
 * @brief 是否最后一个。
 * @endif
 */
typedef enum {
    HFP_AG_LAST_NO = 0,
    HFP_AG_LAST_YES = 1,
} hfp_ag_last_t;

/**
 * @if Eng
 * @brief Extended Audio Gateway Error Codes.
 * @else
 * @brief 扩展的音频网关错误码。
 * @endif
 */
typedef enum {
    HFP_CMEERR_AGFAILURE                  = 0,  /*!< @if Eng AG failure @else AG失败 @endif */
    HFP_CMEERR_NOCONN2PHONE               = 1,  /*!< @if Eng No connection to phone @else 没有连接到手机 @endif */
    HFP_CMEERR_OPERATION_NOTALLOWED       = 3,  /*!< @if Eng Operation not allowed @else 操作不允许 @endif */
    HFP_CMEERR_OPERATION_NOTSUPPORTED     = 4,  /*!< @if Eng Operation not supported @else 操作不支持 @endif */
    HFP_CMEERR_PHSIMPIN_REQUIRED          = 5,  /*!< @if Eng PH-SIM PIN required @else 需要PH-SIM卡PIN码 @endif */
    HFP_CMEERR_SIMNOT_INSERTED            = 10, /*!< @if Eng SIM not inserted @else 没有插入SIM卡 @endif */
    HFP_CMEERR_SIMPIN_REQUIRED            = 11, /*!< @if Eng SIM PIN required @else 需要SIM卡PIN码 @endif */
    HFP_CMEERR_SIMPUK_REQUIRED            = 12, /*!< @if Eng SIM PUK required @else 需要SIM卡PUK码 @endif */
    HFP_CMEERR_SIM_FAILURE                = 13, /*!< @if Eng SIM failure @else SIM卡失败 @endif */
    HFP_CMEERR_SIM_BUSY                   = 14, /*!< @if Eng SIM busy @else SIM卡忙 @endif */
    HFP_CMEERR_INCORRECT_PASSWORD         = 16, /*!< @if Eng Incorrect password @else 不正确的密码 @endif */
    HFP_CMEERR_SIMPIN2_REQUIRED           = 17, /*!< @if Eng SIM PIN2 required @else 需要SIM卡PIN2码 @endif */
    HFP_CMEERR_SIMPUK2_REQUIRED           = 18, /*!< @if Eng SIM PUK2 required @else 需要SIM卡PUK2码 @endif */
    HFP_CMEERR_MEMORY_FULL                = 20, /*!< @if Eng Memory full @else 内存满 @endif */
    HFP_CMEERR_INVALID_INDEX              = 21, /*!< @if Eng Invalid index @else 无效索引 @endif */
    HFP_CMEERR_MEMORY_FAILURE             = 23, /*!< @if Eng Memory failure @else 内存申请失败 @endif */
    HFP_CMEERR_TEXTSTRING_TOOLONG         = 24, /*!< @if Eng Text string too long @else 文本字符串过长 @endif */
    HFP_CMEERR_INVALID_CHAR_INTEXTSTRING  = 25, /*!< @if Eng Invalid characters in text string
                                                     @else 文本字符串中的无效字符 @endif */
    HFP_CMEERR_DIAL_STRING_TOOLONG        = 26, /*!< @if Eng Dial string too long @else 拨号过长 @endif */
    HFP_CMEERR_INVALID_CHAR_INDIALSTRING  = 27, /*!< @if Eng Invalid characters in dial string
                                                     @else 拨号中的无效字符 @endif */
    HFP_CMEERR_NETWORK_NOSERVICE          = 30, /*!< @if Eng No network service @else 没有网络服务 @endif */
    HFP_CMEERR_NETWORK_TIMEOUT            = 31, /*!< @if Eng Network timeout @else 网络超时 @endif */
    HFP_CMEERR_EMERGENCYCALL_ONLY         = 32,  /*!< @if Eng Network not allowed, emergency calls only
                                                     @else 网络不允许，仅支持紧急呼叫 @endif */
} hfp_ag_cmee_error_t;

/**
 * @if Eng
 * @brief The mode of clcc.
 * @else
 * @brief 模式
 * @endif
 */
typedef enum {
    HFP_AG_CLCC_VOICE = 0,
    HFP_AG_CLCC_DATA = 1,
    HFP_AG_CLCC_FAX = 2,
} hfp_ag_clcc_mode_t;

/**
 * @if Eng
 * @brief The flag of multiparty.
 * @else
 * @brief 是否多方通话
 * @endif
 */
typedef enum {
    HFP_AG_CLCC_NOT_MULTI = 0,
    HFP_AG_CLCC_MULTI = 1,
} hfp_ag_clcc_mpty_t;

/**
 * @if Eng
 * @brief The direction of call.
 * @else
 * @brief 呼叫方向
 * @endif
 */
typedef enum {
    HFP_AG_CLCC_OUTGOING = 0,
    HFP_AG_CLCC_INCOMING = 1,
} hfp_ag_clcc_dir_t;

/**
 * @if Eng
 * @brief The value of cnum service.
 * @else
 * @brief 号码信息服务值
 * @endif
 */
typedef enum {
    HFP_AG_CNUM_ASYNC = 0,
    HFP_AG_CNUM_SYNC = 1,
    HFP_AG_CNUM_PAD = 2,
    HFP_AG_CNUM_PACKET = 3,
    HFP_AG_CNUM_VOICE = 4,
    HFP_AG_CNUM_FAX = 5,
} hfp_ag_cnum_service_t;

/**
 * @if Eng
 * @brief The format of operator.
 * @else
 * @brief 运营商信息格式
 * @endif
 */
typedef enum {
    HFP_AG_COPS_LONG_ALPHANUM = 0,   /*!< @if Eng Long format alphanumeric <oper>, limits the max length is 16
                                          @else 长格式的字符串数值，最大长度为16 @endif */
    HFP_AG_COPS_SHORT_ALPHANUM = 1,  /*!< @if Eng Short format alphanumeric <oper>, limits the max length is 8
                                          @else 短格式的字符串数值，最大长度为8 @endif */
    HFP_AG_COPS_NUM = 2,             /*!< @if Eng Numeric <oper> @else 数值格式 @endif */
} hfp_ag_cops_format_t;

/**
 * @if Eng
 * @brief The mode of operator.
 * @else
 * @brief 运营商信息模式
 * @endif
 */
typedef enum {
    HFP_AG_COPS_AUTO = 0,       /*!< @if Eng Automatic (<oper> field is ignored) @else 自动 @endif */
    HFP_AG_COPS_MANU = 1,       /*!< @if Eng Manual (<oper> field shall be present, and <AcT> optionally)
                                     @else 手动 @endif */
    HFP_AG_COPS_OFF = 2,        /*!< @if Eng Deregister from network @else 断开网络 @endif */
    HFP_AG_COPS_DISABLE = 3,    /*!< @if Eng set only <format> (for read command +COPS?),
                                     do not attempt registration/deregistration (<oper> and <AcT> fields are ignored);
                                     this value is not applicable in read command response
                                     @else 仅在读命令中配置 @endif */
    HFP_AG_COPS_MANU_AUTO = 4,   /*!< @if Eng manual/automatic (<oper> field shall be present);
                                     if manual selection fails, automatic mode (<mode>=0) is entered
                                     @else 自动/手动自动切换 @endif */
} hfp_ag_cops_mode_t;

typedef struct {
    uint8_t type;      /*!< @if Eng The format of the phone number provided, the region of value is 128-175
                            @else 电话号码类型，值范围是128-175 @endif */
    uint8_t service;   /*!< @if Eng Indicates which service this phone number relates to.
                            Shall be either 4 (voice) or 5 (fax). See @ref hfp_ag_cnum_service_t
                            @else 服务类型，协议支持4或5，见 @ref hfp_ag_cnum_service_t @endif */
    uint8_t num_len;                             /*!< @if Eng The length of the phone number @else 电话号码长度 @endif */
    int8_t  number[HFP_AG_PHONENUM_MAX_DIGITS];  /*!< @if Eng Subscriber number, the length shall be 32
                                                      @else 订阅号码，最大长度是32 @endif */
    uint8_t name_len;                            /*!< @if Eng Length of subaddr @else 子地址长度 @endif */
    int8_t  alpha_str[1];                    /*!< @if Eng string type subaddress of format specified by <cli_validity>
                                                  @else 子地址，协议默认是0 @endif */
} hfp_phone_info_t;

typedef struct {
    uint8_t idx;       /*!< @if Eng The numbering (start with 1) of the call given by the sequence of setting up or
                            receiving the calls @else 通话记录索引 @endif */
    uint8_t dir;       /*!< @if Eng Direction, see @ref hfp_ag_clcc_dir_t
                            @else 方向，见 @ref hfp_ag_clcc_dir_t @endif */
    uint8_t status;    /*!< @if Eng Status, see @ref hfp_ag_call_state_t
                            @else 状态，见 @ref hfp_ag_call_state_t @endif */
    uint8_t mode;      /*!< @if Eng Mode, see @ref hfp_ag_clcc_mode_t @else 模式，见 @ref hfp_ag_clcc_mode_t @endif */
    uint8_t mpty;      /*!< @if Eng Mutiparty flag, see @ref hfp_ag_clcc_mpty_t
                            @else 多方通话标记，见 @ref hfp_ag_clcc_mpty_t @endif */
    uint8_t type;      /*!< @if Eng The format of the phone number provided, the region of value is 128-175
                            @else 电话号码类型，值范围是128-175 @endif */
    uint8_t num_len;   /*!< @if Eng The length of the phone number @else 电话号码长度 @endif */
    int8_t  number[1]; /*!< @if Eng Phone number @else 电话号码 @endif */
} hfp_clcc_info_t;

typedef struct {
    uint8_t mode;            /*!< @if Eng Current mode. see @ref hfp_ag_cops_mode_t
                                  @else 当前模式 见 @ref hfp_ag_cops_mode_t @endif */
    uint8_t format;          /*!< @if Eng the format of the operator parameter string, and shall always be 0 for this
                                  specification. see @ref hfp_ag_cops_format_t
                                  @else 字符串格式, 协议仅支持长格式 见 @ref hfp_ag_cops_format_t @endif */
    uint8_t operator_len;     /*!< @if Eng The length of the network operator @else 运营商名字长度 @endif */
    int8_t  operator_name[1]; /*!< @if Eng The name of the network operator @else 运营商名字 @endif */
} hfp_cops_info_t;

typedef struct {
    uint8_t     service;            /*!< @if Eng Service. see @ref hfp_ag_service_indi_t
                                         @else 服务 见 @ref hfp_ag_service_indi_t @endif */
    uint8_t     call;               /*!< @if Eng Call. see @ref hfp_ag_call_indi_t
                                         @else 呼叫 见 @ref hfp_ag_call_indi_t @endif */
    uint8_t     callsetup;          /*!< @if Eng Callsetup. see @ref hfp_ag_callsetup_indi_t
                                         @else 呼叫建立 见 @ref hfp_ag_callsetup_indi_t @endif */
    uint8_t     callheld;           /*!< @if Eng Callheld.see @ref hfp_ag_callheld_indi_t
                                         @else 呼叫保持 见 @ref hfp_ag_callheld_indi_t @endif */
    uint8_t     signal;             /*!< @if Eng Signal, region is 0~5. @else 信号强度，值范围0-5。 @endif */
    uint8_t     roam;               /*!< @if Eng Roam see @ref hfp_ag_roam_indi_t
                                         @else 呼叫保持 见 @ref hfp_ag_roam_indi_t @endif */
    uint8_t     battchg;            /*!< @if Eng Battery charge, region is 0~5. @else 电量，值范围0-5。 @endif */
} hfp_cind_info_t;

/**
 * @if Eng
 * @brief Callback invoked in response to hfp connect state changed.
 * @par Description: When registered, the bts uses this callback to deliver hfp connect state after hfp connect updated.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  state    Connect state of hfp. See {@link profile_connect_state_t}
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief hfp连接状态更新回调函数。
 * @par 说明: 注册该回调函数之后，BTS每次hfp连接状态更新后调用该接口反馈结果给上层。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  state    hfp的连接状态。见 {@link profile_connect_state_t}
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);

/**
 * @if Eng
 * @brief Callback invoked in response to sco connect state changed.
 * @par Description: When registered, the bts uses this callback to deliver sco connect stat after sco connect updated.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  state    Connect state of sco. See {@link hfp_sco_connect_state_t}
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief sco连接状态更新回调函数。
 * @par 说明: 注册该回调函数之后，BTS每次sco连接状态更新后调用该接口反馈结果给上层。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  state    sco的连接状态。见 {@link hfp_sco_connect_state_t}.
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_sco_conn_state_changed_callback)(const bd_addr_t *bd_addr, hfp_sco_connect_state_t state);

/**
 * @if Eng
 * @brief Callback invoked in response to update codec.
 * @par Description: When registered,the bts uses this callback to deliver codec change information
 *                   after codec changed.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  codec    Codec information.See @ref hfp_ag_codec_id_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief codec编码信息变化回调函数。
 * @par 说明: 注册该回调函数之后，BTS每次codec编码格式变化后调用该接口反馈结果给上层。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  codec    编码信息。见 @ref hfp_ag_codec_id_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_codec_changed_callback)(const bd_addr_t *bd_addr, hfp_ag_codec_id_t codec);

/**
 * @if Eng
 * @brief Callback invoked in response to adjust HF volume at AG end.
 * @par Description: When registered,the bts uses this callback to deliver HF volume information
 *                   after HF volume status changes.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  type     Adjust HF volume type. See {@link hfp_volume_type_t}
 * @param  [in]  volume   Acoustic gain, range 0-15.
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief AG端调整HF音量回调函数。
 * @par 注册该回调函数之后，BTS每次AG端调整HF音量后调用该接口反馈结果给上层。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  type     调整HF音量类型。参考 {@link hfp_volume_type_t}
 * @param  [in]  volume   音量增益，范围0-15。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_volume_changed_callback)(const bd_addr_t *bd_addr, hfp_volume_type_t type, unsigned char volume);

/**
 * @if Eng
 * @brief The request of dial call from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of dial call from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  number   Phone number。
 * @param  [in]  len   The length of phone number。
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端拨号请求。
 * @par 说明: 注册该回调函数之后，收到HF端拨号请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  number   手机号码。
 * @param  [in]  len   手机号码长度。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_dial_req_callback)(const bd_addr_t *bd_addr, unsigned char *number, unsigned int len);

/**
 * @if Eng
 * @brief The request of memory dial call from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of memory dial call from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  index   The index of call list。
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端记忆拨号请求。
 * @par 说明: 注册该回调函数之后，收到HF端记忆拨号请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  index   通话列表索引。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_mem_dial_req_callback)(const bd_addr_t *bd_addr, unsigned int index);

/**
 * @if Eng
 * @brief The request of last dial call from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of last dial call from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端最后一次通话拨号请求。
 * @par 说明: 注册该回调函数之后，收到HF端最后一次通话拨号时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_lastnum_dial_req_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief The request of answer call from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of answer call from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  type     The type of call.See @ref hfp_ag_type_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端接听请求。
 * @par 说明: 注册该回调函数之后，收到HF端接听请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  type     通话类型。见 @ref hfp_ag_type_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_answer_call_req_callback)(const bd_addr_t *bd_addr, hfp_ag_type_t type);

/**
 * @if Eng
 * @brief The request of cancel call from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of cancel call from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  type     The type of call.See @ref hfp_ag_type_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端挂断请求。
 * @par 说明: 注册该回调函数之后，收到HF端挂断请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  type     通话类型。见 @ref hfp_ag_type_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_cancel_call_req_callback)(const bd_addr_t *bd_addr, hfp_ag_type_t type);

/**
 * @if Eng
 * @brief The request of DTMF from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of DTMF from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  type     DTMF code.
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端DTMF请求。
 * @par 说明: 注册该回调函数之后，收到HF端DTMF请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  code     DTMF编码。
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_dtmf_req_callback)(const bd_addr_t *bd_addr, unsigned int code);

/**
 * @if Eng
 * @brief The request of subscriber number from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of subscriber number from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端用户编号信息请求。
 * @par 说明: 注册该回调函数之后，收到HF端用户编号信息请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_subscriber_number_req_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief The request of current calls from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of subscriber number from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端通话列表请求。
 * @par 说明: 注册该回调函数之后，收到HF端通话列表请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_current_calls_req_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief The request of current indicators from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of current indicators from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端当前指示器值请求。
 * @par 说明: 注册该回调函数之后，收到HF端当前指示器值请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_current_indicator_req_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief The request of network operator format from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of network operator format from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端网络运营商格式请求。
 * @par 说明: 注册该回调函数之后，收到HF端网络运营商格式请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_network_operator_format_req_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief The request of network operator from HF device.
 * @par Description: When registered,
 *                   inform the upper application after receive the request of network operator from HF device.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #void  No return value.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief HF端网络运营商请求。
 * @par 说明: 注册该回调函数之后，收到HF端网络运营商请求时，上报给上层应用。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #void  无返回值。
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
typedef void (*hfp_ag_network_operator_req_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Struct of hfp ag callback function.
 * @else
 * @brief HFP AG回调函数接口定义。
 * @endif
 */
typedef struct {
    hfp_ag_conn_state_changed_callback                  conn_state_changed_cb;
    hfp_ag_sco_conn_state_changed_callback              sco_conn_state_changed_cb;
    hfp_ag_codec_changed_callback                       codec_changed_cb;
    hfp_ag_volume_changed_callback                      volume_changed_cb;
    hfp_ag_dial_req_callback                            dial_req_cb;
    hfp_ag_mem_dial_req_callback                        mem_dial_req_cb;
    hfp_ag_lastnum_dial_req_callback                    lastnum_dial_req_cb;
    hfp_ag_answer_call_req_callback                     answer_call_req_cb;
    hfp_ag_cancel_call_req_callback                     cancel_call_req_cb;
    hfp_ag_dtmf_req_callback                            dtmf_req_cb;
    hfp_ag_subscriber_number_req_callback               subscriber_number_req_callback;
    hfp_ag_current_calls_req_callback                   current_calls_req_callback;
    hfp_ag_current_indicator_req_callback               current_indicator_req_callback;
    hfp_ag_network_operator_format_req_callback         network_operator_format_req_callback;
    hfp_ag_network_operator_req_callback                network_operator_req_callback;
} hfp_ag_callbacks_t;

/**
 * @if Eng
 * @brief Use this funtion to establish service level connection with HF by address.
 * @par Description: Use this funtion to establish service level connection with HF by address.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 建立与HF的SLC(service level connection)。
 * @par 说明: 建立与HF的SLC(service level connection)。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to release service level connection with HF by address.
 * @par Description: Use this funtion to release service level connection with HF by address.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 释放与HF的SLC(service level connection)。
 * @par 说明: 释放与HF的SLC(service level connection)。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to establish audio connection with HF by address.
 * @par Description: Use this funtion to establish audio connection with HF by address.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 建立与HF的音频连接。
 * @par 说明: 建立与HF的音频连接。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_audio_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to disconnect audio connection with HF by address.
 * @par Description: Use this funtion to disconnect audio connection with HF by address.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 断开与HF的音频连接。
 * @par 说明: 断开与HF的音频连接。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_audio_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to get device hfp connection state by address.
 * @par Description: Use this funtion to get device hfp connection state by address.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval Return connecting/connected/disconnecting/disconnected. See @ref profile_connect_state_t.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 通过设备地址查询设备hfp连接状态。
 * @par 说明: 通过设备地址查询设备hfp连接状态。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval 返回值为正在连接，已连接，正在断开连接，已断开连接。参考 @ref profile_connect_state_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_get_device_connect_state(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to get device hfp sco connection state by address.
 * @par Description: Use this funtion to get device hfp sco connection state by address.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval Return connecting/connected/disconnecting/disconnected. See @ref profile_connect_state_t.
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 通过设备地址查询设备hfp sco连接状态。
 * @par 说明: 通过设备地址查询设备hfp sco连接状态。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval 返回值为正在连接，已连接，正在断开连接，已断开连接。参考 @ref profile_connect_state_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_get_sco_connect_state(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to answer incoming calls.
 * @par Description: Use this funtion to answer incoming calls.
 * @attention
 * @param  [in]  bd_addr      A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 接听呼入电话。
 * @par 说明: 接听呼入电话。
 * @attention
 * @param  [in]  bd_addr      对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_answer_call_event(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Use this funtion to end up calling.
 * @par Description: Use this funtion to end up calling.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  type           Action type。见 @ref hfp_ag_action_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 结束通话。
 * @par 说明: 结束通话。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  type           挂断类型。见 @ref hfp_ag_action_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_finish_call_event(const bd_addr_t *bd_addr, hfp_ag_action_t type);

/**
 * @if Eng
 * @brief Use this funtion to originate calls.
 * @par Description: Use this funtion to originate calls.
 * @attention
 * @param  [in]  bd_addr      A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 呼出电话。
 * @par 说明: 呼出电话。
 * @attention
 * @param  [in]  bd_addr      对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_originate_call_event(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief AG sends The event of incoming call.
 * @par Description: AG sends The event of incoming call.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  phone_info     incoming info.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief AG端发送来电事件。
 * @par 说明: AG端发送来电事件。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  phone_info     来电信息。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_network_incoming_event(const bd_addr_t *bd_addr, hfp_phone_info_t *phone_info);

/**
 * @if Eng
 * @brief AG sends The event of network.
 * @par Description: AG sends The event of incoming call.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  event          The type of event.See @ref hfp_ag_network_event_t
 * @param  [in]  param          The param of event.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief AG端发送其他网络事件。
 * @par 说明: AG端发送其他网络事件。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  event         事件类型。见 @ref hfp_ag_network_event_t
 * @param  [in]  param         事件参数。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_network_event(const bd_addr_t *bd_addr, hfp_ag_network_event_t event, uint8_t param);

/**
 * @if Eng
 * @brief Use this funtion to start to adjust volume at HF end.
 * @par Description: Use this funtion to start to adjust volume at HF end.
 * @attention
 * @param  [in]  bd_addr  A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  type     Adjust HF volume type. See {@link hfp_volume_type_t}.
 * @param  [in]  volume   Acoustic gain, range 0-15.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief HF端发起音量调整。
 * @par 说明: HF端发起音量调整。
 * @attention
 * @param  [in]  bd_addr  对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  type     调整HF音量类型。参考 {@link hfp_volume_type_t}。
 * @param  [in]  volume   音量增益，范围0-15。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_set_volume(const bd_addr_t *bd_addr, hfp_volume_type_t type, unsigned char volume);

/**
 * @if Eng
 * @brief AG sends the call info.
 * @par Description: AG sends the call info.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  info          The call info.
 * @param  [in]  complete      whether it is the last available call.See @ref hfp_ag_last_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 发送通话列表。
 * @par 说明: 发送通话列表。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  info         通话信息。
 * @param  [in]  complete         是否最后一条。见 @ref hfp_ag_last_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_current_calls(const bd_addr_t *bd_addr, hfp_clcc_info_t *info, hfp_ag_last_t complete);

/**
 * @if Eng
 * @brief AG sends the subscriber number info.
 * @par Description: AG sends the call info.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  info          The subscriber number info.
 * @param  [in]  complete      whether it is the last.See @ref hfp_ag_last_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief 发送订阅号码信息。
 * @par 说明: 发送订阅号码信息。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  info         订阅号码信息。
 * @param  [in]  complete         是否最后一条。见 @ref hfp_ag_last_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_subscriber_number(const bd_addr_t *bd_addr, hfp_phone_info_t *info, hfp_ag_last_t complete);

/**
 * @if Eng
 * @brief AG send battery charge.
 * @par Description: AG send battery charge.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  value          battery charge value.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief AG端发送电量信息。
 * @par 说明: AG端发送电量信息。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  value         电量值。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_battery_charge(const bd_addr_t *bd_addr, uint8_t value);

/**
 * @if Eng
 * @brief AG send cmee error code.
 * @par Description: AG send cmee error code.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  error_code     CMEE error code.See @ref hfp_ag_cmee_error_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief AG端发送扩展错误码。
 * @par 说明: AG端发送扩展错误码。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  error_code      扩展的音频网关错误码。见 @ref hfp_ag_cmee_error_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_cmee_error(const bd_addr_t *bd_addr, hfp_ag_cmee_error_t error_code);

/**
 * @if Eng
 * @brief AG send network operator info.
 * @par Description: AG send network operator info.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  info           The network operator info.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief AG端发送运营商信息。
 * @par 说明: AG端发送运营商信息。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  info          网络运营商信息
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_network_operator(const bd_addr_t *bd_addr, hfp_cops_info_t *info);


/**
 * @if Eng
 * @brief AG send indicator value info.
 * @par Description: AG send indicator value info.
 * @attention
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss.See @ref bd_addr_t
 * @param  [in]  info           The indicator value info.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @else
 * @brief AG端发送指示器信息。
 * @par 说明: AG端发送指示器信息。
 * @attention
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。见 @ref bd_addr_t
 * @param  [in]  info           指示器值信息
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @endif
 */
int hfp_ag_send_indicator_val(const bd_addr_t *bd_addr, hfp_cind_info_t *info);

/**
 * @if Eng
 * @brief Use this funtion to register callback function of framework.
 * @par Description: Use this funtion to register callback function of framework.
 * @attention
 * @param  [in]  func  A poniter of the Callback funcs.See @ref hfp_ag_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @else
 * @brief 注册上层应用的回调。
 * @par 说明: 注册上层应用的回调。
 * @attention
 * @param  [in]  func  指向回调函数接口定义的指针。见 @ref hfp_ag_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 *            @li bts_hfp_ag.h
 * @see hfp_ag_callbacks_t
 * @endif
 */
int hfp_ag_register_callbacks(hfp_ag_callbacks_t *func);

#ifdef __cplusplus
}
#endif

#endif /* end of bts_hfp_ag.h */
