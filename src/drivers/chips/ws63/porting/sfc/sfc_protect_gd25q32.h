/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#ifndef SFC_PROTECT_GD25Q32_H
#define SFC_PROTECT_GD25Q32_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SR1_VALID_MASK  0x9C
#define SR1_VALID_VAL   0x1C

#define SR2_VALID_MASK  0x43
#define SR2_VALID_VAL   0x02

errcode_t sfc_port_fix_sr_gd25q32(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
