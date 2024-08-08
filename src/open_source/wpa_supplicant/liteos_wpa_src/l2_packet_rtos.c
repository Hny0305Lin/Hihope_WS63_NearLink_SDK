/*
 * WPA Supplicant - Layer2 packet handling with Linux packet sockets
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/common.h"
#include "utils/eloop.h"
#ifdef CONFIG_DRIVER_SOC
#include "driver_soc_ioctl.h"
#endif /* CONFIG_DRIVER_SOC */
#include "securec.h"

struct l2_packet_data {
	struct dl_list list;
	struct l2_packet_netif *netif;

	u16 protocol;
	void (*rx_callback)(void *ctx, const u8 *src_addr, const u8 *buf, size_t len);
	void *rx_callback_ctx;
	int l2_hdr; /* whether to include layer 2 (Ethernet) header data
				 * buffers */
};

struct l2_packet_netif {
	struct dl_list list;
	struct dl_list datalist;

	void *eloop_event;
	char ifname[IFNAMSIZ + 1];
	u8 own_addr[ETH_ALEN];
};

static struct dl_list g_l2_packet_list = {0};

int l2_packet_get_own_addr(const struct l2_packet_data *l2, u8 *addr, size_t len)
{
	(void)len;
	if (l2 == NULL || l2->netif == NULL || addr == NULL)
		return -1;
	(void)os_memcpy(addr, l2->netif->own_addr, sizeof(l2->netif->own_addr));

	return 0;
}

int l2_packet_send(const struct l2_packet_data *l2, const u8 *dst_addr, u16 proto,
		   const u8 *buf, size_t len)
{
	int ret = 0;

	(void)proto;
	if (l2 == NULL || l2->netif == NULL)
		return -1;
#ifdef CONFIG_DRIVER_SOC
	ret = drv_soc_eapol_packet_send(l2->netif->ifname, l2->netif->own_addr, dst_addr, (unsigned char *)buf, len);
#endif /* CONFIG_DRIVER_SOC */
	return ret;
}

static void l2_packet_receive(void *eloop_ctx, void *sock_ctx)
{
	struct l2_packet_netif *netif = eloop_ctx;
	struct l2_packet_data *tmp = NULL;
	struct l2_packet_data *nxt = NULL;
	unsigned short ether_type;

	(void)sock_ctx;
	if ((netif == NULL) || (netif->eloop_event == NULL)) {
		return;
	}

#ifdef CONFIG_DRIVER_SOC
	ext_rx_eapol_stru st_rx_eapol;
	unsigned char *puc_src = NULL;

    /* eloop_post_event is invoked each time an eapol frame is received.
       Therefore, eloop_post_event is called to release the memory applied for by eloop_post_event
       when eapol data is read from the driver code. */
    (void)eloop_read_event(netif->eloop_event, 0);
	/* Callback is called only once per multiple packets, drain all of them */
	while (drv_soc_eapol_packet_receive(netif->ifname, &st_rx_eapol) == EXT_SUCC) {
		puc_src = (unsigned char *)(st_rx_eapol.buf + ETH_ALEN);
		ether_type = *(unsigned short *)(st_rx_eapol.buf + ETH_ALEN + ETH_ALEN);

		dl_list_for_each_safe(tmp, nxt, &netif->datalist, struct l2_packet_data, list) {
			if (ntohs(ether_type) != tmp->protocol) {
				continue;
			}
			if (tmp->rx_callback != NULL && st_rx_eapol.len <= EAPOL_PKT_BUF_SIZE) {
				tmp->rx_callback(tmp->rx_callback_ctx, puc_src, st_rx_eapol.buf, st_rx_eapol.len);
			}
			break;
		}

		os_free(st_rx_eapol.buf);
		st_rx_eapol.buf = NULL;
	}

	if (st_rx_eapol.buf != NULL) {
		os_free(st_rx_eapol.buf);
		st_rx_eapol.buf = NULL;
	}
#endif /* CONFIG_DRIVER_SOC */
}

