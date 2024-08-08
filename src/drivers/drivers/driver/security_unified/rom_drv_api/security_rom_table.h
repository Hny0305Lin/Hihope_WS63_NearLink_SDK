/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: define rom functions address
 *
 * Create: 2023-05-17
*/

#ifndef DRV_ROM_TABLE_H
#define DRV_ROM_TABLE_H

/* drv rom cipher API */
#define DRV_ROM_CIPHER_INIT                     0x1024dc
#define DRV_ROM_CIPHER_SHA256                   0x102506
#define DRV_ROM_CIPHER_PKE_BP256R_VERIFY        0x10262a

/* PKE */
#define HAL_PKE_INIT                0x103424
#define HAL_PKE_DEINIT              0x103438
#define HAL_PKE_LOCK                0x103490
#define HAL_PKE_UNLOCK              0x103508
#define HAL_PKE_ENABLE_NOISE        0x10351e
#define HAL_PKE_DISABLE_NOISE       0x103532
#define HAL_PKE_PRE_PROCESS         0x103542
#define HAL_PKE_CHECK_FREE          0x103574
#define HAL_PKE_SET_RAM             0x10359c
#define HAL_PKE_GET_RAM             0x10364a
#define HAL_PKE_CLEAN_RAM           0x103692
#define HAL_PKE_SET_MODE            0x1036a8
#define HAL_PKE_START               0x10371a
#define HAL_PKE_WAIT_DONE           0x103738
#define HAL_PKE_GET_ALIGN_VAL       0x10375e
#define HAL_PKE_SET_MONT_PARA       0x1037bc
#define HAL_PKE_SET_ECC_PARAM       0x1037d0
#define HAL_PKE_SET_INIT_PARAM      0x10389c
/* PKE algorithm API */
#define ECC_ECFP_MUL_ADD            0x10287a

/* TRNG */
#define HAL_CIPHER_TRNG_INIT        0x1038ba
#define HAL_CIPHER_TRNG_GET_RANDOM  0x1038c8

/* HASH */
#define HAL_CIPHER_HASH_INIT        0x102d3c
#define HAL_HASH_LOCK               0x102fd4
#define HAL_HASH_UNLOCK             0x102d8a
#define HAL_CIPHER_HASH_CONFIG      0x102daa
#define HAL_CIPHER_HASH_ADD_IN_NODE 0x102eb4
#define HAL_CIPHER_HASH_START       0x102f28
#define HAL_CIPHER_HASH_WAIT_DONE   0x1030fc
#define HAL_CIPHER_HASH_DONE_TRY    0x102fba
#endif  /* DRV_ROM_TABLE_H */