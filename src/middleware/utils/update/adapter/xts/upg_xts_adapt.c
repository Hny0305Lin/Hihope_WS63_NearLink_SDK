/*
 * * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "malloc.h"
#include "hal_hota_board.h"
#include "upg_debug.h"
#include "upg.h"

#define PUBKEY_LENGTH 270

static const unsigned char g_pub_key_buf[PUBKEY_LENGTH] = {
    0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xBF, 0xAA, 0xA5, 0xB3, 0xC2, 0x78, 0x5E,
    0x63, 0x07, 0x84, 0xCF, 0x37, 0xF0, 0x45, 0xE8, 0xB9, 0x6E, 0xEF, 0x04, 0x88, 0xD3, 0x43, 0x06,
    0x2C, 0xFC, 0x09, 0x8C, 0x84, 0x3B, 0x43, 0x07, 0x89, 0x6B, 0x23, 0x8F, 0xA2, 0xF0, 0x9D, 0x91,
    0xCB, 0xA8, 0x02, 0x60, 0xD8, 0x5C, 0x18, 0xD9, 0x34, 0xF0, 0x3C, 0x49, 0x10, 0x0E, 0xE3, 0xC7,
    0x19, 0xA5, 0x51, 0x93, 0x38, 0xFD, 0xE0, 0x62, 0x64, 0xBA, 0x6D, 0x11, 0x2E, 0xE1, 0x6E, 0x60,
    0x12, 0x16, 0x1B, 0x35, 0xA6, 0x54, 0x3F, 0x0B, 0x5D, 0x54, 0x08, 0xC9, 0x23, 0x51, 0x15, 0xA9,
    0xE2, 0x07, 0xCC, 0xF9, 0xFD, 0x19, 0x8A, 0xB3, 0x7E, 0xCE, 0x69, 0xED, 0x26, 0x34, 0xD2, 0x02,
    0xF3, 0xEB, 0x07, 0x13, 0x69, 0xE3, 0x03, 0x87, 0xB3, 0x6A, 0x3E, 0x91, 0x94, 0xAC, 0x2C, 0xBA,
    0xF6, 0xEE, 0x4C, 0x41, 0x0C, 0x2E, 0xD0, 0xE7, 0x58, 0xA7, 0xE6, 0x7F, 0x1A, 0xC0, 0xB8, 0xE3,
    0x12, 0x18, 0x97, 0x8D, 0x99, 0xAB, 0x35, 0x7B, 0xAD, 0x41, 0xA0, 0xFB, 0xCB, 0x23, 0xF6, 0x51,
    0xE6, 0x94, 0x1F, 0xF7, 0xD8, 0x16, 0xCD, 0x15, 0x67, 0x59, 0x10, 0xA2, 0x40, 0x55, 0xD1, 0x67,
    0xDA, 0x18, 0xCD, 0x63, 0x5B, 0x10, 0xAF, 0x22, 0x99, 0xD8, 0x9D, 0x56, 0x66, 0xCD, 0x80, 0x64,
    0x32, 0xB5, 0xD7, 0xF5, 0xBA, 0x91, 0x4A, 0x8D, 0x97, 0x14, 0x8A, 0xB2, 0xB0, 0x42, 0x4A, 0xE6,
    0x43, 0x22, 0x3B, 0x6E, 0xD6, 0x1E, 0x1F, 0xDA, 0xEC, 0x83, 0xCF, 0x20, 0xFA, 0x02, 0xF3, 0xFB,
    0x6E, 0x09, 0x2A, 0x0D, 0xB7, 0x81, 0x1E, 0xD5, 0x71, 0xDF, 0x80, 0xC5, 0x33, 0x78, 0xE5, 0x41,
    0x33, 0xDF, 0x9A, 0xBD, 0x36, 0x51, 0xAC, 0x96, 0xF4, 0xC6, 0x11, 0xC3, 0x93, 0x78, 0x26, 0x96,
    0x9F, 0x67, 0x05, 0x1D, 0xDF, 0xB3, 0xAA, 0x26, 0x25, 0x02, 0x03, 0x01, 0x00, 0x01
};
typedef enum {
    PARTITION_OTA = 0,
    PARTITION_KERNEL_A = 1,
    PARTITION_MAX
} hota_partition_t;

static const ComponentTableInfo g_component_table[] = {
    { PARTITION_OTA, "ota_file", "", 0},
    { PARTITION_KERNEL_A, "kernel_A", "/sdcard/update/kernel.bin", 0},
    { PARTITION_MAX, NULL, NULL, 0}
};

typedef enum {
    UPG_STATUS_UNINIT = 0,
    UPG_STATUS_INIT = 1,
    UPG_STATUS_START = 2,
    UPG_STATUS_FINISH = 3,
    UPG_STATUS_MAX
} upg_proc_status_t;
static uint8_t g_upg_status = UPG_STATUS_UNINIT;

errcode_t upg_init(void)
{
    errcode_t ret;
    upg_func_t upg_func = {0};
    upg_func.malloc = malloc;
    upg_func.free = free;
    upg_func.serial_putc = NULL;
    ret = uapi_upg_init(&upg_func);
    if (ret != ERRCODE_SUCC && ret != ERRCODE_UPG_ALREADY_INIT) {
        upg_log_err("[UPG] upgrade init failed!\r\n");
        return ret;
    }
    upg_log_info("[UPG] upgrade init OK!\r\n");
    return ERRCODE_SUCC;
}

int HotaHalInit(void)
{
    errcode_t res = upg_init();
    if (res != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }
    g_upg_status = UPG_STATUS_INIT;
    upg_prepare_info_t info = {0};
    uapi_upg_prepare(&info);
    return OHOS_SUCCESS;
}

int HotaHalGetUpdateIndex(unsigned int *index)
{
#ifndef CONFIG_MIDDLEWARE_SUPPORT_UPG_AB
    *index = PARTITION_OTA;
    return OHOS_SUCCESS;
#endif
}

int HotaHalDeInit(void)
{
    if (g_upg_status != UPG_STATUS_START) {
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRead(int partition, unsigned int offset, unsigned int bufLen, unsigned char *buffer)
{
    if (g_upg_status != UPG_STATUS_START) {
        return OHOS_FAILURE;
    }
    if (uapi_upg_read_package(offset, buffer, bufLen) != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int HotaHalWrite(int partition, unsigned char *buffer, unsigned int offset, unsigned int buffLen)
{
    if (partition != PARTITION_OTA || g_upg_status != UPG_STATUS_START) {
        return OHOS_FAILURE;
    }
    unsigned int result = uapi_upg_write_package_sync(offset, buffer, buffLen);
    if (result != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRestart(void)
{
    upg_reboot();
    return OHOS_SUCCESS;
}

int HotaHalSetBootSettings(void)
{
    return OHOS_SUCCESS;
}
/* no support */
int HotaHalRollback(void)
{
    return OHOS_SUCCESS;
}

const ComponentTableInfo *HotaHalGetPartitionInfo(void)
{
    return g_component_table;
}

unsigned char *HotaHalGetPubKey(unsigned int *length)
{
    if (length == NULL) {
        return NULL;
    }

    *length = sizeof(g_pub_key_buf);
    return (unsigned char *)g_pub_key_buf;
}


int HotaHalGetUpdateAbility(void)
{
    return 0;
}

/* no support */
int HotaHalGetOtaPkgPath(char *path, int len)
{
    (void)path;
    (void)len;
    return OHOS_SUCCESS;
}

/* support reboot when update locally */
int HotaHalIsDeviceCanReboot(void)
{
    return 1;
}

/* no support */
int HotaHalGetMetaData(UpdateMetaData *metaData)
{
    (void)metaData;
    return OHOS_SUCCESS;
}

/* no support */
int HotaHalSetMetaData(UpdateMetaData *metaData)
{
    (void)metaData;
    return OHOS_SUCCESS;
}

/* no support */
int HotaHalRebootAndCleanUserData(void)
{
    return OHOS_SUCCESS;
}

/* no support */
int HotaHalRebootAndCleanCache(void)
{
    return OHOS_SUCCESS;
}

int HotaHalCheckVersionValid(const char *currentVersion, const char *pkgVersion, unsigned int pkgVersionLength)
{
    return (strncmp(currentVersion, pkgVersion, pkgVersionLength) == 0) ? 1 : 0;
}
