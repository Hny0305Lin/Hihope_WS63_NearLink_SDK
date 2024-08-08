/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_device_config.h 的头文件
 */


/**
* @defgroup middleware_service_wifi_device_config WiFi Device Config
* @ingroup middleware_service_wifi
* @{
*/
#include <stdint.h>

#ifndef SERVICE_WIFI_DEVICE_CONFIG_H
#define SERVICE_WIFI_DEVICE_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  the maximum length of a WiFi SSID.The maximum length is 32, and the last bit is reserved and set to '\\0'.
 * @else
 * @brief  表示WiFi SSID的最大长度，最大长度为32，最后一位保留，设置为'\\0'。
 * @endif
 */
#define WIFI_MAX_SSID_LEN 33 // 32 + \0

/**
 * @if Eng
 * @brief  the maximum length of a WiFi key.The maximum length is 64, and the last bit is reserved and set to '\\0'.
 * @else
 * @brief  表示WiFi key的最大长度，最大长度为64，最后一位保留，设置为'\\0'。
 * @endif
 */
#define WIFI_MAX_KEY_LEN 65 // 64 + \0

/**
 * @if Eng
 * @brief  The maximum length of a WiFi PSK.
 * @else
 * @brief  WiFi PSK的最大长度。
 * @endif
 */
#define WIFI_PSK_LEN 32

/**
 * @if Eng
 * @brief  Indicates the maximum length of a Wi-Fi MAC address or a WiFi BSSID.
 * @else
 * @brief  表示Wi-Fi MAC地址或者WiFi BSSID的最大长度。
 * @endif
 */
#define WIFI_MAC_LEN 6

/**
 * @if Eng
 * @brief  Indicates the maximum number of DNS servers, A maximum of two DNS servers are allowed.
 * @else
 * @brief  DNS服务器的最大数量，最多允许两个DNS服务器。
 * @endif
 */
#define WIFI_MAX_DNS_NUM 2

/**
 * @if Eng
 * @brief  Indicates the maximum length of a WiFi WPS PIN.
 *         The maximum length is 8,and the last bit is reserved and set to '\0'.
 * @else
 * @brief  表示WiFi WPS PIN的最大长度，最大长度为8，最后一位保留，设置为'\0'。
 * @endif
 */
#define WIFI_WPS_PIN_MAX_LEN_NUM 9

/**
 * @if Eng
 * @brief  Indicates the length of IPV6 address.
 * @else
 * @brief  IPV6地址长度。
 * @endif
 */
#define WIFI_IPV6_ADDR_LEN 16

/**
 * @if Eng
 * @brief  Indicates the length of IPV6 DNS.
 * @else
 * @brief  IPV6 DNS的长度。
 * @endif
 */
#define WIFI_IPV6_DNS_LEN 16

/**
 * @if Eng
 * @brief  Indicates the max length of interiface name.
 * @else
 * @brief  网络接口名最大长度。
 * @endif
 */
#define WIFI_IFNAME_MAX_SIZE 16

/**
 * @if Eng
 * @brief  CSI data report callback.
 * @param  [in]  csi_data 4 bytes extend timestamp + 758 bytes 64bit big endian data.
 * @param  [in]  len data length.
 * @retval void
 * @else
 * @brief  用户注册的回调函数，用于处理CSI上报的数据。
 * @param  [in]  csi_data 4字节扩展时间戳+758字节64位小端存储格式的CSI数据.
 * @param  [in]  len 数据长度,固定为762字节。
 * @retval void
 * @endif
 */
typedef void (*wifi_csi_data_cb)(uint8_t *csi_data, int32_t len);

