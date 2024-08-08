/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: UPG encry Interface Header.
 */
#ifndef UPG_ENCRY_PORTING_H
#define UPG_ENCRY_PORTING_H

#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_COMPRESS_ENCRY
#include "upg.h"
#include "upg_lzmadec.h"
#include "crypto_km_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define IMAGE_HEADER_LEN    ((KEY_AREA_STRUCTURE_LENGTH) + (CODE_INFO_STRUCTURE_LENGTH))
#define FLASH_ENCRY_FLAG    0x1E6987C3
#define OTA_ENCRY_FLAG      0x3C7896E1
#define OTA_IMAGE_SALT_LEN  28
#define MAX_DUMP_LENGHT     64
#define BYTES_IN_ONE_LINE   16
#ifndef KERN_CONT
#define KERN_CONT
#endif

typedef enum {
    CRYPTO_KDF_ENCRY = 0,
    CRYPTO_KDF_DENCRY,
    CRYPTO_KDF_BUFF,
} upg_crypto_type;

typedef struct {
    crypto_kdf_hard_key_type key_type; /* 加密key类型 */
    uint32_t chnum;                    /* 加密通道号 */
    uint8_t salt[OTA_IMAGE_SALT_LEN];
    uint8_t iv[IV_LEN];
} ota_cryto_cfg;

typedef struct {
    ota_cryto_cfg decry_cfg;
    ota_cryto_cfg encry_cfg;
} upg_cryto_cfg;

/*
 * 加解密初始化
 * image 需要镜像的enc_pk_l1及iv
 */
errcode_t upg_cryto_init(const upg_image_header_t *image);

/*
 * 加解密去初始化
 * image 需要镜像的re_enc_flag判断是否去初始化
 */
errcode_t upg_cryto_deinit(const upg_image_header_t *image);

/*
 * 加密参数处理
 * head_offset   出参，返回镜像偏移
 * is_image_head 入参，判断是否为镜像头
 */
errcode_t upg_process_cryto_info(uint32_t *head_offset, td_bool *is_image_head);

/*
 * 数据解密
 * buf   需要解密的数据
 * len   数据长度
 * image 镜像信息，判断是否需要解密
 */
errcode_t upg_decry_fota_pkt(uint8_t *buf, uint32_t len, const upg_image_header_t *image);

/*
 * 数据加密
 * buf   需要加密的数据
 * len   数据长度
 * image 镜像信息，判断是否需要加密
 */
errcode_t upg_encry_fota_pkt(uint8_t *buf, uint32_t len, const upg_image_header_t *image);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_LZMADEC_H */
#endif
