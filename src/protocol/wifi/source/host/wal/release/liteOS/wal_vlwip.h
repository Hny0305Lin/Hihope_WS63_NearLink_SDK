/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for wal_vlwip.c
 * Create: 2019-08-04
 */

#ifndef __WAL_VLWIP_H__
#define __WAL_VLWIP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_VLWIP
typedef enum {
    WIFI_FILTER_NETIF_LWIP             = 0,
    WIFI_FILTER_NETIF_VLWIP            = 1,
    WIFI_FILTER_NETIF_BOTH             = 2,
    WIFI_FILTER_NETIF_BUTT
} wifi_filter_netif_enum;

typedef td_s32 (*driverif_input_func)(const struct netif *netif, struct pbuf *pbuf);
int uapi_vlwip_set_debug_callback(driverif_input_func cb);
td_u32 wal_vlwip_register_netdev(oal_net_device_stru *netdev);
td_void wal_vlwip_unregister_netdev(oal_net_device_stru *netdev);
td_u8 wal_vlwip_get_netif_type(td_void);
td_void hwal_packet_receive(oal_net_device_stru *netdev, oal_netbuf_stru *netbuf);

#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end __WAL_VLWIP_H__ */

