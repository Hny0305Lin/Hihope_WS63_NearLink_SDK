/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT AVRCP controller module.
 */

/**
 * @defgroup bluetooth_bts_avrcp_controller AVRCP CT API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_AVRCP_CONTROLLER_H
#define BTS_AVRCP_CONTROLLER_H

#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Enum of avrcp key operation.
 * @else
 * @brief  AVRCP的按键操作，取值的定义来自于规范：1394 Trade Association。
 * @endif
 */
typedef enum {
    AVRCP_KEY_VOLUME_UP = 0x41,    /*!< @if Eng Turns the volume to high @else 音量调大操作 @endif */
    AVRCP_KEY_VOLUME_DOWN = 0x42,  /*!< @if Eng Turns the volume to low @else 音量调小操作 @endif */
    AVRCP_KEY_MUTE = 0x43,         /*!< @if Eng Turns the volume to mute @else 静音操作 @endif */
    AVRCP_KEY_PLAY = 0x44,         /*!< @if Eng play operation @else play操作 @endif */
    AVRCP_KEY_STOP = 0x45,         /*!< @if Eng stop operation @else stop操作 @endif */
    AVRCP_KEY_PAUSE = 0x46,        /*!< @if Eng pause operation @else pause操作 @endif */
    AVRCP_KEY_RECORD = 0x47,       /*!< @if Eng record operation @else record操作 @endif */
    AVRCP_KEY_REWIND = 0x48,       /*!< @if Eng Moves the position toward the beginning of the medium
                                             @else 将媒体设置为从起始位置开始 @endif */
    AVRCP_KEY_FAST_FORWARD = 0x49, /*!< @if Eng Fast forward, moves the position away from the beginning of the medium
                                             @else 设置媒体快进 @endif */
    AVRCP_KEY_FORWARD = 0x4B,      /*!< @if Eng Switches the contents, such as music tune or video chapter to the
                                             forward one @else 下一首 @endif */
    AVRCP_KEY_BACKWARD = 0x4C,     /*!< @if Eng Switches the contents, such as music tune or video chapter to the
                                             forward one @else 上一首 @endif */
    AVRCP_KEY_INVALID = 0x7F,      /*!< @if Eng Invalid @else 未知操作 @endif */
} avrcp_key_operation_t;

/**
 * @if Eng
 * @brief  Enum of AVRCP element attribute ID.
 * @else
 * @brief  AVRCP元素属性ID，在 {@link avrcp_ct_get_element_attr_para_t}
 * @endif
 */
typedef enum {
    AVRCP_CT_ELEMENT_ATTR_TITLE = 0x01, /*!< @if Eng Title, song name or content description
                                       @else 标题，歌曲名或者内容描述 @endif */
    AVRCP_CT_ELEMENT_ATTR_ARTIST_NAME =
        0x02, /*!< @if Eng Artist(s), performer(s) or group coded per specified character set
             @else 艺人、表演者或团体 @endif */
    AVRCP_CT_ELEMENT_ATTR_ALBUM_NAME = 0x03,   /*!< @if Eng Album @else 唱片，专辑 @endif */
    AVRCP_CT_ELEMENT_ATTR_TRACK_NUMBER = 0x04, /*!< @if Eng The order number of the audio-file on its original recording
                                              @else 音频文件的原始编号 @endif */
    AVRCP_CT_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS = 0x05, /*!< @if Eng The total number of tracks/elements on the
                                                        original recording @else tracks/elements的总个数 @endif */
    AVRCP_CT_ELEMENT_ATTR_TRACK_GENRE = 0x06,            /*!< @if Eng Genre @else 风格，流派 @endif */
    AVRCP_CT_ELEMENT_ATTR_TRACK_PLAYING_TIME = 0x07      /*!< @if Eng The length of the audio file in milliseconds
                                                        @else 音频文件的播放时间 @endif */
} avrcp_ct_element_attr_id_t;


/**
 * @if Eng
 * @brief  Enum of event ID of notification.
 * @else
 * @brief  定义notification事件对应的"EventIDs",
 *         在接口 {@link avrcp_ct_register_notification} 中使用，参考Audio/Video Remote Control 1.6.2 Section 28 Appendix
 * @endif
 */
