/**
* @file soc_wifi_p2p_api.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
* Description: header file for wifi p2p api.CNcomment:描述：WiFi p2p api接口头文件。CNend
*
* Create: 2020-7-8
*/

/**
 * @defgroup soc_wifi_p2p WiFi Basic Settings
 * @ingroup soc_wifi
 */

#ifndef EXT_WIFI_P2P_API_H
#define EXT_WIFI_P2P_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps dev name length.CNcomment:WPS dev name最大长度CNend
 */
#define WPS_DEV_NAME_MAX_LEN            32

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps serial number length.CNcomment:WPS serial number最大长度CNend
 */
#define WPS_SERIAL_NUMBER_MAX_LEN       32

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps manufacturer length.CNcomment:WPS manufacturer最大长度CNend
 */
#define WPS_MANUFACTURER_MAX_LEN        64

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps model name length.CNcomment:WPS model name最大长度CNend
 */
#define WPS_MODEL_NAME_MAX_LEN          32

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps model number length.CNcomment:WPS model number最大长度CNend
 */
#define WPS_MODEL_NUMBER_MAX_LEN        32

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps mode length.CNcomment:WPS mode最大长度CNend
 */
#define WPS_MODE_MAX_LEN                15

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps state length.CNcomment:WPS state最大长度CNend
 */
#define WPS_STATE_MAX_LEN               19

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps ip length.CNcomment:WPS IP最大长度CNend
 */
#define WPS_IP_MAX_LEN                  15

/**
 * @ingroup soc_wifi_p2p
 *
 * max wps ip length.CNcomment:WPS IP最大长度CNend
 */
#define WPS_PINCODE_MAX_LEN              9

/**
 * @ingroup soc_wifi_p2p
 *
 * p2p invalid value.CNcomment:P2P无效参数CNend
 */
#define P2P_INVALID_VAL                 (-1)

/**
 * @ingroup soc_wifi_basic
 *
 * Length of MAC address.CNcomment:MAC地址长度定义.CNend
 */
#define P2P_MAC_LEN        6

/**
 * @ingroup soc_wifi_basic
 *
 * Max length of SSID.CNcomment:SSID最大长度定义.CNend
 */
#define P2P_MAX_SSID_LEN  32

/**
 * @ingroup soc_wifi_basic
 *
 * Max num of p2p client.CNcomment:p2p默认支持client的最大数量.CNend
 */
#define WIFI_DEFAULT_P2P_MAX_NUM_CLIENT     4

/**
 * @ingroup soc_wifi_basic
 *
 * length of device type.CNcomment:device type的长度.CNend
 */
#define WPS_DEV_TYPE_LEN 8

/**
 * @ingroup soc_wifi_basic
 *
 * length of methond config.CNcomment:关联方法的长度.CNend
 */
#define MAX_METHOND_CONFIG_LEN 44

/**
 * @ingroup soc_wifi_basic
 *
 * Max length of PSK.CNcomment:PSK长度.CNend
 */
#define PSK_LEN 32

/**
 * @ingroup soc_wifi_basic
 *
 * Total address length of the maximum number of clients.CNcomment:最大数client的地址总长度.CNend
 */
#define MAX_CLINET_SAVE_ADDR_LEN 48

/**
 * @ingroup soc_wifi_basic
 *
 * Len of Prefix name.CNcomment:网络前缀名称长度.CNend
 */
#define PREFIX_NAME_LEN 32

/**
 * @ingroup soc_wifi_basic
 *
 * Max num of persistent group.CNcomment:永久网络最大数量.CNend
 */
#define P2P_MAX_PERSISTENT_CNT 10

/**
 * @ingroup hi_wifi_basic
 *
 * p2p max scan number of ap.CNcomment:p2p扫描结果的最多数目.CNend
 */
#define P2P_SCAN_DEVICE_LIMIT               32

