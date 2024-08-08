/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2023. All rights reserved.
* Description: header file for wifi api.
*/

/**
 * @defgroup soc_wifi_basic WiFi Basic Settings
 * @ingroup soc_wifi
 */

#ifndef EXT_WIFI_API_H
#define EXT_WIFI_API_H

#include <stdint.h>
#include "osal_types.h"
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * mac transform string.CNcomment:地址转为字符串.CNend
 */
#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef MAC2STR
#define mac2str(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif

#ifndef bit
#define bit(x) (1U << (x))
#endif

/**
 * @ingroup soc_wifi_basic
 *
 * TKIP of cipher mode.CNcomment:加密方式为TKIP.CNend
 */
#define WIFI_CIPHER_TKIP                 bit(3)

/**
 * @ingroup soc_wifi_basic
 *
 * CCMP of cipher mode.CNcomment:加密方式为CCMP.CNend
 */
#define WIFI_CIPHER_CCMP                 bit(4)

/**
 * @ingroup soc_wifi_basic
 *
 * Channel numbers of 2.4G frequency.CNcomment:2.4G频段的信道数量.CNend
 */
#define WIFI_24G_CHANNEL_NUMS 14

/**
 * @ingroup soc_wifi_basic
 *
 * max interiface name length.CNcomment:网络接口名最大长度.CNend
 */
#define WIFI_IFNAME_MAX_SIZE             16

/**
 * @ingroup soc_wifi_basic
 *
 * The minimum timeout of a single reconnection.CNcomment:最小单次重连超时时间.CNend
 */
#define WIFI_MIN_RECONNECT_TIMEOUT   2

/**
 * @ingroup soc_wifi_basic
 *
 * The maximum timeout of a single reconnection, representing an infinite number of loop reconnections.
 * CNcomment:最大单次重连超时时间，表示无限次循环重连.CNend
 */
#define WIFI_MAX_RECONNECT_TIMEOUT   65535

/**
 * @ingroup soc_wifi_basic
 *
 * The minimum auto reconnect interval.CNcomment:最小自动重连间隔时间.CNend
 */
#define WIFI_MIN_RECONNECT_PERIOD    1

/**
 * @ingroup soc_wifi_basic
 *
 * The maximum auto reconnect interval.CNcomment:最大自动重连间隔时间.CNend
 */
#define WIFI_MAX_RECONNECT_PERIOD   65535

/**
 * @ingroup soc_wifi_basic
 *
 * The minmum times of auto reconnect.CNcomment:最小自动重连连接次数.CNend
 */
#define WIFI_MIN_RECONNECT_TIMES    1

/**
 * @ingroup soc_wifi_basic
 *
 * The maximum times of auto reconnect.CNcomment:最大自动重连连接次数.CNend
 */
#define WIFI_MAX_RECONNECT_TIMES   65535

/**
 * @ingroup soc_wifi_basic
 *
 * max scan number of ap.CNcomment:支持扫描ap的最多数目.CNend
 */
#define WIFI_SCAN_AP_LIMIT               64

/**
 * @ingroup soc_wifi_basic
 *
 * length of status buff.CNcomment:获取连接状态字符串的长度.CNend
 */
#define WIFI_STATUS_BUF_LEN_LIMIT        512

/**
 * @ingroup soc_wifi_basic
 *
 * Decimal only WPS pin code length.CNcomment:WPS中十进制pin码长度.CNend
 */
#define WIFI_WPS_PIN_LEN             8

/**
 * @ingroup soc_wifi_basic
 *
 * default max num of station.CNcomment:默认支持的station最大个数.CNend
 */
#define WIFI_DEFAULT_MAX_NUM_STA 8
/**
 * @ingroup soc_wifi_basic
 *
 * scan_cnt default value enable bit 0B0001.CNcomment:scan_cnt默认值使能位0B0001.CNend
 */
#define WIFI_SCAN_CNT_DEFAULT_ENABLE                     (1 << 0)
/**
 * @ingroup soc_wifi_basic
 *
 * scan_time default value enable bit 0B0010.CNcomment:scan_time默认值使能位0B0010.CNend
 */
#define WIFI_SCAN_TIME_DEFAULT_ENABLE                    (1 << 1)
/**
 * @ingroup soc_wifi_basic
 *
 * scan_channel_interval default value enable bit 0B0100.CNcomment:scan_channel_interval默认值使能位0B0100.CNend
 */
#define WIFI_SCAN_CHANNEL_INTERVAL_DEFAULT_ENABLE        (1 << 2)
/**
 * @ingroup soc_wifi_basic
 *
 * work_time_on_home_channel default value enable bit 0B1000.CNcomment:work_time_on_home_channel默认值使能位0B1000.CNend
 */
#define WIFI_WORK_TIME_ON_HOME_CHANNEL_DEFAULT_ENABLE    (1 << 3)

/**
 * @ingroup soc_wifi_basic
 *
 * single_probe_req_send_times default value enable bit 0B10000.CNcomment:probe_req_send_cnt默认值使能位0B10000.CNend
 */
#define WIFI_SINGLE_PROBE_REQ_SEND_TIMES_DEFAULT_ENABLE    (1 << 4)

/**
 * @ingroup soc_wifi_basic
 *
 * return success value.CNcomment:返回成功标识.CNend
 */
#define EXT_WIFI_OK      0

/**
 * @ingroup soc_wifi_basic
 *
 * return failed value.CNcomment:返回值错误标识.CNend
 */
#define EXT_WIFI_FAIL    (-1)

/**
 * @ingroup soc_wifi_basic
 *
 * Max length of SSID.CNcomment:SSID最大长度定义.CNend
 */
#define EXT_WIFI_MAX_SSID_LEN  32

/**
 * @ingroup soc_wifi_basic
 *
 * Length of MAC address.CNcomment:MAC地址长度定义.CNend
 */
#define EXT_WIFI_MAC_LEN        6

/**
 * @ingroup soc_wifi_basic
 *
 * Length of device type.CNcomment:device type长度定义.CNend
 */
#define EXT_WIFI_DEV_TYPE_LEN        8

/**
 * @ingroup soc_wifi_basic
 *
 * Length of device name.CNcomment:device name长度定义.CNend
 */
#define EXT_WIFI_DEV_NAME_LEN        32

/**
 * @ingroup soc_wifi_basic
 *
 * Length of group name.CNcomment:group name长度定义.CNend
 */
#define EXT_WIFI_GROUP_NAME_LEN       100

/**
 * @ingroup soc_wifi_basic
 *
 * String length of bssid, eg. 00:00:00:00:00:00.CNcomment:bssid字符串长度定义(00:00:00:00:00:00).CNend
 */
#define EXT_WIFI_TXT_ADDR_LEN   17

/**
 * @ingroup soc_wifi_basic
 *
 * Length of Key.CNcomment:KEY 密码长度定义.CNend
 */
#define EXT_WIFI_AP_KEY_LEN     64

/**
 * @ingroup soc_wifi_basic
 *
 * Maximum  length of Key.CNcomment:KEY 最大密码长度.CNend
 */
#define EXT_WIFI_MAX_KEY_LEN    64

/**
 * @ingroup soc_wifi_basic
 *
 * Return value of invalid channel.CNcomment:无效信道返回值.CNend
 */
#define EXT_WIFI_INVALID_CHANNEL 0xFF

/**
 * @ingroup soc_wifi_basic
 *
 * Index of Vendor IE.CNcomment:Vendor IE 最大索引.CNend
 */
#define EXT_WIFI_VENDOR_IE_MAX_IDX 1

/**
 * @ingroup soc_wifi_basic
 *
 * Max length of Vendor IE.CNcomment:Vendor IE 最大长度.CNend
 */
#define EXT_WIFI_VENDOR_IE_MAX_LEN 255

/**
 * @ingroup soc_wifi_basic
 *
 * Minimum length of custom's frame.CNcomment:用户定制报文最小长度值.CNend
 */
#define EXT_WIFI_CUSTOM_PKT_MIN_LEN 24

/**
 * @ingroup soc_wifi_basic
 *
 * Max length of custom's frame.CNcomment:用户定制报文最大长度值.CNend
 */
#define EXT_WIFI_CUSTOM_PKT_MAX_LEN 1400

/**
 * @ingroup soc_wifi_basic
 *
 * Length of wpa psk.CNcomment:wpa psk的长度.CNend
 */
#define EXT_WIFI_STA_PSK_LEN                 32

/**
 * @ingroup soc_wifi_basic
 *
 * Max num of retry.CNcomment:软件重传的最大次数.CNend
 */
#define EXT_WIFI_RETRY_MAX_NUM               15

/**
 * @ingroup soc_wifi_basic
 *
 * Max time of retry.CNcomment:软件重传的最大时间.CNend
 */
#define EXT_WIFI_RETRY_MAX_TIME              200

/**
 * @ingroup soc_wifi_basic
 *
 * Minimum priority of callback task.CNcomment:事件回调task的最小优先级.CNend
 */
#define EXT_WIFI_CB_MIN_PRIO                 10

/**
 * @ingroup soc_wifi_basic
 *
 * Max priority of callback task.CNcomment:事件回调task的最大优先级.CNend
 */
#define EXT_WIFI_CB_MAX_PRIO                 30

/**
 * @ingroup soc_wifi_basic
 *
 * max usr ie length.CNcomment:用户IE字段最大长度CNend
 */
#define EXT_WIFI_USR_IE_MAX_SIZE 257

/**
 * @ingroup soc_wifi_basic
 *
 * max identity length.CNcomment:身份字段最大长度CNend
 */
#define EXT_WIFI_IDENTITY_LEN 64

/* 驱动上报的私有断连错误码偏移量, 与驱动中mac_status_code_enum对应,大于此偏移的错误码认为是STA主动断开 */
#define WIFI_MAC_REPORT_DISCONNECT_OFFSET 5200
 /* 未找到network的私有错误码 */
#define WIFI_NETWORK_NOT_FOUND_ERROR 5300
#define WLAN_REASON_MIC_FAILURE 14
/**
 * @ingroup soc_wifi_basic
 *
 * Event type of WiFi event.CNcomment:WiFi的事件类型CNend
 */
typedef enum {
    EXT_WIFI_CAP_DISABLE = 0,    /**< 关闭某项WIFI能力 */
    EXT_WIFI_CAP_ENABLE,          /**< 使能某项WIFI能力 */
    EXT_WIFI_CAP_BUTT
} ext_wifi_cap;

/**
 * @ingroup soc_wifi_basic
 *
 * Reporting data type of monitor mode.CNcomment:混杂模式上报的数据类型.CNend
 */
typedef enum {
    EXT_WIFI_MONITOR_OFF,                /**< close monitor mode. CNcomment: 关闭混杂模式.CNend */
    EXT_WIFI_MONITOR_MCAST_DATA,         /**< report multi-cast data frame. CNcomment: 上报组播(广播)数据包.CNend */
    EXT_WIFI_MONITOR_UCAST_DATA,         /**< report single-cast data frame. CNcomment: 上报单播数据包.CNend */
    EXT_WIFI_MONITOR_MCAST_MANAGEMENT,   /**< report multi-cast mgmt frame. CNcomment: 上报组播(广播)管理包.CNend */
    EXT_WIFI_MONITOR_UCAST_MANAGEMENT,   /**< report sigle-cast mgmt frame. CNcomment: 上报单播管理包.CNend */

    EXT_WIFI_MONITOR_BUTT
} ext_wifi_monitor_mode;

/*!
 * \typedef ext_wifi_mac_disconnect_reason
 * \brief 驱动上报的私有断连状态码, 与mac_common_frame_rom.h的值对应
 */
typedef enum {
    WLAN_JOIN_RSP_TIMEOUT = 5200,
    WLAN_AUTH_RSP2_TIMEOUT = 5201,
    WLAN_AUTH_RSP4_TIMEOUT = 5202,
    WLAN_ASOC_RSP_TIMEOUT = 5203,
    WLAN_AUTH_RSP_TIMEOUT = 5204,
    WLAN_ACTIVE_DISCONNECT_OFFSET = 5205,
    WLAN_DISASOC_MISC_LINKLOSS = 5206,
    WLAN_MAC_EXT_AUTH_FAIL = 5210
} ext_wifi_mac_disconnect_reason; /* 驱动上报的私有断连状态 */
/**
 * @ingroup soc_wifi_basic
 *
 * Definition of protocol frame type.CNcomment:协议报文类型定义.CNend
 */
typedef enum {
    EXT_WIFI_PKT_TYPE_BEACON,        /**< Beacon packet. CNcomment: Beacon包.CNend */
    EXT_WIFI_PKT_TYPE_PROBE_REQ,     /**< Probe Request packet. CNcomment: Probe Request包.CNend */
    EXT_WIFI_PKT_TYPE_PROBE_RESP,    /**< Probe Response packet. CNcomment: Probe Response包.CNend */
    EXT_WIFI_PKT_TYPE_ASSOC_REQ,     /**< Assoc Request packet. CNcomment: Assoc Request包.CNend */
    EXT_WIFI_PKT_TYPE_ASSOC_RESP,    /**< Assoc Response packet. CNcomment: Assoc Response包.CNend */

    EXT_WIFI_PKT_TYPE_BUTT
}ext_wifi_pkt_type;

/**
 * @ingroup soc_wifi_basic
 *
 * Interface type of wifi.CNcomment:wifi 接口类型.CNend
 */
typedef enum {
    EXT_WIFI_IFTYPE_UNSPECIFIED,
    EXT_WIFI_IFTYPE_ADHOC,
    EXT_WIFI_IFTYPE_STATION = 2,         /**< Station. CNcomment: STA类型.CNend */
    EXT_WIFI_IFTYPE_AP = 3,              /**< SoftAp. CNcomment: SoftAp类型.CNend */
    EXT_WIFI_IFTYPE_AP_VLAN,
    EXT_WIFI_IFTYPE_WDS,
    EXT_WIFI_IFTYPE_MONITOR,
    EXT_WIFI_IFTYPE_MESH_POINT = 7,      /**< Mesh. CNcomment: Mesh类型.CNend */
    EXT_WIFI_IFTYPE_P2P_CLIENT,
    EXT_WIFI_IFTYPE_P2P_GO,
    EXT_WIFI_IFTYPE_P2P_DEVICE,

    EXT_WIFI_IFTYPES_BUTT
} ext_wifi_iftype;

/**
 * @ingroup soc_wifi_basic
 *
 * Definition of bandwith type.CNcomment:接口带宽定义.CNend
 */
typedef enum {
    EXT_WIFI_BW_HIEX_5M,     /**< 5M bandwidth. CNcomment: 窄带5M带宽.CNend */
    EXT_WIFI_BW_HIEX_10M,    /**< 10M bandwidth. CNcomment: 窄带10M带宽.CNend */
    EXT_WIFI_BW_LEGACY_20M,  /**< 20M bandwidth. CNcomment: 20M带宽.CNend */
    EXT_WIFI_BW_BUTT
} ext_wifi_bw;

/**
 * @ingroup hi_wifi_basic
 *
 * The protocol mode of station which softap connected to.CNcomment:与softap连接的sta的phy协议.CNend
 */
typedef enum {
    WIFI_11B_PHY_PROTOCOL_MODE              = 0,    /**< 11b CCK. CNcomment:11b.CNend */
    WIFI_LEGACY_OFDM_PHY_PROTOCOL_MODE      = 1,    /**< 11g/a OFDM. CNcomment:11g/a.CNend */
    WIFI_HT_PHY_PROTOCOL_MODE               = 2,    /**< 11n HT. CNcomment:11n.CNend */
    WIFI_HE_SU_FORMAT                       = 3,    /**< 11ax HE SU Format type. CNcomment:11ax.CNend */
} ext_phy_protocol_enum;

/**
 * @ingroup soc_wifi_basic
 *
 * Authentification type enum.CNcomment:认证类型(连接网络不支持EXT_WIFI_SECURITY_WPAPSK).CNend
 */
