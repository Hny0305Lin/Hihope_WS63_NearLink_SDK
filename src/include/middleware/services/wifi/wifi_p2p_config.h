/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_p2p_config.h 的头文件
 */


#ifndef SERVICE_WIFI_P2P_CONFIG_C_H
#define SERVICE_WIFI_P2P_CONFIG_C_H

#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_p2p_config WiFi P2P Config
 * @ingroup middleware_service_wifi
 * @{
 */

/**
 * @if Eng
 * @brief  Maximum length of the WPS device name.
 * @else
 * @brief  device名称最大长度。
 * @endif
 */
#define WPS_DEV_NAME_LEN 33


/**
 * @if Eng
 * @brief  WPS connection mode.
 * @else
 * @brief  wps连接方式。
 * @endif
 */
typedef enum {
    WPS_PBC,           /*!< @if Eng PBC connection.
                            @else   PBC方式连接。  @endif */
    WPS_PIN_DISPLAY,   /*!< @if Eng Connected in PIN DISPLAY mode, which is not supported in 92.
                            @else   PIN DISPLAY方式连接,92不支持。  @endif */
    WPS_PIN_KEYPAD,    /*!< @if Eng Connected in PIN KEYPAD mode, which is not supported in 92.
                            @else   PIN KEYPAD方式连接,92不支持。  @endif */
    WPS_PIN_LABEL,     /*!< @if Eng Connected in PIN LABEL mode, which is not supported in 92.
                            @else   PIN LABEL方式连接,92不支持。  @endif */
    WPS_OTHER,         /*!< @if Eng Connected in other mode, which is not supported in 92.
                            @else   其他方式连接,92不支持。  @endif */
} wps_method_enum;


/**
 * @if Eng
 * @brief  P2P mode.
 * @else
 * @brief  p2p模式。
 * @endif
 */
typedef enum {
    P2P_MODE_GC,             /*!< @if Eng P2P GC mode.
                                  @else   P2P GC模式。  @endif */
    P2P_MODE_GO,             /*!< @if Eng P2P GO mode.
                                  @else   P2P GO模式。  @endif */
    P2P_MODE_DEVICE_ONLY,    /*!< @if Eng P2P not negotiated GO.
                                  @else   P2P未协商GO。  @endif */
    P2P_MODE_BUTT,
} p2p_mode_enum;


/**
 * @if Eng
 * @brief  P2P association status.
 * @else
 * @brief  p2p关联状态。
 * @endif
 */
typedef enum {
    P2P_DISCONNECTED,   /*!< @if Eng Disconnected.
                             @else   断连。  @endif */
    P2P_CONNECTED,      /*!< @if Eng Connected.
                             @else   已连接。  @endif */
    P2P_CONNECTING,     /*!< @if Eng Connecting.
                             @else   连接中。  @endif */
    P2P_CONN_STATUS_BUTT,
} p2p_conn_state_enum;


/**
 * @if Eng
 * @brief  WFD-related information structure.
 * @else
 * @brief  WFD相关信息结构体。
 * @endif
 */
typedef struct wfd_info_stru {
    int16_t device_info;    /*!< @if Eng WFD Device Type.
                              @else   WFD设备类型。  @endif */
    int16_t ctrl_port;      /*!< @if Eng Control port number.
                              @else   控制端口号。  @endif */
    int16_t max_throughput; /*!< @if Eng Maximum throughput.
                              @else   最大吞吐量。  @endif */
    int8_t reserved[2];
} wfd_info_stru;


/**
 * @if Eng
 * @brief  P2P scanning result.
 * @else
 * @brief  p2p扫描结果。
 * @endif
 */
typedef struct p2p_device_stru {
    int8_t name[WPS_DEV_NAME_LEN];  /*!< @if Eng Device name.
                                         @else   设备device名称。  @endif */
    uint8_t bssid[WIFI_MAC_LEN];    /*!< @if Eng Device MAC address.
                                         @else   设备device MAC地址。  @endif */
    uint8_t wps_method;             /*!< @if Eng WPS connection mode supported.
                                                 - Bit 0 indicates the PBC,
                                                 - bit 1 indicates the PIN LABEL,
                                                 - bit 2 indicates the PIN DISPLAY,
                                                 - bit 3 indicates the PIN KEYPAD,
                                                 - bit 4 indicates other modes.
                                         @else   支持的WPS连接方式，
                                                 - bit0表示PBC
                                                 - bit1表示PIN LABEL
                                                 - bit2表示PIN DISPLAY
                                                 - bit3表示PIN KEYPAD
                                                 - bit4表示其他方式。  @endif */
    uint8_t is_go           : 1;    /*!< @if Eng Check whether the scanned device is a P2P go device.
                                         @else   扫描出来的设备是否是p2p go。  @endif */
    uint8_t enable_add_group : 1;   /*!< @if Eng Indicates whether to allow GC to be added to the group corresponding
                                                 to the scanned go.
                                         @else   扫描出的go对应的group，是否允许添加gc。  @endif */
    uint8_t reserved       : 6;
    int8_t reserved1[3];
    wfd_info_stru wfd_info;         /*!< @if Eng Not used temporarily.
                                         @else   暂不使用。  @endif */
} p2p_device_stru;


/**
 * @if Eng
 * @brief  P2P association configuration information.
 * @else
 * @brief  p2p关联的配置信息。
 * @endif
 */
typedef struct p2p_config_stru {
    uint8_t bssid[WIFI_MAC_LEN];           /*!< @if Eng Device MAC address of the peer.
                                                @else   peer的device MAC地址。  @endif */
    int8_t pin[WIFI_WPS_PIN_MAX_LEN_NUM];  /*!< @if Eng Pin Code of WPS.
                                                @else   WPS的Pin码。  @endif */
    uint8_t wps_method;                    /*!< @if Eng Connection mode of WPS.
                                                @else   WPS的连接方式。  @endif */
    uint8_t go_intent;                     /*!< @if Eng Go intent, the value ranges from 0 to 15.
                                                @else   go intent，有效范围0~15。  @endif */
    uint8_t persistent;                    /*!< @if Eng 1: Persistent network storage;
                                                        0: not based on permanent network storage.
                                                @else   1:按照永久网络存储;0:不按照永久网络存储。  @endif */
    int8_t reserved[2];
} p2p_config_stru;


/**
 * @if Eng
 * @brief  P2P association status information.
 * @else
 * @brief  p2p设置信息。
 * @endif
 */
typedef struct p2p_status_info_stru {
    int32_t operation_channel;            /*!< @if Eng Center frequency of the working channel after the GO/GC
                                                       connection.
                                                       This parameter is valid when wpa_state is set to CONNECTED.
                                               @else   GO/GC连接之后工作信道的中心频点，在wpa_state为CONNECTED时有效。  @endif */
    uint8_t mode;                         /*!< @if Eng P2P mode, which corresponds to P2pMode.
                                               @else   P2P模式, 和P2pMode对应。  @endif */
    uint8_t wpa_state;                    /*!< @if Eng P2P association status, which corresponds to P2pConnState.
                                               @else   p2p关联状态，和P2pConnState对应.。  @endif */
    int8_t group_ssid[WPS_DEV_NAME_LEN];  /*!< @if Eng Group SSID, valid only when wpa_state is set to CONNECTED
                                                       and P2P WPS succeeds.
                                               @else   Group SSID，并且P2P的WPS关联成功（当前阶段组协商成功）时有效。  @endif */
    uint8_t group_bssid[WIFI_MAC_LEN];    /*!< @if Eng Group BSSID==go bssid, valid only when wpa_state is set to
                                                       CONNECTED and P2P WPS succeeds.
                                               @else   Group BSSID==go bssid，在wpa_state为CONNECTED，
                                                       并且P2P的WPS关联成功（当前阶段组协商成功）时有效。  @endif */
    int8_t reserved[3];
} p2p_status_info_stru;


/**
 * @if Eng
 * @brief  P2P connected GC information.
 * @else
 * @brief  p2p已连接的gc信息。
 * @endif
 */
typedef struct p2p_client_info_stru {
    uint8_t gc_bssid[WIFI_MAC_LEN];           /*!< @if Eng MAC address of the client connected to the GO.
                                                   @else   与go相连的client mac地址。  @endif */
    uint8_t gc_device_bssid[WIFI_MAC_LEN];    /*!< @if Eng Device address of the client connected to the GO.
                                                   @else   与go相连的client dev地址。  @endif */
    int8_t gc_device_name[WPS_DEV_NAME_LEN];  /*!< @if Eng Client device name.
                                                   @else   client 设备名称。  @endif */
    int8_t reserved[3];
} p2p_client_info_stru;


/**
 * @if Eng
 * @brief  P2P device information.
 * @else
 * @brief  p2p设备信息。
 * @endif
 */
typedef struct {
    int8_t dev_name[WPS_DEV_NAME_LEN];      /*!< @if Eng Device name.
                                                 @else   设备名称。  @endif */
    uint8_t wps_method;                     /*!< @if Eng WPS connection mode supported.The options are WpsMethod.
                                                         The options are 0, 1, 2, and 3.
                                                 @else   支持的WPS连接方式，可选项为WpsMethod,仅支持配置0/1/2/3。  @endif */
    int32_t listen_channel;                 /*!< @if Eng Channel for replying to the probe response.
                                                 @else   回复probe response的信道。  @endif */
    int32_t oper_channel;                   /*!< @if Eng Recommended working channel: 0 indicates a random channel.
                                                         A non-zero valid value indicates the recommended channel.
                                                 @else   建议的工作信道：0表示随机信道；非0有效值表示建议的信道。  @endif */
} p2p_device_config_stru;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_P2P_CONFIG_C_H
