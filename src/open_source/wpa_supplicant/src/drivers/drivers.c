/*
 * Driver interface list
 * Copyright (c) 2004-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "driver.h"
#ifdef CONFIG_LITEOS_WPA
#ifdef CONFIG_DRIVER_WEXT
extern struct wpa_driver_ops wpa_driver_wext_ops; /* driver_wext.c */
#endif /* CONFIG_DRIVER_WEXT */
#ifdef CONFIG_DRIVER_NL80211
extern struct wpa_driver_ops wpa_driver_nl80211_ops; /* driver_nl80211.c */
#endif /* CONFIG_DRIVER_NL80211 */
#ifdef CONFIG_DRIVER_HOSTAP
extern struct wpa_driver_ops wpa_driver_hostap_ops; /* driver_hostap.c */
#endif /* CONFIG_DRIVER_HOSTAP */
#ifdef CONFIG_DRIVER_MADWIFI
extern struct wpa_driver_ops wpa_driver_madwifi_ops; /* driver_madwifi.c */
#endif /* CONFIG_DRIVER_MADWIFI */
#ifdef CONFIG_DRIVER_BSD
extern struct wpa_driver_ops wpa_driver_bsd_ops; /* driver_bsd.c */
#endif /* CONFIG_DRIVER_BSD */
#ifdef CONFIG_DRIVER_OPENBSD
extern struct wpa_driver_ops wpa_driver_openbsd_ops; /* driver_openbsd.c */
#endif /* CONFIG_DRIVER_OPENBSD */
#ifdef CONFIG_DRIVER_NDIS
extern struct wpa_driver_ops wpa_driver_ndis_ops; /* driver_ndis.c */
#endif /* CONFIG_DRIVER_NDIS */
#ifdef CONFIG_DRIVER_WIRED
extern struct wpa_driver_ops wpa_driver_wired_ops; /* driver_wired.c */
#endif /* CONFIG_DRIVER_WIRED */
#ifdef CONFIG_DRIVER_MACSEC_QCA
 /* driver_macsec_qca.c */
extern struct wpa_driver_ops wpa_driver_macsec_qca_ops;
#endif /* CONFIG_DRIVER_MACSEC_QCA */
#ifdef CONFIG_DRIVER_TEST
extern struct wpa_driver_ops wpa_driver_test_ops; /* driver_test.c */
#endif /* CONFIG_DRIVER_TEST */
#ifdef CONFIG_DRIVER_ROBOSWITCH
/* driver_roboswitch.c */
extern struct wpa_driver_ops wpa_driver_roboswitch_ops;
#endif /* CONFIG_DRIVER_ROBOSWITCH */
#ifdef CONFIG_DRIVER_ATHEROS
extern struct wpa_driver_ops wpa_driver_atheros_ops; /* driver_atheros.c */
#endif /* CONFIG_DRIVER_ATHEROS */
#ifdef CONFIG_DRIVER_AR6003
extern struct wpa_driver_ops wpa_driver_ar6003_ops; /* driver_arwc.c */
#endif /* CONFIG_DRIVER_AR6003 */
#ifdef CONFIG_DRIVER_REALTEK
extern struct wpa_driver_ops wpa_driver_realtek;
#endif
#ifdef CONFIG_DRIVER_MEDIATEK
extern struct wpa_driver_ops wpa_driver_mediatek_ops;
#endif
#ifdef CONFIG_DRIVER_NONE
extern struct wpa_driver_ops wpa_driver_none_ops; /* driver_none.c */
#endif /* CONFIG_DRIVER_NONE */
#ifdef CONFIG_DRIVER_SOC
extern struct wpa_driver_ops wpa_driver_ext_ops;
#endif /* CONFIG_DRIVER_SOC */
#endif

const struct wpa_driver_ops *const wpa_drivers[] =
{
#ifdef CONFIG_DRIVER_NL80211
	&wpa_driver_nl80211_ops,
#endif /* CONFIG_DRIVER_NL80211 */
#ifdef CONFIG_DRIVER_WEXT
	&wpa_driver_wext_ops,
#endif /* CONFIG_DRIVER_WEXT */
#ifdef CONFIG_DRIVER_HOSTAP
	&wpa_driver_hostap_ops,
#endif /* CONFIG_DRIVER_HOSTAP */
#ifdef CONFIG_DRIVER_MADWIFI
	&wpa_driver_madwifi_ops,
#endif /* CONFIG_DRIVER_MADWIFI */
#ifdef CONFIG_DRIVER_BSD
	&wpa_driver_bsd_ops,
#endif /* CONFIG_DRIVER_BSD */
#ifdef CONFIG_DRIVER_OPENBSD
	&wpa_driver_openbsd_ops,
#endif /* CONFIG_DRIVER_OPENBSD */
#ifdef CONFIG_DRIVER_NDIS
	&wpa_driver_ndis_ops,
#endif /* CONFIG_DRIVER_NDIS */
#ifdef CONFIG_DRIVER_WIRED
	&wpa_driver_wired_ops,
#endif /* CONFIG_DRIVER_WIRED */
#ifndef EXT_CODE_CROP
#ifdef CONFIG_DRIVER_MACSEC_LINUX
	&wpa_driver_macsec_linux_ops,
#endif /* CONFIG_DRIVER_MACSEC_LINUX */
#endif /* EXT_CODE_CROP */
#ifdef CONFIG_DRIVER_MACSEC_QCA
	&wpa_driver_macsec_qca_ops,
#endif /* CONFIG_DRIVER_MACSEC_QCA */
#ifdef CONFIG_DRIVER_ROBOSWITCH
	&wpa_driver_roboswitch_ops,
#endif /* CONFIG_DRIVER_ROBOSWITCH */
#ifdef CONFIG_DRIVER_ATHEROS
	&wpa_driver_atheros_ops,
#endif /* CONFIG_DRIVER_ATHEROS */
#ifdef CONFIG_DRIVER_SOC
	&wpa_driver_ext_ops,
#endif /* CONFIG_DRIVER_SOC */
#ifdef CONFIG_DRIVER_NONE
	&wpa_driver_none_ops,
#endif /* CONFIG_DRIVER_NONE */
	NULL
};
