/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: driver_soc_ioctl
 */

#include "utils/common.h"
#include "driver_soc_ioctl.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static int32 drv_soc_ioctl_command_set(const int8 *ifname, const void *buf, unsigned int cmd)
{
	ext_ioctl_command_stru ioctl_cmd;
	(void)memset_s(&ioctl_cmd, sizeof(ext_ioctl_command_stru), 0, sizeof(ext_ioctl_command_stru));
	ioctl_cmd.cmd = cmd;
	ioctl_cmd.buf = buf;
	return drv_soc_ioctl(ifname, &ioctl_cmd);
}

int32 drv_soc_eapol_packet_send(const int8  *ifname, const uint8 *src_addr, const uint8 *dst_addr,
                                const uint8 *buf, uint32 length)
{
	ext_tx_eapol_stru tx_eapol;
	(void)src_addr;
	(void)dst_addr;
	(void)memset_s(&tx_eapol, sizeof(ext_tx_eapol_stru), 0, sizeof(ext_tx_eapol_stru));
	tx_eapol.puc_buf = buf;
	tx_eapol.ul_len  = length;
	return drv_soc_ioctl_command_set(ifname, &tx_eapol, EXT_IOCTL_SEND_EAPOL);
}

int32 drv_soc_eapol_packet_receive(const int8 *ifname, ext_rx_eapol_stru *rx_eapol)
{
	if (rx_eapol == NULL)
		return -EXT_EFAIL;
	rx_eapol->buf = os_zalloc(EAPOL_PKT_BUF_SIZE);
	if (rx_eapol->buf == EXT_PTR_NULL) {
		return -EXT_EFAIL;
	}
	rx_eapol->len = EAPOL_PKT_BUF_SIZE;
	return drv_soc_ioctl_command_set(ifname, rx_eapol, EXT_IOCTL_RECEIVE_EAPOL);
}

int32 drv_soc_eapol_enable(const int8 *ifname, void (*notify_callback)(void *, void *), void *context)
{
	ext_enable_eapol_stru enable_eapol;
	(void)memset_s(&enable_eapol, sizeof(ext_enable_eapol_stru), 0, sizeof(ext_enable_eapol_stru));
	enable_eapol.callback = notify_callback;
	enable_eapol.contex   = context;

	return drv_soc_ioctl_command_set(ifname, &enable_eapol, EXT_IOCTL_ENALBE_EAPOL);
}

int32 drv_soc_eapol_disable(const int8 *ifname)
{
	return drv_soc_ioctl_command_set(ifname, ifname, EXT_IOCTL_DISABLE_EAPOL);
}

int32 drv_soc_ioctl_get_own_mac(const int8 *ifname, const int8 *mac_addr)
{
	return drv_soc_ioctl_command_set(ifname, mac_addr, HIIS_IOCTL_GET_ADDR);
}

#ifdef CONFIG_ACS
int32 drv_soc_ioctl_get_survey(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_GET_SURVEY);
}
#endif

#ifdef CONFIG_IEEE80211R
int32 drv_soc_ioctl_update_ft_ies(const int8 *ifname, const void *buf)
{
    return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_UPDATE_FT_IES);
}
#endif /* CONFIG_IEEE80211R */

int32 drv_soc_ioctl_set_ap(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SET_AP);
}

int32 drv_soc_ioctl_change_beacon(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_CHANGE_BEACON);
}

int32 drv_soc_ioctl_get_hw_feature(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, HIIS_IOCTL_GET_HW_FEATURE);
}

int32 drv_soc_ioctl_send_mlme(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SEND_MLME);
}

int32 drv_soc_ioctl_new_key(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_NEW_KEY);
}

int32 drv_soc_ioctl_del_key(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_DEL_KEY);
}

int32 drv_soc_ioctl_set_key(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SET_KEY);
}

int32 drv_soc_ioctl_set_mode(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SET_MODE);
}

int32 drv_soc_ioctl_get_mode(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_GET_MODE);
}

int32 drv_soc_ioctl_del_beacon(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_DEL_BEACON);
}

int32 drv_soc_ioctl_set_netdev(const int8 *ifname, const ext_bool_enum_uint8 *netdev)
{
	return drv_soc_ioctl_command_set(ifname, netdev, EXT_IOCTL_SET_NETDEV);
}

int32 drv_soc_ioctl_scan(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SCAN);
}
int32 drv_soc_ioctl_wifi_app_service_handle(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_WIFI_APP_SERVICE);
}

int32 drv_soc_ioctl_disconnet(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_DISCONNET);
}

int32 drv_soc_ioctl_assoc(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_ASSOC);
}

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
int32 drv_soc_ioctl_set_rekey_info(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SET_REKEY_INFO);
}
#endif

int32 drv_soc_ioctl_send_action(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SEND_ACTION);
}

int32 drv_soc_ioctl_mesh_usr_add(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, buf_addr, EXT_IOCTL_SET_MESH_USER);
}

int32 drv_soc_ioctl_mesh_set_mgtk(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, buf_addr, EXT_IOCTL_SET_MESH_GTK);
}

static int32 drv_soc_get_cmd_from_mesh_flag(enum ext_mesh_enable_flag_type flag_type)
{
	int32 cmd_list[] = {(int32)EXT_IOCTL_EN_ACCEPT_PEER, (int32)EXT_IOCTL_EN_ACCEPT_STA};
	return cmd_list[flag_type];
}

