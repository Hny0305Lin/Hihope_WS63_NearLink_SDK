/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLinkKV harmonyos file实现源文件（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_kv_adapter.h"
#include <stdbool.h>
#include "hilink_sal_defines.h"
#include "securec.h"
#include "hilink_str_adapter.h"
#include "lfs.h"
#include "fcntl.h"
#include "sfc.h"
#include "securec.h"
#include "soc_osal.h"
#include "littlefs_config.h"
#include "securec.h"
#include "partition.h"
#include "littlefs_adapt.h"

#if defined(HILINK_SDK_BUILD_IN) && defined(HI_3861)
#define MAX_PATH_LEN        16
#define MAX_FILENAME_LEN    16
#else
#define MAX_PATH_LEN        32
#define MAX_FILENAME_LEN    32
#endif

static bool g_isKVInit = false;
static char g_filePath[MAX_PATH_LEN];

typedef struct KeyNameItem {
    const char *hilinkKey;
    const char *fileName;
} KeyNameItem;

/* 需要对key进行转换，防止key变更导致无法前向兼容 */
static const KeyNameItem KEY_NAME_LIST[] = {
    {
        "hilink_running",
        "running"
    },
    {
        "hilink_timer",
        "timer"
    },

    {
        "hilink_ble_sensitive",
        "sensitive"
    },
    {
        "hilink_ble_regstate",
        "regstate"
    },
    {
        "hilink_cert",
        "cert"
    },
};

#ifdef DUAL_FILE_BACKUP
/* 使用双备份后对于非预置文件，以.bak为后缀做备份 */
static const char *BACKUP_FILE_SUFFIX = ".bak";
static const char *PRESET_FILE[] = {"hilink.cfg", "hilink_bak.cfg"};

static bool IsPresetFile(const char *fileName)
{
    for (unsigned int i = 0; i < sizeof(PRESET_FILE) / sizeof(char *); ++i) {
        if (HILINK_Strcmp(fileName, PRESET_FILE[i]) == 0) {
            return true;
        }
    }
    return false;
}

static const char *GetBackupFileName(const char *fileName, char *buf, unsigned int len)
{
    if (sprintf_s(buf, len, "%s%s", fileName, BACKUP_FILE_SUFFIX) <= 0) {
        HILINK_SAL_WARN("sprintf error\r\n");
        return NULL;
    }
    return buf;
}
#endif

static const char *FindFileName(const char *key)
{
    for (unsigned int i = 0; i < (sizeof(KEY_NAME_LIST) / sizeof(KEY_NAME_LIST[0])); ++i) {
        if (HILINK_Strcmp(key, KEY_NAME_LIST[i].hilinkKey) == 0) {
            return KEY_NAME_LIST[i].fileName;
        }
    }
    return NULL;
}

static int SetConfigInfoPath(const char *path)
{
    if (HILINK_Strlen(path) >= MAX_PATH_LEN) {
        HILINK_SAL_WARN("path too long: %s\r\n", path);
        return HILINK_SAL_KV_INTI_ERR;
    }

    (void)memset_s(g_filePath, MAX_PATH_LEN, 0, MAX_PATH_LEN);
    /* 长度为0保存在根目录下 */
    if (HILINK_Strlen(path) == 0) {
        HILINK_SAL_NOTICE("empty path\r\n");
        return HILINK_SAL_OK;
    }
    if (strcpy_s(g_filePath, MAX_PATH_LEN, path) != EOK) {
        HILINK_SAL_WARN("strncpy_s error\r\n");
        return HILINK_SAL_STRCPY_ERR;
    }
    if (g_filePath[HILINK_Strlen(path) - 1] == '/') {
        g_filePath[HILINK_Strlen(path) - 1] = '\0';
    }

    return HILINK_SAL_OK;
}

static int CreateFileIfNotExists(const char *filePath)
{
    if (filePath == NULL) {
        HILINK_SAL_ERROR("Create filepath is null.\r\n");
        return HILINK_SAL_KV_INTI_ERR;
    }

    HILINK_SAL_DEBUG("Create file [%s]\r\n", filePath);
    int fp = fs_adapt_open(filePath, O_RDWR | O_CREAT);
    if (fp < 0) {
        HILINK_SAL_ERROR("Create file failed.\r\n");
        return HILINK_SAL_KV_INTI_ERR;
    }

    int ret = fs_adapt_close(fp);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("close file %s failed\n", filePath);
        return HILINK_SAL_NOK;
    }

    HILINK_SAL_DEBUG("Create file %s SUCCESS\r\n", filePath);

    return HILINK_SAL_OK;
}