/**
 * @if Eng
 * @brief  callback function definition of monitor mode.
 * @param  [in]  recv_buf frame data.
 * @param  [in]  frame_len frame len.
 * @param  [in]  rssi rssi.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  混杂模式收包回调接口定义。
 * @param  [in]  recv_buf 帧数据。
 * @param  [in]  frame_len 帧长度。
 * @param  [in]  rssi 信号强度。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
typedef int32_t (*wifi_promis_cb)(void* recv_buf, int32_t frame_len, int8_t rssi);

/**
 * @if Eng
 * @brief  callback function definition of management frame report.
 * @param  [in]  recv_buf frame data.
 * @param  [in]  frame_len frame len.
 * @param  [in]  rssi rssi.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  管理帧收包回调接口定义。
 * @param  [in]  recv_buf 帧数据。
 * @param  [in]  frame_len 帧长度。
 * @param  [in]  rssi 信号强度。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
typedef int32_t (*wifi_rx_mgmt_cb)(void* recv_buf, int32_t frame_len, int8_t rssi);

/**
 * @if Eng
 * @brief  callback function definition of psd.
 * @param  [in]  recv_buf psd data.
 * @param  [in]  data_len psd data len.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  PSD数据上报回调接口定义。
 * @param  [in]  recv_buf psd 数据。
 * @param  [in]  data_len psd数据长度。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
typedef int32_t (*wifi_psd_cb)(void *recv_buf, uint32_t data_len);

/**
 * @if Eng
 * @brief  Type of Psk.
 * @else
 * @brief  Psk的类型。
 * @endif
 */
typedef enum {
    WIFI_WPA_PSK_NOT_USE,   /*!< @if Eng The PSK does not need to be calculated in advance.
                                       In this case, the PSK in wifi_fast_connect_stru is not used.
                                 @else 不用提前计算PSK，本情况下不使用wifi_fast_connect_stru中的psk。 @endif */
} wifi_wpa_psk_type_enum;

/**
 * @if Eng
 * @brief  PMF type enum.
 * @else
 * @brief  PMF管理帧保护模式类型。
 * @endif
 */
typedef enum {
    WIFI_MGMT_FRAME_PROTECTION_CLOSE,         /*!< @if Eng Disable.
                                                   @else 管理帧保护模式:关闭。 @endif */
    WIFI_MGMT_FRAME_PROTECTION_OPTIONAL,      /*!< @if Eng OPTIONAL.
                                                   @else 管理帧保护模式:可选。 @endif */
    WIFI_MGMT_FRAME_PROTECTION_REQUIRED,      /*!< @if Eng REQUIRED.
                                                   @else 管理帧保护模式:必须。 @endif */
    WIFI_MGMT_FRAME_PROTECTION_BUTT
} wifi_pmf_option_enum;

/**
 * @if Eng
 * @brief  Type of WiFi interface.
 * @else
 * @brief  Type of WiFi interface。
 * @endif
 */
typedef enum {
    IFTYPE_STA,         /*!< @if Eng STAION.
                             @else STAION。 @endif */
    IFTYPE_AP,          /*!< @if Eng HOTSPOT.
                             @else HOTSPOT。 @endif */
    IFTYPE_P2P_CLIENT,  /*!< @if Eng P2P CLIENT.
                             @else P2P CLIENT。 @endif */
    IFTYPE_P2P_GO,      /*!< @if Eng P2P GO.
                             @else P2P GO。 @endif */
    IFTYPE_P2P_DEVICE,  /*!< @if Eng P2P DEVICE.
                             @else P2P DEVICE。 @endif */
    IFTYPES_BUTT
} wifi_if_type_enum;

/**
 * @if Eng
 * @brief  Ie index, there are four indexes to choose.
 * @else
 * @brief  ie索引,有四个索引可供选择。
 * @endif
 */
typedef enum ie_index_enmu {
    IE_FIRST,   /*!< @if Eng Index 1.
                     @else 索引 1。 @endif */
    IE_SECOND,   /*!< @if Eng Index 2.
                     @else 索引 2。 @endif */
    IE_THIRD,   /*!< @if Eng Index 3.
                     @else 索引 3。 @endif */
    IE_FORTH,   /*!< @if Eng Index 4.
                     @else 索引 4。 @endif */
    IE_BUTT
} ie_index_enmu;

/**
 * @if Eng
 * @brief   Struct of psd param.
 * @else
 * @brief  设置psd状态。
 * @endif
 */
typedef struct {
    uint8_t enable;       /*!< @if Eng enable.
                               @else PSD使能标记 0关1开。 @endif */
    uint8_t resv;
    uint16_t duration;    /*!< @if Eng duration.
                               @else 采样时长 取值1~65535单位min。 @endif */
    uint32_t cycle;       /*!< @if Eng cycle.
                               @else 采样间隔 单位ms；取值100~1000。 @endif */
} ext_psd_option_param;