typedef enum {
    EXT_WIFI_SEC_TYPE_INVALID = -1,          /**< INVALID. CNcomment: 认证类型:非法值.CNend */
    EXT_WIFI_SECURITY_OPEN,                  /**< OPEN. CNcomment: 认证类型:开放.CNend */
    EXT_WIFI_SECURITY_WEP,                   /**< WEP. CNcomment: 认证类型:WEP.CNend */
    EXT_WIFI_SECURITY_WPA2PSK,               /**< WPA-PSK. CNcomment: 认证类型:WPA2-PSK.CNend */
    EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX,    /**< WPA/WPA2-PSK MIX. CNcomment: 认证类型:WPA-PSK/WPA2-PSK混合.CNend */
    EXT_WIFI_SECURITY_WPAPSK,                /**< WPA-PSK. CNcomment: 认证类型:WPA-PSK.CNend */
    EXT_WIFI_SECURITY_WPA,                   /**< WPA-Enterprise. CNcomment: 认证类型:WPA企业级.CNend */
    EXT_WIFI_SECURITY_WPA2,                  /**< WPA2-Enterprise. CNcomment: 认证类型:WPA2企业级.CNend */
    EXT_WIFI_SECURITY_SAE,                   /**< SAE. CNcomment: 认证类型:SAE.CNend */
    EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX,     /**< WPA3/WPA2-PSK MIX. CNcomment: 认证类型:WPA3/WPA2-PSK混合.CNend */
    EXT_WIFI_SECURITY_WPA3,                  /**< WPA3-Enterprise. CNcomment: 认证类型:WPA3企业级.CNend */
    EXT_WIFI_SECURITY_OWE,                   /**< OWE. CNcomment: 认证类型:OWE.CNend */
    EXT_WIFI_SECURITY_WAPI_PSK,              /**< WAPI-PSK. CNcomment: 认证类型:WAPI-PSK.CNend */
    EXT_WIFI_SECURITY_WAPI_CERT,             /**< WAPI-CERT. CNcomment: 认证类型:WAPI企业级.CNend */
    EXT_WIFI_SECURITY_WPA3_WPA2_MIX,         /**< WPA3/WPA2-Enterprise MIX. CNcomment: 认证类型:WPA3/WPA2企业级混合.CNend */
    EXT_WIFI_SECURITY_WEP_OPEN,              /**< WEP-OPEN. CNcomment: 认证类型:WEP-OPEN.CNend */
    EXT_WIFI_SECURITY_WPA3_192BIT,           /**< WPA3-Enterprise 192bit. CNcomment: 认证类型:WPA3企业级192位.CNend */
    EXT_WIFI_SECURITY_UNKNOWN                /**< UNKNOWN. CNcomment: 其他认证类型:UNKNOWN.CNend */
} ext_wifi_auth_mode;

/**
 * @ingroup soc_wifi_basic
 *
 * Encryption type enum.CNcoment:加密类型.CNend
 *
 */
typedef enum {
    EXT_WIFI_PARIWISE_UNKNOWN,               /**< UNKNOWN. CNcomment: 加密类型:UNKNOWN.CNend */
    EXT_WIFI_PAIRWISE_AES,                   /**< AES. CNcomment: 加密类型:AES.CNend */
    EXT_WIFI_PAIRWISE_TKIP,                  /**< TKIP. CNcomment: 加密类型:TKIP.CNend */
    EXT_WIFI_PAIRWISE_TKIP_AES_MIX,          /**< TKIP/AES MIX. CNcomment: 加密类型:TKIP AES混合.CNend */
    EXT_WIFI_PAIRWISE_CCMP256,               /**< CCMP-256. CNcomment: 加密类型:CCMP-256.CNend */
    EXT_WIFI_PAIRWISE_CCMP256_CCMP,          /**< CCMP-256/CCMP. CNcomment: 加密类型:CCMP-256 CCMP.CNend */
    EXT_WIFI_PAIRWISE_CCMP256_TKIP,          /**< CCMP-256/TKIP. CNcomment: 加密类型:CCMP-256 TKIP.CNend */
    EXT_WIFI_PAIRWISE_BUTT
} ext_wifi_pairwise;

/**
 * @ingroup soc_wifi_basic
 *
 * sae_pwe type enum.CNcomment:SAE PWE 生成类型.CNend
 */
typedef enum {
    WIFI_SAE_PWE_UNSPECIFIED,     /**< UNSPECIFIED */
    WIFI_SAE_PWE_HUNT_AND_PECK,   /**< hunting-and-pecking loop only. */
    WIFI_SAE_PWE_HASH_TO_ELEMENT, /**< hash-to-element only */
    WIFI_SAE_PWE_BOTH             /** <both hunting-and-pecking loop and hash-to-element enabled */
} wifi_sae_pwe_option_enum;

/**
 * @ingroup soc_wifi_basic
 *
 * Type of connect's status.CNcomment:连接状态.CNend
 */
typedef enum {
    EXT_WIFI_DISCONNECTED,   /**< Disconnected. CNcomment: 连接状态:未连接.CNend */
    EXT_WIFI_CONNECTED,      /**< Connected. CNcomment: 连接状态:已连接.CNend */
    EXT_WIFI_CONNECTING,      /**< Connecting. CNcomment: 连接状态:连接中.CNend */
} ext_wifi_conn_status;

/**
 * @ingroup soc_wifi_basic
 *
 * Frame type that usr ies will insert into.CNcomment: 待插入ie字段的帧类型.CNend
 */
typedef enum  {
    EXT_WIFI_EXTEND_IE1,
    EXT_WIFI_EXTEND_IE2,
    EXT_WIFI_EXTEND_IE3,
    EXT_WIFI_EXTEND_IE4,
    EXT_WIFI_EXTEND_IE_BUTT
} ext_wifi_extend_ie_index;

typedef enum {
    EXT_WIFI_BEACON_IE       = (1 << 0),
    EXT_WIFI_PROBE_REQ_IE    = (1 << 1),
    EXT_WIFI_PROBE_RSP_IE    = (1 << 2)
} ext_wifi_frame_type;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of WiFi event.CNcomment:WiFi的事件类型.CNend
 */
typedef enum {
    EXT_WIFI_EVT_UNKNOWN,                       /**< UNKNWON.
                                                     CNcomment: UNKNOWN.CNend */
    EXT_WIFI_EVT_SCAN_DONE,                     /**< Scan finish.
                                                     CNcomment: STA扫描完成.CNend */
    EXT_WIFI_EVT_CONNECTED,                     /**< Connected.
                                                     CNcomment: 已连接.CNend */
    EXT_WIFI_EVT_DISCONNECTED,                  /**< Disconnected.
                                                     CNcomment: 断开连接.CNend */
    EXT_WIFI_EVT_WPS_TIMEOUT,                   /**< WPS timeout.
                                                     CNcomment: WPS事件超时.CNend */
    EXT_WIFI_EVT_WPS_SUCCESS,                   /**< WPS success.
                                                     CNcomment: WPS事件成功.CNend */
    EXT_WIFI_EVT_WPS_FAIL,                      /**< WPS fail.
                                                     CNcomment: WPS事件失败.CNend */
    EXT_WIFI_EVT_MESH_CONNECTED,                /**< MESH connected.
                                                     CNcomment: MESH已连接.CNend */
    EXT_WIFI_EVT_MESH_DISCONNECTED,             /**< MESH disconnected.
                                                     CNcomment: MESH断开连接.CNend */
    EXT_WIFI_EVT_AP_START,                      /**< AP start.
                                                     CNcomment: AP开启.CNend */
    EXT_WIFI_EVT_AP_DISABLE,                    /**< AP disable.
                                                     CNcomment: AP未使能.CNend */
    EXT_WIFI_EVT_STA_CONNECTED,                 /**< STA connected with ap.
                                                     CNcomment: AP和STA已连接.CNend */
    EXT_WIFI_EVT_STA_DISCONNECTED,              /**< STA disconnected from ap.
                                                     CNcomment: AP和STA断开连接.CNend */
    EXT_WIFI_EVT_STA_NO_NETWORK,                /**< STA or P2P connect, but can't find network
                                                     CNcomment: STA或P2P连接时扫描不到网络.CNend */
    EXT_WIFI_EVT_MESH_CANNOT_FOUND,             /**< MESH can't find network.
                                                     CNcomment: MESH关联扫不到对端.CNend */
    EXT_WIFI_EVT_MESH_SCAN_DONE,                /**< MESH AP scan finish.
                                                     CNcomment: MESH AP扫描完成.CNend */
    EXT_WIFI_EVT_MESH_STA_SCAN_DONE,            /**< MESH STA scan finish.
                                                     CNcomment: MESH STA扫描完成.CNend */
    EXT_WIFI_EVT_AP_SCAN_DONE,                  /**< AP scan finish.
                                                     CNcomment: AP扫描完成.CNend */
    EXT_WIFI_EVT_P2P_FIND_STOP,                 /**< P2P find stop.
                                                     CNcomment: p2p查找停止.CNend */
    EXT_WIFI_EVT_P2P_PROV_DISC_PBC_REQ,         /**< p2p provision discovery pbc requets.
                                                     CNcomment: p2p pbc发现请求.CNend */
    EXT_WIFI_EVT_P2P_PROV_DISC_PBC_RESP,        /**< p2p provision discovery pbc response.
                                                     CNcomment: p2p pbc发现回应.CNend */
    EXT_WIFI_EVT_P2P_GO_NEG_REQUEST,            /**< p2p go neg request.
                                                     CNcomment: p2p go协商请求.CNend */
    EXT_WIFI_EVT_P2P_GO_NEG_SUCCESS,            /**< p2p go neg success.
                                                     CNcomment: p2p go协商成功.CNend */
    EXT_WIFI_EVT_P2P_GO_NEG_FAILURE,            /**< p2p go neg failure.
                                                     CNcomment: p2p go协商失败.CNend */
    EXT_WIFI_EVT_P2P_GROUP_FORMATION_SUCCESS,   /**< p2p group formation success.
                                                     CNcomment: p2p组网成功.CNend */
    EXT_WIFI_EVT_P2P_GROUP_FORMATION_FAILURE,   /**< p2p group formation failure.
                                                     CNcomment: p2p组网失败.CNend */
    EXT_WIFI_EVT_P2P_GROUP_STARTED,             /**< p2p group start.
                                                     CNcomment: p2p组网开始.CNend */
    EXT_WIFI_EVT_P2P_INVITATION_RECIEVE,        /**< p2p invite result.
                                                     CNcomment: p2p收到邀请事件.CNend */
    EXT_WIFI_EVT_P2P_INVITATION_RESULT,         /**< p2p invite result.
                                                     CNcomment: p2p邀请结果.CNend */
    EXT_WIFI_EVT_P2P_INVITATION_ACCEPT,         /**< p2p invite result.
                                                     CNcomment: p2p邀请接收结果.CNend */
    EXT_WIFI_EVT_BUTT
} ext_wifi_event_type;

/**
 * @ingroup soc_wifi_basic
 *
 * Scan type enum.CNcomment:扫描类型.CNend
 */
typedef enum {
    EXT_WIFI_BASIC_SCAN,             /**< Common and all channel scan. CNcomment: 普通扫描.CNend */
    EXT_WIFI_CHANNEL_SCAN,           /**< Specified channel scan. CNcomment: 指定信道扫描.CNend */
    EXT_WIFI_SSID_SCAN,              /**< Specified SSID scan. CNcomment: 指定SSID扫描.CNend */
    EXT_WIFI_SSID_PREFIX_SCAN,       /**< Prefix SSID scan. CNcomment: SSID前缀扫描.CNend */
    EXT_WIFI_BSSID_SCAN,             /**< Specified BSSID scan. CNcomment: 指定BSSID扫描.CNend */
} ext_wifi_scan_type;

/**
 * @ingroup soc_wifi_basic
 *
 * WPA PSK usage type.CNcomment: WPA PSK使用策略.CNend
 */
typedef enum {
    EXT_WIFI_WPA_PSK_NOT_USE,        /**< Not use. CNcomment: 不使用.CNend */
    EXT_WIFI_WPA_PSK_USE_INNER,      /**< Inner PSK. CNcomment: 使用内部PSK.CNend */
    EXT_WIFI_WPA_PSK_USE_OUTER,      /**< Outer PSK. CNcomment: 使用外部PSK.CNend */
} ext_wifi_wpa_psk_usage_type;

/**
 * @ingroup soc_wifi_basic
 *
 * XLDO voltage.CNcomment: XLDO控制电压.CNend
 */
typedef enum {
    EXT_WIFI_XLDO_VOLTAGE_0 = 0x00,      /**< 1.5V. CNcomment: 1.5V.CNend */
    EXT_WIFI_XLDO_VOLTAGE_1 = 0x11,      /**< 1.8V. CNcomment: 1.8V.CNend */
    EXT_WIFI_XLDO_VOLTAGE_2 = 0x22,      /**< 2.1V. CNcomment: 2.1V.CNend */
    EXT_WIFI_XLDO_VOLTAGE_3 = 0x33,      /**< 2.4V. CNcomment: 2.4V.CNend */
} ext_wifi_xldo_voltage;

/**
 * @ingroup soc_wifi_basic
 *
 * eap method.CNcomment: EAP方法.CNend
 */
typedef enum {
    EXT_WIFI_EAP_METHOD_TLS,      /**< EAP-TLS. CNcomment: EAP-TLS方法.CNend */
    EXT_WIFI_EAP_METHOD_BUTT
} ext_wifi_eap_method;

/**
 * @ingroup soc_wifi_basic
 *
 * Scan type enum.CNcomment:重传报文类型.CNend
 */
typedef enum {
    EXT_WIFI_RETRY_FRAME_DATA,             /**< data frame retrans. CNcomment: 重传数据帧.CNend */
    EXT_WIFI_RETRY_FRAME_MGMT,             /**< mgmt frame retrans. CNcomment: 重传管理帧.CNend */
    EXT_WIFI_RETRY_FRAME_BUTT,
} ext_wifi_retry_frame_type;

typedef struct {
    unsigned int rate_value;    /*!< @if Eng fixed rate value.
                                     @else  固定速率值。  @endif */
} ext_alg_param_stru;

typedef struct {
    uint8_t enable;       /*!< @if Eng enable.
                               @else PSD使能标记 0关1开。 @endif */
    uint8_t resv;
    uint16_t duration;    /*!< @if Eng duration.
                               @else 采样时长 取值1~65535单位min。 @endif */
    uint32_t cycle;       /*!< @if Eng cycle.
                               @else 采样间隔 单位ms；取值100~1000。 @endif */
} psd_option_param;

typedef struct {
    int8_t mdata_en  : 1;   /*!< @if Eng get multi-cast data frame flag.
                                 @else 使能接收组播(广播)数据包。 @endif */
    int8_t udata_en  : 1;   /*!< @if Eng get single-cast data frame flag.
                                 @else 使能接收单播数据包。 @endif */
    int8_t mmngt_en  : 1;   /*!< @if Eng get multi-cast mgmt frame flag.
                                 @else 使能接收组播(广播)管理包。 @endif */
    int8_t umngt_en  : 1;   /*!< @if Eng get single-cast mgmt frame flag.
                                 @else 使能接收单播管理包。 @endif */
    int8_t custom_en : 1;   /*!< @if Eng get beacon/probe response flag.
                                 @else 使能接收beacon/probe request包。 @endif */
    int8_t resvd     : 3;   /*!< @if Eng reserved bits.
                                 @else 保留字段。 @endif */
} ext_wifi_ptype_filter_stru;

/**
 * @ingroup soc_wifi_basic
 *
 * parameters of scan.CNcomment:station和mesh接口scan参数.CNend
 */
