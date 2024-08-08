/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: hal fapc header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef HAL_FAPC_H
#define HAL_FAPC_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    CRYPTO_FAPC_MID_TEE_CPU           = (1 << 0),
    CRYPTO_FAPC_MID_CIPHER_TEE        = (1 << 1),   /* Cipher cannel locked by TEE CPU */
    CRYPTO_FAPC_MID_CIPHER_ACPU       = (1 << 2),  /* Cipher cannel locked by ACPU */
    CRYPTO_FAPC_MID_CIPHER_PCPU       = (1 << 3),  /* Cipher cannel locked by PCPU */
    CRYPTO_FAPC_MID_CIPHER_AIDSP      = (1 << 4), /* Cipher cannel locked by AIDSP */
    CRYPTO_FAPC_MID_HASH_TEE          = (1 << 5),     /* Cipher cannel locked by TEE CPU */
    CRYPTO_FAPC_MID_HASH_ACPU         = (1 << 6),    /* Cipher cannel locked by ACPU */
    CRYPTO_FAPC_MID_HASH_PCPU         = (1 << 7),    /* Cipher cannel locked by PCPU */
    CRYPTO_FAPC_MID_HASH_AIDSP        = (1 << 8),   /* Cipher cannel locked by AIDSP */
    CRYPTO_FAPC_MID_AIDSP             = (1 << 9),
    CRYPTO_FAPC_MID_ACPU              = (1 << 10),
    CRYPTO_FAPC_MID_EMMC              = (1 << 11),
    CRYPTO_FAPC_MID_SDIO              = (1 << 12),
    CRYPTO_FAPC_MID_USB               = (1 << 13),
    CRYPTO_FAPC_MID_TDE               = (1 << 14), /* TDE */
    CRYPTO_FAPC_MID_PCPU              = (1 << 15),
    CRYPTO_FAPC_MID_LMI               = (1 << 16),
    CRYPTO_FAPC_MID_PSDIO             = (1 << 17),
    CRYPTO_FAPC_MID_SFC               = (1 << 18),
    CRYPTO_FAPC_MID_NAND              = (1 << 19),
    CRYPTO_FAPC_MID_VDP               = (1 << 20),
    CRYPTO_FAPC_MID_AUDIO_CGRA        = (1 << 21),
    CRYPTO_FAPC_MID_AUDIO_AIAO        = (1 << 22),
    CRYPTO_FAPC_MID_AUDIO_MAD         = (1 << 23),
    CRYPTO_FAPC_MID_LPMCU             = (1 << 24),
    CRYPTO_FAPC_MID_IOMCU             = (1 << 25),
} crypto_fapc_mid;

typedef enum {
    CRYPTO_FAPC_REGION_MODE_NORMAL = 0,   /* Normal mode, SFC will bypass the data */
    CRYPTO_FAPC_REGION_MODE_DEC = 2,      /* Decryption mode, SFC will decrypt the data */
    CRYPTO_FAPC_REGION_MODE_DEC_AUTH = 3, /* Decryption and authentication mode, SFC will decrypt and verify the data */
    CRYPTO_FAPC_REGION_MODE_MAX,
    CRYPTO_FAPC_REGION_MODE_INVALID = 0xffffffff,
} crypto_fapc_region_mode;

td_s32 hal_fapc_set_region_addr(td_u32 region, td_u32 start_addr, td_u32 end_addr);

td_s32 hal_fapc_set_region_permission(td_u32 region, td_u32 read_mid, td_u32 write_mid);

td_s32 hal_fapc_set_region_mac_addr(td_u32 region, td_u32 mac_addr);

td_s32 hal_fapc_set_region_mode(td_u32 region, crypto_fapc_region_mode region_mode);

td_s32 hal_fapc_region_enable(td_u32 region, td_bool enable);

td_s32 hal_fapc_set_region_iv_start_addr(td_u32 region, td_u32 start_addr);

/* Define the flash online decryption iv type */
typedef enum {
    CRYPTO_FAPC_IV_TYPE_ACPU = 0x00,  /* ACPU flash online decryption iv */
    CRYPTO_FAPC_IV_TYPE_PCPU,         /* PCPU flash online decryption iv */
    CRYPTO_FAPC_IV_TYPE_TEE,          /* TEE flash online decryption iv */
    CRYPTO_FAPC_IV_TYPE_MAX,
    CRYPTO_FAPC_IV_TYPE_INVALID = 0xffffffff,
} crypto_fapc_iv_type;
td_s32 hal_fapc_set_region_iv(crypto_fapc_iv_type iv_type, const td_u8 *iv, td_u32 iv_length);

td_s32 hal_fapc_region_lock(td_u32 region);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif