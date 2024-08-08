/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: host platform firmware header file
 * Create: 2021-06-08
 */

#ifndef __PLAT_FIRMWARE_H__
#define __PLAT_FIRMWARE_H__

#include "oal_plat_type.h"
#include "oal_net.h"
#if defined(_PRE_OS_VERSION) && defined(_PRE_OS_VERSION_LINUX) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_exception_rst.h"
#endif

/* memory */
#define OS_KZALLOC_GFP(size)                kzalloc(size, (GFP_KERNEL | __GFP_NOWARN))

#define READ_MEG_TIMEOUT            (2000)      /* 200ms */
#define READ_MEG_JUMP_TIMEOUT       (15000)   /* 15s */

#define FILE_CMD_WAIT_TIME_MIN      (5000)     /* 5000us */
#define FILE_CMD_WAIT_TIME_MAX      (5100)     /* 5100us */

#define SEND_BUF_LEN                (520)
#define RECV_BUF_LEN                (512)
#define VERSION_LEN                 (64)

#define READ_CFG_BUF_LEN            (2048)

#define HOST_DEV_TIMEOUT            (3)
#define INT32_STR_LEN               (32)

#define CMD_JUMP_EXEC_RESULT_SUCC   (0)
#define CMD_JUMP_EXEC_RESULT_FAIL   (1)

/* 以下是发往device命令的关键字 */
#define VER_CMD_KEYWORD             "VERSION"
#define JUMP_CMD_KEYWORD            "JUMP"
#define FILES_CMD_KEYWORD           "FILES"
#define SETPM_CMD_KEYWORD           "SETPM"
#define SETBUCK_CMD_KEYWORD         "SETBUCK"
#define SETSYSLDO_CMD_KEYWORD       "SETSYSLDO"
#define SETNFCRETLDO_CMD_KEYWORD    "SETNFCRETLDO"
#define SETPD_CMD_KEYWORD           "SETPD"
#define SETNFCCRG_CMD_KEYWORD       "SETNFCCRG"
#define SETABB_CMD_KEYWORD          "SETABB"
#define SETTCXODIV_CMD_KEYWORD      "SETTCXODIV"
#define RMEM_CMD_KEYWORD            "READM"
#define WMEM_CMD_KEYWORD            "WRITEM"
#define QUIT_CMD_KEYWORD            "QUIT"

/* 以下是device对命令执行成功返回的关键字，host收到一下关键字则命令执行成功 */
#define MSG_FROM_DEV_WRITEM_OK      "WRITEM OK"
#define MSG_FROM_DEV_READM_OK       ""
#define MSG_FROM_DEV_FILES_OK       "FILES OK"
#define MSG_FROM_DEV_READY_OK       "READY"
#define MSG_FROM_DEV_JUMP_OK        "JUMP OK"
#define MSG_FROM_DEV_SET_OK         "SET OK"
#define MSG_FROM_DEV_QUIT_OK        ""


#define COMPART_KEYWORD             ' '
#define CMD_LINE_SIGN               ';'

#define CFG_INFO_RESERVE_LEN        (8)

#define FILE_COUNT_PER_SEND           (1)
#define MIN_FIRMWARE_FILE_TX_BUF_LEN  (4096)
#define MAX_FIRMWARE_FILE_TX_BUF_LEN  (512 * 1024)
#define MAX_FIRMWARE_FILE_RX_BUF_LEN  (128 * 1024)  // 512
#define MAX_PACKAGE_SIZE_INI            8


#define WCPU_ROM_START                     0x00008000
#define WCPU_ROM_END                       0x00090000
#define WCPU_ITCM_START                    0x00090000
#define WCPU_ITCM_END                      0x000CFFFF
#define WCPU_DTCM_START                    0x20000000
#define WCPU_DTCM_END                      0x20047FFF
#define WCPU_PKT_START                     0x60000000
#define WCPU_PKT_END                       0x6007FFFF

typedef enum firmware_cfg_cmd_enum {
    ERROR_TYPE_CMD = 0,            /* 错误的命令 */
    FILE_TYPE_CMD,                 /* 下载文件的命令 */
    NUM_TYPE_CMD,                  /* 下载配置参数的命令 */
    QUIT_TYPE_CMD,                 /* 退出命令 */
} firmware_cfg_cmd_enum;

#define FIRMWARE_BOARD_INFO_NAME_LEN       10
#define FIRMWARE_BOARD_INFO_PARA_LEN       50

#define FIRMWARE_DOWNLOAD_BT_ROM_OFFSET    1
#define FIRMWARE_DOWNLOAD_BT_RAM_OFFSET    2

#define FIRMWARE_REG_BOARD_INFO_SET_UU     "2,0x50000204,0x693C"
#define FIRMWARE_REG_BOARD_INFO_SET_UT     "2,0x50000204,0x533C"
#define FIRMWARE_REG_BOARD_INFO_SET_ST     "2,0x50000204,0x5353"
#define FIRMWARE_REG_BOARD_INFO_SET_SS     "2,0x50000204,0x3C53"
#define FIRMWARE_REG_BOARD_INFO_SET_ET     "2,0x50000204,0x5370"
#define FIRMWARE_REG_BOARD_INFO_SET_EU     "2,0x50000204,0x7070"

typedef struct file os_kernel_file_stru;

extern osal_s32 firmware_download_etc(void);
extern osal_s32 wifi_device_mem_dump(struct wifi_dump_mem_info *mem_dump_info, osal_u32 count);
extern osal_s32 read_device_reg16(osal_u32 address, osal_s16* value);
extern osal_s32 write_device_reg16(osal_u32 address, osal_s16 value);
osal_s32 test_sdio_new_opt(osal_u32 cnt);
osal_s32 test_sdio_new_read(osal_s32 reg_addr);
osal_s32 test_sdio_new_write(osal_s32 reg_addr, osal_s32 val);
osal_s32 test_sdio_newaddr_write(osal_u32 addr, osal_u32 cnt, osal_u32 data, osal_u32 reg_addr);
osal_s32 test_sdio_newaddr_read(osal_u32 addr, osal_u32 cnt, osal_u32 reg_addr);
#endif /* end of plat_firmware.h */
