/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Secure Verify for Loaderboot and Flashboot
 *
 * Create: 2023-03-09
 */

#ifndef SECURE_VERIFY_BOOT_H
#define SECURE_VERIFY_BOOT_H

#include <stdint.h>
#include "errcode.h"

#if defined(CONFIG_BOOT_SUPPORT_ECC_VERIFY) || defined(CONFIG_BOOT_SUPPORT_SM2_VERIFY)  || \
    defined(CONFIG_BOOT_NO_SEC_VERIFY)
#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH            0x80
#define KEY_AREA_STRUCTURE_LENGTH                   0x100
#define CODE_INFO_STRUCTURE_LENGTH                  0x200
#define BOOT_PUBLIC_KEY_LEN                         64
#define BOOT_SIG_LEN                                64
#define BOOT_EXT_SIG_LEN                            64
#elif defined(CONFIG_BOOT_SUPPORT_RSA3072_VERIFY)
#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH            0x200
#define KEY_AREA_STRUCTURE_LENGTH                   0x400
#define CODE_INFO_STRUCTURE_LENGTH                  0x400
#define BOOT_PUBLIC_KEY_LEN                         388    /* n:384 bytes, e:4 bytes */
#define BOOT_SIG_LEN                                384
#define BOOT_EXT_SIG_LEN                            384
#elif defined(CONFIG_BOOT_SUPPORT_RSA4096_VERIFY)
#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH            0x400
#define KEY_AREA_STRUCTURE_LENGTH                   0x500
#define CODE_INFO_STRUCTURE_LENGTH                  0x300
#define BOOT_PUBLIC_KEY_LEN                         516    /* n:512 bytes, e:4 bytes */
#define BOOT_SIG_LEN                                512
#define BOOT_EXT_SIG_LEN                            0      /* No external signatures are reserved for RSA4096 */
#endif

#define DIE_ID_LEN                      16      /* DIEID has 160 bits, but we just use 128 bits */
#define HASH_LEN                        32
#define PROTECT_KEY_LEN                 16
#define IV_LEN                          16

typedef enum {
    LOADER_BOOT_TYPE = 0,
    PARAMS_BOOT_TYPE = 1,
    FLASH_BOOT_TYPE = 2,
    FLASH_BOOT_BACK_UP_TYPE = 3,
    SECOND_FLASH_BOOT_TYPE = 4,
    SECOND_FLASH_BOOT_BACK_UP_TYPE = 5,
    APP_BOOT_TYPE = 6,
    INVALID_BOOT_TYPE
} image_type_t;

typedef enum {
    KEY_EREA_TYPE = 0,
    CODE_INFO_TYPE = 1,
    PARAMS_KEY_AREA_TYPE = 2,
    PARAMS_AREA_IOFO_TYPE = 3,
    INVALID_EREA_TYPE
} area_type_t;

/* root public key area, size is 0x80 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;  /* currently version is 0x00010000 */
    uint32_t      structure_length;
    uint32_t      key_owner_id;
    uint32_t      key_id;
    uint32_t      key_alg;            /* 0x2A13C812: ECC256;  0x2A13C823: SM2 */
    uint32_t      ecc_curve_type;     /* 0x2A13C812: RFC 5639, BrainpoolP256r1 */
    uint32_t      key_length;
    uint8_t       reserved[ROOT_PUBLIC_KEY_STRUCTURE_LENGTH - 32 - BOOT_PUBLIC_KEY_LEN]; /* 32 bytes above */
    uint8_t       root_key_area[BOOT_PUBLIC_KEY_LEN];
} root_public_key_area_t;

/* Params key area structure, size is 0x100 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;  /* currently version is 0x00010000 */
    uint32_t      structure_length;
    uint32_t      signature_length;
    uint32_t      key_owner_id;
    uint32_t      key_id;
    uint32_t      key_alg;            /* 0x2A13C812: ECC256;  0x2A13C823: SM2 */
    uint32_t      ecc_curve_type;     /* 0x2A13C812: RFC 5639, BrainpoolP256r1 */
    uint32_t      key_length;
    uint32_t      params_key_version_ext;
    uint32_t      mask_params_key_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    uint32_t      maintenance_mode;   /* 0x3C7896E1: enable */
    uint8_t       die_id[DIE_ID_LEN];
    uint32_t      params_info_addr;   /* 0 means followed Params Key Area */
    uint8_t       reserved[KEY_AREA_STRUCTURE_LENGTH - 76 - BOOT_PUBLIC_KEY_LEN - BOOT_SIG_LEN]; /* 76 bytes above */
    uint8_t       params_ext_key_area[BOOT_PUBLIC_KEY_LEN];
    uint8_t       sig_params_key_area[BOOT_SIG_LEN];
} params_key_area_t;


