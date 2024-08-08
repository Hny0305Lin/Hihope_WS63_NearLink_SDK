/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG config file
 */
#ifndef UPG_DEFAULT_CONFIG_H
#define UPG_DEFAULT_CONFIG_H

#ifndef UPG_PRODUCT_CONFIG_H
#error "Include "upg_config.h" if necessary. Do not include this file directly! "
#endif

#ifndef UPG_CFG_DEBUG_ENABLED
#define UPG_CFG_DEBUG_ENABLED                   NO
#endif

#ifndef UPG_CFG_DEBUG_PRINT_ENABLED
#define UPG_CFG_DEBUG_PRINT_ENABLED             NO
#endif

#ifndef UPG_CFG_DIRECT_FLASH_ACCESS
#define UPG_CFG_DIRECT_FLASH_ACCESS             NO
#endif

#ifndef UPG_CFG_PROCESS_NOTIFY_SUPPORT
#define UPG_CFG_PROCESS_NOTIFY_SUPPORT          NO
#endif

#ifndef UPG_CFG_VERIFICATION_SUPPORT
#define UPG_CFG_VERIFICATION_SUPPORT            NO
#endif

#ifndef UPG_CFG_NV_UPGRADE_SUPPORT
#define UPG_CFG_NV_UPGRADE_SUPPORT              NO
#endif

#ifndef UPG_CFG_DIFF_UPGRADE_SUPPORT
#define UPG_CFG_DIFF_UPGRADE_SUPPORT            YES
#endif

#ifndef UPG_CFG_IMGAE_DECRYPT_SUPPORT
#define UPG_CFG_IMGAE_DECRYPT_SUPPORT           NO
#endif

#ifndef UPG_CFG_PROGRESS_RECOVERY_SUPPORT
#define UPG_CFG_PROGRESS_RECOVERY_SUPPORT       YES
#endif

#ifndef UPG_CFG_ANTI_ROLLBACK_SUPPORT
#define UPG_CFG_ANTI_ROLLBACK_SUPPORT           NO
#endif

#ifndef UPG_CFG_HASH_VERIFY_SUPPORT
#define UPG_CFG_HASH_VERIFY_SUPPORT             NO
#endif

#ifndef UPG_CFG_SIGNATURE_VERIFY_SUPPORT
#define UPG_CFG_SIGNATURE_VERIFY_SUPPORT        NO
#endif

#ifndef UPG_CFG_SUPPORT_FILE_SYSTEM
#define UPG_CFG_SUPPORT_FILE_SYSTEM             YES
#endif

#if ((UPG_CFG_DIFF_UPGRADE_SUPPORT == YES) && (UPG_CFG_PROGRESS_RECOVERY_SUPPORT != YES))
#error "MUST SUPPORT PROGRESS RECOVERY IN DIFF UPGRADE !"
#endif

#ifndef UPG_CFG_MEMORY_DCACHE_ENABLED
#define UPG_CFG_MEMORY_DCACHE_ENABLED           NO
#endif

#ifndef UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM
#define UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM    NO
#endif

#ifndef UPG_CFG_SUPPORT_RESOURCES_FILE
#define UPG_CFG_SUPPORT_RESOURCES_FILE          NO
#endif

#ifndef UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE
#define UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE       NO
#endif

#endif /* UPG_DEFAULT_CONFIG_H */