typedef enum {
    AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED = 0x01, /*!< @if Eng Change in playback status of the current track
                                                          @else 播放状态变更事件 @endif */
    AVRCP_NOTIFY_EVENT_TRACK_CHANGED = 0x02, /*!< @if Eng Change of current track @else track状态变更事件 @endif */
    AVRCP_NOTIFY_EVENT_TRACK_REACHED_END = 0x03,   /*!< @if Eng Reached end of a track @else track结束事件 @endif */
    AVRCP_NOTIFY_EVENT_TRACK_REACHED_START = 0x04, /*!< @if Eng Reached start of a track @else track开始事件 @endif */
    AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED = 0x05, /*!< @if Eng Change in playback position
                                                       @else 播放位置改变事件 @endif */
    AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED =
        0x06, /*!< @if Eng Change in battery status @else 电池状态变更事件 @endif */
    AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED =
        0x07, /*!< @if Eng Change in system status @else 系统状态变更事件 @endif */
    AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08, /*!< @if Eng Change in player application setting
                                                                     @else 播放器配置变更事件 @endif */
    AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED = 0x09, /*!< @if Eng The content of the Now Playing list has changed
                                                              @else 播放内容变更事件 @endif */
    AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED = 0x0A,   /*!< @if Eng The available players have changed
                                                              @else 可使用的播放器变更事件 @endif */
    AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED = 0x0B,    /*!< @if Eng The Addressed Player has been changed
                                                              @else 播放地址变更事件 @endif */
    AVRCP_NOTIFY_EVENT_UIDS_CHANGED = 0x0C,                /*!< @if Eng The UIDs have changed
                                                              @else UID变更事件 @endif */
    AVRCP_NOTIFY_EVENT_VOLUME_CHANGED = 0x0D,              /*!< @if Eng The volume has been changed locally on the TG
                                                              @else TG端音量变更事件 @endif */
    AVRCP_NOTIFY_EVENT_RESERVED = 0x0E                     /*!< @if Eng Reserved for future use
                                                              @else 保留 @endif */
} avrcp_notify_event_t;

/**
 * @if Eng
 * @brief  Enum of absolute volume.
 * @else
 * @brief  定义绝对音量范围,有效的取值范围[0x00, 0x7F]，表示0%~100%。
 *         参考Audio/Video Remote Control 1.6.2 Section 6.13.1 Absolute Volume。
 * @endif
 */
typedef enum {
    /** 绝对音量0% */
    AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_0 = 0x00, /*!< @if Eng Absolute volume value of 0% @else 绝对音量0% @endif */
    /** 绝对音量100% */
    AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 = 0x7F, /*!< @if Eng Absolute volume value of 100% @else 绝对音量0% @endif */
    /** 无效值 */
    AVRCP_ABSOLUTE_VOLUME_INVALID = 0x80, /*!< @if Eng Invalid absolute volume value @else 无效值 @endif */
} avrcp_absolute_volume_t;

/**
 * @if Eng
 * @brief  Enum of avrcp media play status.
 * @else
 * @brief  AVRCP媒体播放状态。
 * @endif
 */
typedef enum {
    AVRCP_PLAY_STATUS_STOPPED = 0x00,  /*!< @if Eng Playing status of stop @else AVRCP媒体stopped状态 @endif */
    AVRCP_PLAY_STATUS_PLAYING = 0x01,  /*!< @if Eng Playing status of playing @else AVRCP媒体playing状态 @endif */
    AVRCP_PLAY_STATUS_PAUSED = 0x02,   /*!< @if Eng Playing status of pause @else AVRCP媒体Paused状态 @endif */
    AVRCP_PLAY_STATUS_FWD_SEEK = 0x03, /*!< @if Eng Playing status of FWD_SEEK @else AVRCP媒体切换下一首 @endif */
    AVRCP_PLAY_STATUS_REV_SEEK = 0x04, /*!< @if Eng Playing status of REV_SEEK @else AVRCP媒体切换上一首 @endif */
    AVRCP_PLAY_STATUS_ERROR = 0xFF     /*!< @if Eng Invalid status of avrcp media @else AVRCP媒体未知状态 @endif */
} avrcp_play_status_t;

/**
 * @if Eng
 * @brief  Enum of avrcp battery status.
 * @else
 * @brief  AVRCP电量状态,在结构 {@link avrcp_ct_notification_value_cb_t} 中使用。
 *        与 {@link AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED} 对应。
 * @endif
 */
