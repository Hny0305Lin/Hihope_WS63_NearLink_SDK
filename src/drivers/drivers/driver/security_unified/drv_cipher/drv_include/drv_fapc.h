/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver fapc header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_FAPC_H
#define DRV_FAPC_H

#include "hal_fapc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 drv_fapc_set_iv(crypto_fapc_iv_type type, const td_u8 *iv, td_u32 length);

/* FAPC: Flash access permission controller */
typedef struct {
    td_u32 start_addr;     /* FAPC region_n start address, should align to 256bytes, means the lowest 8bit are 0 */
    td_u32 end_addr;       /* FAPC region_n end address, should align to 256bytes, means the lowest 8bit are 0 */
    td_u32 mac_addr;       /* Define the start address of MAC value if current region work in decrypt and verify mode */
    td_u32 iv_start_addr;  /* Define the start address of encrypt to calculate the iv,
                                should be align to 256bytes, means the lowest 8bit are 0 */
    td_u32 read_mid;       /* Define which module/master can read the region, each bit represents a module/master */
    td_u32 write_mid;      /* Define which module/master can write the region, each bit represents a module/master */
    crypto_fapc_region_mode mode; /* SFC work mode, bypass/decrypt/decrypt and verify */
    td_bool enable;        /* RAPC region_n enable/disable, default is disable */
    td_bool lock;          /* The one way locker of FAPC region_n configuration,
                              if FAPC region_n configuration is locked, it cannot be unlock */
} crypto_fapc_config;

td_s32 drv_fapc_set_config(td_u32 region, const crypto_fapc_config *config);

td_s32 drv_fapc_set_bypass_config(td_u32 region, td_u32 start_addr, td_u32 end_addr, td_bool lock);

td_s32 drv_fapc_get_config(td_u32 region, const crypto_fapc_config *config);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif