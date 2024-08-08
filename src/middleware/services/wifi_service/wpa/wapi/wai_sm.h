/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
 */

#ifndef WAI_SM_H
#define WAI_SM_H

int wai_sm_process(struct wpa_supplicant *wpa, const struct wpabuf *waibuf);
int wapi_psk_to_bk(struct wpa_supplicant *wpa, const struct wpa_ssid *ssid);

#endif /* end of wai_sm.h */