static int ReadFile(const char *filePath, unsigned int offset, unsigned char *value, unsigned int len)
{
    if ((filePath == NULL) || (filePath[0] == '\0')) {
        HILINK_SAL_ERROR("invalid path\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    HILINK_SAL_DEBUG("read %s, read len %u, offset %u\r\n", filePath, len, offset);

    int fp = fs_adapt_open(filePath, O_RDWR);
    if (fp < 0) {
        HILINK_SAL_ERROR("open %s failed\r\n", filePath);
        return HILINK_SAL_NOK;
    }

    int ret = fs_adapt_seek(fp, (int)offset, LFS_SEEK_SET);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("set file %s offset %u failed\n", filePath, offset);
        goto unnormal_close;
    }

    ret = fs_adapt_read(fp, (char *)value, len);
    if (ret < 0) {
        HILINK_SAL_ERROR("read file %s failed\n", filePath);
        goto unnormal_close;
    }

    ret = fs_adapt_close(fp);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("close file %s failed\n", filePath);
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;

unnormal_close:
    ret = fs_adapt_close(fp);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("close file %s failed\n", filePath);
    }

    return HILINK_SAL_NOK;
}

static int WritFile(const char *filePath, unsigned int offset, const unsigned char *value, unsigned int len)
{
    HILINK_SAL_DEBUG("write %s, write len %u, offset %u\r\n", filePath, len, offset);

    int fp = fs_adapt_open(filePath, O_RDWR | O_CREAT);
    if (fp < 0) {
        HILINK_SAL_ERROR("open %s failed\r\n", filePath);
        return HILINK_SAL_NOK;
    }

    int ret = fs_adapt_seek(fp, (int)offset, LFS_SEEK_SET);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("set file %s offset %u failed\n", filePath, offset);
        goto unnormal_close;
    }

    ret = fs_adapt_write(fp, (char *)value, len);
    if (ret < 0) {
        HILINK_SAL_ERROR("write file %s failed\n", filePath);
        goto unnormal_close;
    }

    ret = fs_adapt_close(fp);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("close file %s failed\n", filePath);
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;

unnormal_close:
    ret = fs_adapt_close(fp);
    if (ret < LFS_ERR_OK) {
        HILINK_SAL_ERROR("close file %s failed\n", filePath);
    }

    return HILINK_SAL_NOK;
}

static int CreateEmptyFileAfterDelete(const char *filePath)
{
    HILINK_SAL_NOTICE("delete %s\r\n", filePath);
    return HILINK_SAL_OK;
}

