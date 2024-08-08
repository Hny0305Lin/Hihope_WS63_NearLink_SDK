/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Network adaptation implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_GetLocalIp(char *localIp, unsigned char len)
{
    return app_call2(APP_CALL_HILINK_GET_LOCAL_IP, int, char *, localIp, unsigned char, len);
}

int HILINK_GetMacAddr(unsigned char *mac, unsigned char len)
{
    return app_call2(APP_CALL_HILINK_GET_MAC_ADDR, int, unsigned char *, mac, unsigned char, len);
}

int HILINK_GetWiFiSsid(char *ssid, unsigned int *ssidLen)
{
    return app_call2(APP_CALL_HILINK_GET_WIFI_SSID, int, char *, ssid, unsigned int *, ssidLen);
}

int HILINK_SetWiFiInfo(const char *ssid, unsigned int ssidLen, const char *pwd, unsigned int pwdLen)
{
    return app_call4(APP_CALL_HILINK_SET_WIFI_INFO, int,
        const char *, ssid, unsigned int, ssidLen, const char *, pwd, unsigned int, pwdLen);
}

void HILINK_ReconnectWiFi(void)
{
    app_call0(APP_CALL_HILINK_RECONNECT_WIFI, void);
}

int HILINK_ConnectWiFi(void)
{
    return app_call0(APP_CALL_HILINK_CONNECT_WIFI, int);
}

int HILINK_GetNetworkState(int *state)
{
    return app_call1(APP_CALL_HILINK_GET_NETWORK_STATE, int, int *, state);
}

int HILINK_GetWiFiBssid(unsigned char *bssid, unsigned char *bssidLen)
{
    return app_call2(APP_CALL_HILINK_GET_WIFI_BSSID, int, unsigned char *, bssid, unsigned char *, bssidLen);
}

int HILINK_GetWiFiRssi(signed char *rssi)
{
    return app_call1(APP_CALL_HILINK_GET_WIFI_RSSI, int, signed char *, rssi);
}