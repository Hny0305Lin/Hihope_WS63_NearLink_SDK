/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: driver_soc header
 */

#ifndef DRIVER_EXT_H
#define DRIVER_EXT_H

#include "drivers/driver.h"
#include "driver_soc_common.h"
#include "wifi_api.h"

struct modes {
	int32 modes_num_rates;
	int32 mode;
};

typedef struct _ext_driver_data_stru {
	struct hostapd_data *hapd;
	const int8 iface[IFNAMSIZ + 1];
	int8 resv[3];
	uint64 send_action_cookie;
	void *ctx;
	void *event_queue;
	ext_iftype_enum_uint8 nlmode;
	struct l2_packet_data *eapol_sock;  /* EAPOL message sending and receiving channel */
	uint8 own_addr[ETH_ALEN];
	uint8 resv1[2];
	uint32 associated;
	uint8 bssid[ETH_ALEN];
	uint8 ssid[MAX_SSID_LEN];
	uint8 resv2[2];
	uint32 ssid_len;
	struct wpa_scan_res *res[SCAN_AP_LIMIT];
	uint32 scan_ap_num;
	uint32 beacon_set;
} ext_driver_data_stru;

typedef struct _ext_cmd_stru {
	int32 cmd;
	const struct wpabuf *src;
} ext_cmd_stu;

extern int wal_init_drv_wlan_netdev(ext_wifi_iftype type, protocol_mode_enum en_mode, char *ifname, int *len);
extern int wal_deinit_drv_wlan_netdev(const char *ifname);
#ifdef CONFIG_MESH
int32 drv_soc_mesh_enable_flag(const char *ifname, enum ext_mesh_enable_flag_type flag_type, uint8 enable_flag);
#endif /* CONFIG_MESH */
int32 drv_soc_set_usr_app_ie(const char *ifname, uint8 set, wifi_extend_ie_index ie_index,
                             const unsigned char frame_type_bitmap, const uint8 *ie, uint16 ie_len);
int32 drv_soc_set_csi_config(const char *ifname, const ext_csi_config *config);
int32 drv_soc_csi_switch(const char *ifname, const int *switch_flag);
#endif /* DRIVER_EXT_H */