typedef struct {
    /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];
    /**< BSSID. CNcomment: BSSID.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];
    /**< SSID length. CNcomment: SSID长度.CNend */
    unsigned char ssid_len;
    /**< Channel list. CNcomment: 信道号列表，范围1-14，不同区域有差异.CNend  */
    unsigned char chan_list[WIFI_24G_CHANNEL_NUMS];
    /**< Numbers of channel list. CNcomment: 信道列表数目.CNend */
    unsigned char chan_num;
    /**< Scan type. CNcomment: 扫描类型.CNend */
    ext_wifi_scan_type scan_type;
} ext_wifi_scan_params;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of scan param.CNcomment:扫描参数结构体.CNend
 */
typedef struct {
    /* default_enable. CNcomment:默认值使能位，1表示使能 */
    unsigned int default_enable;
    /* scan_cnt. CNcomment:扫描次数，取值范围1~2，默认为2 */
    unsigned char scan_cnt;
    /* scan_time. CNcomment:扫描驻留时间，取值范围20~120，默认为20 */
    unsigned char scan_time;
    /* scan_channel_interval. CNcomment:间隔n个信道，切回工作信道工作一段时间，n取值范围1~6，默认6 */
    unsigned char scan_channel_interval;
    /* work_time_on_home_channel. CNcomment:背景扫描时，返回工作信道工作的时间，取值范围30~120，默认110 */
    unsigned char work_time_on_home_channel;
    /* single_probe_send_times. CNcomment:单个probe req报文的发送次数，取值范围1~3，默认1 */
    unsigned char single_probe_send_times;
    unsigned char rsv[3];
} ext_wifi_scan_param_stru;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of scan result.CNcomment:扫描结果结构体.CNend
 */
typedef struct {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];   /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];  /**< BSSID. CNcomment: BSSID.CNend */
    ext_wifi_auth_mode auth;                /**< Authentication type. CNcomment: 认证类型.CNend */
    unsigned int channel;                   /**< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    int rssi;                               /**< Signal Strength. CNcomment: 信号强度.CNend */
    unsigned char wps_flag : 1;             /**< WPS flag. CNcomment: WPS标识.CNend */
    unsigned char wps_session : 1;          /**< WPS session:PBC-0/PIN-1. CNcomment: WPS会话类型,PBC-0/PIN-1.CNend */
    unsigned char wmm : 1;                  /**< WMM flag. CNcomment: WMM标识.CNend */
    unsigned char ft_flag : 1;              /**< FT flag. CNcomment: FT 标识 */
    unsigned char ext_mesh_flag : 1;        /**< MESH flag. CNcomment: MESH标识.CNend */
    ext_wifi_pairwise pairwise;             /**< Encryption type. CNcomment: 加密方式,不需指定时置0.CNend */
} ext_wifi_ap_info;

/* 公共结构体定义 */
typedef struct {
    unsigned short length;          /* 不仅是本公共结构体长度，该字段是发送到驱动的参数结构体的总长度, frw消息时使用 */
    unsigned char type;             /* 业务类型 */
    unsigned char rsv;              /* 预留 */
    int (*wifi_app_service_cb)(void *param); /* 统一的回调函数 */
} wifi_app_common_input_param;

typedef struct {
    unsigned char *ssid;            /* 链表元素ssid ie;   从驱动回填 */
    unsigned char *bssid;           /* 链表元素bssid;   从驱动回填 */
    unsigned short caps;            /* 链表元素能力位;  从驱动回填 */
    unsigned short beacon_int;      /* 链表元素beacon周期;  从驱动回填 */
    int freq;                       /* 链表元素信道中心频率;  从驱动回填 */
    int level;                      /* 链表元素rssi;  从驱动回填 */
    unsigned char *frame_ie;        /* 链表元素ie指针;  从驱动回填 */
    unsigned short frame_ie_len;    /* 链表元素ie长度;  从驱动回填 */
    unsigned char ssid_ie_len;      /* 链表元素ssid ie长度; 从驱动回填 */
    unsigned char channel;          /* 链表元素所在信道, 目前仅获取扫描结果时使用 */
    unsigned char index;            /* 链表元素索引值, 目前仅获取扫描结果时使用 */
    unsigned char rsv[3];           /* 预留 */
} wifi_app_common_output_param;

typedef struct {
    wifi_app_common_input_param input_para;
    wifi_app_common_output_param output_para;
} wifi_app_common_param;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of connect parameters.CNcomment:station连接结构体.CNend
 */
typedef struct {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];    /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    ext_wifi_auth_mode auth;                 /**< Authentication mode. CNcomment: 认证类型.CNend */
    char key[EXT_WIFI_MAX_KEY_LEN + 1];      /**< Secret key. CNcomment: 秘钥.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];   /**< BSSID. CNcomment: BSSID.CNend */
    ext_wifi_pairwise pairwise;              /**< Encryption type. CNcomment: 加密方式,不需指定时置0.CNend */
    unsigned char hex_flag;                  /**< Hex key flag. CNcomment: 表示当前密码为十六进制格式 */
    unsigned char ft_flag;                   /**< FT flag. CNcomment: FT 标识 */
    unsigned char resv[3];                   /**< Reserved. CNcomment: 预留.CNend */
} ext_wifi_assoc_request;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of fast connect parameters.CNcomment:station快速连接结构体.CNend
 */
typedef struct {
    ext_wifi_assoc_request req;              /**< Association request. CNcomment: 关联请求.CNend */
    unsigned char channel;                  /**< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    unsigned char psk[EXT_WIFI_STA_PSK_LEN]; /**< PSK. CNcomment: PSL.CNend */
    ext_wifi_wpa_psk_usage_type psk_flag;    /**< PSK TYPE. CNcomment: psk的标志,不需指定时置0.CNend */
} ext_wifi_fast_assoc_request;

/**
 * @ingroup soc_wifi_basic
 *
 * Status of sta's connection.CNcomment:获取station连接状态.CNend
 */
typedef struct {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];    /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];   /**< BSSID. CNcomment: BSSID.CNend */
    unsigned int channel;                   /**< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    ext_wifi_conn_status status;             /**< Connect status. CNcomment: 连接状态.CNend */
} ext_wifi_status;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of wifi scan done.CNcomment:扫描完成事件.CNend
 */
typedef struct {
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];      /**< interface name. CNcomment: 接口名称.CNend */
    unsigned short bss_num;
} event_wifi_scan_done;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of wifi connected CNcomment:wifi的connect事件信息.CNend
 */
typedef struct {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];    /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];   /**< BSSID. CNcomment: BSSID.CNend */
    unsigned char ssid_len;                 /**< SSID length. CNcomment: SSID长度.CNend */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];  /**< Iftype name. CNcomment: 接口名称.CNend */
} event_wifi_connected;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of wifi disconnected.CNcomment:wifi的断开事件信息.CNend
 */
typedef struct {
    unsigned char bssid[EXT_WIFI_MAC_LEN];    /**< BSSID. CNcomment: BSSID.CNend */
    unsigned short reason_code;              /**< reason code. CNcomment: 断开原因.CNend */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];   /**< Iftype name. CNcomment: 接口名称.CNend */
    unsigned char locally_generated;         /**< locally_generated. CNcomment: 是否为主动触发的断连(1是0否).CNend */
    unsigned char wpa_state;                 /**< wpa_state. CNcomment: 当前WPA状态(enum wpa_states).CNend */
} event_wifi_disconnected;

/**
 * @ingroup soc_wifi_basic
 *
 * struct of interface name.CNcomment:wifi interface 名称.CNend
 */
typedef struct {
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];   /**< Iftype name. CNcomment: 接口名称.CNend */
} event_wifi_interface_name;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of wps fail.CNcomment:wifi wps协商失败事件信息.CNend
 */
typedef struct {
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];     /**< Iftype name. CNcomment: 接口名称.CNend */
    int msg;                                  /**< wps fail message. CNcomment: wps失败信息.CNend */
    unsigned short int config_error;          /**< wps config error. CNcomment: wps失败配置信息.CNend */
    unsigned short int error_indication;      /**< wps reason_code. CNcomment: wps错误码.CNend */
} event_wifi_wps_fail;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of ap connected sta.CNcomment:ap连接sta事件信息.CNend
 */
typedef struct {
    char addr[EXT_WIFI_MAC_LEN];    /**< user's mac address of SoftAp. CNcomment: 连接AP的sta地址.CNend */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];
} event_ap_sta_connected;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of ap disconnected sta.CNcomment:ap断开sta事件信息.CNend
 */
typedef struct {
    unsigned char addr[EXT_WIFI_MAC_LEN];    /**< User's mac address of SoftAp. CNcomment: AP断开STA的MAC地址.CNend */
    unsigned short reason_code;             /**< Reason code. CNcomment: AP断开连接的原因值.CNend */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];
} event_ap_sta_disconnected;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of mesh connected.CNcomment:mesh的connect事件信息.CNend
 */
typedef struct {
    unsigned char addr[EXT_WIFI_MAC_LEN];    /**< User's mac address of MESH. CNcomment: MESH连接的peer MAC地址.CNend */
} event_mesh_connected;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of mesh disconnected.CNcomment:mesh的disconnect事件信息.CNend
 */
typedef struct {
    unsigned char addr[EXT_WIFI_MAC_LEN];    /**< User's mac address of MESH. CNcomment: 断开连接的peer MAC地址.CNend */
    unsigned short reason_code;             /**< Reason code. CNcomment: MESH断开连接的原因.CNend */
} event_mesh_disconnected;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p provision discovery pbc request.CNcomment:p2p pbc 发现请求事件.CNend
 */
typedef struct {
    unsigned char dev_addr[EXT_WIFI_MAC_LEN];           /**< User's device address of P2P.
                                                             CNcomment: 连接的peer MAC地址.CNend */
    unsigned char pri_dev_type[EXT_WIFI_DEV_TYPE_LEN];  /**< User's device type of P2P.
                                                             CNcomment: 连接的device类型.CNend */
    char dev_name[EXT_WIFI_DEV_NAME_LEN + 1];           /**< User's device name of P2P.
                                                             CNcomment: 连接的device名称.CNend */
    unsigned short int supp_config_methods;             /**< User's config methods of P2P.
                                                             CNcomment: 连接方式.CNend */
    unsigned char dev_capab;                            /**< User's device capbility of P2P.
                                                             CNcomment: 连接的device能力.CNend */
    unsigned char group_capab;                          /**< User's group capbility of P2P.
                                                             CNcomment: 连接的group能力.CNend */
    char group[EXT_WIFI_GROUP_NAME_LEN];                /**< User's group interface name P2P.
                                                             CNcomment: 连接的group名称.CNend */
} event_p2p_prov_disc_pbc_request;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p provision discovery pbc response.CNcomment:p2p pbc 发现回应事件.CNend
 */
typedef struct {
    int peer_go;                              /**< User's peer go of P2P. CNcomment: 对端是否是go.CNend */
    unsigned char peer[EXT_WIFI_MAC_LEN];     /**< User's mac address of P2P. CNcomment: 连接的peer MAC地址.CNend */
} event_p2p_prov_disc_pbc_response;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p go neg request.CNcomment:p2p的go协商请求事件.CNend
 */
typedef struct {
    unsigned char addr[EXT_WIFI_MAC_LEN];    /**< User's mac address of P2P. CNcomment: 连接的peer MAC地址.CNend */
    unsigned short int dev_passwd_id;        /**< User's device password id. CNcomment: 设备密码id.CNend */
    unsigned char go_intent;                 /**< User's go intent. CNcomment:  协商成为go的信息.CNend */
} event_p2p_go_neg_request;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p go neg success.CNcomment:p2p的go协商成功事件.CNend
 */
typedef struct {
    int role_go;                                            /**< role. CNcomment:本端角色.CNend */
    int freq;                                               /**< freq. CNcomment:频率.CNend */
    unsigned char peer_device_addr[EXT_WIFI_MAC_LEN];       /**< peer device addr. CNcomment:对端设备addr.CNend */
    unsigned char peer_interface_addr[EXT_WIFI_MAC_LEN];    /**< peer addr. CNcomment:对端mac地址.CNend */
    int wps_method;                                         /**< wps methon. CNcomment:wps关联方式.CNend */
} event_p2p_go_neg_success;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p go neg fail.CNcomment:p2p的go协商失败事件.CNend
 */
typedef struct {
    int status;                                            /**< status. CNcomment:失败状态事件信息.CNend */
} event_p2p_go_neg_failure;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p group start.CNcomment:p2p group start 事件.CNend
 */
typedef struct {
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];            /**< interface name. CNcomment: 接口名称.CNend */
    int go;                                       /**< go or not. CNcomment: 是否是go.CNend */
    char ssid_txt[EXT_WIFI_MAX_SSID_LEN * 4 + 1]; /**< group name. CNcomment: 组名称.CNend */
    int freq;                                     /**< group' freq. CNcomment: 组的频率.CNend */
    unsigned char go_dev_addr[EXT_WIFI_MAC_LEN];  /**< go device addr. CNcomment: go的device地址.CNend */
    int persistent;                               /**< persistent or not. CNcomment: 是否是永久网络.CNend */
} event_p2p_group_started;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p invite.CNcomment:p2p的invite接受事件.CNend
 */
typedef struct {
    unsigned char sa[EXT_WIFI_MAC_LEN];                  /**< sa addr. CNcomment:sa地址.CNend */
    unsigned char go_dev_addr[EXT_WIFI_MAC_LEN];         /**< go device addr. CNcomment:go device地址.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];               /**< bssid. CNcomment: bssid.CNend */
    int op_freq;                                         /**< operational. freq CNcomment: 所在的频率.CNend */
    int persistent_id;                                   /**< network id. freq CNcomment: 永久网络id.CNend */
    int unknow_network;                                  /**< unknow network.CNcomment: 未在本地存储的网络.CNend */
} event_p2p_invite_recieve;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p invite result.CNcomment:p2p的邀请结果事件.CNend
 */
typedef struct {
    int status;
} event_p2p_invite_result;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type of p2p invite.CNcomment:p2p的invite接受事件.CNend
 */
typedef struct {
    unsigned char sa[EXT_WIFI_MAC_LEN];                  /**< sa addr. CNcomment:sa地址.CNend */
    unsigned char go_dev_addr[EXT_WIFI_MAC_LEN];         /**< go device addr. CNcomment:go device地址.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];               /**< bssid. CNcomment: bssid.CNend */
    int op_freq;                                         /**< operational. freq CNcomment: 所在的频率.CNend */
    int persistent_id;                                   /**< network id. freq CNcomment: 永久网络id.CNend */
} event_p2p_invite_accept;

/**
 * @ingroup soc_wifi_basic
 *
 * Event type wifi information.CNcomment:wifi的事件信息体.CNend
 */