/**
 * @ingroup soc_wifi_p2p
 *
 * the peer connect flag.CNcomment:p2p对端连接标志
 */
typedef enum {
    EXT_WIFI_P2P_PEER_CONNECT_AUTO,        /**< automatically accept connections. CNcomment: 自动接受连接.CNend */
    EXT_WIFI_P2P_PEER_CONNECT_MANUAL,      /**< manually accept connections. CNcomment: 手动接受连接.CNend */
    EXT_WIFI_P2P_PEER_CONNECT_BUTT
} ext_wifi_p2p_peer_connect_choose;

/**
 * @ingroup soc_wifi_p2p
 *
 * p2p mode.CNcomment:p2p模式。
 */
typedef enum {
    EXT_WIFI_P2P_MODE_GC,                 /**< gc mode. CNcomment: gc 模式.CNend */
    EXT_WIFI_P2P_MODE_GO,                 /**< go mode. CNcomment: go 模式.CNend */
    EXT_WIFI_P2P_DEVICE_ONLY,             /**< group in process. CNcomment: 正在创建组.CNend */
    EXT_WIFI_P2P_MODE_BUTT
} ext_wifi_p2p_mode;

/**
 * @ingroup soc_wifi_p2p
 *
 * p2p Connection state.CNcomment:p2p连接状态。
 */
typedef enum {
    EXT_WIFI_P2P_DISCONNECTED,           /**< disconnect. CNcomment: 断开连接状态.CNend */
    EXT_WIFI_P2P_CONNECTED,              /**< connected. CNcomment: 连接完成.CNend */
    EXT_WIFI_P2P_CONNECTING,             /**< connecting. CNcomment: 正在连接.CNend */
    EXT_WIFI_P2P_CONN_STATUS_BUTT
}ext_wifi_p2p_state;

/**
 * @ingroup soc_wifi_p2p
 *
 * Struct of p2p's peer info.CNcomment:p2p对端结果CNend
 *
 */
typedef struct {
    unsigned char device_addr[P2P_MAC_LEN];     /**< device addr. CNcomment: device地址.CNend */
    char device_name[WPS_DEV_NAME_MAX_LEN + 1]; /**< device name. CNcomment: device名称.CNend */
    unsigned short config_methods;              /**< config methods. CNcomment: 配置方式.CNend */
    unsigned char group_capab;                  /**< group capbility. CNcomment: 组能力.CNend */
    int persistent_id;                          /**< persistent id corresponding to MAC address.
                                                     CNcomment: mac地址对应的永久网络id.CNend */
} ext_wifi_p2p_peer_info;

/**
 * @ingroup soc_wifi_p2p
 *
 * Struct of p2p's config that can be updated.CNcomment:p2p 可以更新的配置CNend
 *
 */
typedef struct {
    char device_name[WPS_DEV_NAME_MAX_LEN + 1];        /**< device name. CNcomment: 设备名称.CNend */
    char manufacturer[WPS_MANUFACTURER_MAX_LEN + 1];   /**< manufacturer. CNcomment: 设备厂商名称.CNend */
    char serial_number[WPS_SERIAL_NUMBER_MAX_LEN + 1]; /**< serial number. CNcomment: 设备序列号.CNend */
    char model_name[WPS_MODEL_NAME_MAX_LEN + 1];       /**< model name. CNcomment: 设备型号.CNend */
    char model_number[WPS_MODEL_NUMBER_MAX_LEN + 1];   /**< model number. CNcomment: 设备编号.CNend */
    int  go_intent;                                    /**< go intent. CNcomment: GO协商倾向值.CNend */
    int  oper_channel;                                 /**< operating channel. CNcomment: 工作信道.CNend */
    int  listen_channel;                               /**< listen channel. CNcomment: 监听信道.CNend */
    int  wps_method;                                   /**< wps method. CNcomment: wps的连接方式.CNend */
} ext_wifi_p2p_user_conf;

