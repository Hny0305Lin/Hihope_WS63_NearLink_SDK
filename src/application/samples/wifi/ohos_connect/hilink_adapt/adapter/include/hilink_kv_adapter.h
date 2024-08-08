/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLinkKV 头文件（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_KV_ADAPTER_H
#define HILINK_KV_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化KV存储模块
 *
 * @param path [IN] 存储数据的路径，仅文件系统使用
 * @param key [IN] 存储数据的标识列表，数组可读长度至少为num
 * @param num [IN] 存储数据标识数量
 * @return 0成功，其他失败
 */
int HILINK_KVStoreInit(const char *path, const char *key[], unsigned int num);

/**
 * @brief 根据标识持久化存储数据
 *
 * @param key [IN] 存储数据的标识
 * @param offset [IN] 存储数据的偏移
 * @param value [IN] 存储的数据，可读长度至少为len
 * @param len [IN] 存储数据的长度
 * @return 0成功，其他失败
 * @attention 对于支持文件系统的产品HiLink SDK会直接使用该接口对预置配置文件进行写入，key为文件名
 */
int HILINK_SetValue(const char *key, unsigned int offset, const unsigned char *value, unsigned int len);

/**
 * @brief 根据标识读取数据
 *
 * @param key [IN] 存储数据的标识
 * @param offset [IN] 读取数据的偏移
 * @param value [OUT] 读取数据的缓冲区,可写长度至少为len
 * @param len [IN] 读取数据的缓冲区大小
 * @return 0成功，其他失败
 * @attention 对于未存储过的key读取不应返回错误，对于支持文件系统的产品
 *            HiLink SDK会直接使用该接口对预置配置文件进行读出，key为文件名
 */
int HILINK_GetValue(const char *key, unsigned int offset, unsigned char *value, unsigned int len);

/**
 * @brief 根据指定标识删除数据
 *
 * @param key [IN] 存储数据的标识
 * @attention 删除不应影响该标识数据的再次读写
 */
void HILINK_DeleteValue(const char *key);

/**
 * @brief 根据key查询文件名
 *
 * @param key [IN] 存储数据的标识
 * @param out [IN][OUT] 保存key对应的文件名
 * @param len [IN] 文件名最大长度
 * @return 0成功，其他失败
 */
int HILINK_GetFileName(const char *key, char *out, unsigned int len);
#define FILE_PATH_LEN_MAX 100
#define FILE_IS_CREATED 1
#define FILE_NOT_CREATE 0
#define FILE_SIZE_MAX 0x1000
typedef struct hilink_file {
    char file_path[FILE_PATH_LEN_MAX];
    char *file_handle;
    unsigned short file_size;
    unsigned short pos;
    char isCreat;
} hilink_file_t;
#ifdef __cplusplus
}
#endif
#endif /* HILINK_KV_ADAPTER_H */