typedef enum {
    AVRCP_BATTERY_STATUS_NORMAL = 0x00,      /*!< @if Eng Battery operation is in normal state
                                                       @else 电池的电量处于正常状态 @endif */
    AVRCP_BATTERY_STATUS_WARNING = 0x01,     /*!< @if Eng unable to operate soon. Is provided when the battery level
                                                       is going down @else 电池的电量处于告警状态 @endif */
    AVRCP_BATTERY_STATUS_CRITICAL = 0x02,    /*!< @if Eng Cannot operate any more. Is provided when the battery level is
                                                       going down @else 电池的电量处于红色告警状态，不能在使用 @endif */
    AVRCP_BATTERY_STATUS_EXTERNAL = 0x03,    /*!< @if Eng Plugged to external power supply @else 插接外部电源 @endif */
    AVRCP_BATTERY_STATUS_FULL_CHARGE = 0x04, /*!< @if Eng when the device is completely charged from the external
                                                       power supp @else 电池已充满电 @endif */
} avrcp_battery_status_t;

/**
 * @if Eng
 * @brief  Enum of avrcp system status.
 * @else
 * @brief  AVRCP系统状态,在结构 {@link avrcp_ct_notification_value_cb_t} 中使用。
 *         与 {@link AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED} 对应。
 * @endif
 */
typedef enum {
    AVRCP_CT_SYSTEM_STATUS_POWER_ON = 0x00,  /*!< @if Eng Power on of avrcp system @else AVRCP开机事件 @endif */
    AVRCP_CT_SYSTEM_STATUS_POWER_OFF = 0x01, /*!< @if Eng Power off of avrcp system @else AVRCP关机事件 @endif */
    AVRCP_CT_SYSTEM_STATUS_UNPLUGGED = 0x02  /*!< @if Eng unplugging of avrcp system @else AVRCP未充电状态 @endif */
} avrcp_ct_system_status_t;

/**
 * @if Eng
 * @brief  Enum of avrcp element attribute.
 * @else
 * @brief  待获取值的element attribute列表,在结构 {@link avrcp_ct_get_element_attributes} 中使用。
 * @endif
 */
typedef struct {
    unsigned char attr_num;     /*!< @if Eng number of attribute @else attribute数量 @endif */
    unsigned int *attr_id_list; /*!< @if Eng list of attribute @else attribute ID列表，协议规定每个ID占4字节
                                   取值参考 {@link avrcp_ct_element_attr_id_t} @endif */
} avrcp_ct_get_element_attr_para_t;

/**
 * @if Eng
 * @brief  Struct of support companys of TG.
 * @else
 * @brief  获取到的TG支持的company,在结构 {@link AvrcpCtSupportedCompaniesCallback} 中使用。
 * @endif
 */
typedef struct {
    unsigned char company_num; /*!< @if Eng number of support companys of TG @else TG支持的company ID的数量 @endif */
    unsigned int *company_id;  /*!< @if Eng list of support companys of TG @else TG支持的company ID列表，协议规定
                                   每个ID占3字节，接口这里每个ID分配4字节 @endif */
} avrcp_ct_support_company_para_t;

/**
 * @if Eng
 * @brief  Struct of support event of TG.
 * @else
 * @brief  获取到的TG支持的Event,协议规定 {@link avrcp_notify_event_t} 中的0x01和0x02是强制支持的
 *         eventNum取值最小为2在结构 {@link AvrcpCtSupportedCompaniesCallback} 中使用。
 * @endif
 */
typedef struct {
    unsigned char event_num; /*!< @if Eng number of support events of TG @else TG支持的event的数量 @endif */
    unsigned char *event_id; /*!< @if Eng list of support events of TG @else TG支持的event ID列表，协议规定
                                 每个eventId占1个字节，取值参考 {@link avrcp_notify_event_t} @endif */
} avrcp_ct_support_event_para_t;

/**
 * @if Eng
 * @brief  Struct of attribute value of TG.
 * @else
 * @brief  TG返回的attribute值,参考Audio/Video Remote Control 1.6.2 Section 6.6.1 Table 6.27。
 * @endif
 */