/**
 * @ingroup soc_wifi_p2p
 *
 * Struct of p2p's connect status.CNcomment:p2p 连接状态CNend
 *
 */
typedef struct {
    char ssid[P2P_MAX_SSID_LEN + 1];  /**< group ssid. CNcomment: 组名称.CNend */
    unsigned char bssid[P2P_MAC_LEN]; /**< group bssid. CNcomment: 组地址.CNend */
    int op_freq;                      /**< oprational freq. CNcomment: 工作信道.CNend */
    ext_wifi_p2p_mode mode;           /**< mode. CNcomment: 模式.CNend */
    ext_wifi_p2p_state p2p_state;     /**< p2p state. CNcomment:状态.CNend */
} ext_wifi_p2p_status_info;

/**
 * @ingroup soc_wifi_p2p
 *
 * Struct of p2p's DHCP config.CNcomment:p2p DHCP可设置的网络CNend
 *
 */
typedef struct {
    char wpa_p2p_dhcps_ip[WPS_IP_MAX_LEN + 1];
    char wpa_p2p_dhcps_netmask[WPS_IP_MAX_LEN + 1];
    char wpa_p2p_dhcps_gateway[WPS_IP_MAX_LEN + 1];
} ext_wifi_p2p_dhcps_ifconfig;

/**
 * @ingroup soc_wifi_p2p
 *
 * mac address of go's client.CNcomment:与p2p相连的client mac地址.CNend
 *
 */
typedef struct {
    unsigned char mac[P2P_MAC_LEN];             /**< MAC address.CNcomment:与go相连的client mac地址.CNend */
    unsigned char dev_addr[P2P_MAC_LEN];        /**< MAC address.CNcomment:与go相连的client dev地址.CNend */
    char device_name[WPS_DEV_NAME_MAX_LEN + 1]; /**< device name. CNcomment: 设备名称.CNend */
} ext_wifi_p2p_client_info;

/**
 * @ingroup soc_wifi_p2p
 *
 * wifi mode.CNcomment:wifi模式。
 */
enum wifi_mode {
    /**< infrastructure mode， associate with an AP. CNcomment:基础结构模式，可以关联ap.CNend */
    WIFI_MODE_INFRA = 0,
    /**< IBSS peer-to-peer. CNcomment: IBSS模式，点对点.CNend */
    WIFI_MODE_IBSS = 1,
    /**< access point. CNcomment: 接入点.CNend */
    WIFI_MODE_AP = 2,
    /**< P2P Group Owner. CNcomment: p2p组拥有者.CNend */
    WIFI_MODE_P2P_GO = 3,
    /**< P2P Group Formation. CNcomment: p2p组形成.CNend */
    WIFI_MODE_P2P_GROUP_FORMATION = 4,
    /**< mesh. CNcomment: mesh.CNend */
    WIFI_MODE_MESH = 5,
};

/**
 * @ingroup soc_wifi_p2p
 *
 * Struct of p2p's connect.CNcomment:p2p 连接入参结构体CNend
 *
 */
typedef struct {
    unsigned char peer_addr[P2P_MAC_LEN];   /**< peer device addr. CNcomment: 对端的device地址.CNend */
    char pin[WPS_PINCODE_MAX_LEN];          /**< Pin Code of WPS.CNcomment:WPS的Pin码.CNend */
    int  join_group;                        /**< Join an existing groupo or not . CNcomment: 是否加入已存在的组 */
    int  persistent;                        /**< persistent or not. CNcomment: 是否期望永久网络.CNend */
    int  go_intent;                         /**< go intent. CNcomment: 组网成为go的意愿值.CNend */
    int  wps_method;                        /**< wps_method. CNcomment: 连接的WPS方式.CNend */
} ext_wifi_p2p_connect;

/**
 * @ingroup soc_wifi_p2p
 *
 *  Struct of p2p's persistent ssid.CNcomment:p2p永久网络ssid结构体。
 */