/**
 * @if Eng
 * @brief  Enumeration of WiFi security types.
 * @else
 * @brief  WiFi安全类型的枚举。
 * @endif
 */
typedef enum wifi_security_enum {
    WIFI_SEC_TYPE_INVALID = -1,         /*!< @if Eng Invalid security type.
                                            @else 无效安全类型. @endif */
    WIFI_SEC_TYPE_OPEN,                 /*!< @if Eng Open.
                                             @else Open。 @endif */
    WIFI_SEC_TYPE_WEP,                  /*!< @if Eng Wired Equivalent Privacy SHARED system (WEP-SHARED).
                                             @else WEP。 @endif */
    WIFI_SEC_TYPE_WPA2PSK,              /*!< @if Eng WPA2-Personal.
                                             @else WPA2 个人级。 @endif */
    WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX,     /*!< @if Eng WPA-Personal and WPA2-Personal mixed.
                                             @else 个人级的WPA和WPA2混合。 @endif */
    WIFI_SEC_TYPE_WPAPSK,               /*!< @if Eng WPA-Personal.
                                             @else WPA 个人级。 @endif */
    WIFI_SEC_TYPE_WPA,                  /*!< @if Eng WPA-Enterprise.
                                             @else WPA 企业级。 @endif */
    WIFI_SEC_TYPE_WPA2,                 /*!< @if Eng WPA2-Enterprise.
                                             @else WPA2 企业级。 @endif */
    WIFI_SEC_TYPE_SAE,                  /*!< @if Eng Simultaneous Authentication of Equals (SAE).
                                             @else SAE(WPA3个人级)。 @endif */
    WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX,    /*!< @if Eng WPA2-Personal and WPA3-Personal mixed.
                                             @else 个人级的WPA2和WPA3混合。 @endif */
    WIFI_SEC_TYPE_WPA3,                 /*!< @if Eng WPA3-Enterprise.
                                             @else WPA3 企业级。 @endif */
    WIFI_SEC_TYPE_OWE,                  /*!< @if Eng Opportunistic wireless encryption(OWE).
                                             @else OWE。 @endif */
    WIFI_SEC_TYPE_WAPI_PSK,             /*!< @if Eng WAPI Personal.
                                             @else WAPI 个人级。 @endif */
    WIFI_SEC_TYPE_WAPI_CERT,            /*!< @if Eng WAPI Enterprise.
                                             @else WAPI 企业级。 @endif */
    WIFI_SEC_TYPE_WPA3_WPA2_MIX,        /*!< @if Eng WPA2-Enterprise and WPA3-Enterprise mixed.
                                             @else 企业级的WPA2和WPA3混合。 @endif */
    WIFI_SEC_TYPE_WEP_OPEN,             /*!< @if Eng Wired Equivalent Privacy OPEN system (WEP-OPEN).
                                             @else WEP-OPEN。 @endif */
    WIFI_SEC_TYPE_UNKNOWN               /*!< @if Eng Other certification types.
                                             @else 其它认证类型。 @endif */
} wifi_security_enum;

/**
 * @if Eng
 * @brief  Type of WiFi interface.
 * @else
 * @brief  Type of WiFi interface。
 * @endif
 */
typedef enum {
    WIFI_IFTYPE_UNSPECIFIED,        /*!< @if Eng UNSPECIFIED.
                                         @else UNSPECIFIED @endif */
    WIFI_IFTYPE_ADHOC,              /*!< @if Eng ADHOC.
                                         @else ADHOC @endif */
    WIFI_IFTYPE_STATION = 2,        /*!< @if Eng STATION.
                                         @else STATION @endif */
    WIFI_IFTYPE_AP = 3,             /*!< @if Eng HOTSPOT.
                                         @else HOTSPOT @endif */
    WIFI_IFTYPE_AP_VLAN,            /*!< @if Eng HOTSPOT VLAN.
                                         @else HOTSPOT VLAN @endif */
    WIFI_IFTYPE_WDS,                /*!< @if Eng WDS.
                                         @else WDS @endif */
    WIFI_IFTYPE_MONITOR,            /*!< @if Eng MONITOR.
                                         @else MONITOR @endif */
    WIFI_IFTYPE_MESH_POINT = 7,     /*!< @if Eng MESH POINT.
                                         @else MESH POINT @endif */
    WIFI_IFTYPE_P2P_CLIENT,         /*!< @if Eng P2P CLIENT.
                                         @else P2P CLIENT @endif */
    WIFI_IFTYPE_P2P_GO,             /*!< @if Eng P2P GO.
                                         @else P2P GO @endif */
    WIFI_IFTYPE_P2P_DEVICE,         /*!< @if Eng P2P DEVICE.
                                         @else P2P DEVICE @endif */
    WIFI_IFTYPES_BUTT
} wifi_iftype_t;

