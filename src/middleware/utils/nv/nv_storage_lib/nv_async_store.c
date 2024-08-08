/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV Async Store Used Hash
 */

#include "nv_porting.h"
#include "nv_config.h"
#include "nv_storage_handle.h"
#include "common_def.h"

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
#include "osal_task.h"
#include "osal_msgqueue.h"
#include "osal_semaphore.h"
#include "cstl_hash.h"
#include "nv_async_store.h"

#define MAX_BINARY_VAL 1
#define NV_QUEUE_ID 0XFFFFFF
#define NV_MSG_MAX_SIZE sizeof(nv_queue_msg_handle_t)

static unsigned long g_nv_msg_queue_id = NV_QUEUE_ID;
static uint32_t g_nv_async_flash_space = KV_STORE_DATA_SIZE;
osal_semaphore nv_async_sem;
CstlHash *g_hash_table = NULL;

static void nv_async_hash_init(void)
{
    /* 使用了自定义的数据类型 注册nv_psram_free函数 */
    CstlDupFreeFuncPair value_func = {NULL, nv_psram_free};
    g_hash_table = CstlHashCreate(NV_BACKET_SIZE, NULL, NULL, NULL, &value_func);
}

static void nv_async_hash_add(nv_key_map_t* cur_key)
{
    CstlHashIterator it = CstlHashFind(g_hash_table,  cur_key->key_id);
    if (it != CstlHashIterEnd(g_hash_table)) {
        (void)CstlHashErase(g_hash_table,  cur_key->key_id);
    }
    uint32_t cur_key_size = sizeof(nv_key_map_t) + (cur_key->key_id * sizeof(uint8_t));
    int ret = CstlHashInsert(g_hash_table, cur_key->key_id, sizeof(uint16_t), (uintptr_t)cur_key, (size_t)cur_key_size);
}