typedef union {
    event_wifi_scan_done wifi_scan_done;                    /**< Scan finish event.
                                                                 CNcomment: WIFI扫描完成事件信息.CNend */
    event_wifi_connected wifi_connected;                    /**< STA's connected event.
                                                                 CNcomment: STA的连接事件信息.CNend */
    event_wifi_disconnected wifi_disconnected;              /**< STA's dsiconnected event.
                                                                 CNcomment: STA的断连事件信息.CNend */
    event_wifi_interface_name wifi_wps_timeout;             /**< STA's timeout event.
                                                                 CNcomment: STA的WPS超时事件信息.CNend */
    event_wifi_interface_name wifi_wps_success;             /**< STA's success event.
                                                                 CNcomment: STA的WPS断开连接事件信息.CNend */
    event_wifi_wps_fail wifi_wps_fail;                      /**< STA's dsiconnected event.
                                                                 CNcomment: STA的wps断连事件信息.CNend */
    event_wifi_interface_name wifi_ap_start;                /**< AP's start event.
                                                                 CNcomment: AP启动事件信息.CNend */
    event_wifi_interface_name wifi_ap_fail;                 /**< AP's fail event.
                                                                 CNcomment: AP启动失败事件信息.CNend */
    event_ap_sta_connected ap_sta_connected;                /**< AP's connected event .
                                                                 CNcomment: AP的连接事件信息.CNend */
    event_ap_sta_disconnected ap_sta_disconnected;          /**< AP's disconnected event.
                                                                 CNcomment: AP的断连事件信息.CNend */
    event_wifi_interface_name scan_no_network;              /**< can not scan network event.
                                                                 CNcomment: 扫描不到网络事件信息.CNend */
    event_mesh_connected mesh_connected;                    /**< MESH's connected event.
                                                                 CNcomment: MESH连接事件信息.CNend */
    event_mesh_disconnected mesh_disconnected;              /**< MESH's disconnected event.
                                                                 CNcomment: MESH断连事件信息.CNend */
    event_p2p_prov_disc_pbc_request p2p_disc_pbc_request;   /**< p2p's pbc discovery request event.
                                                                 CNcomment: P2P PBC发现请求信息.CNend */
    event_p2p_prov_disc_pbc_response p2p_disc_pbc_response; /**< p2p's pbc discovery responset event.
                                                                 CNcomment: P2P PBC回应请求信息.CNend */
    event_p2p_go_neg_request p2p_go_neg_request;            /**< p2p's go neg request event.
                                                                 CNcomment: P2P对端go协商请求信息.CNend */
    event_p2p_go_neg_success p2p_go_neg_success;            /**< p2p's go neg success event.
                                                                 CNcomment: P2P对端go协商请求成功.CNend */
    event_p2p_go_neg_failure p2p_go_neg_failure;            /**< p2p's go neg failure event.
                                                                 CNcomment: P2P对端go协商请求信息.CNend */
    event_p2p_group_started p2p_go_group_started;           /**< p2p's group start event.
                                                                 CNcomment: P2P组开始时间.CNend */
    event_p2p_invite_recieve p2p_invite_recieve;            /**< p2p's invite recieve event.
                                                                 CNcomment: P2P收到邀请信息.CNend */
    event_p2p_invite_result p2p_invite_result;              /**< p2p's invite result event.
                                                                 CNcomment: P2P邀请结果信息.CNend */
    event_p2p_invite_accept p2p_invite_accept;              /**< p2p's invite accept event.
                                                                 CNcomment: P2P邀请接受信息.CNend */
} ext_wifi_event_info;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of WiFi event.CNcomment:WiFi事件结构体.CNend
 *
 */
typedef struct {
    ext_wifi_event_type event;   /**< Event type. CNcomment: 事件类型.CNend */
    ext_wifi_event_info info;    /**< Event information. CNcomment: 事件信息.CNend */
} ext_wifi_event;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of softap's basic config.CNcomment:softap基本配置.CNend
 *
 */
typedef struct {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];    /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    char key[EXT_WIFI_AP_KEY_LEN + 1];       /**< Secret key. CNcomment: 秘钥.CNend */
    unsigned char channel_num;              /**< Channel number. CNcomment: 信道号，范围1-14，不同区域有差异.CNend */
    int ssid_hidden;                        /**< Hidden ssid. CNcomment: 是否隐藏SSID.CNend */
    ext_wifi_auth_mode authmode;             /**< Authentication mode. CNcomment: 认证方式.CNend */
    ext_wifi_pairwise pairwise;              /**< Encryption type. CNcomment: 加密方式,不需指定时置0.CNend */
} ext_wifi_softap_config;

/**
 * @ingroup soc_wifi_basic
 *
 * mac address of softap's user.CNcomment:与softap相连的station mac地址.CNend
 *
 */
typedef struct {
    unsigned char mac[EXT_WIFI_MAC_LEN];     /**< MAC address.CNcomment:与softap相连的station mac地址.CNend */
} ext_wifi_ap_sta_info;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of WPA psk calc config.CNcomment:计算WPA psk需要设置的参数.CNend
 */
typedef struct {
    unsigned char ssid[EXT_WIFI_MAX_SSID_LEN + 1]; /**< SSID. CNcomment: SSID 只支持ASCII字符.CNend */
    char key[EXT_WIFI_AP_KEY_LEN + 1];             /**< Secret key. CNcomment: 秘钥.CNend */
} ext_wifi_sta_psk_config;

/**
 * @ingroup hi_wifi_basic
 *
* Struct of scan result.CNcomment:扫描结果参数.CNend
 */
typedef struct {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];        /**< Service set ID (SSID).CNcomment:SSID.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];   /**< Basic service set ID (BSSID).CNcomment:BSSID.CNend */
    char reserved;
    int security_type;       /**< Security type.For details, see {@link wifi_security_enumType}.CNcomment:安全类型.CNend */
    int rssi;               /**< Received signal strength indicator (RSSI).CNcomment:RSSI.CNend */
    int band;               /**< Frequency band.CNcomment:频带.CNend */
    int channel_num;         /**< channel num.CNcomment:信道号.CNend */
} ext_wifi_scan_result_stru;

/**
 * @ingroup soc_wifi_basic
 *
 * Struct of report datarate.CNcomment:数据发包参数.CNend
 */
typedef struct {
    unsigned int best_rate;
} ext_wifi_report_tx_params;

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of wpa-enterprise param import.CNcommment:定制化扫描回调函数.CNend
 */
typedef void (*ext_wifi_scan_no_save_cb)(ext_wifi_scan_result_stru *scan_result);

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of wpa-enterprise param import.CNcommment:企业级导入回调接口定义.CNend
 */
typedef int (*uapi_wifi_ent_import_cb)(unsigned char **buf, unsigned int *len);

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of psd data report.CNcommment:PSD数据上报回调接口定义.CNend
 */
typedef int32_t (*wifi_psd_cb)(void *recv_buf, uint32_t data_len);

/**
 * @ingroup soc_wifi_basic
 *
 * parameters of wpa-enterprise callback.CNcomment:企业级回调注册参数.CNend
 */
typedef struct {
    uapi_wifi_ent_import_cb  ca_cert_import;   /**< Import CA certificate callback. CNcomment: 导入CA证书回调.CNend */
    uapi_wifi_ent_import_cb  cli_cert_import;  /**< Import client certificate callback. CNcomment: 导入客户端证书回调.CNend */
    uapi_wifi_ent_import_cb  cli_key_import;   /**< Import client private key callback. CNcomment: 导入客户端密钥回调.CNend */
} ext_wifi_ent_import_callback;

typedef struct {
    unsigned char user_idx;                     /* CSI白名单用户index 取值范围0~3，最多4个用户 */
    unsigned char enable;                       /* CSI白名单用户开关，0去使能，忽略以下所有参数， 1使能，校验后续参数 */
    unsigned char cfg_match_ta_ra_sel;          /* CSI白名单地址过滤类型 0 RA 1 TA */
    unsigned char resv;                         /**< 保留1位，字节对齐 */
    unsigned char mac_addr[EXT_WIFI_MAC_LEN];   /* 白名单MAC地址 */
    unsigned char frame_type_filter_bitmap;     /* 帧类型过滤具体参数 取值范围0~7，bit0管理帧 bit1控制帧 bit2数据帧 */
    unsigned char sub_type_filter_enable;       /* 帧子类型过滤开关，0关闭 1打开，只有在1时才会校验sub_type_filter_bitmap */
    unsigned char sub_type_filter;              /* 帧子类型过滤具体参数 4位二进制数对应的十进制结果（如 1100即为12） */
    unsigned char ppdu_filter_bitmap;           /* ppdu format过滤具体参数 取值范围0~63
                                               bit[0]:non-HT
                                               bit[1]:HE_(ER)SU
                                               bit[2]:HE_MU_MIMO
                                               bit[3]:HE_MU_OFDMA
                                               bit[4]:HT
                                               bit[5]:VHT */
    unsigned short period;                      /* CSI上报时间间隔 单位ms */
} ext_csi_config;

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of monitor mode.CNcommment:混杂模式收包回调接口定义.CNend
 */
typedef int32_t (*wifi_promis_cb)(void* recv_buf, int32_t frame_len, int8_t rssi);

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of wifi event.CNcommment:wifi事件回调接口定义.CNend
 */
typedef void (*uapi_wifi_event_cb)(const ext_wifi_event *event);

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of wpa-enterprise param import.CNcommment:CSI调接口定义.CNend
 */
typedef void (*wifi_csi_data_cb)(unsigned char *csi_data, int len);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:扫描完成接口定义.CNend
 */
unsigned int uapi_wifi_force_scan_complete(void);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:删除驱动扫描链表接口定义.CNend
 */
unsigned int uapi_wifi_scan_results_clear(void);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:获取扫描状态接口定义.CNend
 */
int uapi_wifi_get_scan_flag(void);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:AP扫描接口定义.CNend
 */
int uapi_wifi_ap_scan(void);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:可自定义回调函数扫描接口定义.CNend
 */
int uapi_wifi_sta_raw_scan(ext_wifi_scan_params *sp, ext_wifi_scan_no_save_cb cb);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:设置CSI配置函数接口定义.CNend
 */
int uapi_wifi_csi_set_config(const char *ifname, const ext_csi_config *config);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:CSI注册回调函数接口定义.CNend
 */
void uapi_csi_register_data_report_cb(wifi_csi_data_cb data_cb);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:CSI启动函数接口定义.CNend
 */
int uapi_wifi_csi_start(void);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:CSI关闭函数接口定义.CNend
 */
int uapi_wifi_csi_stop(void);


/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:设置报文重传函数接口定义.CNend
 */
td_s32 uapi_wifi_set_pkt_retry_policy(td_u8 frame_type, td_u8 retry_count);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:mac_phy重置函数接口定义.CNend
 */
int uapi_wifi_mac_phy_reset(void);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:设置RTS阈值函数接口定义.CNend
 */
td_u32 uapi_wifi_set_rts_threshold(td_u8 mode, td_u16 pkt_length);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:设置phy发送速率函数接口定义.CNend
 */
uint32_t uapi_wifi_set_phy_txrate(td_u8 auto_rate, ext_alg_param_stru *alg_param);

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of wifi event to get goodput and average send times.
 * CNcommment:wifi获取吞吐量、平均发送次数事件回调接口定义.CNend
 */
typedef unsigned int (*uapi_wifi_report_tx_params_callback)(ext_wifi_report_tx_params*);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:设置算法抗干扰函数接口定义.CNend
 */
td_u32 uapi_wifi_enable_intrf_mode(const td_char *ifname, td_u8 enable, td_u16 flag);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:获取发送参数函数接口定义.CNend
 */
td_s32 uapi_wifi_get_tx_params(const td_char *mac_addr, td_u8 mac_len, td_u32 *tx_best_rate);

/**
 * @ingroup soc_wifi_basic
 *
 * function definition of wifi event.CNcommment:设置低功耗函数接口定义.CNend
 */
td_s32 uapi_set_low_current_boot_mode(td_u16 flag);