typedef struct {
    /**< ssid of persistent group. CNcomment: 永久网络名称.CNend */
    unsigned char ssid[EXT_WIFI_MAX_SSID_LEN + 1];
    /**< len of ssid. CNcomment: 永久网络名称长度.CNend */
    unsigned int ssid_len;
    /**< bssid is set or not. CNcomment: bssid是否设置.CNend */
    unsigned int bssid_set;
    /**< bssid of persistent group. CNcomment:永久网络bssid.CNend */
    unsigned char bssid[EXT_WIFI_MAC_LEN];
    /**< mode. CNcomment:工作模式.CNend */
    enum wifi_mode mode;
    /**< device addr. CNcomment:go的device addr.CNend */
    unsigned char go_p2p_dev_addr[EXT_WIFI_MAC_LEN];
    /**< psk. CNcomment:psk数据.CNend */
    unsigned char psk[PSK_LEN];
    /**< psk is set or not. CNcomment:psk是否设置.CNend */
    int psk_set;
    /**< disabled is set or not. CNcomment:wpa p2p设置的标志.CNend */
    int disabled;
    /**< list of client.CNcomment:关联的client表.CNend */
    unsigned char p2p_client_list[MAX_CLINET_SAVE_ADDR_LEN];
    /**< num of client.CNcomment:关联的client数量.CNend */
    unsigned int num_p2p_clients;
    /**< persistent or not.CNcomment:是否是永久网络.CNend */
    int p2p_persistent_group;
    /**< wps enable or not.CNcomment:wps是否开启.CNend */
    int wps_disabled;
} p2p_persistent_ssid;

/**
 * @ingroup soc_wifi_p2p
 *
 *  Struct of p2p's persistent config .CNcomment:p2p永久网络配置结构体。
 */
typedef struct {
    char device_name[WPS_DEV_NAME_MAX_LEN + 1];        /**< device name.CNcomment:device 名称.CNend */
    char manufacturer[WPS_MANUFACTURER_MAX_LEN + 1];   /**< manufacturer. CNcomment: 设备厂商名称.CNend */
    char serial_number[WPS_SERIAL_NUMBER_MAX_LEN + 1]; /**< serial number. CNcomment: 设备序列号.CNend */
    char model_name[WPS_MODEL_NAME_MAX_LEN + 1];       /**< model name. CNcomment: 设备型号.CNend */
    char model_number[WPS_MODEL_NUMBER_MAX_LEN + 1];   /**< model number. CNcomment: 设备编号.CNend */
} p2p_persistent_config;

/**
 * @ingroup soc_wifi_p2p
 *
 *  Struct of p2p's persistent .CNcomment:p2p永久网络结构体。
 */
typedef struct {
    p2p_persistent_config conf;                        /**< config of persistent.CNcomment:永久网络配置.CNend */
    p2p_persistent_ssid ssid[P2P_MAX_PERSISTENT_CNT];         /**< ssid config of persistent.CNcomment:ssid配置.CNend */
} p2p_user_persistent;

/**
 * @ingroup soc_wifi_p2p
 *
 *  Struct of p2p's persistent group.CNcomment:p2p永久网络组结构体。
 */
typedef struct {
    int network_id;                                        /**< network id.CNcomment:网络id.CNend */
    unsigned char network_name[EXT_WIFI_MAX_SSID_LEN + 1]; /**< network name.CNcomment:网络名称.CNend */
    unsigned char  go_bssid[EXT_WIFI_MAC_LEN];             /**< go bssid.CNcomment:go的bssid.CNend */
    char  reserved;
} ext_p2p_persistent_group;