int32 drv_soc_ioctl_mesh_enable_flag(const int8 *ifname, enum ext_mesh_enable_flag_type flag_type,
                                     const void *buf_addr)
{
	if (flag_type >= EXT_MESH_ENABLE_FLAG_BUTT) {
		return -EXT_EFAIL;
	}

	return drv_soc_ioctl_command_set(ifname, buf_addr, (unsigned int)drv_soc_get_cmd_from_mesh_flag(flag_type));
}

int32 drv_soc_ioctl_set_csi_config(const int8 *ifname, const void *csi_config)
{
    return drv_soc_ioctl_command_set(ifname, csi_config, EXT_IOCTL_SET_CSI_CONFIG);
}

 int32 drv_soc_ioctl_csi_switch(const int8 *ifname, const void *switch_flag)
 {
     return drv_soc_ioctl_command_set(ifname, switch_flag, EXT_IOCTL_CSI_SWITCH);
 }

 int32 drv_soc_ioctl_set_usr_app_ie(const int8 *ifname, const void *usr_app_ie)
{
	return drv_soc_ioctl_command_set(ifname, usr_app_ie, EXT_IOCTL_SET_USR_APP_IE);
}

int32 drv_soc_ioctl_sta_remove(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, buf_addr, EXT_IOCTL_STA_REMOVE);
}

#ifdef CONFIG_WPS_AP
int32 drv_soc_ioctl_set_ap_wps_p2p_ie(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf, EXT_IOCTL_SET_AP_WPS_P2P_IE);
}
#endif

#ifdef LOS_CONFIG_P2P
int32 drv_soc_ioctl_probe_request_report(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_PROBE_REQUEST_REPORT);
}

int32 drv_soc_ioctl_remain_on_channel(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_REMAIN_ON_CHANNEL);
}

int32 drv_soc_ioctl_cancel_remain_on_channel(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_CANCEL_REMAIN_ON_CHANNEL);
}

int32 drv_soc_ioctl_add_if(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_ADD_IF);
}

int32 drv_soc_ioctl_remove_if(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_REMOVE_IF);
}

int32 drv_soc_ioctl_get_p2p_addr(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_GET_P2P_MAC_ADDR);
}

int32 drv_soc_ioctl_set_p2p_noa(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_SET_P2P_NOA);
}

int32 drv_soc_ioctl_set_p2p_powersave(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, (void *)buf_addr, EXT_IOCTL_SET_P2P_POWERSAVE);
}
#endif /* LOS_CONFIG_P2P */

int32 drv_soc_ioctl_get_drv_flags(const int8 *ifname, const void *buf_addr)
{
	return drv_soc_ioctl_command_set(ifname, buf_addr, EXT_IOCTL_GET_DRIVER_FLAGS);
}

int32 drv_soc_ioctl_set_delay_report(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_DELAY_REPORT);
}

int32 drv_soc_ioctl_send_ext_auth_status(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_SEND_EXT_AUTH_STATUS);
}

int32 drv_soc_ioctl_update_dh_ie(const int8 *ifname, const void *buf)
{
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_UPDATE_DH_IE);
}

int32 drv_soc_ioctl_add_pmkid(const int8 *ifname, const void *buf)
{
	wpa_error_log0(MSG_DEBUG, "drv_soc_ioctl_add_pmkid");
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_ADD_PMKID);
}

int32 drv_soc_ioctl_remove_pmkid(const int8 *ifname, const void *buf)
{
	wpa_error_log0(MSG_DEBUG, "drv_soc_ioctl_remove_pmkid");
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_DEL_PMKID);
}

int32 drv_soc_ioctl_flush_pmkid(const int8 *ifname, const void *buf)
{
	wpa_error_log0(MSG_DEBUG, "drv_soc_ioctl_flush_pmkid");
	return drv_soc_ioctl_command_set(ifname, buf, EXT_IOCTL_FLUSH_PMKID);
}

int32 drv_soc_ioctl(const int8 *ifname, const ext_ioctl_command_stru *ioctl_cmd)
{
	int32 ret;
	if (ioctl_cmd == NULL)
		return -EXT_EFAIL;
	wpa_error_log1(MSG_DEBUG, "drv_soc_ioctl ioctl_cmd->cmd=%u.", ioctl_cmd->cmd);
	osal_printk("drv_soc_ioctl ioctl_cmd->cmd=%u.\r\n", ioctl_cmd->cmd);

	ret = drv_soc_hwal_wpa_ioctl((int8 *)ifname, ioctl_cmd);
	if (ret != EXT_SUCC) {
		if ((ret == (-EXT_EINVAL)) && (ioctl_cmd->cmd == EXT_IOCTL_RECEIVE_EAPOL)) {
			/* When receiving eapol message, the last empty message will be received, which is a normal phenomenon */
			wpa_warning_log2(MSG_DEBUG, "hwal_wpa_ioctl ioctl_cmd->cmd=%u, lret=%d.", ioctl_cmd->cmd, ret);
		} else {
			/* When issuing the del key command, if the driver has deleted the user resource,
			   it will return an error, which is a normal phenomenon */
			if (ioctl_cmd->cmd != EXT_IOCTL_DEL_KEY) {
				wpa_error_log2(MSG_DEBUG, "hwal_wpa_ioctl ioctl_cmd->cmd=%u, lret=%d.", ioctl_cmd->cmd, ret);
			}
		}
		return -EXT_EFAIL;
	}
	return EXT_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