typedef struct {
    unsigned int attr_id;            /*!< @if Eng Specifies the attribute ID to be written
                                        @else 属性ID @endif */
    unsigned short character_set_id; /*!< @if Eng Specifies the character set ID to be displayed on CT
                                       @else 字符集ID @endif */
    unsigned short value_len; /*!< @if Eng Length of the value of the attribute. 0-65535 (0, if no name is provided)
                                 @else 属性名称的长度, 缺省默认0值 @endif */
    unsigned char *value;            /*!< @if Eng Name of the value of the attribute
                                         @else 属性名称 @endif */
} avrcp_ct_element_attr_value_cb_t;

/**
 * @if Eng
 * @brief  Struct of attribute param of TG.
 * @else
 * @brief  TG返回的GetElementAttributes响应参数,在接口 {@link AvrcpCtGetElementAttributeCallback} 中使用。
 * @endif
 */
typedef struct {
    unsigned char attr_num;                       /*!< @if Eng Specifies the attribute number, in range(1, 255)
                                                       @else TG返回的attribute数量，1-255 @endif */
    avrcp_ct_element_attr_value_cb_t *attr_value; /*!< @if Eng Specifies the attribute msg
                                                       @else 属性信息，根据attrNum大小动态分配空间 @endif */
} avrcp_ct_element_attr_para_cb_t;

/**
 * @if Eng
 * @brief  Struct of the addressed player changed notification msg of TG.
 * @else
 * @brief  TG返回的Addressed Player Changed notification响应数据,在结构 {@link avrcp_ct_notification_value_cb_t}
 *         中使用。
 * @endif
 */
typedef struct {
    unsigned short player_id;   /*!< @if Eng Specifies the unique Media Player Id
                                   @else 播放器的id @endif */
    unsigned short uid_counter; /*!< @if Eng Specifies the UID counter
                                   @else UID 计数 @endif */
} avrcp_ct_addressed_player_changed_cb_t;

/**
 * @if Eng
 * @brief  Struct of playe status msg of TG. if TG does not support songLength and songPosition on TG,
 *         then TG shall return 0xFFFFFFFF.
 * @else
 * @brief  TG返回的播放状态响应数据,在接口 {@link AvrcpCtGetPlayStatusCallback} 中使用。
 * @endif
 */
typedef struct {
    unsigned int song_length;        /*!< @if Eng Specifies the total length of the playing song in milliseconds
                                        @else 歌曲的播放时间，单位ms @endif */
    unsigned int song_position;      /*!< @if Eng Specifies the current position of the playing in milliseconds elapsed
                                        @else 歌曲已经播放的时间，单位ms @endif */
    avrcp_play_status_t play_status; /*!< @if Eng Specifies the current status of playing
                                      @else 歌曲的播放状态 @endif */
} avrcp_ct_play_status_cb_t;

/**
 * @if Eng
 * @brief  Struct of the notification value from TG.
 * @else
 * @brief  TG发送Notification的值,在接口 {@link AvrcpCtNotificationCallback} 中使用。
 * @endif
 */
typedef union {
    avrcp_play_status_t play_status; /*!< @if Eng Specifies the current status of playing @else 播放状态 @endif */
    unsigned long long identifier;   /*!< @if Eng Specifies the identifier @else 标识符 @endif */
    /*!< @if Eng Current playback position in millisecond. if no track currently selected,
       then return 0xFFFFFFFF in the INTERIM response.
       @else 当前歌曲已播放时长，单位ms, 无效值0xFFFFFFFF @endif */
    unsigned int playback_position;
    avrcp_battery_status_t battery_status;  /*!< @if Eng Specifies the current status of battery @else 电池状态 @endif */
    avrcp_ct_system_status_t system_status; /*!< @if Eng Specifies the current status of system @else 系统状态 @endif */
    /*!< @if Eng Specifies the addressed player changed notification msg
       @else TG返回的播放器地址变更的状态信息 @endif */
    avrcp_ct_addressed_player_changed_cb_t addressed_player;
    unsigned short uid_counter; /*!< @if Eng Specifies the uid counter @else uid计数 @endif */
    unsigned char volume;       /*!< @if Eng Specifies the current volume of player
                                    @else 当前播放器的音量 @endif */
} avrcp_ct_notification_value_cb_t;

