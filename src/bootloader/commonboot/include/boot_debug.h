/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_DEBUG_H
#define BOOT_DEBUG_H

#include <stdint.h>

typedef enum {
    HI_BOOT_ENTER_C = 0x5A01,               /* 从汇编进入到c代码 */
    HI_BOOT_CHIP_INIT,                      /* CHIP初始化完成 */
    HI_BOOT_WAIT_UART_INTERRUPT,            /* 进入等待UART阶段 */
    HI_BOOT_WAIT_LOAD_LOADERBOOT,           /* 等待下载loaderboot阶段 */
    HI_BOOT_START_LOAD_LOADERBOOT,          /* 进入加载loaderboot阶段 */
    HI_BOOT_START_LOAD_FLASHBOOT,           /* 进入加载flashboot阶段 */
    HI_BOOT_START_LOAD_TAIL_FLASHBOOT,      /* 进入加载备份flashboot阶段 */
    HI_BOOT_START_LOADERBOOT_VERIFY,        /* 启动loaderboot安全认证 */
    HI_BOOT_START_FLASHBOOT_VERIFY,         /* 启动flashboot安全认证 */
    HI_BOOT_START_TAIL_FLASHBOOT_VERIFY,    /* 启动备份flashboot安全认证 */
    HI_BOOT_DECYPT_BOOT,                    /* 进入解密boot阶段 */
    HI_BOOT_BOOT_ROOT_VERIFY,               /* 启动根密钥验证 */
    HI_BOOT_BOOT_SUBKEY_VERIFY,             /* 根密钥验证通过，启动二级密钥验证 */
    HI_BOOT_BOOT_CODE_VERIFY,               /* 二级密钥验证通过，启动校验code内容 */
    HI_BOOT_OMIT_VERIFY,                    /* 非安全启动检验flashboot完整性 */
    HI_BOOT_OMIT_TAIL_VERIFY,               /* 非安全启动检验备份flashboot完整性 */
    HI_BOOT_START_REPAIR_FLASHBOOT,         /* 开始修复主flashboot */
    HI_BOOT_LOADER_START,                   /* 跳转引导loaderboot启动 */
    HI_BOOT_FLASH_START,                    /* 跳转引导flashboot启动 */
    HI_BOOT_NOIOT_START,                    /* 跳转引导noiot启动 */
    HI_BOOT_SOFT_REBOOT,                    /* boot安全认证失败，重启单板 */
    HI_BOOT_IDX_MAX,                        /* 启动状态最大值 */
} hi_boot_start_status;

void set_boot_status(hi_boot_start_status status);
uint16_t get_boot_status(void);

#endif