/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: crash data
 * Author:
 * Create:
 */

#ifndef CRASH_DATA_H
#define CRASH_DATA_H
#include "exception.h"
#include "panic.h"

#ifdef SUPPORT_CRASH_DATA_RAM
#define CRASH_MAX_CALL_STACK 64

typedef struct {
    uint8_t core;
    uint8_t save_after_reboot;
    uint8_t padding[2];
    uint32_t time_s;
    exception_info_t exception_info;
    panic_desc_t panic;
    exc_info_save_t exc_info_save;
    uint8_t stack[CRASH_MAX_CALL_STACK * EXC_STACK_INFO_SIZE];
} crash_data_t;

#ifdef SAVE_EXC_INFO
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
/**
 * @brief  Write exc info to the file system.
 * @return The length of file written if success or -1 if failed.
 */
int32_t crash_data_write(const char *path, uint32_t offset, const uint8_t *buf, uint32_t size);
#endif
#ifdef CFG_DRIVERS_MMC
/**
 * @brief  Emmc write exc info to the file system from sector after reboot system.
 * @return Success or fail.
 */
errcode_t crash_data_write_emmc(void);
#endif
errcode_t crash_data_save(void);
#endif

void crash_data_clear(void);

void crash_data_set_save_after_reboot(void);

void crash_data_set_time_s(void);

exc_info_save_t *crash_data_get_exc_info_save(void);
void crash_data_set_exc_info_save(exc_info_save_t *exc_info_save, uint16_t stack_size);

exception_info_t *crash_data_get_exception_info(void);
void crash_data_set_exception_info(exception_info_t *exception_info);

panic_desc_t *crash_data_get_panic(void);
void crash_data_set_panic(panic_desc_t *panic);
#endif
#endif
