/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: KV Storage Library definitions
 */

#ifndef NV_CONFIG_H
#define NV_CONFIG_H

#define NV_TASKS_MAX_NUM             64             /* 可支持的NV保存任务的最大数（多核系统使用） */
#define NV_NORMAL_KVALUE_MAX_LEN     4060           /* 普通NV的最大数据长度 */
#define NV_ENCRYPTED_KVALUE_MAX_LEN  4048           /* 加密NV的最大数据长度 */

/* Flash 物理地址起始和结束 */
#define FLASH_PHYSICAL_ADDR_START     0x200000
#define FLASH_PHYSICAL_ADDR_END       0xA00000

#define FLASH_MAPPED_ADDR_START       0x000000
#define FLASH_MAPPED_END              0x800000

#define FLASH_PAGE_SIZE 4096
#define KV_PAGE_SIZE                  FLASH_PAGE_SIZE

/* NV运行区默认起始地址(物理地址) */
#define KV_STORE_START_ADDR           0x22C000
/* NV运行区page数 */
#define KV_STORE_DATA_PAGE_NUM        4
/* NV运行区默认长度（不包含备份区） */
#define KV_STORE_DATA_SIZE            (KV_STORE_DATA_PAGE_NUM * KV_PAGE_SIZE)

/* NV备份区默认起始地址(物理地址) */
#define KV_BACKUP_START_ADDR          (KV_STORE_START_ADDR + KV_STORE_DATA_SIZE)
/* NV备份区page数 */
#define KV_BACKUP_PAGE_NUM            4
/* NV备份区默认长度 */
#define KV_BACKUP_DATA_SIZE           (KV_BACKUP_PAGE_NUM * KV_PAGE_SIZE)

/**
 * Fixed number of pages for each KV store
 */
/* Consider using NV region contents to determine actual number of pages and stores */
#define KV_STORE_PAGES_SCPU    0
#define KV_STORE_PAGES_ACPU    3

#define MCORE_REGISTER_NV_NOTIFY_MAX_NUM 10

/*
 * NV处理数据的块大小（取值范围[128 ~ 4096]，必须为16字节倍数）
 * 此长度影响NV内部处理数据所需的内存大小，如内存紧张，则不宜太大。
 * 在需要加密NV但又不支持分段加解密的情况下，最好设置为最大值4096，以避免数据被分段处理。
 */
#define NV_KEY_DATA_CHUNK_LEN            128

/*
 * NV支持异步存储时(CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE特性宏设置为NV_YES)相关配置项
 * NV_QUEUE_MAX_SIZE：消息队列大小
 * NV_THREAD_PRIORITY_NUM：NV异步存储线程优先级
 * NV_BACKET_SIZE: 哈希桶大小
 * 哈希桶大小建议配置为总NV数量/0.75
 * 哈希桶过大占用内存较高，过小则影响查询效率
*/
#define NV_QUEUE_MAX_SIZE                64
#define NV_THREAD_PRIORITY_NUM           26
#define NV_BACKET_SIZE                   22

/* -------------------------------------  特性宏定义 ------------------------------------- */

#define NV_YES     1
#define NV_NO      0

/* 特性： 支持NV升级 */
#ifndef CONFIG_NV_SUPPORT_OTA_UPDATE
#define CONFIG_NV_SUPPORT_OTA_UPDATE            NV_YES
#endif

/* 特性： 支持NV备份恢复 */
#ifndef CONFIG_NV_SUPPORT_BACKUP_RESTORE
#define CONFIG_NV_SUPPORT_BACKUP_RESTORE        NV_YES
#endif

/* 特性： 支持NV加密 */
#ifndef CONFIG_NV_SUPPORT_ENCRYPT
#define CONFIG_NV_SUPPORT_ENCRYPT               NV_NO
#endif

/* 特性： 支持NV跳过被破坏的NV项 */
#ifndef CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY
#define CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY      NV_YES
#endif

/* 特性： 加密NV使用HASH校验 */
#ifndef CONFIG_NV_SUPPORT_HASH_FOR_CRYPT
#define CONFIG_NV_SUPPORT_HASH_FOR_CRYPT        NV_NO
#endif

/* NV debug */
#ifndef CONFIG_NV_SUPPORT_DEBUG
#define CONFIG_NV_SUPPORT_DEBUG                 NV_NO
#endif

/* 特性：NV异步存储 */
#ifndef CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE
#define CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE    NV_NO
#endif

/* 特性：NV使用CRC16替换CRC32 */
#ifndef CONFIG_NV_SUPPORT_CRC16_VERIFY
#define CONFIG_NV_SUPPORT_CRC16_VERIFY          NV_NO
#endif

/* 特性：NV备份单独存放 */
#ifndef CONFIG_NV_SUPPORT_BACKUP_REGION
#define CONFIG_NV_SUPPORT_BACKUP_REGION         NV_YES
#endif

/* 特性：NV备份区支持升级 */
#ifndef CONFIG_NV_SUPPORT_BACKUP_UPGRADE
#define CONFIG_NV_SUPPORT_BACKUP_UPGRADE        NV_NO
#endif

#endif /* NV_CONFIG_H */

