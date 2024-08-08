/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: Function headers of different chips in the NV module.
 */

#ifndef NV_PORTING_H
#define NV_PORTING_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "securec.h"
#include "soc_osal.h"
#include "osal_addr.h"
#include "nv_config.h"
#include "nv_key.h"

static inline void *kv_malloc(uint32_t x)
{
    return osal_kmalloc(x, 0);
}

static inline void *kv_zalloc(uint32_t x)
{
    return osal_kzalloc(x, 0);
}

static inline void kv_free(void *x)
{
    osal_kfree(x);
}

/*
 * nv_psram_malloc：从psram上分配内存存放NV数据
 * NV支持异步存储时(CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE特性宏设置为NV_YES)有效
 * 根据实际NV支持异步存储时NV数据存放器件进行配置
 * 默认内存分配函数为kv_malloc
*/
static inline void *nv_psram_malloc(uint32_t x)
{
    return kv_malloc(x);
}

/*
 * nv_psram_free：从psram上释放内存
 * NV支持异步存储时(CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE特性宏设置为NV_YES)有效
 * 根据实际NV支持异步存储时NV数据存放器件进行配置
 * 默认内存释放函数为kv_free
*/
static inline void nv_psram_free(void *x)
{
    kv_free(x);
}

errcode_t kv_flash_read(const uint32_t flash_offset, const uint32_t read_size, uint8_t *read_buffer);
errcode_t kv_flash_write(const uint32_t flash_offset, uint32_t write_size, const uint8_t *write_data, bool do_erase);
errcode_t kv_flash_erase(const uint32_t flash_addr, uint32_t size);

/* 创建加解密通道 */
errcode_t nv_crypto_claim_aes(uint32_t *crypto_handle, const kv_key_header_t *header);

/* 释放加解密通道 */
void nv_crypto_release_aes(uint32_t crypto_handle);

/* NV数据加密 */
errcode_t nv_crypto_encode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length);

/* NV数据解密 */
errcode_t nv_crypto_decode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length);

/* 获取加密Tag */
errcode_t nv_crypto_get_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t *tag_len);

/* 设置当前NV的Tag */
errcode_t nv_crypto_set_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t tag_len);

/* 校验Tag */
errcode_t nv_crypto_validate_tag(uint32_t crypto_handle);

/* 获取随机数 */
void nv_crypto_generate_random(uint32_t *rnd);

/* 启动HASH计算 */
errcode_t nv_crypto_start_hash(void);

/* 更新HASH */
errcode_t nv_crypto_update_hash(const uint8_t *src, uint32_t length);

/* 完成HASH计算并返回HASH值 */
errcode_t nv_crypto_complete_hash(uint8_t *hash);

#if (defined(CONFIG_NV_SUPPORT_DEBUG) && (CONFIG_NV_SUPPORT_DEBUG == NV_YES))
#define nv_log_err(fmt, arg...) osal_printk(fmt, ##arg)
#define nv_log_info(fmt, arg...) osal_printk(fmt, ##arg)
#define nv_log_debug(fmt, arg...) osal_printk(fmt, ##arg)
#else
#define nv_log_err(fmt, arg...)
#define nv_log_info(fmt, arg...)
#define nv_log_debug(fmt, arg...)
#endif

#endif /* NV_PORTING_H */