static errcode_t nv_asynv_send_msg_to_write_flash(nv_key_map_t* cur_key)
{
    nv_queue_msg_handle_t nv_msg;
    nv_msg.key_id = cur_key->key_id;
    nv_msg.key_ram_location = (uintptr_t)cur_key;
    int ret = osal_msg_queue_write_copy(g_nv_msg_queue_id, (void *)&nv_msg, sizeof(nv_queue_msg_handle_t), 0);
    if (ret != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static bool nv_async_check_writeable(uint16_t key, nv_key_attr_t *attr, uint16_t kvalue_length)
{
    uintptr_t value;
    if (CstlHashAt(g_hash_table, key, &value) != CSTL_OK) {
        return true;
    }

    nv_key_map_t *cur_key = (nv_key_map_t *)value;
    if (cur_key->permanent == 1 || (cur_key->encrypted == 1 &&
    (attr == NULL || (attr != NULL && attr->encrypted == false)))
        || (attr == NULL && cur_key->upgrade == 1)) {
        return false;
    }

    return true;
}

static uint16_t nv_async_calculate_flash_size(uint16_t kvlaue_length, nv_key_attr_t *attr)
{
    uint16_t size;
    kv_attributes_t attributes = {0};
    uint16_t hash_crc_size = KV_CRYPTO_CRC_SIZE;

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (attr->encrypted) {
        hash_crc_size = KV_CRYPTO_HASH_SIZE;
        attributes = KV_ATTRIBUTE_ENCRYPTED;
    }
#endif

    size = (uint16_t)sizeof(kv_key_header_t) + hash_crc_size;
    size += kv_key_padded_data_length(attributes, kvlaue_length);

    return size;
}

static errcode_t nv_async_update_attr(nv_key_map_t* cur_key, nv_key_attr_t *attr)
{
    if (attr == NULL) {
        return ERRCODE_SUCC;
    }

    /* 保持hash表中数据和传入的属性保持一致 */
    cur_key->permanent = attr->permanent;
    cur_key->encrypted = attr->encrypted;
    cur_key->upgrade = attr->non_upgrade;

    return ERRCODE_SUCC;
}

static errcode_t nv_async_update_value(nv_key_map_t* cur_key, uint16_t key, const uint8_t *kvalue,
    uint16_t kvalue_length, nv_key_attr_t *attr)
{
    errcode_t res = ERRCODE_SUCC;
    cur_key->key_id = key;
    cur_key->key_len = kvalue_length;
    if (memcpy_s(cur_key->kvalue, cur_key->key_len, kvalue, cur_key->key_len) != EOK) {
        nv_log_err("[NV] nv async update value: memcpy_s fail!\r\n");
        return ERRCODE_FAIL;
    }
    (void)nv_async_update_attr(cur_key, attr);
    res = nv_asynv_send_msg_to_write_flash(cur_key);
    return res;
}

static errcode_t nv_async_add_key(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr)
{
    errcode_t ret;
    nv_key_map_t *cur_key = NULL;
    uint32_t cur_key_size = sizeof(nv_key_map_t) + kvalue_length * (sizeof(uint8_t));
    cur_key = (nv_key_map_t *)nv_psram_malloc(cur_key_size);
    if (cur_key == NULL) {
        nv_log_err("[NV] nv async add key: malloc fail!\r\n");
        return ERRCODE_MALLOC;
    }
    if (memset_s(cur_key, cur_key_size, 0, cur_key_size) != EOK) {
        nv_log_err("[NV] nv async add key: memeset_s fail!\r\n");
        nv_psram_free(cur_key);
        return ERRCODE_FAIL;
    }
    ret = nv_async_update_value(cur_key, key, kvalue, kvalue_length, attr);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_async_update_value fail!\r\n");
        nv_psram_free(cur_key);
        return ERRCODE_FAIL;
    }
    (void)nv_async_hash_add(cur_key);
    return ERRCODE_SUCC;
}

static errcode_t nv_async_store_init_keys_one_page(kv_page_handle_t *page)
{
    errcode_t res;
    kv_key_handle_t key;
    nv_key_map_t *cur_key = NULL;

    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);
    while (key_found == ERRCODE_SUCC) {
        if (kv_key_is_valid(&key)) {
            /* 给后续的柔性数组分配内存指定大小的内存 */
            uint32_t cur_key_size = sizeof(nv_key_map_t) + (key.header.length * sizeof(uint8_t));

            cur_key = (nv_key_map_t *)nv_psram_malloc(cur_key_size);
            if (cur_key == NULL) {
                nv_log_err("[NV] nv async store init keys one page: malloc fail!\r\n");
                nv_psram_free(cur_key);
                return ERRCODE_MALLOC;
            }
            if (memset_s(cur_key, cur_key_size, 0, cur_key_size) != EOK) {
                nv_log_err("[NV] nv async store init keys one page: memset_s fail!\r\n");
                nv_psram_free(cur_key);
                return ERRCODE_FAIL;
            }
            cur_key->key_id = key.header.key_id;
            cur_key->key_len = key.header.length;
            /* 对三个属性进行判断 */
            if (key.header.type != KV_KEY_TYPE_NORMAL) {
                cur_key->permanent = 1;
                cur_key->upgrade = 1;
            }
            if (key.header.enc_key != 0) {
                cur_key->encrypted = 1;
                cur_key->upgrade = 1;
            }
            if (key.header.upgrade != KV_KEY_TYPE_NORMAL) {
                cur_key->upgrade = 1;
            }
            /* 传入柔性数组的数组名既首地址，获取flash数据 */
            res = kv_key_read_data(&key, cur_key->kvalue);
            if (res != ERRCODE_SUCC) {
                nv_log_err("[NV] nv async store init keys one page: kv key read data fail key= 0x%d\r\n",
                    key.header.key_id);
                nv_psram_free(cur_key);
                return ERRCODE_FAIL;
            }
            (void)nv_async_hash_add(cur_key);
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }
    return ERRCODE_SUCC;
}

errcode_t nv_async_read_with_attr(uint16_t key, uint16_t kvalue_max_length,
    uint16_t *kvalue_length, uint8_t *kvalue, nv_key_attr_t *attr)
{
    if (g_hash_table == NULL) {
        nv_log_err("[NV] nv async read with attr: g_hash_table not init!\r\n");
        return ERRCODE_NV_INIT_FAILED;
    }

    if (osal_sem_down_timeout(&nv_async_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        nv_log_err("[NV] nv async read with attr: semaphore error!\r\n");
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    uintptr_t value;
    if (CstlHashAt(g_hash_table, key, &value) != CSTL_OK) {
        osal_sem_up(&nv_async_sem);
        nv_log_err("[NV] nv async read with attr: not found key 0x%x!\r\n", key);
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    nv_key_map_t *cur_key = (nv_key_map_t *)value;

    *kvalue_length = cur_key->key_len;
    if (kvalue_max_length < cur_key->key_len) {
        osal_sem_up(&nv_async_sem);
        return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(kvalue, cur_key->key_len, cur_key->kvalue, cur_key->key_len) != EOK) {
        osal_sem_up(&nv_async_sem);
        nv_log_err("[NV] nv async read with attr: memcpy_s fail!\r\n");
        return ERRCODE_FAIL;
    }
    if (cur_key->permanent == 1) {
        attr->permanent = true;
        attr->non_upgrade = true;
    }
    if (cur_key->encrypted == 1) {
        attr->encrypted = true;
        attr->non_upgrade = true;
    }
    if (cur_key->upgrade == 1) {
        attr->non_upgrade = true;
    }
    osal_sem_up(&nv_async_sem);
    return ERRCODE_SUCC;
}

errcode_t nv_async_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr,
    nv_storage_completed_callback func)
{
    unused(func);
    errcode_t ret;
    if (g_hash_table == NULL) {
        nv_log_err("[NV] nv async write with attr: g_hash_table not init!\r\n");
        return ERRCODE_NV_INIT_FAILED;
    }

    if (osal_sem_down_timeout(&nv_async_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        nv_log_err("[NV] nv async write with attr: semaphore error!\r\n");
        return ERRCODE_NV_SEM_WAIT_ERR;
    }
    if (nv_async_check_writeable(key, attr, kvalue_length) == false) {
        osal_sem_up(&nv_async_sem);
        nv_log_info("[NV] nv async write with attr: not support write!\r\n");
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }

    uint32_t flash_size = nv_async_calculate_flash_size(kvalue_length, attr);
    if (g_nv_async_flash_space < flash_size) {
        osal_sem_up(&nv_async_sem);
        nv_log_info("[NV] nv async write with attr: not enough space!\r\n");
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }
    /* 如果hash表里没旧key，直接新增一个节点 */
    CstlHashIterator it = CstlHashFind(g_hash_table, key);
    if (it == CstlHashIterEnd(g_hash_table)) {
        ret = nv_async_add_key(key, kvalue, kvalue_length, attr);
        osal_sem_up(&nv_async_sem);
        return ret;
    }

    nv_key_map_t *old_key = (nv_key_map_t *)CstlHashIterValue(g_hash_table, it);
    /* 哈希表里存在旧key,且长度大于新写入key的长度，更新节点值 */
    if (old_key->key_len >= kvalue_length) {
        ret = nv_async_update_value(old_key, key, kvalue, kvalue_length, attr);
    } else {
        /* 否则，新建节点 */
        ret = nv_async_add_key(key, kvalue, kvalue_length, attr);
    }

    osal_sem_up(&nv_async_sem);
    return ret;
}

static errcode_t nv_async_msg_process(nv_queue_msg_handle_t *msg_data_t)
{
    errcode_t res;
    CstlHashIterator it;

    if (osal_sem_down_timeout(&nv_async_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        nv_log_err("[NV] nv async msg process: semaphore error!\r\n");
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    it = CstlHashFind(g_hash_table,  msg_data_t->key_id);
    if (it == CstlHashIterEnd(g_hash_table)) {
        nv_log_info("[NV] nv async msg process not found key in hashtable!\r\n");
        osal_sem_up(&nv_async_sem);
        return ERRCODE_FAIL;
    }

    if (msg_data_t->key_ram_location != (uint32_t)CstlHashIterValue(g_hash_table, it)) {
        nv_log_info("[NV] nv async msg process location not same!\r\n");
        osal_sem_up(&nv_async_sem);
        return ERRCODE_FAIL;
    }
    nv_key_map_t *msg_data =  (nv_key_map_t *)msg_data_t->key_ram_location;
    nv_key_attr_t attr;
    attr.permanent = (msg_data->permanent == 1) ? true : false;
    attr.encrypted = (msg_data->encrypted == 1) ? true : false;
    attr.non_upgrade = (msg_data->upgrade == 1) ? true : false;
    nv_log_debug("[NV] nv async msg process key=0x%x,len=%d,attr %d %d %d\r\n",
        msg_data->key_id, msg_data->key_len, attr.permanent, attr.encrypted, attr.non_upgrade);
    res = nv_direct_write_with_attr(msg_data->key_id, msg_data->kvalue, msg_data->key_len, &attr, NULL);
    nv_store_status_t status;

    res = nv_direct_get_store_status(&status);
    if (res != ERRCODE_SUCC) {
        nv_log_err("[NV] nv async msg process flash_space get fail!\r\n");
        osal_sem_up(&nv_async_sem);
        return res;
    }

    g_nv_async_flash_space = status.max_key_space;
    osal_sem_up(&nv_async_sem);
    return res;
}

static errcode_t nv_async_msg_process_thread(void)
{
    nv_queue_msg_handle_t msg_data_t;
    uint32_t msg_data_size = sizeof(nv_queue_msg_handle_t);

    while (true) {
        (void)memset_s(&msg_data_t, msg_data_size, 0, msg_data_size);
        osal_msg_queue_read_copy(g_nv_msg_queue_id, &msg_data_t, &msg_data_size, OSAL_MSGQ_WAIT_FOREVER);
        nv_async_msg_process(&msg_data_t);
    }

    return ERRCODE_SUCC;
}

static errcode_t nv_async_thread_msg_event_init(void)
{
    int ret;
    ret = osal_msg_queue_create("nv_write_task", NV_QUEUE_MAX_SIZE, &g_nv_msg_queue_id,
                                0, NV_MSG_MAX_SIZE);
    if (ret != 0) {
        nv_log_err("[NV] nv async thread msg event init: osal msg queue create!\r\n");
        return ERRCODE_FAIL;
    }
    osal_kthread_lock();
    osal_task *task = osal_kthread_create(nv_async_msg_process_thread, NULL, "nv_write_task", 0x1000);
    if (task == NULL) {
        osal_kthread_unlock();
        nv_log_err("[NV] nv async thread msg event init:: osal kthread create!\r\n");
        return ERRCODE_FAIL;
    }
    osal_kthread_set_priority(task, NV_THREAD_PRIORITY_NUM);
    osal_kthread_unlock();
    return ERRCODE_SUCC;
}

errcode_t nv_async_flush(void)
{
    nv_queue_msg_handle_t msg_data_t;
    uint32_t msg_data_size = sizeof(nv_queue_msg_handle_t);
    uint8_t cur_queue_num = (uint8_t)osal_msg_queue_get_msg_num(g_nv_msg_queue_id);

    while (cur_queue_num > 0) {
        (void)memset_s(&msg_data_t, msg_data_size, 0, msg_data_size);
        osal_msg_queue_read_copy(g_nv_msg_queue_id, &msg_data_t, &msg_data_size, OSAL_MSGQ_NO_WAIT);
        nv_async_msg_process(&msg_data_t);
        cur_queue_num--;
    }

    return ERRCODE_SUCC;
}

errcode_t nv_key_map_init(kv_store_t store, uint8_t num_pages)
{
    errcode_t ret;
    kv_page_handle_t kv_page;
    nv_store_status_t status;
    (void)nv_direct_get_store_status(&status);
    g_nv_async_flash_space = status.max_key_space;
    for (uint8_t page_index = 0; page_index < num_pages; page_index++) {
        ret = kv_store_get_page_handle(store, page_index, &kv_page);
        if (ret != ERRCODE_SUCC) {
            nv_log_err("[NV] nv_key_map_init: get page handle error!\r\n");
            return ret;
        }
        ret = nv_async_store_init_keys_one_page(&kv_page);
        if (ret != ERRCODE_SUCC) {
            nv_log_err("[NV] nv_key_map_init: init keys one page error!\r\n");
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t nv_async_init(kv_store_t store, uint8_t num_pages)
{
    errcode_t ret;
    if (osal_sem_binary_sem_init(&nv_async_sem, MAX_BINARY_VAL) != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_key_map_init: osal sem binary sem init!\r\n");
        return ERRCODE_FAIL;
    }

    /* 创建哈希表 */
    (void)nv_async_hash_init();
    if (g_hash_table == NULL) {
        nv_log_err("[NV] nv_key_map_init: nv async hash init fail!\r\n");
        return ERRCODE_FAIL;
    }

    /* 创建nv写线程 */
    ret = nv_async_thread_msg_event_init();
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_key_map_init: nv async thread msg event init!\r\n");
        return ret;
    }

    /* hash表数据初始化 */
    ret = nv_key_map_init(store, num_pages);
    return ret;
}

#endif /* #if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES) */

