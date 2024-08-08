/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_FLASH_H
#define BOOT_FLASH_H

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t (*flash_init_func)(void);
typedef uint32_t (*flash_read_func)(uint32_t flash_addr, uint32_t flash_read_size, uint8_t *p_flash_read_data);
typedef uint32_t (*flash_write_func)(uint32_t flash_addr, uint32_t flash_write_size,
                                     const uint8_t *p_flash_write_data, bool do_erase);
typedef uint32_t (*flash_erase_func)(uint32_t flash_addr, uint32_t flash_erase_size);
typedef void (*flash_deinit_func)(void);

typedef struct {
    flash_init_func init;
    flash_read_func read;
    flash_write_func write;
    flash_erase_func erase;
    flash_deinit_func deinit;
} flash_cmd_func;

flash_cmd_func *boot_get_flash_funcs(void);
uint32_t boot_regist_flash_cmd(const flash_cmd_func *funcs);

#endif