static int GetFilePathByKey(const char *key, char *buf, unsigned int len)
{
    const char *fileName = NULL;

    fileName = FindFileName(key);
    if (fileName == NULL) {
        HILINK_SAL_ERROR("get file path failed\n");
        return HILINK_SAL_NOK;
    }

    if (memcpy_s(buf, len - 1, fileName, strlen(fileName)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\n");
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;
}

#if defined(HILINK_SDK_BUILD_IN) && defined(DUAL_FILE_BACKUP)
static const char *GetBakFilePathByKey(const char *key, char *buf, unsigned int len)
{
    const char *fileName = FindFileName(key);
    /* 预置文件不做备份 */
    if (IsPresetFile(fileName)) {
        return NULL;
    }
    char bakFileName[MAX_FILENAME_LEN] = {0};
    if (GetBackupFileName(fileName, bakFileName, sizeof(bakFileName)) == NULL) {
        return NULL;
    }

    return GetFullFilePath(bakFileName, buf, len);
}
#endif

static void DeleteFileByKey(const char *key)
{
    char filePath[MAX_PATH_LEN + MAX_FILENAME_LEN] = {0};
    if (GetFilePathByKey(key, filePath, sizeof(filePath)) != HILINK_SAL_OK) {
        return;
    }
    (void)CreateEmptyFileAfterDelete(filePath);
#if defined(HILINK_SDK_BUILD_IN) && defined(DUAL_FILE_BACKUP)
    (void)memset_s(filePath, sizeof(filePath), 0, sizeof(filePath));
    if (GetBakFilePathByKey(key, filePath, sizeof(filePath)) != NULL) {
        CreateEmptyFileAfterDelete(filePath);
    }
#endif
}

static int ReadFileByKey(const char *key, unsigned int offset, unsigned char *value, unsigned int len)
{
    char filePath[MAX_PATH_LEN + MAX_FILENAME_LEN] = {0};
    if (GetFilePathByKey(key, filePath, sizeof(filePath)) != HILINK_SAL_OK) {
        return HILINK_SAL_KV_GET_ITEM_ERR;
    }
    int ret = ReadFile(filePath, offset, value, len);
    if (ret == HILINK_SAL_OK) {
        return HILINK_SAL_OK;
    }
#if defined(HILINK_SDK_BUILD_IN) && defined(DUAL_FILE_BACKUP)
    (void)memset_s(filePath, sizeof(filePath), 0, sizeof(filePath));
    if (GetBakFilePathByKey(key, filePath, sizeof(filePath)) != NULL) {
        ret = ReadFile(filePath, offset, value, len);
        if (ret == HILINK_SAL_OK) {
            return HILINK_SAL_OK;
        }
    }
#endif
    return ret;
}

static int WriteFileByKey(const char *key, unsigned int offset, const unsigned char *value, unsigned int len)
{
    char filePath[MAX_PATH_LEN + MAX_FILENAME_LEN] = {0};
    if (GetFilePathByKey(key, filePath, sizeof(filePath)) != HILINK_SAL_OK) {
        return HILINK_SAL_KV_SET_ITEM_ERR;
    }
    int ret = WritFile(filePath, offset, value, len);
    if (ret != HILINK_SAL_OK) {
        return ret;
    }

#if defined(HILINK_SDK_BUILD_IN) && defined(DUAL_FILE_BACKUP)
    (void)memset_s(filePath, sizeof(filePath), 0, sizeof(filePath));
    if (GetBakFilePathByKey(key, filePath, sizeof(filePath)) != NULL) {
        ret = WritFile(filePath, offset, value, len);
        if (ret != HILINK_SAL_OK) {
            return ret;
        }
    }
#endif
    return HILINK_SAL_OK;
}

static int InitFileByKey(const char *key)
{
    if ((key == NULL) || (key[0] == '\0')) {
        HILINK_SAL_WARN("invalid parm\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    char filePath[MAX_PATH_LEN + MAX_FILENAME_LEN] = {0};
    if (GetFilePathByKey(key, filePath, sizeof(filePath)) != HILINK_SAL_OK) {
        return HILINK_SAL_KV_INTI_ERR;
    }

    int ret = CreateFileIfNotExists(filePath);
    if (ret != 0) {
        return ret;
    }

#if defined(HILINK_SDK_BUILD_IN) && defined(DUAL_FILE_BACKUP)
    (void)memset_s(filePath, sizeof(filePath), 0, sizeof(filePath));
    if (GetBakFilePathByKey(key, filePath, sizeof(filePath)) == NULL) {
        return HILINK_SAL_KV_INTI_ERR;
    }
    ret = CreateFileIfNotExists(filePath);
    if (ret != 0) {
        return ret;
    }
#endif
    return HILINK_SAL_OK;
}

int HILINK_KVStoreInit(const char *path, const char *key[], unsigned int num)
{
    if ((key == NULL) || (num == 0) || (path == NULL)) {
        HILINK_SAL_WARN("invalid parm\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    HILINK_SAL_DEBUG("HILINK_KVStoreInit path[%s], num %d\r\n", path, num);
    if (SetConfigInfoPath(path) != HILINK_SAL_OK) {
        HILINK_SAL_WARN("set config path error\r\n");
        return HILINK_SAL_KV_INTI_ERR;
    }
    int ret;
    for (unsigned int i = 0; i < num; ++i) {
        HILINK_SAL_DEBUG("HILINK_KVStoreInit key [%s]\r\n", key[i]);
        ret = InitFileByKey(key[i]);
        if (ret != HILINK_SAL_OK) {
            return ret;
        }
    }
    g_isKVInit = true;
    return HILINK_SAL_OK;
}

int HILINK_SetValue(const char *key, unsigned int offset, const unsigned char *value, unsigned int len)
{
    if ((key == NULL)  || (key[0] == '\0') || (value == NULL) || (len == 0)) {
        HILINK_SAL_WARN("invalid parm\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (!g_isKVInit) {
        HILINK_SAL_WARN("not init\r\n");
        return HILINK_SAL_NOT_INIT;
    }

    return WriteFileByKey(key, offset, value, len);
}

int HILINK_GetValue(const char *key, unsigned int offset, unsigned char *value, unsigned int len)
{
    if ((key == NULL) || (key[0] == '\0') || (value == NULL) || (len == 0)) {
        HILINK_SAL_WARN("invalid parm\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (!g_isKVInit) {
        HILINK_SAL_WARN("not init\r\n");
        return HILINK_SAL_NOT_INIT;
    }

    return ReadFileByKey(key, offset, value, len);
}

void HILINK_DeleteValue(const char * key)
{
    if ((key == NULL) || (key[0] == '\0')) {
        HILINK_SAL_NOTICE("invalid parm\r\n");
        return;
    }

    if (!g_isKVInit) {
        HILINK_SAL_WARN("not init\r\n");
        return;
    }

    DeleteFileByKey(key);
}

int HILINK_GetFileName(const char *key, char *out, unsigned int len)
{
    if (key == NULL || out == NULL || len < MAX_FILENAME_LEN) {
        HILINK_SAL_WARN("invalid parm\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    const char *fileName = key;
#ifdef HILINK_SDK_BUILD_IN
    fileName = FindFileName(key);
#endif
    if (strcpy_s(out, len, fileName) != EOK) {
        HILINK_SAL_WARN("cpy file name err\n");
        return HILINK_SAL_NOK;
    }
    return HILINK_SAL_OK;
}
