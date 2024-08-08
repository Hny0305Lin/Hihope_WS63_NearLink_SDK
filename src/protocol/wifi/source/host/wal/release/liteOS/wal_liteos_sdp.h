/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: Function related to wifi aware(NAN) feature.
 * Create: 2021-04-16
 */

#ifndef __WAL_LITEOS_SDP_H__
#define __WAL_LITEOS_SDP_H__

#include "wal_ioctl.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LITEOS_SDP_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
td_u32  uapi_ccpriv_adjust_tx_power(oal_net_device_stru *net_dev, osal_s8 *param);
osal_u32 uapi_ccpriv_sdp_init(oal_net_device_stru *net_dev, osal_s8 *param);
osal_u32 uapi_ccpriv_sdp_start_subscribe(oal_net_device_stru *net_dev, osal_s8 *param);
osal_u32 uapi_ccpriv_sdp_cancle_subscribe(oal_net_device_stru *net_dev, osal_s8 *param);
osal_u32 uapi_ccpriv_sdp_send_data(oal_net_device_stru *net_dev, osal_s8 *param);
osal_u32 uapi_ccpriv_sdp_test(oal_net_device_stru *net_dev, osal_s8 *param);
osal_void wal_sdp_srt_to_hex(const osal_s8 *param, osal_u8 len, osal_s8 *value);
osal_u32 uapi_ccpriv_sdp_start_subscribe_svr(oal_net_device_stru *net_dev, osal_s8 *param);
osal_u32 uapi_ccpriv_sdp_cancel_subscribe_svr(oal_net_device_stru *net_dev, osal_s8 *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
