/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: Function related to wifi aware(NAN) feature.
 * Create: 2021-04-16
 */

#ifndef __WAL_CONFIG_SDP_H__
#define __WAL_CONFIG_SDP_H__

#include "hmac_ext_if.h"
#include "frw_hmac.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WAL_SDP_EN_SC 2  /* enable sdp and switch channel test */
#define WAL_SDP_ENABLE 1  /* enable sdp */
#define WAL_SDP_DISABLE 0 /* disable sdp */
#define WAL_LOCAL_HANDLE_LOW 1
#define WAL_LOCAL_HANDLE_HIGH 255

#define WAL_SDP_BEACON_SWITCH_ENABLE 1  /* enable beacon switch */
#define WAL_SDP_BEACON_SWITCH_DISABLE 0 /* disable beacon switch */

osal_s32 wal_config_sdp_init(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_start_publish(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_start_subscribe(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_cancel_subscribe(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_cancel_publish(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_send_data(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_retry_times(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_beacon_switch(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_config_sdp_test(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_sdp_process_rx_data(hmac_vap_stru *hmac_vap, frw_msg *msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_config_sdp.h */
