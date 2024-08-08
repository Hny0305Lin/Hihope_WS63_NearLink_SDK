/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 网络适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_NETWORK_ADAPTER_H
#define HILINK_NETWORK_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAC_ADDRESS_LEN
#define MAC_ADDRESS_LEN 6
#endif

#define HILINK_MAX_AP_SSID_LEN 33
#define HILINK_BSSID_HEX_LEN 6

/** @brief 扫描目标AP字段 */
typedef enum {
    /* 根据AP的SSID名称扫描 */
    WIFI_SCAN_TPYE_SSID,
    /* 根据AP的BSSID扫描 */
    WIFI_SCAN_TPYE_BSSID,
    /* 根据AP频率扫描 */
    WIFI_SCAN_TPYE_FREQ,
} HILINK_WifiScanType;

typedef struct {
    /* 目标AP的SSID, 扫描类型为ssid时必填 */
    char ssid[HILINK_MAX_AP_SSID_LEN];
    /* 目标AP的SSID的长度 */
    unsigned int ssidLen;
    /* 目标AP的bssid */
    char bssid[HILINK_BSSID_HEX_LEN];
    /* 目标AP的频率 */
    int freqs;
    /* 扫描目标AP的字段 */
    HILINK_WifiScanType scanType;
} HILINK_APScanParam;

typedef struct {
    char ssid[HILINK_MAX_AP_SSID_LEN];
    unsigned char bssid[HILINK_BSSID_HEX_LEN];
    int securityType;
    int rssi;
    int band;
    int frequency;
} HILINK_APInfo;

typedef struct {
    HILINK_APInfo *apList;
    unsigned int num;
} HILINK_APList;

typedef struct {
    /**
     * @brief 获取网络优化功能类型
     *
     *        返回的type使用bit位表示功能是否开启，1表示开启，0表示关闭
     *        当前只支持以下几种功能类型
     *
     * @return 0x00：关闭网络优化所有功能
     *         0x01: 表示开启路由引导设备连接至较优AP功能
     *         0x02: 表示开启SDK连接WiFi逻辑，此时需要设备配合关闭本身WiFi重连功能
     *         0x01|0x02：功能全开时
     */
    unsigned int (*getWifiRecoveryType)(void);

    /**
     * @brief 根据参数扫描周围AP信息
     *
     *        此接口指定了BSSID、SSID以及频率相关的类型扫描, 目前只用了SSID方式
     *
     * @param param [IN] 扫描AP信息时指定的目标参数
     * @return 0表示成功, 其他表示失败
     */
    int (*scanAP)(const HILINK_APScanParam *param);

    /**
     * @brief SDK调用scanAP后, 调用此函数获取扫描周围AP信息的结果
     *
     *        scanList中apList字段内存由外部申请, SDK进行释放
     *
     * @param scanList [OUT] 扫描得到的目标AP列表以及个数
     * @return 0表示成功, 1表示扫描未完成, 其他表示失败
     */
    int (*getAPScanResult)(HILINK_APList *scanList);

    /**
     * @brief 重新启动WiFi模块
     *
     *        当设备WiFi断开后, 此函数会被调用重启WiFi模块;
     *
     * @return 0表示成功, 其他表示失败
     */
    int (*restartWiFi)(void);

    /**
     * @brief 连接指定bssid的与配网ssid同名的Wi-Fi
     *
     *        securityType来源于getAPScanResult扫描结果
     *        目标bssid来源于指定ssid扫描得到的列表
     *        接口通常在扫描指定ssid后调用, 用于设备切换到更好的Wi-Fi
     *
     * @param securityType [IN] 目标Wi-Fi的加密类型
     * @param bssid [IN] 目标Wi-Fi的bssid
     * @param bssidLen [IN] bssid的长度
     * @return 返回值必须为0
     */
    int (*connectWiFiByBssid)(int securityType, const unsigned char *bssid, unsigned int bssidLen);

    /**
     * @brief 获取上一次连接WiFi失败原因
     *
     *        1、此接口由厂家实现, 当前在调用HILINK_ConnectWiFiByBssid接口后, 调用此接口获取连接失败原因
     *        2、指定BSSID连接后将轮询调用此接口, 若连接动作未有结果, 函数返回-1, 直到连接动作有结果
     *        3、如果WiFi连接成功, 出参result返回0
     *
     * @param result [OUT] 存放连接WiFi失败原因的内存指针
     * @return 0表示成功, 其他表示失败
     */
    int (*lastConnResult)(int *result);
} WiFiRecoveryApi;

