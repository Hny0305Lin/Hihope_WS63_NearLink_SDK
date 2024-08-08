/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: crash data
 * Author:
 * Create:
 */

#include "crash_data.h"
#include "preserve.h"
#ifdef SAVE_EXC_INFO
#include "dfx_adapt_layer.h"
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
#include "fcntl.h"
#include "unistd.h"
#include "dfx_file_operation.h"
#include "sys/stat.h"
#ifdef CFG_DRIVERS_NANDFLASH
#include "nandflash_config.h"
#endif /* CFG_DRIVERS_NANDFLASH */
#ifdef CFG_DRIVERS_MMC
#include "block.h"
#endif /* CFG_DRIVERS_MMC */
#endif /* #if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES */
#endif /* SAVE_EXC_INFO */

#ifdef SUPPORT_CRASH_DATA_RAM
#include "systick.h"
#include "securec.h"

#define APP_CORE_CRASH_OFFSET 0x0
#define BT_CORE_CRASH_OFFSET (APP_CORE_CRASH_OFFSET + 0x400)

#ifdef SAVE_EXC_INFO
#ifdef CFG_DRIVERS_MMC
#define ONE_SECTOR_SIZE       512
#define SECTOR_NUM            16
#define START_SECTOR_NUM      4445186
#endif

#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
static const char *g_exc_info_path = "/user/exc/exc_info.bin";
#else
#define DUMP_INFO_OFFSET 0x0
#endif
#endif /* SAVE_EXC_INFO */

static crash_data_t *crash_data_get_by_core(uint8_t core)
{
    if (core == APPS) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + APP_CORE_CRASH_OFFSET);
    } else if (core == BT) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + BT_CORE_CRASH_OFFSET);
    }
    return NULL;
}

#ifdef SAVE_EXC_INFO
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
int32_t crash_data_write(const char *path, uint32_t offset, const uint8_t *buf, uint32_t size)
{
    int fd;
    int ret;
    ssize_t length;
    fd = open(path, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        if (dfx_file_mkdir(path) != 0) {
            return -1;
        }
        fd = open(path, O_RDWR | O_CREAT, 0);
        if (fd < 0) {
            return -1;
        }
    }
    int file_pos = (int)lseek(fd, offset, SEEK_SET);
    if (file_pos < 0) {
        close(fd);
        return -1;
    }
    length = write(fd, buf, size);
    if (length < 0) {
        close(fd);
        return -1;
    }
    ret = close(fd);
    if (ret < 0) {
        return -1;
    }
    return length;
}
#endif
#ifdef CFG_DRIVERS_MMC
errcode_t crash_data_write_emmc(void)
{
    char *buffer = (char *)dfx_malloc(0, ONE_SECTOR_SIZE * SECTOR_NUM);
    uint32_t sector_num = mmc_direct_read(0, buffer, START_SECTOR_NUM, SECTOR_NUM);
    if (sector_num == 0) {
        return ERRCODE_FAIL;
    }
    if (is_preserve_data_saved((uintptr_t)buffer)) {
        return ERRCODE_SUCC;
    }
    int32_t ret = crash_data_write(g_exc_info_path, 0, (const uint8_t *)buffer, ONE_SECTOR_SIZE * SECTOR_NUM);
    if (ret < 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif
errcode_t crash_data_save(void)
{
    int32_t ret;
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES) && defined(CFG_DRIVERS_MMC)
    uint32_t len = ONE_SECTOR_SIZE * SECTOR_NUM;
#else
    uint32_t len = PRESERVED_REGION_LENGTH + APP_PRESERVED_REGION_LENGTH + CRASH_MEM_REGION_LENGTH;
#endif
    uint8_t *buffer = (uint8_t *)dfx_zalloc(0, len);
    if (memcpy_s(buffer, len, (uint8_t *)(uintptr_t)PRESERVED_REGION_ORIGIN, PRESERVED_REGION_LENGTH) != EOK) {
        dfx_free(0, buffer);
        return ERRCODE_FAIL;
    }
    if (memcpy_s(buffer + PRESERVED_REGION_LENGTH, len - PRESERVED_REGION_LENGTH,
                 (uint8_t *)(uintptr_t)APP_PRESERVED_REGION_ORIGIN, APP_PRESERVED_REGION_LENGTH) != EOK) {
        dfx_free(0, buffer);
        return ERRCODE_FAIL;
    }
    if (memcpy_s(buffer + PRESERVED_REGION_LENGTH + APP_PRESERVED_REGION_LENGTH,
                 len - PRESERVED_REGION_LENGTH - APP_PRESERVED_REGION_LENGTH,
                 (uint8_t *)(uintptr_t)CRASH_MEM_REGION_START, CRASH_MEM_REGION_LENGTH) != EOK) {
        dfx_free(0, buffer);
        return ERRCODE_FAIL;
    }
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
#ifdef CFG_DRIVERS_NANDFLASH
    nand_driver_init(SPEED_SLOW);
    set_trans_type(TRANS_BY_CPU_SINGLE_LINE);
    ret = crash_data_write(g_exc_info_path, 0, buffer, len);
#endif
#ifdef CFG_DRIVERS_MMC
    ret = (int32_t)mmc_write_in_exception(0, buffer, START_SECTOR_NUM, SECTOR_NUM);
#endif
#else
    ret = dfx_flash_erase(FLASH_OP_TYPE_DUMP_INFO, DUMP_INFO_OFFSET, len);
    if (ret == ERRCODE_SUCC) {
        ret = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, DUMP_INFO_OFFSET, buffer, len, 0);
    }
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES */
    dfx_free(0, buffer);
    if (ret <= 0) {
        return ERRCODE_FAIL;
    }
    memset_s((void *)(uintptr_t)(APP_PRESERVED_REGION_ORIGIN), APP_PRESERVED_REGION_LENGTH, 0,
             APP_PRESERVED_REGION_LENGTH);
    PRINT("save crash data done!\r\n");
    return ERRCODE_SUCC;
}
#endif

void crash_data_clear(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    memset_s(crash_data, sizeof(crash_data_t), 0, sizeof(crash_data_t));
    crash_data->core = CORE;
}

void crash_data_set_save_after_reboot(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    crash_data->save_after_reboot = 1;
}

void crash_data_set_time_s(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    crash_data->time_s = (uint32_t)uapi_systick_get_s();
}

exc_info_save_t *crash_data_get_exc_info_save(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    return &crash_data->exc_info_save;
}

void crash_data_set_exc_info_save(exc_info_save_t *exc_info_save, uint16_t stack_size)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    memcpy_s(&crash_data->exc_info_save, EXC_INFO_SAVE_SIZE + CRASH_MAX_CALL_STACK * EXC_STACK_INFO_SIZE,
        exc_info_save, EXC_INFO_SAVE_SIZE + stack_size * EXC_STACK_INFO_SIZE);
}

exception_info_t *crash_data_get_exception_info(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    return &crash_data->exception_info;
}

void crash_data_set_exception_info(exception_info_t *exception_info)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    memcpy_s(&crash_data->exception_info, sizeof(exception_info_t), exception_info, sizeof(exception_info_t));
}

panic_desc_t *crash_data_get_panic(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    return &crash_data->panic;
}

void crash_data_set_panic(panic_desc_t *panic)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    memcpy_s(&crash_data->panic, sizeof(panic_desc_t), panic, sizeof(panic_desc_t));
}
#endif /* SUPPORT_CRASH_DATA_RAM */
