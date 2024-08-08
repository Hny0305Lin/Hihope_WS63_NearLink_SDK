/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: driver_soc_ioctl.header
 */

#ifndef __DRIVER_SOC_IOCTL_H__
#define __DRIVER_SOC_IOCTL_H__

#include "driver_soc_common.h"
#include "wifi_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef LOS_CONFIG_WPA_ENTERPRISE
#define EAPOL_PKT_BUF_SIZE 1600
#else
#define EAPOL_PKT_BUF_SIZE 800
#endif /* LOS_CONFIG_WPA_ENTERPRISE */

#ifndef EXT_CHECK_DHCP_TIME
#define EXT_CHECK_DHCP_TIME 30
#endif

#ifndef NETDEV_UP
#define NETDEV_UP 0x0001
#endif

typedef enum {
	EXT_CHAN_WIDTH_20_NOHT,
	EXT_CHAN_WIDTH_20,
	EXT_CHAN_WIDTH_40,

	EXT_CHAN_WIDTH_BUTT
} ext_chan_width_enum;
typedef uint8 ext_chan_width_enum_uint8;

typedef enum {
	EXT_WPA_VERSION_1 = 1 << 0,
	EXT_WPA_VERSION_2 = 1 << 1,
} ext_wpa_versions_enum;
typedef uint8 ext_wpa_versions_enum_uint8;

typedef enum {
	EXT_DISCONNECT,
	EXT_CONNECT,
} ext_connect_status_enum;
typedef uint8 ext_connect_status_enum_uint8;

int32 drv_soc_eapol_packet_send(const int8  *ifname, const uint8 *src_addr, const uint8 *dst_addr,
                                const uint8 *buf, uint32 length);
int32 drv_soc_eapol_packet_receive(const int8 *ifname, ext_rx_eapol_stru *rx_eapol);
int32 drv_soc_eapol_enable(const int8 *ifname, void (*notify_callback)(void *, void *context), void *context);
int32 drv_soc_eapol_disable(const int8 *ifname);
int32 drv_soc_ioctl_set_ap(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_change_beacon(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_send_mlme(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_new_key(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_del_key(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_set_key(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_set_mode(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_get_mode(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_del_beacon(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_set_csi_config(const int8 *ifname, const void *csi_config);
int32 drv_soc_ioctl_csi_switch(const int8 *ifname, const void *switch_flag);

#ifdef CONFIG_ACS
int32 drv_soc_ioctl_get_survey(const int8 *ifname, const void *buf);
#endif
#ifdef CONFIG_IEEE80211R
int32 drv_soc_ioctl_update_ft_ies(const int8 *ifname, const void *buf);
#endif /* CONFIG_IEEE80211R */
int32 drv_soc_ioctl_get_own_mac(const int8 *ifname, const int8 *mac_addr);
int32 drv_soc_ioctl_get_hw_feature(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_scan(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_disconnet(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_assoc(const int8 *ifname, const void *buf);
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
extern int32 drv_soc_ioctl_set_rekey_info(const int8 *ifname, const void *buf);
#endif
int32 drv_soc_ioctl_set_max_sta_num(const int8 *ifname, const void *buf_max_sta_num);
int32 drv_soc_ioctl(const int8 *ifname, const ext_ioctl_command_stru *ioctl_cmd);
int32 drv_soc_ioctl_set_netdev(const int8 *ifname, const ext_bool_enum_uint8 *netdev);
int32 drv_soc_ioctl_ip_notify_driver(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_set_pm_on(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_sta_remove(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_mesh_usr_add(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_get_drv_flags(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_set_delay_report(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_send_ext_auth_status(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_update_dh_ie(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_send_action(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_mesh_set_mgtk(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_mesh_enable_flag(const int8 *ifname, enum ext_mesh_enable_flag_type flag_type,
                                     const void *buf_addr);
int32 drv_soc_ioctl_set_usr_app_ie(const int8 *ifname, const void *usr_app_ie);
int32 drv_soc_ioctl_get_mac_by_iftype(const int8 *ifname, void *buf);
#ifdef LOS_CONFIG_P2P
#ifdef CONFIG_WPS_AP
int32 drv_soc_ioctl_set_ap_wps_p2p_ie(const int8 *ifname, const void *buf);
#endif
int32 drv_soc_ioctl_probe_request_report(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_remain_on_channel(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_cancel_remain_on_channel(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_add_if(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_remove_if(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_get_p2p_addr(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_set_p2p_noa(const int8 *ifname, const void *buf_addr);
int32 drv_soc_ioctl_set_p2p_powersave(const int8 *ifname, const void *buf_addr);
#endif /* LOS_CONFIG_P2P */

int32 drv_soc_ioctl_add_pmkid(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_remove_pmkid(const int8 *ifname, const void *buf);
int32 drv_soc_ioctl_flush_pmkid(const int8 *ifname, const void *buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of driver_soc_ioctl.h */