/**
 * @brief 获取本地IP
 *
 * @param localIp [IN] 缓冲区, 为点分十进制格式 eg. 192.168.1.2
 * @param len [OUT] 存放IP的缓冲长度
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetLocalIp(char *localIp, unsigned char len);

/**
 * @brief 获取本地广播地址,用于app广播发现
 *
 * @param broadcastIp [IN] 缓冲区, 为点分十进制格式 eg. 192.168.1.255
 * @param len [OUT] 存放IP的缓冲长度
 * @return 0成功, 非0不支持app广播发现
 */
int HILINK_GetLocalBroadcastIp(char *broadcastIp, unsigned char len);

/**
 * @brief 获取网络MAC地址 mac格式为{0xa1,0xb2,0xc3,0xd4,0xe5,0xf6}
 *
 * @param mac [IN] 存放MAC的缓冲区
 * @param len [OUT] 缓冲区长度, 为MAC_ADDRESS_LEN
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetMacAddr(unsigned char *mac, unsigned char len);

/**
 * @brief 获取当前连接Wi-Fi的ssid
 *
 * @param ssid [IN] 存放ssid的缓冲区
 * @param ssidLen [OUT] 缓冲区长度
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetWiFiSsid(char *ssid, unsigned int *ssidLen);

/**
 * @brief 设置Wi-Fi的连接信息
 *
 *        ssid和pwd为空表示清除WiFi信息, 设置的Wi-Fi信息需要持久化, 以确保设备重启后依然可以连接Wi-Fi
 *
 * @param ssid [OUT] 待连接Wi-Fi的ssid
 * @param ssidLen [OUT] ssid的长度
 * @param pwd [OUT] 待连接Wi-Fi的密码
 * @param pwdLen [OUT] Wi-Fi密码长度
 * @return 0表示成功, 其他表示失败
 * 注意: (1)ssid和pwd为空表示清除wifi信息
 *       (2)设置的wifi信息需要持久化，确保设备重启后依然可以连接wifi
 */
int HILINK_SetWiFiInfo(const char *ssid, unsigned int ssidLen, const char *pwd, unsigned int pwdLen);

/**
 * @brief 重新启动WiFi模块
 *
 *        当设备WiFi断开后, 此函数会被调用重启WiFi模块;
 *
 * @return 0表示成功, 其他表示失败
 */
int HILINK_RestartWiFi(void);

/**
 * @brief 连接指定bssid的与配网ssid同名的Wi-Fi
 *
 *        securityType来源于HILINK_GetAPScanResult扫描结果
 *        目标bssid来源于指定ssid扫描得到的列表
 *        接口通常在扫描指定ssid后调用, 用于设备切换到更好的Wi-Fi
 *
 * @param securityType [IN] 目标Wi-Fi的加密类型
 * @param bssid [IN] 目标Wi-Fi的bssid
 * @param bssidLen [IN] bssid的长度
 * @return 0表示成功, 其他表示失败
 */
int HILINK_ConnectWiFiByBssid(int securityType, const unsigned char *bssid, unsigned int bssidLen);

