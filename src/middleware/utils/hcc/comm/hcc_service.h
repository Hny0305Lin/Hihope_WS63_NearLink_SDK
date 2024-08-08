/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc service.
 * Author:
 * Create: 2021-09-11
 */

#ifndef HCC_SERVICE_HEADER
#define HCC_SERVICE_HEADER

#include "td_type.h"
#include "hcc_types.h"
#include "hcc_cfg_comm.h"
#include "hcc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 hcc_service_get_credit(td_u8 chl, hcc_service_type serv, td_u32 *credit);
td_bool hcc_service_is_busy(td_u8 chl, hcc_service_type service_type);
td_void hcc_service_enable_switch(td_u8 chl, hcc_service_type service_type, td_bool enable);

hcc_serv_info *hcc_add_service_list(hcc_handler *hcc, hcc_service_type serv_type);
td_void hcc_del_service_list(hcc_handler *hcc, hcc_service_type serv_type);
hcc_serv_info *hcc_get_serv_info(hcc_handler *hcc, hcc_service_type serv_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_SERVICE_HEADER */