/**
 * @if Eng
 * @brief  Struct of WIFI_DEV.
 * @else
 * @brief  WIFI_DEV的相关参数。
 * @endif
 */
typedef struct {
    uint32_t iftype;                        /*!< @if Eng IFTYPE, value from wifi_iftype_t.
                                                 @else IFTYPE。 @endif */
    void *priv;                             /*!< @if Eng priv.
                                                 @else priv。 @endif */
    uint32_t network_id;                    /*!< @if Eng network_id.
                                                 @else network_id。 @endif */
    uint32_t ifname_len;                    /*!< @if Eng ifname_len.
                                                 @else ifname_len。 @endif */
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];  /*!< @if Eng ifname.
                                                 @else ifname。 @endif */
    char reserve[1];
} wifi_dev_t;

/**
 * @if Eng
 * @brief  Config of IPV4.
 * @else
 * @brief  IPV4的配置。
 * @endif
 */
typedef struct {
    uint32_t ip_address;                        /*!< @if Eng IP address of the WiFi device.
                                                     @else WiFi device的IP地址。 @endif */
    uint32_t gateway;                           /*!< @if Eng Gateway of the WiFi device.
                                                     @else WiFi device的Gateway。 @endif */
    uint32_t dns_servers[WIFI_MAX_DNS_NUM];     /*!< @if Eng DNS server addresses for the Wi-Fi device.
                                                     @else WiFi device的DNS服务器地址。 @endif */
    uint32_t netmask;                           /*!< @if Eng Subnet mask of the Wi-Fi device.
                                                     @else WiFi device的子网掩码。 @endif */
} ip_config_stru;

/**
 * @if Eng
 * @brief  Config of IPV6.
 * @else
 * @brief  IPV6的配置。
 * @endif
 */
typedef struct {
    uint8_t ipv6_address[WIFI_IPV6_ADDR_LEN];                       /*!< @if Eng IPV6 address.
                                                                         @else IPV6地址。 @endif */
    uint8_t ipv6_dns_servers[WIFI_MAX_DNS_NUM][WIFI_IPV6_DNS_LEN];  /*!< @if Eng DNS server addresses.
                                                                         @else DNS服务器地址。 @endif */
} ipv6_config_stru;

/**
 * @if Eng
 * @brief  Assignment type of IP.
 * @else
 * @brief  IP的分配类型。
 * @endif
 */
typedef enum {
    STATIC_IP,  /*!< @if Eng Static IP address.
                     @else 静态IP地址。 @endif */
    DHCP,       /*!< @if Eng IP address dynamically assigned by DHCP.
                     @else 由DHCP动态分配的IP地址。 @endif */
    UNKNOWN     /*!< @if Eng Unknown IP address type.
                     @else 未知的IP地址类型。 @endif */
} ip_type_stru_enum;

/**
 * @if Eng
 * @brief  WiFi station configuration used to connect to a specified Wi-Fi device.
 * @else
 * @brief  连接到指定WiFi device的WiFi sta配置。
 * @endif
 */
typedef struct wifi_sta_config_stru {
    int8_t ssid[WIFI_MAX_SSID_LEN];                 /*!< @if Eng Service set ID (SSID).
                                                         @else SSID。 @endif */
    uint8_t bssid[WIFI_MAC_LEN];                    /*!< @if Eng Basic service set ID (BSSID).
                                                         @else BSSID。 @endif */
    int8_t pre_shared_key[WIFI_MAX_KEY_LEN];        /*!< @if Eng Pre Shared Key.
                                                         @else 预共享密钥。 @endif */
    wifi_security_enum security_type;               /*!< @if Eng Security type,It is defined in link wifi_security_enum.
                                                         @else 安全类型。 @endif */
    int8_t wifi_psk_type;                          /*!< @if Eng The type of Psk.
                                                         @else Psk的类型。 @endif */
    int8_t strengthen_verify;                      /*!< @if Eng Enhanced verification.
                                                                A non-zero value indicates that whether the password
                                                                length is 0 needs to be verified in open mode.
                                                                The default value is 0, indicating no verification.
                                                         @else 加强校验，非零值表示open模式下需要校验密码长度是否为0，
                                                               默认为0不校验。 @endif */
    uint8_t channel;                                /*!< @if Eng channel num.
                                                         @else 关联的指定信道号，可选参数。 @endif */
    int8_t rsv;                                 /*!< @if Eng 1 bytes reserved.
                                                         @else 保留1字节。 @endif */
    ip_type_stru_enum ip_type;                      /*!< @if Eng The assignment type of IP.
                                                         @else IP的分配类型。 @endif */
    ip_config_stru static_ip;                       /*!< @if Eng Static IPV4 address.
                                                         @else 静态IPV4地址。 @endif */
    ipv6_config_stru static_ipv6;                   /*!< @if Eng Static IPV6 address.
                                                         @else 静态IPV6地址。 @endif */
} wifi_sta_config_stru;

/**
 * @if Eng
 * @brief  Types of WiFi scan.
 * @else
 * @brief  WiFi扫描的类型。
 * @endif
 */
typedef enum {
    WIFI_BASIC_SCAN,        /*!< @if Eng A basic scan.
                                 @else 普通扫描 @endif */
    WIFI_CHANNEL_SCAN,      /*!< @if Eng A scan based on a specified chanel.
                                 @else 基于指定信道的扫描。 @endif */
    WIFI_SSID_SCAN,         /*!< @if Eng A scan based on a specified SSID.
                                 @else 基于指定SSID的扫描。 @endif */
    WIFI_SSID_PREFIX_SCAN,  /*!< @if Eng  A scan based on a specified Prefix SSID.
                                 @else 基于指定前缀SSID的扫描。 @endif */
    WIFI_BSSID_SCAN,        /*!< @if Eng  A scan based on a specified BSSID.
                                 @else 基于指定BSSID的扫描。 @endif */
    STA_SCAN_BUTT
} wifi_scan_type_enum;

/**
 * @if Eng
 * @brief  The protocol mode of softap and station interfaces.
 * @else
 * @brief  softap和station接口的protocol模式。
 * @endif
 */
typedef enum {
    WIFI_MODE_UNDEFINE,        /*!< @if Eng Not configured.
                                    @else 未配置。 @endif */
    WIFI_MODE_11B,             /*!< @if Eng 11b.
                                    @else 11b。 @endif */
    WIFI_MODE_11B_G,           /*!< @if Eng 11b/g.
                                    @else 11b/g。 @endif */
    WIFI_MODE_11B_G_N,         /*!< @if Eng 11b/g/n.
                                    @else 11b/g/n。 @endif */
    WIFI_MODE_11B_G_N_AX,      /*!< @if Eng 11b/g/n/ax.
                                    @else 11b/g/n/ax。 @endif */
} protocol_mode_enum;

/**
 * @if Eng
 * @brief  Setting of scanning network parameters.
 * @else
 * @brief  扫描参数的设置。
 * @endif
 */
typedef struct {
    int8_t ssid[WIFI_MAX_SSID_LEN];             /*!< @if Eng Service set ID (SSID).
                                                     @else SSID。 @endif */
    int8_t ssid_len;                            /*!< @if Eng Length of the SSID.
                                                     @else SSID的长度。 @endif */
    int8_t bssid[WIFI_MAC_LEN];                 /*!< @if Eng Basic service set ID (BSSID).
                                                     @else BSSID。 @endif */
    int32_t channel_num;                        /*!< @if Eng channel Num (BSSID).
                                                     @else 信道号。 @endif */
    wifi_scan_type_enum scan_type;              /*!< @if Eng scan_type, which is defined by WifiScanType.
                                                     @else WiFi的扫描类型。 @endif */
} wifi_scan_params_stru;

/**
 * @if Eng
 * @brief  Setting of scanning network strategy.
 * @else
 * @brief  扫描策略设置。
 * @endif
 */