/**
 * @if Eng
 * @brief  Callback invoked in response to avrcp ct connect state changed.
 * @par Description:When registered,the bts uses this callback to deliver avrcp connect state after avrcp connect
 * updated.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  state {@link bt_profile_connect_state_t}.
 * @retval #void  no return value.
 * @par Dependency:
 * @li  bts_avrcp_controller.h
 * @see avrcp_ct_callbacks_t
 * @else
 * @brief  avrcp ct 连接状态更新回调函数。
 * @par    说明:注册该回调函数之后，BTS每次avrcp连接状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  state avrcp的连接状态。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li  bts_avrcp_controller.h
 * @see avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);

/**
 * @if Eng
 * @brief  Callback invoked in response to avrcp tg press button.
 * @par Description:Callback invoked in response to avrcp tg press button.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention   2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  keyOperation key operation {@link bt_profile_connect_state_t}.
 *
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG的press button响应时回调函数。
 * @par 说明:收到来自TG的press button响应时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  s2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  keyOperation 按键操作。
 *
 * @retval #void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_press_button_response_callback)(const bd_addr_t *bd_addr, int key_operation);

/**
 * @if Eng
 * @brief  Callback invoked in response to avrcp tg release button.
 * @par Description: Callback invoked in response to avrcp tg release button.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  keyOperation key operation {@link bt_profile_connect_state_t}.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG的release button响应时回调函数。
 * @par Description:收到来自TG的release button响应时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  keyOperation 按键操作。
 * @retval #void 无返回值
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_release_button_response_callback)(const bd_addr_t *bd_addr, int key_operation);

/**
 * @if Eng
 * @brief  Callback invoked in response to receive avrcp tg supported company.
 * @par Description:Callback invoked in response to receive avrcp tg supported company.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  company company message {@link avrcp_ct_support_company_para_t}.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG的支持的company列表时回调函数。
 * @par Description:收到来自TG的支持的company列表时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  company 支持的company数量及ID，参考AvrcpCtSupportCompanyPara。
 * @retval #void 无返回值。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_supported_companies_callback)(
    const bd_addr_t *bd_addr, avrcp_ct_support_company_para_t *company);

/**
 * @if Eng
 * @brief  Callback invoked in response to receive avrcp tg supported event list.
 * @par Description:Callback invoked in response to receive avrcp tg supported event list.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  event event message {@link avrcp_ct_support_company_para_t}.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG的支持的event列表时回调函数。
 * @par 说明:收到来自TG的支持的event列表时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  event 支持的company数量及ID，参考AvrcpCtSupportCompanyPara。
 * @retval #void 无返回值。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_supported_event_callback)(const bd_addr_t *bd_addr, avrcp_ct_support_event_para_t *event);

/**
 * @if Eng
 * @brief  Callback invoked in response to receive the event of avrcp tg getting element attributes.
 * @par Description:Callback invoked in response to receive the event of avrcp tg getting element attributes.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  attrCbPara element attributes param.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG返回的GetElementAttributes响应时回调函数。
 * @par 说明:收到来自TG返回的GetElementAttributes响应时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  attrCbPara TG返回的属性信息，参考AvrcpCtElementAttrParaCb。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_get_element_attribute_callback)(
    const bd_addr_t *bd_addr, avrcp_ct_element_attr_para_cb_t *attr_cb_para);

/**
 * @if Eng
 * @brief  Callback invoked in response to receive the event of avrcp tg getting paly status.
 * @par Description:Callback invoked in response to receive the event of avrcp tg getting paly status.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  status element attributes param {@link avrcp_ct_play_status_cb_t}.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG返回的GetPlayStatus响应时回调函数。
 * @par Description:收到来自TG返回的GetPlayStatus响应时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  status TG返回的播放状态信息，参考AvrcpCtPlayStatusCb。
 * @retval #void 无返回值。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_get_play_status_callback)(const bd_addr_t *bd_addr, avrcp_ct_play_status_cb_t *status);

/**
 * @if Eng
 * @brief  Callback invoked in response to receive the event of avrcp tg notification.
 * @par Description:Callback invoked in response to receive the event of avrcp tg notification.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of <devices> are requested and freed by the bts automatically.
 * @attention  3. The protocol definition is not return the specific value such as 0x03/0x04/0x09/0x0a.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  eventId event id.
 * @param  [in]  notification the value of event.
 * @retval #void  no return value.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @else
 * @brief  收到来自TG返回的Notification时回调函数。
 * @par 说明:收到来自TG返回的Notification时回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @attention  3. avrcp_notify_event_t 中0x03/0x04/0x09/0x0A协议定义是不返回具体值。因此在
 * @attention  {@link avrcp_ct_notification_value_cb_t}中不体现。
 * @attention  HOS_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED需单独定义，
 * @attention  notification回调函数，不在本接口注。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  eventId TG返回的eventid。
 * @param  [in]  notification eventId对应的值。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @see  avrcp_ct_callbacks_t
 * @endif
 */
