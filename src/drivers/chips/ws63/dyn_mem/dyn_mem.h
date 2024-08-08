/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  dyn mem header file.
 *
 * Create:  2023-04-09
 */

#ifndef DYN_MEM_H
#define DYN_MEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* share mem ctl sel */
#define SHARE_RAM_CTL_CFG_RAM_SEL 0x44006c08
#define CFG_RAM_CKEN 0x44006c04
#define BT_EM_GT_MODE 0x49003074
#define RAM_SW_CONFLICT_ST 0x44006C10
#define RAMX_SW_CONFLICT_BASE_ADDR 0x44006C14
#define RAM_SW_CONFLICT_CLR 0x44006C0C

void dyn_mem_cfg(void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif