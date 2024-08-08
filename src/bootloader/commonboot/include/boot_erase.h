/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for flash erase
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_ERASE_H
#define BOOT_ERASE_H

#define FLASH_CHIP_ERASE_SIZE 0xffffffff
#define FLASH_CHIP_ERASE_FILE_LEN 0

bool loader_download_is_flash_all_erased(void);
void loader_download_set_flash_all_erase(bool all_erased);
uint32_t loader_erase_all_process(const uart_ctx *cmd_ctx, bool *write_flash);
#endif