/**
* @ingroup  soc_wifi_p2p
* @brief  Start Start wifi p2p.CNcomment:开启P2P.CNend
*
* @par Description:
*           Start wifi p2p.CNcomment:开启P2P.CNend
*
* @attention  1. Multiple interfaces of the same type are not supported.CNcomment:1. 不支持使用多个同类型接口.CNend\n
*             2. Dual interface coexistence support: STA + P2P.
*                CNcomment:2. 双接口共存支持：STA + P2P.CNend\n
*             3. The memories of <ifname> and <len> should be requested by the caller，
*                the input value of len must be the same as the length of ifname
*                (The minimum length is 5 bytes and the recommended length is 17 bytes).\n
*                CNcomment:3. <ifname>和<len>由调用者申请内存，用户写入len的值必须与ifname长度一致。
*                (最小长度是5字节，建议长度为17Bytes).CNend\n
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
int uapi_wifi_p2p_start(char *ifname, int *len);

/**
* @ingroup  soc_wifi_p2p
* @brief  Close wifi p2p.CNcomment:关闭P2P.CNend
*
* @par Description:
*           Close wifi p2p.CNcomment:关闭P2P.CNend
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
int uapi_wifi_p2p_stop(void);

/**
* @ingroup  soc_wifi_p2p
* @brief    Start p2p scan.CNcomment:开始p2p查找 。CNend
*
* @par Description:
*           Start p2p scan.CNcomment:开始p2p查找 。CNend
*
* @attention  sec: The value range is a positive integer starting from 5 to 120.
                   CNcomment:取值范围为从5到120的正整数。CNend \n
* @param  sec          [IN]     Type  int, scan time(unit s).CNcomment:查找时间。CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_find(int sec);

/**
* @ingroup  soc_wifi_p2p
* @brief    Start p2p listen.CNcomment:开始p2p监听。CNend
*
* @par Description:
*           Start p2p listen.CNcomment:开始p2p监听。CNend
*
* @attention   1.period should be less than interval.CNcomment:1.period应该小于interval。CNend \n;
*              2.Both period and interval should be positive numbers,max num is 65535.
*                CNcomment:2.period和interval都为正数,最大值为65535。CNend \n;
*              3.When the period and interval are both 0, the listen stops.
*                CNcomment:3.当period和interval都为0的时候，监听停止。CNend \n;
*
* @param  period          [IN]     Type  int, listen time once(int ms).
*                                          CNcomment:每次监听时间。CNend
*         interval        [IN]     Type  int, listen time interval once (int ms).
*                                          CNcomment:每次间隔时间时间。CNend
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_listen(int period, int interval);

/**
* @ingroup  soc_wifi_p2p
* @brief    Stop p2p find.CNcomment:停止p2p查找。CNend
*
* @par Description:
*           Stop p2p find.CNcomment:停止p2p查找。CNend
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
int uapi_wifi_p2p_stop_find(void);

/**
* @ingroup  soc_wifi_p2p
* @brief    Set p2p log level.CNcomment:设置P2P的日志级别。CNend
*
* @par Description:
*           Set p2p log level.CNcomment:设置P2P的日志级别。CNend
*
* @attention  NULL
* @param  log_level
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_log_level(char *log_level);

/**
* @ingroup  soc_wifi_p2p
* @brief    Connect to specific p2p device.CNcomment:连接指定的p2p设备。CNend
*
* @par Description:
*           Connect to specific p2p device.CNcomment:连接指定的p2p设备。CNend
*
* @attention   Address length must be 6 bytes.CNcomment:地址长度必须为6字节。CNend \n;
*
* @param  con    [IN]        Type  #ext_wifi_p2p_connect,associated parameter structure.CNcomment:p2p连接入参结构体。CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_connect(ext_wifi_p2p_connect con);

/**
* @ingroup  soc_wifi_p2p
* @brief    reject to specific p2p device.CNcomment:拒绝指定的p2p设备。CNend
*
* @par Description:
*           reject to specific p2p device.CNcomment:拒绝指定的p2p设备。CNend
*
* @attention  Address length must be 6 bytes.CNcomment:地址长度必须为6字节。CNend \n;
* @param  peer_addr    [IN]        Type  #const unsigned char *, peer device address.CNcomment:对端device地址。CNend
*         addr_len     [IN]        Type  #unsigned char , len of peer device address .CNcomment:对端device地址长度。CNend
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_reject(const unsigned char *peer_addr, unsigned char addr_len);

/**
* @ingroup  soc_wifi_p2p
* @brief    1.Enabling a fixed p2p device network.CNcomment:启用永久网络。CNend
*           2.Invite other devices to join the group.CNcomment:邀请其他设备加入网络。CNend
*
* @par Description:
*          1.Enabling a persistent p2p network.CNcomment:启用永久网络。CNend
*          2.Invite other devices to join the group.CNcomment:邀请其他设备加入网络。CNend
*
* @attention Address length must be 6 bytes.CNcomment:地址长度必须为6字节。CNend \n;
* @param  persistent_id   [IN]        Type #int  connect network for persistent save value.
*                                                CNcomment:永久网络id，邀请其他网络的时候为-1。CNend
*         peer_addr       [IN]        Type #const unsigned char * peer mac address.CNcomment:对端device地址。CNend
*         addr_len        [IN]        Type #unsigned char len of peer mac address.
*                                                         CNcomment:对端device地址长度。CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_invite(int persistent_id, const unsigned char *peer_addr, unsigned char addr_len);

/**
* @ingroup  soc_wifi_p2p
* @brief  show information of peer.CNcomment:显示对端信息.CNend
*
* @par Description:
*           show information of peer.CNcomment:显示对端信息.CNend
* @attention  1. The memories of <p2pList> and <peer_num> memories are requested by the caller. \n
*             The <p2pList> size up to : OAL_SIZEOF(ext_wifi_p2p_peer_info p2pList) * 32. \n
*             CNcomment:1. <p2pList>和<peer_num>由调用者申请内存,
*             <p2pList>size最大为：OAL_SIZEOF(ext_wifi_p2p_peer_info p2pList) * 32.CNend \n
*             2. peer_num: parameters can be passed in to specify the number of find results.The maximum is 32. \n
*             CNcomment:2. peer_num: 可以传入参数，指定获取扫描到的结果数量，最大为32。CNend \n
*             3. peer_num should be same with number of ext_wifi_ap_info structures applied,
*                Otherwise, it will cause memory overflow. \n
*             CNcomment:3. peer_num和申请的ext_wifi_p2p_peer_info结构体数量一致，否则可能造成内存溢出。CNend \n
* @param  p2pList           [IN/OUT]    Type #ext_wifi_p2p_peer_info * p2p find result.CNcomment:p2p发现的结果.CNend
* @param  peer_num          [IN/OUT]    Type #unsigned int *, number of scan result.CNcomment:p2p发现的网络数目.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_peers(ext_wifi_p2p_peer_info *p2pList, unsigned int *peer_num);

/**
* @ingroup  soc_wifi_p2p
* @brief    Show connect status of p2p.CNcomment:显示p2p的连接状态信息。CNend
*
* @par Description:
*           Show connect status of p2p.CNcomment:显示p2p的连接状态信息。CNend
*
* @attention  NULL
* @param   status      [OUT]       Type  #ext_wifi_p2p_status_info *, device information.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_status(ext_wifi_p2p_status_info *status);

/**
* @ingroup  soc_wifi_p2p
* @brief  Get all client's information of p2p go.CNcomment:p2p go获取已连接的client的信息.CNend
*
* @par Description:
*           Get all client's information of p2p go .CNcomment:p2p go获取已连接的client的信息.CNend
*
* @attention  1.client_list: malloc by user.CNcomment:1.连接client参数。由用户动态申请。CNend \n
*             2.client_list: max size is ext_wifi_p2p_client_info * 4.
*               CNcomment:2.client_list 足够的结构体大小，最大为ext_wifi_p2p_client_info * 4。CNend \n
*             3.client_num:parameters can be passed in to specify the number of connected client.The maximum is 4.
*               CNcomment:3.可以传入参数，指定获取已接入的client个数，最大为4。CNend \n
*             4.client_num should be the same with number of ext_wifi_p2p_client_info struct applied, Otherwise,
*               it will cause memory overflow.\n
*               CNcomment:4.client_num和申请的ext_wifi_p2p_client_info结构体数量一致，否则可能造成内存溢出。CNend \n
*             5.Only the client that are successfully associated are displayed.
*               CNcomment:5.只显示关联成功的client。CNend
* @param  client_list        [IN/OUT]  Type  #ext_wifi_p2p_client_info *, client information.CNcomment:client信息.CNend
* @param  client_num         [IN/OUT]  Type  #unsigned int *, client number.CNcomment:client个数.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_get_connected_client(ext_wifi_p2p_client_info *client_list, unsigned int *client_num);

/**
* @ingroup  soc_wifi_p2p
* @brief  go deauth user by peer device address.CNcomment:go指定断开连接的client网络.CNend
*
* @par Description:
*          go deauth user by peer device address.CNcomment:go指定断开连接的client网络.CNend
*
* @attention  NULL
* @param  peer_dev_addr    [IN]     Type  #const unsigned char *, gc mac address.CNcomment:MAC地址.CNend
*         addr_len         [IN]     Type  #unsigned char, len of gc mac address.CNcomment:MAC地址.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_p2p_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_remove_client(const unsigned char *peer_dev_addr, unsigned char addr_len);

/**
* @ingroup  soc_wifi_p2p
* @brief    Cancel an ongoing P2P group formation and joining-a-group related operation.
            CNcomment:取消正在进行的P2P群组组建和入群相关操作。CNend
*
* @par Description:
*           Cancel an ongoing P2P group formation and joining-a-group related operation.
            CNcomment:取消正在进行的P2P群组组建和入群相关操作。CNend
*
* @attention  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_cancel(void);

/**
* @ingroup  soc_wifi_p2p
* @brief    disconncet for p2p network.CNcomment:断开p2p连接。CNend
*
* @par Description:
*           disconncet for p2p network.CNcomment:断开p2p连接。CNend
*
* @attention  NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_remove_group(void);

/**
* @ingroup  soc_wifi_p2p
* @brief    Update p2p configuration.CNcomment:p2p可修改的参数。CNend
*
* @par Description:
*           Update p2p configuration.CNcomment:p2p可修改的参数。CNend
*
* @attention  NULL
* @param   updateConfig      [IN]       Type  #ext_wifi_p2p_user_conf, configuration need update.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_user_config(ext_wifi_p2p_user_conf updateConfig);

/**
* @ingroup  soc_wifi_p2p
* @brief    Get p2p configuration.CNcomment:获取p2p的参数。CNend
*
* @par Description:
*           Get p2p configuration.CNcomment:获取p2p的参数。CNend
*
* @attention  NULL
* @param   updateConfig      [OUT]       Type  #ext_wifi_p2p_user_conf.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_get_user_config(ext_wifi_p2p_user_conf *GetConfig);

/**
* @ingroup  soc_wifi_p2p
* @brief    user accept p2p .CNcomment:p2p是否接受连接。CNend
*
* @par Description:
*           user accept p2p.
*
* @attention  NULL
* @param   con        [IN]        Type  #ext_wifi_p2p_connect, associated parameter structure.
*                                       CNcomment:连接入参结构体。CNend
*         assoc       [IN]        Type  #int assoc, accept or reject.CNcomment:p2p是否接受连接。CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_user_accept(ext_wifi_p2p_connect con, int assoc);

/**
* @ingroup  soc_wifi_p2p
* @brief  Set the status that P2P automatically accepts connections or manually receives connections.
*         CNcomment:设置p2p是自动接收连接还是手动接收连接.CNend
*
* @par Description:
*           Set the status that P2P automatically accepts connections or manually receives connections.
*           CNcomment:设置p2p是自动接收连接还是手动接收连接.CNend
*
* @attention  NULL
* @param  flag [IN]  Type  #ext_wifi_p2p_peer_connect_choose CNcomment:设置p2p是自动连接还是手动连接的标志.CNend
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_p2p_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_set_peer_connect_choose(ext_wifi_p2p_peer_connect_choose flag);

/**
* @ingroup	soc_wifi_p2p
* @brief  Get the status that P2P automatically accepts connections or manually receives connections.
*         CNcomment:返回目前p2p是自动接收连接还是手动接收连接.CNend
*
* @par Description:
*           Get the status that P2P automatically accepts connections or manually receives connections.
*           CNcomment:返回目前p2p是自动接收连接还是手动接收连接.CNend
*
* @attention  NULL
* @param      NULL
*
* @retval #EXT_WIFI_P2P_PEER_CONNECT_AUTO or EXT_WIFI_P2P_PEER_CONNECT_MANUAL.
* @par Dependency:
*			 @li soc_wifi_api.h: WiFi API
* @see	NULL
* @since
*/
ext_wifi_p2p_peer_connect_choose uapi_wifi_get_peer_connect_choose(void);

