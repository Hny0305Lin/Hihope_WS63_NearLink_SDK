/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_event_rom.c.
 */

#ifndef HAL_EVENT_ROM_H
#define HAL_EVENT_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_common_ops_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* HAL层注册的回调函数类型 */
typedef osal_s32 (*hal_event_to_dmac_callback)(osal_u16 msg_id, osal_u8 pri,
    osal_u8 vap_id, osal_u8 *data, osal_u32 data_len);

/*****************************************************************************
   函数声明
*****************************************************************************/
osal_void hal_event_to_dmac_regsiter(osal_void *callback);
osal_void *hal_event_to_dmac_get_callback(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_event_rom.h */