/**
 * @brief 获取上一次连接WiFi失败原因
 *
 *        1、此接口由厂家实现, 当前在调用HILINK_ConnectWiFiByBssid接口后, 调用此接口获取连接失败原因;
 *        2、指定BSSID连接后将轮询调用此接口, 若连接动作未有结果, 函数返回-1, 直到连接动作有结果;
 *        3、如果WiFi连接成功, 出参result返回0;
 *
 * @param result [OUT] 存放连接WiFi失败原因的内存指针
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetLastConnectResult(int *result);

/**
 * @brief 根据参数扫描周围AP信息
 *
 *        此接口指定了BSSID、SSID以及频率相关的类型扫描, 目前只用了SSID方式
 *
 * @param param [IN] 扫描AP信息时指定的目标参数
 * @return 0表示成功, 其他表示失败
 */
int HILINK_ScanAP(const HILINK_APScanParam *param);

/**
 * @brief SDK调用HILINK_ScanAP后, 调用此函数获取扫描周围AP信息的结果
 *
 *        scanList中apList字段内存由外部申请, SDK进行释放
 *
 * @param scanList [OUT] 扫描得到的目标AP列表以及个数
 * @return 0表示成功, 1表示扫描未完成, 其他表示失败
 */
int HILINK_GetAPScanResult(HILINK_APList *scanList);

 /**
 * @brief 厂家实现网络优化相关功能函数，通过此接口注册给SDK使用
 *
 * @param cb [IN] 当前SDK仅在鸿蒙系统实现以下接口，鸿蒙系统可直接注册使用，其它形态需自行实现：
 *                  HILINK_ConnectWiFiByBssid、HILINK_ScanAP、HILINK_GetAPScanResult、
 *                  HILINK_RestartWiFi、HILINK_GetLastConnectResult
 *                  以上接口只在部分芯片上验证通过，如果直接使用请自行验证，以保证功能正常。
 *                getWifiRecoveryType需要厂商实现
 * @param cbSize [IN] 回调函数结构体的大小，即sizeof(WiFiRecoveryApi)
 * @warning 使用此接口需配备特定SDK版本，否则启动后功能不生效
 */
int HILINK_RegWiFiRecoveryCallback(const WiFiRecoveryApi *cb, unsigned int cbSize);

/**
 * @brief 设置WiFi自恢复时的相关参数
 *
 * @param scanTimes [IN] WiFi自恢复时扫描AP的最大次数
 * @param connectTimes [IN] WiFi自恢复时每个AP最大连接次数
 * @return 正常返回0，异常返回其他错误码
 */
int HILINK_SetWiFiRecoveryTimesParam(unsigned int scanTimes, unsigned int connectTimes);

/**
 * @brief 设置差WiFi环境下心跳超时离线导致重连WiFi的阈值
 *
 * @param hbLimit [IN] 心跳超时离线导致重连WiFi的阈值
 * @return 正常返回0，异常返回其他错误码
 */
int HILINK_SetHeartbeatLimit(unsigned int hbLimit);

/**
 * @brief 断开并重连WiFi
 *
 */
void HILINK_ReconnectWiFi(void);

/**
 * @brief 触发连接WiFi
 *
 * @return 0表示成功, 其他表示失败
 */
int HILINK_ConnectWiFi(void);

/**
 * @brief 获取网络状态
 *
 * @param state [OUT] 返回0表示网络断开或已连接但网卡未分配得ip, 返回1表示已连接且分配得ip
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetNetworkState(int *state);

/**
 * @brief 获取当前连接的WiFi的 bssid
 *
 * @param bssid [IN] 存放bssid的缓冲区
 * @param bssidLen [OUT IN] 出参表示缓冲区长度, 入参表示WiFi bssid长度
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetWiFiBssid(unsigned char *bssid, unsigned char *bssidLen);

/**
 * @brief 获取当前连接的WiFi信号强度, 单位db
 *
 * @param rssi [OUT] 信号强度
 * @return 0表示成功, 其他表示失败
 */
int HILINK_GetWiFiRssi(signed char *rssi);

unsigned int get_wifi_recovery_type(void);

#ifdef __cplusplus
}
#endif
#endif /* HILINK_NETWORK_ADAPTER_H */