/**
* @ingroup  soc_wifi_p2p
* @brief    Get the configuration information of P2P persistent network.CNcomment:获取p2p永久网络配置信息。CNend
*
* @par Description:NULL
*
* @attention  1.The persisetnt network information obtained is used to save it in NV and cannot be modified.
*               Otherwise, it may cause a memory error.
*               CNcomment:1.获取的永久网络信息，用于保存在nv中，不能修改，否则或造成内存错误.CNend
*             2.It need to obtain the permanent network configuration before p2pstop,
*               otherwise the persistent data will be cleared to 0
*               CNcomment:2.需要在p2pstop之前获取数据，否则永久网络数据会被清除。CNend
*
* @param   conf      [IN]       Type  #p2p_user_persistent *, configuration need be got.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.

* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_get_persistent_conf(p2p_user_persistent *conf);

/**
* @ingroup  soc_wifi_p2p
* @brief    Get the configuration information of P2P persistent network.CNcomment:设置p2p永久网络配置信息。CNend
*
* @par Description：NULL
*
* @attention  The set permanent network information should be consistent with the obtained information,
*             otherwise it will cause memory error
*             CNcomment:设置的永久网络信息，要和获取的一致，否则或造成内存错误.CNend
*
* @param   conf      [IN]       Type  #p2p_user_persistent, configuration need be set.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_set_persistent_conf(p2p_user_persistent conf);

/**
* @ingroup  soc_wifi_p2p
* @brief    Get the nformation of P2P persistent network.CNcomment:获取永久网络信息给用户。CNend
*
* @par Description:NULL
*
* @attention  NULL
* @param   info      [IN/OUT]       Type  #ext_p2p_persistent_group *, infomation of persistent group.
*          info_num  [IN/OUT]       Type  #int *, number of persistent group.
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_get_persistent_info(ext_p2p_persistent_group *info, int *info_num);

/**
* @ingroup  soc_wifi_p2p
* @brief    Get the configuration information of P2P persistent network.CNcomment: 删除永久网络信息。CNend
*
* @par Description:
*           network id.
*
* @attention  NULL
* @param   id      [IN]       Type  #int, network id.
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_del_persistent_info(int id);

/**
* @ingroup  soc_wifi_p2p
* @brief    Starting WPS PBC.CNcomment: 启动wps pbc。CNend
*
* @par Description:NULL.
*
* @attention It can only be used when the device is go.CNcomment: 只有go的时候可以使用。CNend
* @param   NULL
*
* @retval #EXT_WIFI_OK        Execute successfully.
* @retval #EXT_WIFI_FAIL      Execute failed.
* @par Dependency:
*            @li soc_wifi_api.h: WiFi API
* @see  NULL
* @since
*/
int uapi_wifi_p2p_wps_pbc(void);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of soc_wifi_p2p_api.h */