static void l2_packet_eapol_callback(void *ctx, void *context)
{
	struct l2_packet_netif *netif = (struct l2_packet_netif *)context;
	(void)ctx;

	if (netif == NULL)
		return;

	(void)eloop_post_event(netif->eloop_event, NULL, 1);
}

struct l2_packet_data * l2_packet_init(
	const char *ifname, const u8 *own_addr, unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr, const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	struct l2_packet_data *l2 = NULL;
	struct l2_packet_netif *netif = NULL;
	struct l2_packet_netif *tmp = NULL;

	(void)own_addr;
	if (ifname == NULL)
		return NULL;

	/* Init for device environment */
	if (g_l2_packet_list.next == NULL || g_l2_packet_list.prev == NULL) {
		dl_list_init(&g_l2_packet_list);
	}

	l2 = os_zalloc(sizeof(struct l2_packet_data));
	if (l2 == NULL) {
		return NULL;
	}

	if (dl_list_len(&g_l2_packet_list) != 0) {
		dl_list_for_each(tmp, &g_l2_packet_list, struct l2_packet_netif, list) {
			if (os_strcmp(ifname, tmp->ifname) == 0) {
				netif = tmp;
				break;
			}
		}
	}

	if (netif == NULL) {
		netif = os_zalloc(sizeof(struct l2_packet_netif));
		if (netif == NULL) {
			os_free(l2);
			return NULL;
		}
		if (os_strlcpy(netif->ifname, ifname, sizeof(netif->ifname)) >= sizeof(netif->ifname)) {
			os_free(netif);
			os_free(l2);
			wpa_error_log0(MSG_ERROR, "l2_packet_init strcpy_s failed.");
			return NULL;
		}
		(void)eloop_register_event(&netif->eloop_event, sizeof(void *), l2_packet_receive, netif, NULL);
		if (netif->eloop_event == NULL) {
			os_free(netif);
			os_free(l2);
			wpa_error_log0(MSG_ERROR, "l2_packet_init eloop register failed.");
			return NULL;
		}
#ifdef CONFIG_DRIVER_SOC
		(void)drv_soc_eapol_enable(netif->ifname, l2_packet_eapol_callback, netif);
		(void)drv_soc_ioctl_get_own_mac(netif->ifname, (char *)netif->own_addr);
#endif /* CONFIG_DRIVER_SOC */

		dl_list_init(&netif->datalist);

		dl_list_add_tail(&g_l2_packet_list, &netif->list);
	}

	l2->netif = netif;
	l2->protocol = protocol;
	l2->rx_callback = rx_callback;
	l2->rx_callback_ctx = rx_callback_ctx;
	l2->l2_hdr = l2_hdr;
	dl_list_add_tail(&netif->datalist, &l2->list);
	return l2;
}

struct l2_packet_data * l2_packet_init_bridge(
	const char *br_ifname, const char *ifname, const u8 *own_addr,
	unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr, const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	(void)ifname;
	return l2_packet_init(br_ifname, own_addr, protocol, rx_callback,
						  rx_callback_ctx, l2_hdr);
}

void l2_packet_deinit(struct l2_packet_data *l2)
{
	struct l2_packet_netif *netif = NULL;

	if (l2 == NULL || l2->netif == NULL) {
		return;
	}

	netif = l2->netif;
	dl_list_del(&l2->list);
	os_free(l2);
	l2 = NULL;

	if (dl_list_len(&netif->datalist) == 0) {
		if (netif->eloop_event != NULL) {
			eloop_unregister_event(netif->eloop_event, 0); // 0:useless
			netif->eloop_event = NULL;
		}
#ifdef CONFIG_DRIVER_SOC
		(void)drv_soc_eapol_disable(netif->ifname);
#endif /* CONFIG_DRIVER_SOC */
		dl_list_del(&netif->list);
		os_free(netif);
	}
}

int l2_packet_get_ip_addr(struct l2_packet_data *l2, char *buf, size_t len)
{
	(void)l2;
	(void)buf;
	(void)len;
	return 0;
}

void l2_packet_notify_auth_start(struct l2_packet_data *l2)
{
	(void)l2;
}