typedef void (*avrcp_ct_notification_callback)(
    const bd_addr_t *bd_addr, avrcp_notify_event_t event_id, avrcp_ct_notification_value_cb_t *notification);

/**
 * @if Eng
 * @brief  Struct of Avrcp CT callback function.
 * @else
 * @brief  Avrcp CT回调函数接口定义。
 * @endif
 */
typedef struct {
    avrcp_ct_conn_state_changed_callback conn_state_changed_cb;
    avrcp_ct_press_button_response_callback press_button_cb;
    avrcp_ct_release_button_response_callback release_button_cb;
    avrcp_ct_supported_companies_callback supported_companies_cb;
    avrcp_ct_supported_event_callback supported_event_cb;
    avrcp_ct_get_element_attribute_callback element_attr_cb;
    avrcp_ct_get_play_status_callback play_status_cb;
    avrcp_ct_notification_callback notification_cb;
} avrcp_ct_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to get avrcp connection state by address.
 * @par Description:Use this funtion to get avrcp connection state by address.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @retval #int  return One ofbt_profile_connect_state_t, connecting/connected/disconnecting/disconnected.
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  通过设备地址查询设备avrcp连接状态。
 * @par 说明:通过设备地址查询设备avrcp连接状态。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @retval #int 返回值为正在连接，已连接，正在断开连接，已断开连接。
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_get_device_connect_state(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to connect to the peer bluetooth device of avrcp connection by address.
 * @par Description:Use this funtion to connect to the peer bluetooth device of avrcp connection by address.
 * @attention        NULL
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  与对端设备建立avrcp连接。
 * @par 说明:与对端设备建立a2dp连接。
 * @attention        NULL
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to disconnect to the peer bluetooth device of avrcp connection by address.
 * @par Description:Use this funtion to disconnect to the peer bluetooth device of avrcp connection by address.
 * @attention        NULL
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  断开与TG的连接。
 * @par 说明:断开与TG的连接。
 * @attention        NULL
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to notify arvcp tg to response press button.
 * @par Description:Use this funtion to notify arvcp tg to response press button.
 * @attention           NULL
 * @param  [in]  bdAddr a poniter of the peer bluetooth device addresss.
 * @param  [in]  keyOperation key operation {@link bt_profile_connect_state_t}.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  通知TG端CT已经按下button。
 * @par 说明:通知TG端CT已经按下button。
 * @attention           NULL
 * @param  [in]  bdAddr 对端蓝牙设备的地址信息。
 * @param  [in]  keyOperation 按键操作。
 * @retval #ERRCODE_BT_SUCCESS  执行成功。
 * @retval #Other              执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_press_button(const bd_addr_t *bd_addr, int key_operation);

/**
 * @if Eng
 * @brief  Use this funtion to notify arvcp tg to response release button.
 * @par Description:Use this funtion to notify arvcp tg to response release button.
 * @attention        NULL
 * @param  [in]  bdAddr        a poniter of the peer bluetooth device addresss.
 * @param  [in]  keyOperation key operation {@link bt_profile_connect_state_t}.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  通知TG端CT已经释放button。
 * @par 说明:通知TG端CT已经释放button。
 * @attention        NULL
 * @param  [in]  bdAddr        对端蓝牙设备的地址信息。
 * @param  [in]  keyOperation 按键操作。
 * @retval #ERRCODE_BT_SUCCESS  执行成功。
 * @retval #Other              执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_release_button(const bd_addr_t *bd_addr, int key_operation);

/**
 * @if Eng
 * @brief  Use this funtion to get the company supported by remote device.
 * @par Description:Use this funtion to get the company supported by remote device.
 * @attention        NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, a poniter of the peer bluetooth device addresss.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  获取TG端支持的公司列表。
 * @par 说明:获取TG端支持的公司列表。
 * @attention        NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, 对端蓝牙设备的地址信息。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_get_supported_companies(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to get the event supported by remote device.
 * @par Description:Use this funtion to get the event supported by remote device.
 * @attention           NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, a poniter of the peer bluetooth device addresss.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  获取TG端支持的事件列表。
 * @par 说明:获取TG端支持的事件列表。
 * @attention           NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, 对端蓝牙设备的地址信息。s
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_get_supported_events(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to get the element attributes of remote device, like song information including title,
 *         album, artist, composer, year, etc.
 * @par Description:
 *         Use this funtion to get the element attributes of remote device, like song information including title,
 *         album, artist, composer, year, etc.
 * @attention        NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, a poniter of the peer bluetooth device addresss.
 * @param  [in]  param type  #avrcp_ct_get_element_attr_para_t attributeId
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  获取TG端支持的属性列表，比如歌曲标题，专辑、艺术风格，年份和作曲家等。
 * @par 说明:获取TG端支持的属性列表，比如歌曲标题，专辑、艺术风格，年份和作曲家等。
 * @attention        NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, 对端蓝牙设备的地址信息。
 * @param  [in]  param type  #avrcp_ct_get_element_attr_para_t attributeId数量及列表。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_get_element_attributes(const bd_addr_t *bd_addr, const avrcp_ct_get_element_attr_para_t *param);

/**
 * @if Eng
 * @brief  Use this funtion to get tthe status of the currently playing media at the TG.
 * @par Description:Use this funtion to get tthe status of the currently playing media at the TG.
 * @attention        NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, a poniter of the peer bluetooth device addresss.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  获取TG端媒体的播放状态信息。
 * @par 说明: 获取TG端媒体的播放状态信息。
 * @attention        NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, 对端蓝牙设备的地址信息。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_get_play_status(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to registers with the TG to receive notifications asynchronously
 *         based on specific events occurring. 
 * @par Description:
 *         Use this funtion to registers with the TG to receive notifications asynchronously
 *         based on specific events occurring. 
 * @attention           NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, a poniter of the peer bluetooth device addresss.
 * @param  [in]  eventId type  #unsigned char, envent id.
 * @param  [in]  interval type  #unsigned int, specifies the time interval (in seconds) at which the change in
 *                            playback position will be notified.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  将TG端支持的事件注册成功后，如果该事件改变会自动通知到CT端。
 * @par 说明:将TG端支持的事件注册成功后，如果该事件改变会自动通知到CT端。
 * @attention           NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, 对端蓝牙设备的地址信息。
 * @param  [in]  eventId type  #unsigned char, 事件id。
 * @param  [in]  interval type  #unsigned int, TG端状态变更时向CT端回响应的时间。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_register_notification(const bd_addr_t *bd_addr, unsigned char event_id, unsigned int interval);

/**
 * @if Eng
 * @brief  Use this funtion to set an absolute volume to be used by the rendering device.
 * @par Description:Use this funtion to set an absolute volume to be used by the rendering device.
 * @attention           NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, a poniter of the peer bluetooth device addresss.
 * @param  [in]  volume type  #int, absolue volume value {@link bt_profile_connect_state_t}.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other                    Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  设置TG端的绝对音量。
 * @par 说明:设置TG端的绝对音量。
 * @attention           NULL
 * @param  [in]  bdAddr type  #bd_addr_t *, 对端蓝牙设备的地址信息。
 * @param  [in]  volume type  #int, 绝对音量百分比，参考AvrcpCtAbsoluteVolume。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_set_absolute_volume(const bd_addr_t *bd_addr, unsigned char volume);

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of avrcp ct.
 * @par Description:Use this funtion to register callback function of avrcp ct.
 * @attention  NULL
 * @param  [in]  func type  #avrcp_ct_callbacks_t *, a poniter of the callback funcs.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_avrcp_controller.h
 * @else
 * @brief  注册Avrcp CT回调函数。
 * @par 说明:注册Avrcp CT回调函数。
 * @attention  NULL
 * @param  [in]  func type  #avrcp_ct_callbacks_t *, 指向回调函数接口定义的指针, 参考BtAvrcpCtCallbacks。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               执行失败。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_avrcp_controller.h
 * @endif
 */
int avrcp_ct_register_callbacks(avrcp_ct_callbacks_t *func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif
/** @} */
