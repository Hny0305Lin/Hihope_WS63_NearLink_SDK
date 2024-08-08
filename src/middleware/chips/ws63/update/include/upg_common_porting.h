/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG common functions header file
 */

#ifndef UPG_COMMON_PORTING_H
#define UPG_COMMON_PORTING_H

#include "memory_config.h"
#include "errcode.h"
#include "upg.h"
#include "upg_definitions_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BOOT_PORTING_RESET_REG      0x40002110
#define BOOT_PORTING_RESET_VALUE    0x4

#ifndef SIZE_MAX
#define SIZE_MAX                        UINT32_MAX
#endif

#if MEMORY_MINI
#define UPG_FILE_PATH                   "/user/"
#define UPG_FILE_NAME                   "/user/update.fwpkg"
#define UPG_RES_INDEX_PATH              "/user/res_index.bin"
#else
#define UPG_FILE_PATH                   "/update/"
#define UPG_FILE_NAME                   "/update/update.fwpkg"
#define UPG_RES_INDEX_PATH              "/update/res_index.bin"
#endif

#define UPG_FLASH_SIZE                  (FLASH_MAX_END - FLASH_START)
#define FLASH_PAGE_SIZE                 4096
#define UPG_FLASH_PAGE_SIZE             FLASH_PAGE_SIZE /* Bytes per sector */
#define FOTA_DATA_STATUS_AREA_LEN       UPG_FLASH_PAGE_SIZE
#define FOTA_DATA_BUFFER_AREA_LEN       UPG_FLASH_PAGE_SIZE
#define FOTA_DATA_FLAG_AREA_LEN         UPG_FLASH_PAGE_SIZE
#define UPG_UPGRADE_FLAG_LENGTH (FOTA_DATA_STATUS_AREA_LEN + FOTA_DATA_BUFFER_AREA_LEN + FOTA_DATA_FLAG_AREA_LEN)

#define UPG_META_DATA_LENGTH            0x0

/* APP Region */
#define APP_FLASH_REGION_START          (0x104000)
#define APP_FLASH_REGION_LENGTH         (0x203000)

#define PARAMS_PARTITION_IMAGE_ID       0x4B87A52D
#define PARAMS_PARTITION_START_ADDR     0x200000
#define PARAMS_PARTITION_LENGTH         0x780

#define EFUSE_REE_SSB_VERSION           0xF0
#define EFUSE_REE_RECOVERT_VERSION      0xF0
#define EFUSE_REE_APP_VERSION           0xF0
#define EFUSE_REE_BT_VERSION            0xF0
#define EFUSE_REE_DSP_MAIN_VERSION      0xF0
#define EFUSE_REE_DSP_OVERLAY_VERSION   0xF0

#define ROOT_PUBLIC_KEY_RSV             32
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
    uint8_t       reserved[ROOT_PUBLIC_KEY_RSV]; /* 32 bytes above */
    uint8_t       root_key_area[PUBLIC_KEY_LEN];
} root_public_key;

/**
* @ingroup  iot_update
* @brief Upgrade the backup area of each image before start the upgrade.
*        CNcomment:升级开始前对各个镜像的备区升级。CNend
*
* @retval #ERRCODE_SUCC         Success.
* @retval #Other     Failure. For details, see soc_errno.h.
*/
errcode_t upg_image_backups_update(void);

void upg_progress_callback_register(void);

errcode_t ws63_upg_init(void);
/*
 * 擦除镜像对应的整个flash分区
 * img_header 镜像信息header，用于获取对应flash分区信息
 */
errcode_t upg_erase_whole_image(const upg_image_header_t *img_header);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_COMMON_PORTING_H */
