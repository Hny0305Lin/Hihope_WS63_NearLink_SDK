/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Default SSB memory configurations
 *
 * Create:  2021-06-16
 */
#ifndef SSB_CONFIG_COMMON_H
#define SSB_CONFIG_COMMON_H

/* ----------------------------------------------------------------------------------------------------------------- */
/* IMAGE Area Defines */
/* SSB FOTA Region */
#define SSB_FOTA_REGION_START (FLASH_START + FOTA_IMAGE_START)

#define FOTA_IAMGE_SIGN_START     (FLASH_START + 0x7EC000)
#define FOTA_ROOT_PUBKEY_START    (FOTA_IAMGE_SIGN_START)
#define FOTA_SUB_PUBKEY_START     (FOTA_ROOT_PUBKEY_START + FLASH_PAGE_SIZE)
#define FOTA_SSB_SIGN_START       (FOTA_SUB_PUBKEY_START + FLASH_PAGE_SIZE)
#define FOTA_RECOVERY_SIGN_START  (FOTA_SSB_SIGN_START + FLASH_PAGE_SIZE)
#define FOTA_BT_SIGN_START        (FOTA_RECOVERY_SIGN_START + FLASH_PAGE_SIZE)
#define FOTA_DSP_SIGN_START       (FOTA_BT_SIGN_START + FLASH_PAGE_SIZE)
#define FOTA_DSP1_SIGN_START      (FOTA_DSP_SIGN_START + FLASH_PAGE_SIZE)
#define FOTA_APP_SIGN_START       (FOTA_DSP1_SIGN_START + FLASH_PAGE_SIZE)

/*
 * ********************* RAM ALLOCATION *********************
 *
 * Three main RAM areas, the 'BCPU' RAM, the 'APP' RAM, and 'share' RAM.
 *      'APP' RAM include 'ITCM' for code and 'DTCM' for DATA.
 * As the name implies, every core can access it's RAM and they all allowed
 * to access 'share' RAM
 *
 * BT core can access all RAM include APP RAM and DSP RAM.
 *
 * The 'shared' RAM is used by all cores, this area visible to them all
 * to exchange larger amounts of data.
 *
 * SHARED RAM               IPC mail box
 *                          LOG Area
 */
/* ----------------------------------------------------------------------------------------------------------------- */
/* SSB based application code */
#define SSB_VECTORS_ORIGIN (APP_ITCM_ORIGIN)
#define SSB_ITCM_ORIGIN (APP_ITCM_ORIGIN)
#define SSB_ITCM_LENGTH (SSB_FLASH_REGION_LENGTH)

/* SSB DTCM config */
#define MCU_DTCM_END    (APP_SRAM_ORIGIN + APP_SRAM_LENGTH)
/* stack for normal 7k */
#define USER_STACK_BASEADDR APP_SRAM_ORIGIN
#define USER_STACK_LEN      0x1c00
#define USER_STACK_LIMIT    (USER_STACK_BASEADDR + USER_STACK_LEN)

/* stack for irq 1k */
#define IRQ_STACK_BASEADDR USER_STACK_LIMIT
#define IRQ_STACK_LEN      0x400
#define IRQ_STACK_LIMIT    (IRQ_STACK_BASEADDR + IRQ_STACK_LEN)

/* stack for exception 1k */
#define EXCP_STACK_BASEADDR IRQ_STACK_LIMIT
#define EXCP_STACK_LEN      0x400
#define EXCP_STACK_LIMIT    (EXCP_STACK_BASEADDR + EXCP_STACK_LEN)
#define SSB_USE_DTCM_ORIGIN EXCP_STACK_LIMIT

/* SSB actually used ram */
#define SSB_USE_ITCM_LENGTH (APP_ITCM_LENGTH - SSB_FLASH_REGION_LENGTH)
#define SSB_USE_ITCM_ORIGIN (MCU_ITCM_END - SSB_USE_ITCM_LENGTH)

/* ROMLOADER USED TO SAVE ROOTKEY(0x400), SUBPUBKEY CERT(0x800), SSB IMAGE SIGN(0x210) */
#define ROMLOADER_USE_ITCM_LENGTH     (0x1000)
#define ROMLOADER_USE_ITCM_ORIGIN     (MCU_ITCM_END - ROMLOADER_USE_ITCM_LENGTH)

#define ROMLOADER_USE_ROOTKEY_ORIGIN  ROMLOADER_USE_ITCM_ORIGIN
#define ROMLOADER_USE_ROOTKEY_LENGTH  0x400

#define ROMLOADER_USE_CERT_ORIGIN     (ROMLOADER_USE_ROOTKEY_ORIGIN + ROMLOADER_USE_ROOTKEY_LENGTH)
#define ROMLOADER_USE_CERT_LENGTH     0x800

#define ROMLOADER_USE_SSB_SIGN_ORIGIN (ROMLOADER_USE_CERT_ORIGIN + ROMLOADER_USE_CERT_LENGTH)
#define ROMLOADER_USE_SSB_SIGN_LENGTH 0x400

/* 48 byte for BCPU system clocks status */
#define MCPU_SYSTEM_CLOCKS_LENGTH 0x30

#define BT_OTP_OFFSET     0
#define BT_OTP_LENGTH     OTP_SIZE_IN_BYTES
#define BT_OTP_MIN_LENGTH 5

/* 0x800 for rsa4096 calculate */
#define MCU_RSA_PUBLIC_KEY_LENGTH  0x400
#define MCU_RSA_SIGNATURE_LENGTH   0x200
#define MCU_RSA_RESULT_LENGTH      0x200
#define MCU_RSA_REGION_LENGTH      (MCU_RSA_PUBLIC_KEY_LENGTH + MCU_RSA_SIGNATURE_LENGTH + MCU_RSA_RESULT_LENGTH)

#define MCU_RSA_REGION_ORIGIN      (MCU_DTCM_END - MCU_RSA_REGION_LENGTH)
#define MCU_RSA_PUBLIC_KEY_ORIGIN  MCU_RSA_REGION_ORIGIN
#define MCU_RSA_SIGNATURE_ORIGIN   (MCU_RSA_PUBLIC_KEY_ORIGIN + MCU_RSA_PUBLIC_KEY_LENGTH)
#define MCU_RSA_RESULT_ORIGIN      (MCU_RSA_SIGNATURE_ORIGIN + MCU_RSA_SIGNATURE_LENGTH)

/* 176 bytes for app core preserve region */
#define MCU_PRESERVED_REGION_LENGTH 0xfc
#define MCU_PRESERVED_REGION_ORIGIN (MCU_RSA_REGION_ORIGIN - MCU_PRESERVED_REGION_LENGTH)

#if defined VIRTUAL_OTP
#define VIRTUAL_OTP_LENGTH (OTP_SIZE_IN_BYTES)
#define COM_RAM_ORIGIN 0x87000000
#define COM_RAM_LENGTH 0x4000
/* Virtual otp range 640KB+96KB - VIRTUAL_OTP_LENGTH -> 640KB+96KB */
#define VIRTUAL_OTP_ORIGIN (COM_RAM_ORIGIN + COM_RAM_LENGTH - VIRTUAL_OTP_LENGTH)
#endif

#define MCPU_SYSTEM_CLOCKS_ORIGIN (MCU_PRESERVED_REGION_ORIGIN - MCPU_SYSTEM_CLOCKS_LENGTH)

#endif