/**
* @ingroup   soc_wifi_basic
* @brief  Set wow switch.CNcomment:设置wow特性开关.CNend
*
* @par Description:
*           Set wow switch.CNcomment:设置wow 特性开关.CNend
*
* @attention  1.Should be called before uapi_wifi_init().
*             CNcomment:只能在uapi_wifi_init()函数前调用.CNend
* @param  enable        [IN]     Type #unsigned char,0:disable. 1:enable. Others:invalid
*
* @retval #EXT_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_wow_switch(uint8_t enable);

/**
* @ingroup   soc_wifi_basic
* @brief  Set wow switch.CNcomment:配置wow特定唤醒报文格式.CNend
*
* @par Description:
*           Set wow pattern.CNcomment:配置wow特性唤醒报文格式, 支持添加/删除/清除操作 .CNend
*
* @attention  1.Should be called before uapi_wifi_init().
*             CNcomment:只能在uapi_wifi_init()函数前调用.CNend
* @param  type        [IN]     Type #unsigned char, 0:add. 1:del. 2:clr Others:invalid
*
*         index       [IN]     Type #unsigned char, range [0-3]. Others:invalid
*
*         pattern     [IN]     Type #unsigned char *, 自定义唤醒报文格式, HEX格式字符串
*
* @retval #EXT_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_wow_pattern(unsigned char type, unsigned char index, unsigned char *pattern);

/**
* @ingroup  soc_wifi_basic
* @brief  Wifi initialize.CNcomment:wifi初始化.CNend
*
* @par Description:
        Wifi driver initialize.CNcomment:wifi驱动初始化，不创建wifi设备.CNend
*
* @attention  NULL
* @param  vap_res_num   [IN]  Type #const unsigned char, vap num[rang: 1-3].CNcomment:vap资源个数，取值[1-3].CNend
* @param  user_res_num  [IN]  Type #const unsigned char, user resource num[1-7].
*           CNcomment:用户资源个数，多vap时共享，取值[1-7].CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other    Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_init(const td_u8 vap_res_num, const td_u8 user_res_num);

/**
* @ingroup  soc_wifi_basic
* @brief  Wifi de-initialize.CNcomment:wifi去初始化.CNend
*
* @par Description:
*           Wifi driver de-initialize.CNcomment:wifi驱动去初始化.CNend
*
* @attention  NULL
* @param  NULL
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other    Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_deinit(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Get wifi initialize status.CNcomment:获取wifi初始化状态.CNend
*
* @par Description:
        Get wifi initialize status.CNcomment:获取wifi初始化状态.CNend
*
* @attention  NULL
* @param  NULL
*
* @retval #1  Wifi is initialized.CNcoment:Wifi已经初始化.CNend
* @retval #0  Wifi is not initialized.CNcoment:Wifi没有初始化.CNend
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
unsigned char uapi_wifi_get_init_status(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Config sae pwe settings of sta.CNcomment:配置station的sae pwe.CNend
*
* @par Description:
*           Config sae pwe settings of sta, set before sta start.CNcomment:配置station的sae pwe, 在sta start之前调用.CNend
*
* @attention  Default sae pwe enum value 2. CNcomment:默认sae pwe枚举值2.CNend
* @param  pwe           [IN]     Type #wifi_sae_pwe_option_enum, sae pwe enum value.CNcoment:sae pwe枚举值.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int wifi_sta_set_sae_pwe(wifi_sae_pwe_option_enum pwe);

/**
* @ingroup  soc_wifi_basic
* @brief  Config sae pwe settings of softap.CNcomment:配置softap的sae pwe.CNend
*
* @par Description:
*           Config sae pwe settings of softap, set before softap start.
*           CNcomment:配置softap的sae pwe, 在softap start之前调用.CNend
*
* @attention  Default sae pwe enum value 2. CNcomment:默认sae pwe枚举值2.CNend
* @param  pwe           [IN]     Type #wifi_sae_pwe_option_enum, sae pwe enum value.CNcoment:sae pwe枚举值.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int wifi_softap_set_sae_pwe(wifi_sae_pwe_option_enum pwe);

/**
* @ingroup  soc_wifi_basic
* @brief  Start wifi station.CNcomment:开启STA.CNend
*
* @par Description:
*           Start wifi station.CNcomment:开启STA.CNend
*
* @attention  1. Multiple interfaces of the same type are not supported.CNcomment:1. 不支持使用多个同类型接口.CNend\n
*             2. Dual interface coexistence support: STA + AP or STA + MESH.
*                CNcomment:2. 双接口共存支持：STA + AP or STA + MESH.CNend\n
*             3. The memories of <ifname> and <len> should be requested by the caller，
*                the input value of len must be the same as the length of ifname
*                (The minimum length is 6 bytes and the recommended length is 17 bytes).\n
*                CNcomment:3. <ifname>和<len>由调用者申请内存，用户写入len的值必须与ifname长度一致。
*                (最小长度是6字节，建议长度为17Bytes).CNend\n
*             4. This is a blocking function.CNcomment:4．此函数为阻塞函数.CNend
* @param  ifname          [IN/OUT]     Type #char *, device name.CNcomment:接口名.CNend
* @param  len             [IN/OUT]     Type #int *, length of device name.CNcomment:接口名长度.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_start(char *ifname, int *len);

/**
* @ingroup  soc_wifi_basic
* @brief  Close wifi station.CNcomment:关闭STA.CNend
*
* @par Description:
*           Close wifi station.CNcomment:关闭STA.CNend
*
* @attention  1. This is a blocking function.CNcomment:1．此函数为阻塞函数.CNend
* @param  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_stop(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Start sta basic scanning in all channels.CNcomment:station进行全信道基础扫描.CNend
*
* @par Description:
*           Start sta basic scanning in all channels.CNcomment:启动station全信道基础扫描.CNend
*
* @attention  NULL
* @param     NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_scan(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Start station scanning with specified parameter.CNcomment:station执行带特定参数的扫描.CNend
*
* @par Description:
*           Start station scanning with specified parameter.CNcomment:station执行带特定参数的扫描.CNend
*
* @attention  1. advance scan can scan with ssid only,channel only,bssid only,prefix_ssid only，
*                and the combination parameters scanning does not support.\n
*             CNcomment:1. 高级扫描分别单独支持 ssid扫描，信道扫描，bssid扫描，ssid前缀扫描, 不支持组合参数扫描方式.CNend\n
*             2. Scanning mode, subject to the type set by scan_type.
*              CNcomment:2 .扫描方式，以scan_type传入的类型为准。CNend \n
*             3. SSID only supports ASCII characters.
*                CNcomment:3. SSID 只支持ASCII字符.CNend
* @param  sp            [IN]    Type #ext_wifi_scan_params * parameters of scan.CNcomment:扫描网络参数设置.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_advance_scan(ext_wifi_scan_params *sp);

/**
* @ingroup  soc_wifi_basic
* @brief  sta start scan.CNcomment:station进行扫描.CNend
*
* @par Description:
*           Get station scan result.CNcomment:获取station扫描结果.CNend
* @attention  1. The memories of <ap_list> and <ap_num> memories are requested by the caller. \n
*             The <ap_list> size up to : OAL_SIZEOF(ext_wifi_ap_info ap_list) * 32. \n
*             CNcomment:1. <ap_list>和<ap_num>由调用者申请内存,
*             <ap_list>size最大为：OAL_SIZEOF(ext_wifi_ap_info ap_list) * 32.CNend \n
*             2. ap_num: parameters can be passed in to specify the number of scanned results.The maximum is 32. \n
*             CNcomment:2. ap_num: 可以传入参数，指定获取扫描到的结果数量，最大为32。CNend \n
*             3. If the user callback function is used, ap num refers to bss_num in event_wifi_scan_done. \n
*             CNcomment:3. 如果使用上报用户的回调函数，ap_num参考event_wifi_scan_done中的bss_num。CNend \n
*             4. ap_num should be same with number of ext_wifi_ap_info structures applied,
*                Otherwise, it will cause memory overflow. \n
*             CNcomment:4. ap_num和申请的ext_wifi_ap_info结构体数量一致，否则可能造成内存溢出。CNend \n
*             5. SSID only supports ASCII characters. \n
*             CNcomment:5. SSID 只支持ASCII字符.CNend \n
*             6. The rssi in the scan results needs to be divided by 100 to get the actual rssi.\n
*             CNcomment:6. 扫描结果中的rssi需要除以100才能获得实际的rssi.CNend
* @param  ap_list         [IN/OUT]    Type #ext_wifi_ap_info * scan result.CNcomment:扫描的结果.CNend
* @param  ap_num          [IN/OUT]    Type #unsigned int *, number of scan result.CNcomment:扫描到的网络数目.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_scan_results(ext_wifi_ap_info *ap_list, unsigned int *ap_num);

/**
* @ingroup  soc_wifi_basic
* @brief  Clear sta scan result.CNcomment:清除sta扫描链表.CNend
*
* @par Description:
*           Clear sta scan result.CNcomment:清除sta扫描链表.CNend
*
* @attention  1. This is a blocking function.CNcomment:1．此函数为阻塞函数.CNend
* @param  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_scan_results_clear(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Clear sta scan result.CNcomment:发送bss query报文.CNend
*
* @par Description:
*           Clear sta scan result.CNcomment:发送bss query报文.CNend
*
* @attention  1. This is a blocking function.CNcomment:1．此函数为阻塞函数.CNend
* @param  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_wnm_bss_query(int reason_code, int candidate_list);

/**
* @ingroup  soc_wifi_basic
* @brief  sta start connect.CNcomment:station进行连接网络.CNend
*
* @par Description:
*           sta start connect.CNcomment:station进行连接网络.CNend
*
* @attention  1.<ssid> and <bssid> cannot be empty at the same time. CNcomment:1. <ssid>与<bssid>不能同时为空.CNend\n
*             2. When <auth_type> is set to OPEN/EXT_WIFI_SECURITY_WPA2/EXT_WIFI_SECURITY_WPA3/
*                EXT_WIFI_SECURITY_WPA3_WPA2_MIX, the <passwd> parameter is not required.
*                CNcomment:2. <auth_type>设置为OPEN/EXT_WIFI_SECURITY_WPA2/EXT_WIFI_SECURITY_WPA3/
*                EXT_WIFI_SECURITY_WPA3_WPA2_MIX,时，无需<passwd>参数.CNend\n
*             3. This function is non-blocking.CNcomment:3. 此函数为非阻塞式.CNend\n
*             4. Pairwise can be set, default is 0.CNcomment:4. pairwise 可设置, 默认为0.CNend\n
*             5. If the station is already connected to a network, disconnect the existing connection and
*                then connect to the new network.\n
*                CNcomment:5. 若station已接入某个网络，则先断开已有连接，然后连接新网络.CNend\n
*             6. If the wrong SSID, BSSID or key is passed in, the EXT_WIFI_OK will be returned,
*                but sta cannot connect the ap.
*                CNcomment:6. 如果传入错误的ssid，bssid或者不正确的密码，返回成功，但连接ap失败。CNend\n
*             7. SSID only supports ASCII characters.
*                CNcomment:7. SSID 只支持ASCII字符.CNend \n
*             8. Only support auth mode as bellow:
*                 EXT_WIFI_SECURITY_SAE,
*                 EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX,
*                 EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX,
*                 EXT_WIFI_SECURITY_WPA2PSK,
*                 EXT_WIFI_SECURITY_WPA2,
*                 EXT_WIFI_SECURITY_WPA3,
*                 EXT_WIFI_SECURITY_WPA3_WPA2_MIX,
*                 EXT_WIFI_SECURITY_WAPI_PSK,
*                 EXT_WIFI_SECURITY_WEP,
*                 EXT_WIFI_SECURITY_OPEN
*                CNcomment:8. 只支持以下认证模式：
*                 EXT_WIFI_SECURITY_SAE,
*                 EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX,
*                 EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX,
*                 EXT_WIFI_SECURITY_WPA2PSK,
*                 EXT_WIFI_SECURITY_WPA2,
*                 EXT_WIFI_SECURITY_WPA3,
*                 EXT_WIFI_SECURITY_WPA3_WPA2_MIX,
*                 EXT_WIFI_SECURITY_WAPI_PSK,
*                 EXT_WIFI_SECURITY_WEP,
*                 EXT_WIFI_SECURITY_OPEN \n
*             9. WEP supports 64 bit and 128 bit encryption.
*                for 64 bit encryption, the encryption key is 10 hexadecimal characters or 5 ASCII characters;
*                for 128 bit encryption, the encryption key is 26 hexadecimal characters or 13 ASCII characters。\n
*                CNcomment:9. WEP支持64位和128位加密，对于64位加密，加密密钥为10个十六进制字符或5个ASCII字符；
*                          对于128位加密，加密密钥为26个十六进制字符或13个ASCII字符。CNend\n
*            10. When the key of WEP is in the form of ASCII character,
*                the key in the input struct needs to be added with double quotation marks;
*                when the key of WEP is in the form of hexadecimal character,
*                the key in the input struct does not need to add double quotation marks.\n
*                CNcomment:10. WEP的秘钥为ASCIl字符形式时，入参结构体中的key需要添加双引号；
*                          WEP的秘钥为为十六进制字符时，入参结构体中的key不需要添加双引号。CNend\n
*            11. If the auth type is EXT_WIFI_SECURITY_SAE, PMF is temporarily enabled as required.\n
*                CNcomment:11. 当使用EXT_WIFI_SECURITY_SAE进行认证时，默认临时开启PMF（强制）。CNend\n
*            12. If the auth type is EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX, PMF is temporarily enabled as optional.\n
*                CNcomment:12. 当使用EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX进行认证时，默认临时开启PMF（可选）。CNend\n
*            13. If the auth type is EXT_WIFI_SECURITY_WPA3, PMF is temporarily enabled as required.\n
*                CNcomment:13. 当使用EXT_WIFI_SECURITY_WPA3进行认证时，默认临时开启PMF（强制）。CNend\n
*            14. If the auth type is EXT_WIFI_SECURITY_WPA3_WPA2_MIX, PMF is temporarily enabled as optional.\n
*                CNcomment:14. 当使用EXT_WIFI_SECURITY_WPA3_WPA2_MIX进行认证时，默认临时开启PMF（可选）。CNend\n
*
* @param  req    [IN]    Type #const ext_wifi_assoc_request * connect parameters of network.CNcomment:连接网络参数设置.CNend
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_connect(const ext_wifi_assoc_request *req);

/**
* @ingroup  soc_wifi_basic
* @brief  Start fast connect.CNcomment:station进行快速连接网络.CNend
*
* @par Description:
*           Start fast connect.CNcomment:station进行快速连接网络.CNend
*
* @attention  1. <ssid> and <bssid> cannot be empty at the same time. CNcomment:1．<ssid>与<bssid>不能同时为空.CNend\n
*             2. When <auth_type> is set to OPEN, the <passwd> parameter is not required.
*                CNcomment:2．<auth_type>设置为OPEN时，无需<passwd>参数.CNend\n
*             3. <chn> There are differences in the range of values, and China is 1-13.
*                CNcomment:3．<chn>取值范围不同区域有差异，中国为1-13.CNend\n
*             4. This is a blocking function.CNcomment:4．此函数为阻塞函数.CNend\n
*             5. Pairwise can be set, set to zero by default.CNcomment:5. pairwise 可设置,默认置零.CNend\n
*             6. <psk> and <psk_flag> are optional parameters, set to zero by default. \n
*                CNcomment:6. <psk>和<psk_flag>为可选参数，无需使用时填0.CNend\n
*             7. If the wrong SSID, BSSID or key is passed in, the EXT_WIFI_FAIL will be returned,
*                and sta cannot connect the ap.
*                CNcomment:7. 如果传入错误的ssid，bssid或者不正确的密码，返回失败并且连接ap失败。CNend\n
*             8. SSID only supports ASCII characters.
*                CNcomment:8. SSID 只支持ASCII字符.CNend \n
*             9. The PSK internal cache calculated in advance will be cleared after the function execution is completed.
*                CNcomment:9. 提前计算的PSK内部缓存将在函数执行完成后清除.CNend \n
*             10. The PSK calculated in advance do not support WPA3-PSK.
*                CNcomment:10. PSK提前计算不支持WPA3-PSK.CNend
*             11. If the auth type is EXT_WIFI_SECURITY_SAE, PMF is temporarily enabled as required.\n
*                CNcomment:11. 当使用EXT_WIFI_SECURITY_SAE进行认证时，默认临时开启PMF（强制）。CNend\n
*             12. If the auth type is EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX, PMF is temporarily enabled as optional.\n
*                CNcomment:12. 当使用EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX进行认证时，默认临时开启PMF（可选）。CNend\n
*
* @param fast_request [IN] Type #const ext_wifi_fast_assoc_request *, fast connect parameters. CNcomment:快速连接网络参数.CNend

* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_fast_connect(const ext_wifi_fast_assoc_request *fast_request);

/**
* @ingroup  soc_wifi_basic
* @brief  Disconnect from network.CNcomment:station断开相连的网络.CNend
*
* @par Description:
*           Disconnect from network.CNcomment:station断开相连的网络.CNend
*
* @attention  NULL
* @param  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_disconnect(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Set reconnect policy.CNcomment:station设置重新连接网络机制.CNend
*
* @par Description:
*           Set reconnect policy.CNcomment:station设置重新连接网络机制.CNend
*
* @attention  1. It is recommended called after STA start or connected successfully.
*             CNcomment:1. 在STA启动后或者关联成功后调用该接口.CNend\n
*             2. The reconnection policy will be triggered when the station is disconnected from ap.\n
*             CNcomment:2. 重连机制将于station下一次去关联时生效,当前已经去关联设置无效.CNend\n
*             3. The Settings will take effect on the next reconnect timer.\n
*             CNcomment:3. 重关联过程中更新重关联配置将于下一次重连计时生效.CNend\n
*             4. After calling station connect/disconnect or station stop, stop reconnecting.
*             CNcomment:4. 调用station connect/disconnect或station stop，停止重连.CNend\n
*             5. If the target network cannot be found by scanning,
                 the reconnection policy cannot trigger to take effect.\n
*             CNcomment:5. 若扫描不到目标网络，重连机制无法触发生效.CNend\n
*             6. When the <seconds> value is 65535, it means infinite loop reconnection.
*             CNcomment:6. <seconds>取值为65535时，表示无限次循环重连.CNend\n
*             7.Enable reconnect, user and lwip will not receive disconnect event when disconnected from ap until 15
*               seconds later and still don't reconnect to ap successfully.
*             CNcomment:7. 使能自动重连,wifi将在15s内尝试自动重连并在此期间不上报去关联事件到用户和lwip协议栈,
*                          做到15秒内重连成功用户和上层网络不感知.CNend\n
*             8.Must call again if add/down/delete SoftAp or MESH's interface status after last call.
*             CNcomment:8. 调用后如果添加/禁用/删除了SoftAp,MESH接口的状态,需要再次调用该接口.CNend\n

* @param  enable        [IN]    Type #int enable reconnect.0-disable/1-enable.CNcomment:使能重连网络参数.CNend
* @param  seconds       [IN]    Type #unsigned int reconnect timeout in seconds for once, range:[2-65535].
*                                                  CNcomment:单次重连超时时间，取值[2-65535].CNend
* @param  period        [IN]    Type #unsigned int reconnect period in seconds, range:[1-65535].
                                                   CNcomment:重连间隔周期，取值[1-65535].CNend
* @param  max_try_count [IN]    Type #unsigned int max reconnect try count number，range:[1-65535].
                                                   CNcomment:最大重连次数，取值[1-65535].CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_set_reconnect_policy(int enable, unsigned int seconds,
    unsigned int period, unsigned int max_try_count);

/**
* @ingroup  soc_wifi_basic
* @brief    STA reconnect without deleting the network configuration, only used for WFA tests.
            CNcomment:station模式下重新发起关联但不删除network配置信息,仅WFA测试使用.CNend
*
* @par Description:
*           STA reconnect without deleting the network configuration, only used for WFA tests.
            CNcomment:station模式下重新发起关联但不删除network配置信息,仅WFA测试使用.CNend
*
* @attention  1. It is recommended called after STA connected successfully, Otherwise, it is invalid.
*             CNcomment:1. 建议在STA关联成功后调用该接口,否则无效CNend\n

* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_reconnect_for_wfa_test(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Get status of sta.CNcomment:获取station连接的网络状态.CNend
*
* @par Description:
*           Get status of sta.CNcomment:获取station连接的网络状态.CNend
*
* @attention  NULL
* @param  connect_status  [IN/OUT]    Type #ext_wifi_status *, connect status， memory is requested by the caller.
*                                                             CNcomment:连接状态, 由调用者申请内存.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_get_connect_info(ext_wifi_status *connect_status);

/**
* @ingroup  soc_wifi_basic
* @brief  Start pbc connect in WPS.CNcomment:设置WPS进行pbc连接.CNend
*
* @par Description:
*           Start pbc connect in WPS.CNcomment:设置WPS进行pbc连接.CNend
*
* @attention  1. bssid can be NULL or MAC. CNcomment:1. bssid 可以指定mac或者填NULL.CNend
*             2. Bssid length must be 6 bytes.CNcomment:地址长度必须为6字节。CNend \n;
*
* @param  bssid     [IN] Type #const unsigned char * mac address
* @param  bssid_len [IN] Type #unsigned int the length of mac address
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_wps_pbc(const unsigned char *bssid, unsigned int bssid_len);

/**
* @ingroup  soc_wifi_basic
* @brief  Start pin connect in WPS.CNcomment:WPS通过pin码连接网络.CNend
*
* @par Description:
*           Start pin connect in WPS.CNcomment:WPS通过pin码连接网络.CNend
*
* @attention  1. Bssid can be NULL or MAC. CNcomment:1. bssid 可以指定mac或者填NULL.CNend \n
*             2. Decimal only WPS pin code length is 8 Bytes.CNcomment:2. WPS中pin码仅限十进制，长度为8 Bytes.CNend
*             3. Bssid length must be 6 bytes.CNcomment:地址长度必须为6字节。CNend \n;
*
* @param  pin       [IN]   Type #const char * pin code
* @param  bssid     [IN]   Type #const unsigned char * mac address
* @param  bssid_len [IN]   Type #unsinged int the length of mac address
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_wps_pin(const char *pin, unsigned int pin_len, const unsigned char *bssid, unsigned int bssid_len);

/**
* @ingroup  soc_wifi_basic
* @brief  Get pin code.CNcomment:WPS获取pin码.CNend
*
* @par Description:
*           Get pin code.CNcomment:WPS获取pin码.CNend
*
* @attention  Decimal only WPS pin code length is 8 Bytes.CNcomment:WPS中pin码仅限十进制，长度为8 Bytes.CNend
* @param  pin    [IN/OUT]   Type #char *, pin code buffer, should be obtained, length is 9 Bytes.
*                                                               The memory is requested by the caller.\n
*                                       CNcomment:待获取pin码,长度为9 Bytes。由调用者申请内存.CNend
* @param  len    [IN]       Type #unsigned int, length of pin code。CNcomment:pin码的长度.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_wps_pin_get(char *pin, unsigned int len);

/**
* @ingroup  soc_wifi_basic
* @brief  Get rssi value.CNcomment:获取rssi值.CNend
*
* @par Description:
*           Get current rssi of ap which sta connected to.CNcomment:获取sta当前关联的ap的rssi值.CNend
*
* @attention  NULL
* @param  NULL
*
* @retval #0x7F          Invalid value.
* @retval #Other         rssi
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_get_ap_rssi(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Get snr value.CNcomment:获取snr值.CNend
*
* @par Description:
*           Get current snr of ap which sta connected to.CNcomment:获取sta当前关联的ap的snr值.CNend
*
* @attention  NULL
* @param  NULL
*
* @retval #Other         snr
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_get_ap_snr(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Get info of sta which ap connected to.CNcomment:获取ap连接的sta信息.CNend
*
* @par Description:
*           Get current rssi and rate of ap sta which ap connected to.CNcomment:获取ap当前关联的sta的rssi和rate值.CNend
*
* @attention  NULL
* @param  mac_addr  [IN]    Type #char, mac address of connected sta. CNcomment:ap连接的sta的mac地址.CNend
* @param  rssi      [OUT]   Type #signed char, signal strength of last packet. CNcomment: ap上一次接收相连的station报文的rssi.CNend
* @param  best_rate  [OUT]   Type #unsigned int, best sending rate. CNcomment: ap与相连的station报文的最佳发送速率.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_ap_get_sta_info(td_char *mac_addr, td_s8 *rssi, td_u32 *best_rate);

/**
* @ingroup  soc_wifi_basic
* @brief  Set sta powersave configuration.CNcomment:设置STA的低功耗参数.CNend
*
* @par Description:
*           Set sta powersave configuration.CNcomment:设置STA的低功耗参数.CNend
*
* @attention  CNcomment:1.参数值越小,功耗越低但性能表现和抗干扰会越差,建议使用默认值或根据流量动态配置.CNend\n
*             CNcomment:2.所有参数配置0,表示该参数使用默认值.CNend\n
*             CNcomment:3.仅支持设置STA的低功耗参数.CNend\n
*             CNcomment:4.需要在关联成功后配置,支持动态配置.CNend\n
*             CNcomment:5.定时器首次启动不计数,故实际睡眠时间为配置的重启次数+1乘以周期.CNend\n
* @param  timer       [IN]  Type  #unsigned char CNcomment:低功耗定时器周期,默认50ms,取值[0-100]ms.CNend
* @param  time_cnt    [IN]  Type  #unsigned char CNcomment:低功耗定时器重启次数,达到该次数后wifi无数据收发则进入休眠,
                                                 默认为4,取值[0-10].CNend
* @param  bcn_timeout [IN]  Type  #unsigned char CNcomment:等待接收beacon的超时时间,默认10ms,取值[0-100]ms.CNend
* @param  mcast_timeout [IN]  Type  #unsigned char CNcomment:等待接收组播/广播帧的超时时间,默认30ms,取值[0-100]ms.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_sta_set_pm_param(td_u8 pm_timeout, td_u8 pm_timer_cnt, td_u8 bcn_timeout,
    td_u8 mcast_timeout, td_u16 sleep_time);

/**
* @ingroup  soc_wifi_basic
* @brief  WPA PSK Calculate.CNcomment:计算WPA PSK.CNend
*
* @par Description:
*           PSK Calculate.CNcomment:计算psk.CNend
*
* @attention  1. support only WPA/WPA2 PSK. CNcomment:1. 只支持WPA/WPA2 PSK计算.CNend \n
*             2. SSID only supports ASCII characters. CNcomment:2. SSID 只支持ASCII字符.CNend
* @param  psk_config    [IN]     Type #ext_wifi_sta_psk_config
* @param  get_psk       [IN/OUT] Type #unsigned char *，Psk to be obtained, length is 32 Bytes.
*                                                               The memory is requested by the caller.
*                                       CNcomment:待获取psk,长度为32 Bytes。由调用者申请内存.CNend
* @param  psk_len       [IN]    Type #unsigned int
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_psk_calc(ext_wifi_sta_psk_config psk_config, unsigned char *get_psk, unsigned int psk_len);

/**
* @ingroup  soc_wifi_basic
* @brief  WPA PSK Calculate，then keep it inside .CNcomment:计算WPA PSK, 并做内部保存.CNend
*
* @par Description:
*           psk Calculate.CNcomment:计算psk.CNend
*
* @attention  1. support only WPA/WPA2 PSK. CNcomment:1. 只支持WPA/WPA2 PSK计算.CNend \n
*             2. SSID only supports ASCII characters. CNcomment:2. SSID 只支持ASCII字符.CNend \n
*             3. The cached PSK will be cleared after the first quick connection. \n
*                CNcomment:3. PSK缓存将在第一次执行快速连接后清除.CNend
* @param  psk_config    [IN]    Type #ext_wifi_sta_psk_config
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_psk_calc_and_store(ext_wifi_sta_psk_config psk_config);

/**
* @ingroup  soc_wifi_basic
* @brief  config calling mode of user's callback interface.CNcomment:配置用户回调接口的调用方式.CNend
*
* @par Description:
*           config calling mode of user's callback interface.CNcomment:配置用户回调接口的调用方式.CNend
*
* @attention  1. Wpa's task has high priority and call wifi's api directly may be cause error.
              CNcomment:1. wpa线程优先级高,直接调用方式下在该回调接口内再次调用某些api会导致线程卡死.CNend
              2. If you have create a task in your app, you should use mode:0, or mode:1 is adervised.
              CNcomment:2. 上层应用已创建task来处理事件回调建议使用直接调用方式,否则建议使用线程调用方式.CNend
              3. Configuration will keep till system reboot, set again when you start a new station.
              CNcomment:3. 参数会保持上一次设置值直到系统重启,重新启动station后建议再配置一次.CNend
              4. Configuration will work immediately whenever you set.
              CNcomment:4. 可随时配置该参数,配置成功即生效.CNend
* @param  mode       [IN]    Type #unsigned char , call mode, 1:direct and 0:create task[default].
                             CNcomment:回调调用方式,1:wpa线程直接调用,0:新建一个低优先级线程调用,默认.CNend
* @param  task_prio  [IN]    Type #unsigned char , task priority, range(10-50) .
                             CNcomment:新建线程优先级,取值范围(10-50).CNend
* @param  stack_size [IN]    Type #unsigned short , task stack size, more than 1K bytes, default: 2k.
                             CNcomment:新建线程栈空间,需大于宏LOS_TASK_MIN_STACK_SIZE的大小,默认2k.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_config_callback(unsigned char mode, unsigned char task_prio, unsigned short stack_size);

/**
* @ingroup  soc_wifi_basic
* @brief  register user callback interface.CNcomment:注册回调函数接口.CNend
*
* @par Description:
*           register user callback interface.CNcomment:注册回调函数接口.CNend
*
* @attention  NULL
* @param  event_cb  [OUT]    Type #uapi_wifi_event_cb, event callback .CNcomment:回调函数.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_register_event_callback(uapi_wifi_event_cb event_cb);

/**
* @ingroup  soc_wifi_basic
* @brief  Set softap's beacon interval.CNcomment:设置softap的beacon周期.CNend
*
* @par Description:
*           Set softap's beacon interval.CNcomment:设置softap的beacon周期.CNend \n
*           Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend
*
* @attention  NULL
* @param  beacon_period      [IN]     Type  #int beacon period in milliseconds, range(33ms~1000ms), default(100ms)
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_set_beacon_period(int beacon_period);

/**
* @ingroup  soc_wifi_basic
* @brief  Set softap's dtim count.CNcomment:设置softap的dtim周期.CNend
*
* @par Description:
*           Set softap's dtim count.CNcomment:设置softap的dtim周期.CNend \n
*           Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend
*
* @attention  NULL
* @param  dtim_period     [IN]     Type  #int, dtim period , range(1~30), default(2)
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_set_dtim_period(int dtim_period);

/**
* @ingroup  soc_wifi_basic
* @brief  Set update time of softap's group key.CNcomment:配置softap组播秘钥更新时间.CNend
*
* @par Description:
*           Set update time of softap's group key.CNcomment:配置softap组播秘钥更新时间.CNend\n
*           Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend\n
*
* @param  wifi_group_rekey [IN]     Type  #int, update time in seconds, range(30s-86400s), default(86400s)
*                                   CNcomment:更新时间以秒为单位，范围（30s-86400s）,默认（86400s）.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_set_group_rekey(int wifi_group_rekey);

/**
* @ingroup  soc_wifi_basic
* @brief  Set short-gi of softap.CNcomment:设置softap的SHORT-GI功能.CNend
*
* @par Description:
*           Enable or disable short-gi of softap.CNcomment:开启或则关闭softap的SHORT-GI功能.CNend\n
*           Initialized config sets before interface starts.CNcomment:初始配置softap启动之前调用.CNend
* @attention  NULL
* @param  flag            [IN]    Type  #int, enable(1) or disable(0). default enable(1).
                                        CNcomment:使能标志，默认使能（1）.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_set_shortgi(int flag);

/**
* @ingroup  soc_wifi_basic
* @brief  Start softap interface.CNcomment:开启SoftAP.CNend
*
* @par Description:
*           Start softap interface.CNcomment:开启SoftAP.CNend
*
* @attention  1. Multiple interfaces of the same type are not supported.CNcomment:不支持使用多个同类型接口.CNend\n
*             2. Dual interface coexistence support: STA + AP. CNcomment:双接口共存支持：STA + AP.CNend \n
*             3. Start timeout 5s.CNcomment:启动超时时间5s。CNend \n
*             4. Softap key length range(8 Bytes - 64 Bytes).CNcomment:softap key长度范围（8 Bytes - 64 Bytes）.CNend \n
*             5. Only support auth mode as bellow: \n
*                 EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX, \n
*                 EXT_WIFI_SECURITY_WPA2PSK, \n
*                 EXT_WIFI_SECURITY_OPEN, \n
*                 EXT_WIFI_SECURITY_SAE, \n
*                 EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX \n
*                CNcomment:5. 只支持以下认证模式：\n
*                 EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX, \n
*                 EXT_WIFI_SECURITY_WPA2PSK, \n
*                 EXT_WIFI_SECURITY_OPEN.CNend \n
*                 EXT_WIFI_SECURITY_SAE.CNend \n
*                 EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX.CNend \n
*             6. The memories of <ifname> and <len> should be requested by the caller，
*                the input value of len must be the same as the length of ifname
*                (The minimum length is 4 bytes and the recommended length is 17 bytes).\n
*                CNcomment:3. <ifname>和<len>由调用者申请内存，用户写入len的值必须与ifname长度一致。
*                (最小长度是4字节，建议长度为17Bytes).CNend\n
*             7. SSID only supports ASCII characters.
*                CNcomment:7. SSID 只支持ASCII字符.CNend \n
*             8. This is a blocking function.CNcomment:8．此函数为阻塞函数.CNend \n
*             9. In WPA2 / WPA3 mode, the encryption method is CCMP.CNcomment:9．WPA2/WPA3混合模式下加密方式为CCMP.CNend\n
*             10. The PMF of WPA3 is REQUIRE, and the PMF of WPA2/WPA3 is OPTIONAL.
*                 CNcomment:10．WPA3的PMF为强制，WPA2/WPA3的PMF为可选.CNend \n
*             11. The PMKID connection is supported. The PMKID is stored in the memory for 12 hours.
*                 CNcomment:11．支持PMKID连接，PMKID保存时间为12小时CNend
* @param  conf            [IN]      Type  #ext_wifi_softap_config *, softap's configuration.CNcomment:SoftAP配置.CNend
* @param  ifname          [IN/OUT]  Type  #char *, interface name.CNcomment:接口名字.CNend
* @param  len             [IN/OUT]  Type  #int *, interface name length.CNcomment:接口名字长度.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_start(ext_wifi_softap_config *conf, char *ifname, int *len);

/**
* @ingroup  soc_wifi_basic
* @brief  Close softap interface.CNcomment:关闭SoftAP.CNend
*
* @par Description:
*           Close softap interface.CNcomment:关闭SoftAP.CNend
*
* @attention  1. This is a blocking function.CNcomment:1．此函数为阻塞函数.CNend
* @param  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_stop(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Get all user's information of softap.CNcomment:softap获取已连接的station的信息.CNend
*
* @par Description:
*           Get all user's information of softap.CNcomment:softap获取已连接的station的信息.CNend
*
* @attention  1.sta_list: malloc by user.CNcomment:1.扫描结果参数。由用户动态申请。CNend \n
*             2.sta_list: max size is ext_wifi_ap_sta_info * 6.
*               CNcomment:2.sta_list 足够的结构体大小，最大为ext_wifi_ap_sta_info * 6。CNend \n
*             3.sta_num:parameters can be passed in to specify the number of connected sta.The maximum is 6.
*               CNcomment:3.可以传入参数，指定获取已接入的sta个数，最大为6。CNend \n
*             4.sta_num should be the same with number of ext_wifi_ap_sta_info structures applied, Otherwise,
*               it will cause memory overflow.\n
*               CNcomment:4.sta_num和申请的ext_wifi_ap_sta_info结构体数量一致，否则可能造成内存溢出。CNend \n
*             5.Only the sta that are successfully associated are displayed.
*               CNcomment:5.只显示关联成功的sta。CNend
* @param  sta_list        [IN/OUT]  Type  #ext_wifi_ap_sta_info *, station information.CNcomment:STA信息.CNend
* @param  sta_num         [IN/OUT]  Type  #unsigned int *, station number.CNcomment:STA个数.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_get_connected_sta(ext_wifi_ap_sta_info *sta_list, unsigned int *sta_num);

/**
* @ingroup  soc_wifi_basic
* @brief  Softap deauth user by mac address.CNcomment:softap指定断开连接的station网络.CNend
*
* @par Description:
*          Softap deauth user by mac address.CNcomment:softap指定断开连接的station网络.CNend
*
* @attention  Address length must be 6 bytes.CNcomment:地址长度必须为6字节。CNend \n;
*
* @param  addr             [IN]     Type  #const unsigned char *, station mac address.CNcomment:MAC地址.CNend
* @param  addr_len         [IN]     Type  #unsigned char, station mac address length, must be 6.
*                                         CNcomment:MAC地址长度,必须为6.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_deauth_sta(const unsigned char *addr, unsigned char addr_len);

/**
* @ingroup  soc_wifi_basic
* @brief  Set softap pairwise.CNcomment:设置pairwise.CNend
*
* @par Description:
*          Set softap pairwise, from bit 1 to bit 14.CNcomment:设置pairwise，取值为bit 1到bit 14.CNend
*
* @attention  NULL
*
* @param  pairwise        [IN]     Type  #int, softap pairwise.CNcomment:softap pairwise.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_softap_set_pairwise(int pairwise);

/**
* @ingroup  soc_wifi_basic
* @brief  Set country code.CNcomment:设置国家码.CNend
*
* @par Description:
*           Set country code(two uppercases).CNcomment:设置国家码，由两个大写字符组成.CNend
*
* @attention  1.Before setting the country code, you must call uapi_wifi_init to complete the initialization.
*             CNcomment:设置国家码之前，必须调用uapi_wifi_init初始化完成.CNend\n
*             2.cc_len should be greater than or equal to 3.CNcomment:cc_len应大于等于3.CNend
* @param  cc               [IN]     Type  #const char *, country code.CNcomment:国家码.CNend
* @param  cc_len           [IN]     Type  #unsigned char, country code length.CNcomment:国家码长度.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_set_country(const td_char *cc, td_u8 cc_len);

/**
* @ingroup  soc_wifi_basic
* @brief  Get country code.CNcomment:获取国家码.CNend
*
* @par Description:
*           Get country code.CNcomment:获取国家码，由两个大写字符组成.CNend
*
* @attention  1.Before getting the country code, you must call uapi_wifi_init to complete the initialization.
*             CNcomment:获取国家码之前，必须调用uapi_wifi_init初始化完成.CNend
* @param  cc               [OUT]     Type  #char *, country code.CNcomment:国家码.CNend
* @param  len              [IN/OUT]  Type  #int *, country code length.CNcomment:国家码长度.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_get_country(td_char *cc, td_u8 *len);

/**
* @ingroup  soc_wifi_basic
* @brief  Set bandwidth.CNcomment:设置带宽.CNend
*
* @par Description:
*           Set bandwidth, support 5M/10M/20M.CNcomment:设置接口的工作带宽，支持5M 10M 20M带宽的设置.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
* @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
* @param  bw               [IN]     Type  #ext_wifi_bw, bandwidth enum.CNcomment:带宽.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_set_bandwidth(const td_char *ifname, td_u8 ifname_len, ext_wifi_bw bw);

/**
* @ingroup  soc_wifi_basic
* @brief  Get bandwidth.CNcomment:获取带宽.CNend
*
* @par Description:
*           Get bandwidth.CNcomment:获取带宽.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
* @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
*
* @retval #bandwidth enum.CNcomment:带宽的枚举值.CNend
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
ext_wifi_bw uapi_wifi_get_bandwidth(const td_char *ifname, td_u8 ifname_len);

/**
* @ingroup  soc_wifi_basic
* @brief  Set channel.CNcomment:设置信道.CNend
*
* @par Description:
*           Set channel.CNcomment:设置信道.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
* @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
* @param  channel          [IN]     Type  #int , listen channel.CNcomment:信道号.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_set_channel(const td_char *ifname, td_u8 ifname_len, td_s32 channel);

/**
* @ingroup  soc_wifi_basic
* @brief  Get channel.CNcomment:获取信道.CNend
*
* @par Description:
*           Get channel.CNcomment:获取信道.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
* @param  ifname_len       [IN]     Type  #unsigned char, interface name length.CNcomment:接口名长度.CNend
*
* @retval #EXT_WIFI_INVALID_CHANNEL
* @retval #Other                   chanel value.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_get_channel(const td_char *ifname, td_u8 ifname_len);

/**
* @ingroup  soc_wifi_basic
* @brief  Set scan.param.CNcomment:设置扫描参数.CNend
*
* @par Description:
*           Set scan_param, support the largest scan_cnt is 10, support the largest scan_time is 120,
*           support the largest scan_channel_interval is 6, support the largest work_time_on_home_channel is 120.
*           CNcomment:设置扫描参数，支持最大扫描次数为10，支持最长驻留时间为120,
*           支持最多间隔6信道返回工作信道工作，支持背景扫描时，返回工作信道工作的最长时间为120.CNend
*
* @attention  1.Set scan_param before starting the scan.
*             CNcomment:扫描参数需在扫描之前设置.
*             2.Restart can restore default scan_param.
*             CNcomment:关闭STA/AP命令再启动STA/AP可以恢复默认值.
*             3.Valid default_enable:0B0000~0B1111.
*             WIFI_SCAN_CNT_DEFAULT_ENABLE: enable scan_cnt default.
*             WIFI_SCAN_TIME_DEFAULT_ENABLE: enable scan_time default.
*             WIFI_SCAN_CHANNEL_INTERVAL_DEFAULT_ENABLE: enable scan_channel_interval default.
*             WIFI_WORK_TIME_ON_HOME_CHANNEL_DEFAULT_ENABLE: enable work_time_on_home_channel default.
*             CNcomment:表示使用默认值使能.
*             WIFI_SCAN_CNT_DEFAULT_ENABLE: 表示使能scan_cnt的默认值.
*             WIFI_SCAN_TIME_DEFAULT_ENABLE: 表示使能scan_time的默认值.
*             WIFI_SCAN_CHANNEL_INTERVAL_DEFAULT_ENABLE: 表示使能scan_channel_interval的默认值.
*             WIFI_WORK_TIME_ON_HOME_CHANNEL_DEFAULT_ENABLE: 表示使能work_time_on_home_channel的默认值.
*             4.Valid scan_cnt:0,1~10. Default scan_cnt is 2. 0 indicates default value.
*             CNcomment:有效扫描次数为1~10.默认扫描次数为2，0表示默认值.
*             5.Valid scan_time:0,20~120. Default scan_time is 20. 0 indicates default value.
*             CNcomment:有效驻留时间为20~120.默认扫描驻留时间为20，0表示默认值.
*             6.Valid scan_channel_interval:0,1~6. Default scan_channel_interval is 6.0 indicates default value.
*             CNcomment:设置间隔n个信道返回工作信道工作一段时间，有效范围为1~6，默认为6，0表示默认值.
*             7.Valid work_time_on_home_channel:0,30~120.
*             Default work_time_on_home_channel is 110.0 indicates default value.
*             CNcomment:设置背景扫描时，返回工作信道工作的时间，有效范围为30~120，默认为110，0表示默认值.
*             8.If scan_cnt does not enable the default value, and scan_cnt is equal to 0,
*             it means that scan_cnt uses the most recently set value. Same for other scan parameters.
*             CNcomment:如果scan cnt不使能默认值,同时scan cnt等于0,则表示scan_cnt使用最近设置的值. 其他扫描参数同理.
*             9.If scan_cnt enables the default value, and scan_cnt is equal to 0,
*             it means that scan_cnt uses default value. Same for other scan parameters.
*             CNcomment:如果scan cnt使能默认值,同时scan cnt等于0,则表示scan_cnt使用默认值. 其他扫描参数同理.
*             10.If the value of scan_cnt is set to a non-zero value,regardless of whether the default value is enabled,
*             it is set to a non-zero value. Same for other scan parameters.
*             CNcomment:如果scan_cnt的值设置为非零值，无论是否启用默认值，都设置为非零值. 其他扫描参数同理.
*             11.Total scan time: foreground scan time = scan_cnt * scan_time * scan_channel_num,
*             background scan time = foreground scan time + (scan_channel_num / scan_channel_interval) *
*                                    work_time_on_home_channel
*             CNcomment:扫描时间：前景扫描时间 = 扫描次数 * 扫描驻留时间 * 扫描信道个数，
*             背景扫描时间 = 前景扫描时间 + (扫描信道个数 / n信道间隔) * 工作信道工作时间
*             12.Total scan time must less than 4.5s.
*             CNcomment:扫描时间需要小于4.5s
*
*
* @param  ifname           [IN]     Type  #const char *, interface name.CNcomment:接口名.CNend
* @param  scan_param    [IN]     Type  #ext_wifi_scan_param_stru *scan_param:扫描参数结构体.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
*/
int uapi_wifi_set_scan_param(const td_char *ifname, ext_wifi_scan_param_stru *scan_param);

/**
* @ingroup  soc_wifi_basic
* @brief  Set monitor mode.CNcomment:设置混杂模式.CNend
*
* @par Description:
*           Enable/disable monitor mode of interface.CNcomment:设置指定接口的混杂模式使能.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char * interface name.CNcomment:接口名.CNend
* @param  enable           [IN]     Type  #int enable(1) or disable(0).CNcomment:开启/关闭.CNend
* @param  filter           [IN]     Type  #const wifi_ptype_filter_stru * filtered frame type enum.CNcomment:过滤列表.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
osal_s32 uapi_wifi_promis_enable(const osal_char *ifname, osal_s32 enable, const ext_wifi_ptype_filter_stru *filter);

/**
* @ingroup  soc_wifi_basic
* @brief  Set wifi aware enable.CNcomment:设置wifi aware使能.CNend
*
* @par Description:
*           Enable/disable wifi aware of interface.CNcomment:设置指定接口的wifi aware使能.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char * interface name.CNcomment:接口名.CNend
* @param  enable           [IN]     Type  #int enable(1) or disable(2).CNcomment:开启/关闭.CNend
* @param  ratio            [IN]     Type  #int ratio(1-9) channel switching percentage.CNcomment:信道切换百分比.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sdp_enable(const osal_char *ifname, osal_s32 enable, osal_s32 ratio);

/**
* @ingroup  soc_wifi_basic
* @brief  Set wifi aware subscribe.CNcomment:设置wifi aware订阅.CNend
*
* @par Description:
*           Enable/disable wifi aware subscribe of interface.CNcomment:设置指定接口的wifi aware订阅.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char * interface name.CNcomment:接口名.CNend
* @param  sdp_subscribe    [IN]     Type  #char * Subscription Service Account.CNcomment:订阅服务号.CNend
* @param  local_handle     [IN]     Type  #int local_handle local service number.CNcomment:本机服务号.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sdp_subscribe(const osal_char *ifname, osal_char *sdp_subscribe, osal_s32 local_handle);

/**
* @ingroup  soc_wifi_basic
* @brief  Register receive callback in monitor mode.CNcomment:注册混杂模式的收包回调函数.CNend
*
* @par Description:
*           1.Register receive callback in monitor mode.CNcomment:1.注册混杂模式的收包回调函数.CNend\n
*           2.Wifi driver will put the receive frames to this callback.
*           CNcomment:2.驱动将混杂模式的收到的报文递交到注册的回调函数处理.CNend
*
* @attention  NULL
* @param  data_cb          [IN]     Type  #wifi_promis_cb callback function pointer.CNcomment:混杂模式回调函数.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_promis_set_rx_callback(wifi_promis_cb data_cb);

/**
* @ingroup  soc_wifi_basic
* @brief    Open/close system power save.CNcomment:开启/关闭WiFi低功耗模式并配置预期休眠时间.CNend
*
* @par Description:
*           Open/close system power save.CNcomment:开启/关闭WiFi低功耗模式并配置预期休眠时间.CNend
*
* @attention  NULL
* @param  enable     [IN] Type  #unsigned char, enable(1) or disable(0).CNcomment:开启/关闭WiFi低功耗.CNend
* @param  sleep_time [IN] Type  #unsigned int, expected sleep time(uint: ms). CNcomment:预期休眠时间(单位: 毫秒),
*                               参考有效范围33ms~4000ms, 准确的时间根据dtim*beacon和sleep_time值计算,
*                               关闭低功耗或者不配置有效休眠时间时需要将sleep_time配置为0(休眠时间由关联的ap决定).CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_set_pm_switch(td_u8 enable, td_u32 sleep_time);

/**
* @ingroup  hi_wifi_basic
* @brief    Set Linkloss paras.CNcomment:设置Linkloss的参数.CNend
*
* @par Description:
*           Set Linkloss paras.CNcomment:设置Linkloss的参数.CNend
*
* @attention  NULL
* @param  probe_request_ratio   [IN] Type  #unsigned char, 1~10 linkloss cnt reach N/10 to send probe requeset.
*                               CNcomment:开始发送probe request帧时的linkloss时间比例（N/10），
*                               如5，即在一半linkloss时间时开始发送单播probe request帧保活.CNend
* @param  linkloss_threshold    [IN] Type  #unsigned int, 100~1000 linkloss time(uint: 100ms).
*                               CNcomment:linkloss老化基础时间(单位: 100毫秒，100),
*                               参考有效范围100~1000, 对应实际基础老化时间为10s~100s，会根据当前运行状态
*                               （是否低功耗，是否DBAC等等）进行调整，所设置时间不一定是最终老化时间.CNend
*
* @retval #EXT_ERR_SUCCESS  Excute successfully
* @retval #Other            Error code
* @par Dependency:
*            @li hi_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
osal_s32 uapi_wifi_set_linkloss_paras(osal_u8 probe_request_ratio, osal_u16 linkloss_threshold);


/**
* @ingroup  soc_wifi_basic
* @brief    Set arp offload on/off.CNcomment:设置arp offload 打开/关闭.CNend
*
* @par Description:
*           Set arp offload on with ip address, or set arp offload off.
*           CNcomment:设置arp offload打开、并且设置相应ip地址，或者设置arp offload关闭.CNend
*
* @attention  NULL
* @param  ifname          [IN]     Type  #const char *, device name.
* @param  en              [IN]     Type  #unsigned char, arp offload type, 1-on, 0-off.
* @param  ip              [IN]     Type  #unsigned int, ip address in network byte order, eg:192.168.50.4 -> 0x0432A8C0.
*
* @retval #EXT_WIFI_OK         Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
unsigned char uapi_wifi_arp_offload_setting(const char *ifname, unsigned char en, unsigned int ip);

/**
* @ingroup  soc_wifi_basic
* @brief    Set nd offload on/off.CNcomment:设置nd offload 打开/关闭.CNend
*
* @par Description:
*           Set nd offload on with ipv6 address, or set nd offload off.
*           CNcomment:设置nd offload打开、设置正确的解析后的ipv6地址，或设置nd offload关闭.CNend
*
* @attention  NULL
* @param  ifname          [IN]     Type  #const char *, device name.
* @param  en              [IN]     Type  #unsigned char, nd offload type, 1-on, 0-off.
* @param  ip6             [IN]     Type  #unsigned char *, ipv6 address after parsing.
*                          eg:FE80::F011:31FF:FEE8:DB6E -> 0xfe80000000f01131fffee8db6e
*
* @retval #EXT_WIFI_OK         Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
unsigned int uapi_wifi_nd_offload_setting(const char *ifname, unsigned char en, unsigned char *ip6);

/**
* @ingroup  soc_wifi_basic
* @brief  Set tx power.CNcomment:设置发送功率上限.CNend
*
* @par Description:
*           Set maximum tx power.CNcomment:设置指定接口的发送功率上限.CNend
*
* @attention  1/only softAP can set maximum tx power.CNcomment:只有AP可以设置最大发送功率.CNend
*             2/should start softAP before set tx power.CNcomment:只有在AP start之后才可以设置.CNend
* @param  ifname           [IN]     Type  #const char * interface name.
* @param  power            [IN]     Type  #int maximum tx power value, range (0-23]dBm.
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_set_txpower_up_limit(const td_char *ifname, td_s32 power);

/**
* @ingroup  soc_wifi_basic
* @brief  Get tx power.CNcomment:获取发送功率上限.CNend
*
* @par Description:
*           Get maximum tx power setting.CNcomment:获取接口的最大发送功率限制值.CNend
*
* @attention  NULL
* @param  ifname           [IN]     Type  #const char * interface name.
*
* @retval #tx power value.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_s32 uapi_wifi_get_txpower_up_limit(const td_char *ifname);

/**
* @ingroup  soc_wifi_basic
* @brief  Set retry params.CNcomment:设置软件重传策略.CNend
*
* @par Description:
*           Set retry params.CNcomment:设置指定接口的软件重传策略.CNend
*
* @attention  1.Need call befora start sta or softap.CNcomment:1.本API需要在STA或AP start之后调用.CNend
* @param  ifname    [IN]     Type  #const char * interface name.CNcomment:接口名.CNend
* @param  type      [IN]     Type  #unsigned char retry type.
*                            CNcomment:0:次数重传（数据帧）; 1:次数重传（管理帧）; 2:时间重传.CNend
* @param  limit     [IN]     Type  #unsigned char limit value.
*                            CNcomment:重传次数(0~15次)/重传时间(0~200个时间粒度,时间粒度10ms).CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_u32 uapi_wifi_set_retry_params(const td_char *ifname, td_u8 type, td_u8 limit);

/**
* @ingroup  soc_wifi_basic
* @brief  Set cca threshold.CNcomment:设置CCA门限.CNend
*
* @par Description:
*           Set cca threshold.CNcomment:设置CCA门限.CNend
*
* @attention  CNcomment:1.threshold设置范围是-128~126时，阈值固定为设置值.CNend\n
*             CNcomment:2.threshold设置值为127时，恢复默认阈值-62dBm，并使能动态调整.CNend
* @param  ifname          [IN]     Type #const char *, device name. CNcomment:接口名.CNend
* @param  threshold       [IN]     Type #char, threshold. CNcomment:门限值.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_u32 uapi_wifi_set_cca_threshold(td_u8 mode, td_s8 threshold);

/**
* @ingroup  soc_wifi_basic
* @brief  Set tx power offset.CNcomment:设置发送功率偏移.CNend
*
* @par Description:
*           Set tx power offset.CNcomment:设置发送功率偏移.CNend
*
* @attention  CNcomment:1.offset设置范围是-100~40，单位0.1dB.参数超出范围按最接近的边界值设置CNend\n
*             CNcomment:2.offset设置,可能会影响信道功率平坦度和evm.CNend
* @param  ifname          [IN]     Type #const char *, device name. CNcomment:接口名.CNend
* @param  offset          [IN]     Type #signed short, offset. CNcomment:门限值.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_u32 uapi_wifi_set_tx_pwr_offset(const td_char *ifname, td_s16 offset);

/**
* @ingroup  soc_wifi_basic
* @brief  Send a custom frame.CNcomment:发送用户定制报文.CNend
*
* @par Description:
*           Send a custom frame.CNcomment:发送用户定制报文.CNend
*
* @attention  1.Maximum length is 1400 bytes.CNcomment:1.最大支持发送1400字节的报文.CNend\n
*             2.Require to 802.11 prcotocol.CNcomment:2.报文须按照802.11协议格式封装.CNend\n
*             3.Send by management rate.CNcomment:3.采用管理帧速率发送,发送长包效率较低.CNend\n
*             4.Return status only specify the data has injected to send queue, isn't real send status.
*             CNcomment:4.返回值仅表示数据是否成功进入发送队列,不表示空口发送状态.CNend\n
*             5.The chip is in a low-power state and does not support sending.
*             CNcomment:5.芯片工作在低功耗状态下,不能调用该接口发送报文.CNend\n
* @param  ifname        [IN]     Type #const char *, device name. CNcomment:接口名.CNend
* @param  data          [IN]     Type #const unsigned char *, frame. CNcomment:帧内容.CNend
* @param  len           [IN]     Type #unsigned int, frame length. CNcomment:帧长度.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_send_custom_pkt(const char* ifname, const unsigned char *data, unsigned int len);

/**
* @ingroup  soc_wifi_basic
* @brief  Add user IEs to management frame.CNcomment:在管理帧中添加用户IE字段。CNend
*
* @par Description:
*           Add user IEs to management frame.CNcomment:在管理帧中添加用户IE字段。CNend
*
* @attention  1.Input ie_len must be the same as the len of ie.CNcomment:1.输入的ie_len的长度必须和ie的长度保持一致.CNend
* @param  iftype              [IN]   Type  #ext_wifi_iftype, interface type. CNcomment:接口类型。CNend
*         ie_index            [IN]   Type  #ext_wifi_extend_ie_index, Ie index, there are four indexes to choose。
*                                    CNcomment:ie索引,有四个索引可供选择。CNend
*         frame_type_bitmap   [IN]   Type  #const unsigned char, frame type bitmap,bit0 is beacon,bit1 is probe request,
*                                    bit2 is probe response.
*                                    CNcomment:帧类型，bit0代表beacon,bit1代表probe request,bit2代表probe response。CNend
*         ie                  [IN]   Type  #const unsigned char *, user IE value.CNcomment:用户IE字段内容。CNend
*         ie_len              [IN]   Type  #unsigned short, user IE length.CNcomment:用户IE字段内容长度。CNend
* @retval #EXT_WIFI_OK      Execute successfully.
* @retval #EXT_WIFI_FAIL    Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_add_usr_app_ie(ext_wifi_iftype iftype, ext_wifi_extend_ie_index ie_index,
                             const unsigned char frame_type_bitmap, const unsigned char *ie, unsigned short ie_len);


/**
* @ingroup  soc_wifi_basic
* @brief  Delete user IEs from management frame.CNcomment:在管理帧中删除用户IE字段。CNend
*
* @par Description:
*           Delete user IEs from management frame.CNcomment:在管理帧中删除用户IE字段。CNend
*
* @attention  NULL
* @param  iftype              [IN]   Type  #ext_wifi_iftype, interface type. CNcomment:接口类型。CNend
*         ie_index            [IN]   Type  #ext_wifi_extend_ie_index, Ie index, there are four indexes to choose。
*                                    CNcomment:ie索引,有四个索引可供选择。CNend
*         frame_type_bitmap   [IN]   Type  #const unsigned char, frame type bitmap,bit0 is beacon,bit1 is probe request,
*                                    bit2 is probe response.
*                                    CNcomment:帧类型，bit0代表beacon,bit1代表probe request,bit2代表probe response。CNend
* @retval #EXT_WIFI_OK      Execute successfully.
* @retval #EXT_WIFI_FAIL    Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_delete_usr_app_ie(ext_wifi_iftype iftype, ext_wifi_extend_ie_index ie_index,
                                const unsigned char frame_type_bitmap);


/**
* @ingroup  soc_wifi_basic
* @brief  Set tcp mode.CNcomment:设置tpc开关.CNend
*
* @par Description:
*           Set tpc mode.CNcomment:设置tpc开关.CNend
*
* @attention  1.Mode set to 1, enable auto power control. set to 0, disbale it.
*             CNcomment:1.mode范围是0~1,1:打开发送功率自动控制,0:关闭发送功率自动控制.CNend
* @param  ifname          [IN]     Type #const char *, device name. CNcomment:接口名.CNend
* @param  ifname_len      [IN]     Type #unsigned char, interface name length.CNcomment:接口名长度.CNend
* @param  tpc_value       [IN]     Type #unsigned int, tpc_value. CNcomment:tpc开关.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_u32 uapi_wifi_set_tpc(td_u32 tpc_value);

/**
* @ingroup  soc_wifi_basic
* @brief  Get Status Codes for Authentication and Association Frames.
*         CNcomment:查询Authentication与Association帧status code.CNend
*
* @par Description:
*           Get Status Codes for Authentication and Association Frames.
*           CNcomment:查询Authentication与Association帧status code.CNend
* @attention 1.the return value is Status Codes for Authentication and Association Frames in 802.11.
*            2.used when STA connect AP fail.
*            3.the -1 return value means no status code, then if you want to get the reason of connect fail, use
*            event_wifi_disconnected.reason_code.
*            CNcomment:
*            1.返回值为802.11协议中Authentication reponse与Association reponse帧的status code
*            2.网卡类型为STA时使用，在STA发起路由器连接并失败时调用
*            3.如果返回值为-1，表示无status code信息，此时获取连接失败的原因请使用event_wifi_disconnected结构体中的
*            reason_code值.CNend
* @param  NULL
*
* @retval #Status Codes value
*
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
short uapi_wifi_get_mac_status_code(void);

/**
* @ingroup  soc_wifi_basic
* @brief  Set evm para.CNcomment:设置evm参数.CNend
*
* @par Description:
*           Set evm para.CNcomment:设置evm参数.CNend
*
* @attention  1.Should be called before uapi_wifi_init().
*             CNcomment:只能在uapi_wifi_init()函数前调用.CNend
* @param  pa_bias        [IN]     Type #unsigned char, PA bias setting.0:0xfe83,default value.1:0xfcc3.Others:invalid.
*                                 CNcomment:PA偏置,0:0xfe83,默认值;1:0xfcc3;其它值:不支持.CNend
* @param  osc_drv_vals   [IN]     Type #ext_wifi_xldo_voltage *, OSC driver values ch1~14.
*                                 CNcomment:信道1~14的OSC驱动强度值.CNend
* @param  size           [IN]     Type #unsigned char, OSC driver values' size.Should be 14.
*                                 CNcomment:OSC驱动强度值数组大小,只能为14.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
td_u32 uapi_wifi_set_evm_para(td_u8 pa_bias, const ext_wifi_xldo_voltage *osc_drv_vals, td_u8 size);

/**
* @ingroup  soc_wifi_basic
* @brief  Set wpa-enterprise eap method.CNcomment:设置企业级方法.CNend
*
* @par Description:
*           Set wpa-enterprise eap method:设置企业级方法.CNend
*
* @attention  NULL
* @param  method        [IN]     Type #ext_wifi_eap_method, eap method type.CNcomment:eap方法类型.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_ent_set_eap_method(ext_wifi_eap_method method);

/**
* @ingroup  soc_wifi_basic
* @brief  Set wpa-enterprise identity.CNcomment:设置企业级身份信息.CNend
*
* @par Description:
*           Set wpa-enterprise identity.CNcomment:设置企业级身份信息.CNend
*
* @attention  1. strlen(identity) == len.CNcomment:1.strlen(identity)等于len.CNend
*             2. len <= EXT_WIFI_IDENTITY_LEN.CNcomment:2.len不大于EXT_WIFI_IDENTITY_LEN_MAX.CNend
*
* @param  identity        [IN]     Type #const char *, identity string.CNcomment:身份字符串.CNend
* @param  len             [IN]     Type #unsigned int, identity string length.CNcomment:身份字符串长度.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_ent_set_identity(const char *identity, unsigned int len);

/**
* @ingroup  soc_wifi_basic
* @brief  Set wpa-enterprise read callback.CNcomment:设置企业级证书/密钥读取的回调接口.CNend
*
* @par Description:
*           Set wpa-enterprise read callback.CNcomment:设置企业级证书/密钥读取的回调接口.CNend
*
* @attention  NULL
* @param  cb              [IN]     Type #ext_wifi_ent_import_callback *, read callback.CNcomment:回调接口.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_sta_ent_set_import_callback(ext_wifi_ent_import_callback *cb);

/**
 * @ingroup soc_wifi_basic
 *
 * callback function definition of management frame report.CNcommment:管理帧收包回调接口定义.CNend
 */