typedef struct {
    uint8_t scan_time; /*!< @if Eng the dwell time of each channel.
                            @else 每个信道停留时间，单位ms，范围20~120ms，默认20ms ，扫描总时间不能超过4.5s。  @endif */
    uint8_t scan_cnt;  /*!< @if Eng Number of slots to be scanned.
                            @else 扫描slot数,一个slot 20ms。  @endif */
    uint8_t single_probe_send_times;  /*!< @if Eng times of single probe req to be sent.
                                           @else 单个probe req报文的发送次数，范围1~3，默认1。  @endif */
    uint8_t reserved;
} wifi_scan_strategy_stru;

/**
 * @if Eng
 * @brief  Config of CSI.
 * @else
 * @brief  CSI的配置。
 * @endif
 */
typedef struct {
    uint8_t user_index;             /*!< @if Eng user index.
                                         @else 用户ID,取值范围0~3，最多4个用户。 @endif */
    uint8_t enable;                 /*!< @if Eng enable.
                                         @else CSI白名单用户开关。 @endif */
    uint8_t match_ta_ra_select;     /*!< @if Eng match_ta_ra_select.
                                         @else CSI白名单地址过滤类型 0 RA 1 TA。 @endif */
    uint8_t resv;                   /*!< @if Eng resv.
                                         @else 保留1字节对齐。 @endif */
    uint8_t mac_addr[WIFI_MAC_LEN]; /*!< @if Eng Mac address.
                                         @else MAC地址。 @endif */
    uint8_t frame_filter_bitmap;    /*!< @if Eng frame_filter_bitmap.
                                         @else 帧类型过滤具体参数。 @endif */
    uint8_t sub_type_filter_enable; /*!< @if Eng subTypeFilterEnable.
                                         @else 帧子类型过滤开关。 @endif */
    uint8_t sub_type_filter;        /*!< @if Eng subTypeFilter.
                                         @else 帧子类型过滤具体参数。 @endif */
    uint8_t ppdu_filter_bitmap;     /*!< @if Eng ppduFilterBitmap.
                                         @else ppdu format过滤具体参数。 @endif */
    uint16_t period;                /*!< @if Eng period.
                                         @else CSI上报时间间隔。 @endif */
} csi_config_stru;

/**
 * @if Eng
 * @brief  Parameter settings for quick connection.
 * @else
 * @brief  快速连接的参数设置。
 * @endif
 */
typedef struct wifi_fast_connect_stru {
    wifi_sta_config_stru config;                /*!< @if Eng The Wi-Fi station configuration used to connect to
                                                           a specified Wi-Fi device.
                                                     @else 连接到指定WiFi device的WiFi sta配置。 @endif */
    uint8_t psk[WIFI_PSK_LEN];                  /*!< @if Eng Pre Shared Key(PSK).
                                                     @else PSK。 @endif */
    wifi_wpa_psk_type_enum psk_flag;            /*!< @if Eng PSK flag.Set this parameter to 0 when it is not used.
                                                     @else psk的标志,不需指定时置0。 @endif */
    uint8_t channel_num;                        /*!< @if Eng Number of channels.
                                                     @else 信道号。 @endif */
} wifi_fast_connect_stru;

/**
 * @if Eng
 * @brief  Struct of frame filter config in monitor mode.
 * @else
 * @brief  混杂模式报文接收过滤设置。
 * @endif
 */
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
} wifi_ptype_filter_stru;

/**
 * @if Eng
 * @brief  Struct of linkloss config.
 * @else
 * @brief  设置linkloss相关参数。
 * @endif
 */
typedef struct {
    uint16_t linkloss_threshold;         /*!< @if Eng Threshold of linkloss.
                                             @else 取值范围: [50, 500]. 设置为n时，表示linkloss阈值基础时间为(100*n)ms。 @endif */
    uint8_t send_probe_request_ratio;   /*!< @if Eng Threshold ratio for sending probe request.
                                             @else 取值范围: [1, 10]. 设置为n时，表示linkloss计数达到阈值的（n/10）时，开始发送探测帧保活。 @endif */
    uint8_t resv[1];
} linkloss_paras_stru;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_DEVICE_CONFIG_H
