/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for wal_net.c.
 */

#ifndef __WAL_NET_H__
#define __WAL_NET_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hmac_ext_if.h"
#include "oam_ext_if.h"
#include "wal_main.h"
#include "soc_wifi_api.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/** Ask if a drvier is ready to send */
#define WAL_SIOCDEVPRIVATE              0x89F0  /* SIOCDEVPRIVATE */
#define WAL_ADDR_MAX                    16

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
typedef enum {
    WAL_PHY_MODE_11N = 0,
    WAL_PHY_MODE_11G = 1,
    WAL_PHY_MODE_11B = 2,
    WAL_PHY_MODE_11AX = 3,
    WAL_PHY_MODE_BUTT
}wal_phy_mode;

typedef enum {
    WAL_ADDR_IDX_STA0 = 0,
    WAL_ADDR_IDX_AP0 = 1,
    WAL_ADDR_IDX_STA1 = 2,
    WAL_ADDR_IDX_STA2 = 3,
    WAL_ADDR_IDX_BUTT
}wal_addr_idx;

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct {
    td_u8   ac_addr[WLAN_MAC_ADDR_LEN];
    td_u16  status;
}wal_dev_addr_stru;

typedef struct {
    td_u8   enable;         /* 开关标志,配置WPA重关联,驱动延时上报LWIP机制开启 */
    td_u8   reconn;         /* WPA自动重连标志 */
    td_u8   resv[2];        /* 2: 四字节对齐用 */
    frw_timeout_stru  delay_timer;
}wal_delay_report_stru;

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
} wal_protocol_mode_enum;
/*****************************************************************************
  10 函数声明
*****************************************************************************/
oal_netbuf_stru *hwal_lwip_skb_alloc(const oal_net_device_stru *netdev, td_u16 lwip_buflen);
td_u32 hwal_skb_struct_free(oal_netbuf_stru *sk_buf);
oal_netbuf_stru *hwal_skb_struct_alloc(td_void);
#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
td_u32 hwal_pbuf_convert_2_skb(oal_lwip_buf *lwip_buf, oal_netbuf_stru **sk_buf);
#else
td_u32 hwal_pbuf_convert_2_skb(oal_lwip_buf *lwip_buf, oal_netbuf_stru *sk_buf);
#endif
oal_lwip_buf *hwal_skb_convert_2_pbuf(const oal_netbuf_stru *sk_buf);
td_void hwal_lwip_receive(oal_lwip_netif *netif, const oal_netbuf_stru *drv_buf);
td_u32 hwal_lwip_notify(const oal_lwip_netif *netif, td_u32 notify_type);
td_u32 wal_netdev_stop(oal_net_device_stru *netdev);
td_u32 wal_netdev_open(oal_net_device_stru *netdev);
td_void wal_netif_set_down(struct netif *netif, oal_net_device_stru *netdev);
td_void wal_netif_set_up(struct netif *netif, oal_net_device_stru *netdev);
osal_s32 wal_report_sta_assoc_info(hmac_vap_stru *hmac_vap, frw_msg *msg);
td_u8 wal_dev_is_running(td_void);
td_s32 wal_init_drv_wlan_netdev(nl80211_iftype_uint8 type, wal_protocol_mode_enum mode, td_char *ifname, td_u32 *len);
td_s32 wal_deinit_drv_wlan_netdev(const td_char *ifname);
td_void set_past_net_device_by_index(td_u32 netdev_index, oal_net_device_stru *netdev);
td_void wal_set_delay_report_config(td_u8 enable, td_u16 timeout);
td_u32 wal_check_auto_conn_status(oal_net_device_stru *netdev, td_u8 auto_reconn, td_u8 *bssid);
td_u32 hwal_lwip_register(oal_net_device_stru *netdev, oal_ip_addr_t *ip, oal_ip_addr_t *netmask, oal_ip_addr_t *gw);
td_void hwal_lwip_unregister_netdev(oal_net_device_stru *netdev);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