typedef int32_t (*wifi_rx_mgmt_cb)(void *recv_buf, int32_t frame_len, int8_t rssi);

/**
* @ingroup  soc_wifi_basic
* @brief  Register receive callback for management frame.CNcomment:注册管理帧收包回调函数.CNend
*
* @par Description:
*           1.Register receive callback for management frame.CNcomment:1.注册管理帧收包回调函数.CNend\n
*           2.Wifi driver will put the management frames received to this callback,except for beacon frame.
*           CNcomment:2.驱动将收到的管理帧报文递交到注册的回调函数处理，收到的beacon帧不会递交到该回调函数.CNend
*           3.The buffer of the frame reacived will be alloc and free by the wifi driver.
*           CNcomment:3.报文的buffer内存由驱动申请和释放，回调函数中不应释放，回调函数运行于驱动任务，不应在回调函数中执行耗时操作.CNend
*
* @attention 1.need to invoke uapi_wifi_set_mgmt_report(NULL) before enable low power mode.
*             CNcomment:开启低功耗睡眠之前需要调用uapi_wifi_set_mgmt_report(NULL)关闭管理帧上报，不然会引起功耗偏高。
* @param  data_cb          [IN]     Type  #wifi_rx_mgmt_cb callback function pointer.CNcomment:管理帧上报回调函数.CNend
* @param  mode             [IN]     Mode of receiving management frame:上报管理帧模式.CNend
*
* @retval #EXT_OK  Excute successfully
* @retval #Other           Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_mgmt_report(wifi_rx_mgmt_cb data_cb, uint8_t mode);


/**
* @ingroup  soc_wifi_basic
* @brief  Set wpa-enterprise read callback.CNcomment:autorate配置速率模式函数接口.CNend
*
* @par Description:
*           Set wpa-enterprise read callback.CNcomment:autorate配置速率模式函数接口.CNend
*
* @attention  NULL
* @param  value              [IN]     Type #unsigned int, switch value.CNcomment:autorate配置速率模式函数接口.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
osal_u32 uapi_alg_ar_fix_cfg_etc(osal_u32 value);

/**
* @ingroup  soc_wifi_basic
* @brief  Set psd enable mode.CNcomment:配置PSD状态函数接口.CNend
*
* @par Description:
*           Set psd enable mode.CNcomment:配置PSD状态函数接口.CNend
*
* @attention  NULL
* @param  psd_option              [IN]     Type #ext_psd_option_param CNcomment:配置PSD状态函数接口.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_psd_enable(psd_option_param *psd_option);

/**
* @ingroup  soc_wifi_basic
* @brief  Set psd callback.CNcomment:配置PSD回调.CNend
*
* @par Description:
*           Set psd callback.CNcomment:配置PSD回调.CNend
*
* @attention  NULL
* @param  data_cb              [IN]     Type #wifi_psd_cb CNcomment:配置PSD回调.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_psd_cb(wifi_psd_cb data_cb);

/**
 * @if Eng
 * @brief  Struct of eth data config.
 * @else
 * @brief  设置eth data相关参数。
 * @endif
 */
typedef struct data_segment {
    uint8_t* data; /* 数据指针 */
    size_t len; /* 数据长度 */
} data_segment_t;

/**
* @ingroup  soc_wifi_basic
* @brief  获取数据发包.CNcomment:配置发包接口.CNend
*
* @par Description:
*           Send data packet.CNcomment:配置发包接口.CNend
*
* @attention  NULL
* @param  data_segment_t          [IN]     Type #eth_data
*         seg_len                 [IN]     Type #length
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_send_pkt(osal_void *dev, const data_segment_t *seg, uint8_t seg_len);

/**
* @ingroup  soc_wifi_basic
* @brief  配置Repeater开关及接口.CNcomment:配置Repeater开关及接口.CNend
*
* @par Description:
*           Set Repeater switch and interface.CNcomment:配置Repeater开关及接口.CNend
*
* @attention  NULL
* @param  oper          [IN]     Type #const char *, identity string.CNcomment:配置操作.CNend
*         if_name       [IN]     Type #const char *, identity string.CNcomment:接口名.CNend
*
* @retval #EXT_WIFI_OK  Excute successfully
* @retval #Other        Error code
*
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_brctl(const char *oper, const char *if_name);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of soc_wifi_api.h */