/* Params area info, size is 0x200 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;  /* currently version is 0x00010000 */
    uint32_t      structure_length;  /* ecc/sm2 is 0x200, rsa3072 is 0x400 */
    uint32_t      signature_length;
    uint32_t      params_version_ext;
    uint32_t      mask_params_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    uint32_t      params_area_addr;   /* 0 means followed Params Area info */
    uint32_t      params_area_len;
    uint8_t       params_area_hash[HASH_LEN];
    uint8_t       reserved[CODE_INFO_STRUCTURE_LENGTH - 72 - BOOT_SIG_LEN - BOOT_EXT_SIG_LEN];      /* 72 bytes above */
    uint8_t       sig_params_info[BOOT_SIG_LEN];
    uint8_t       sig_params_info_ext[BOOT_EXT_SIG_LEN];
} params_area_info_t;

/* Params area structure, size is depend on params_area_len */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;
    uint32_t      version;
    uint16_t      item_offset;
    uint8_t       item_count;
} params_area_head_t;

typedef struct {
    uint32_t      item_addr;
    uint32_t      item_size;
} item_structure_t;

/* Key area, size is 0x100 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;
    uint32_t      structure_length;
    uint32_t      signature_length;
    uint32_t      key_owner_id;
    uint32_t      key_id;
    uint32_t      key_alg;            /* 0x2A13C812: ECC256;  0x2A13C823: SM2 */
    uint32_t      ecc_curve_type;     /* 0x2A13C812: RFC 5639, BrainpoolP256r1 */
    uint32_t      key_length;
    uint32_t      key_version_ext;
    uint32_t      mask_key_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    uint32_t      maintenance_mode;   /* 0x3C7896E1: enable */
    uint8_t       die_id[DIE_ID_LEN];
    uint32_t      code_info_addr; /* 0 means followed image Key Area */
    uint8_t       reserved[KEY_AREA_STRUCTURE_LENGTH - 76 - BOOT_PUBLIC_KEY_LEN - BOOT_SIG_LEN]; /* 76 bytes above */
    uint8_t       ext_pulic_key_area[BOOT_PUBLIC_KEY_LEN];
    uint8_t       sig_key_area[BOOT_SIG_LEN];
} image_key_area_t;

/* Code area info, size is 0x200 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;
    uint32_t      structure_length;
    uint32_t      signature_length;
    uint32_t      version_ext;
    uint32_t      mask_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    uint32_t      code_area_addr;
    uint32_t      code_area_len;
    uint8_t       code_area_hash[HASH_LEN];
    uint32_t      code_enc_flag;
    uint8_t       protection_key_l1[PROTECT_KEY_LEN];
    uint8_t       protection_key_l2[PROTECT_KEY_LEN];
    uint8_t       iv[IV_LEN];
    uint32_t      code_compress_flag; /* 0x3C7896E1: is compressed */
    uint32_t      code_uncompress_len;
    uint32_t      text_segment_size;
    uint8_t       reserved[CODE_INFO_STRUCTURE_LENGTH - 136 - BOOT_SIG_LEN - BOOT_EXT_SIG_LEN];  /* 136 bytes above */
    uint8_t       sig_code_info[BOOT_SIG_LEN];
    uint8_t       sig_code_info_ext[BOOT_EXT_SIG_LEN];
} image_code_info_t;

errcode_t verify_boot_init(void);
errcode_t verify_boot_deinit(void);
errcode_t verify_public_rootkey(uint32_t rootkey_buff_addr);
errcode_t verify_image_head(image_type_t image_type, uint32_t public_key_addr, uint32_t boot_head_addr);
errcode_t verify_image_body(uint32_t boot_head_addr, uint32_t boot_body_addr);
errcode_t verify_params_head(uint32_t root_public_key_addr, uint32_t params_head_addr);
errcode_t verify_params_body(uint32_t params_head_addr, uint32_t params_body_addr);

#endif