/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: offline log file saved to the storage
 */

#include "log_file.h"
#include <unistd.h>
#include <stdlib.h>
#include "uapi_crc.h"
#include "errcode.h"
#include "securec.h"
#include "stdbool.h"
#include "soc_osal.h"
#include "common_def.h"
#include "dfx_adapt_layer.h"
#include "debug_print.h"
#include "log_file_common.h"
#include "log_file_file.h"
#include "log_file_flash.h"

#if (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == YES)

STATIC store_file_manage_t g_logfile_manage = { 0 };
STATIC osal_mutex g_instance_mutex = {NULL};      /* 实例互斥锁 */

store_file_manage_t* logfile_get_manage(void)
{
    return &g_logfile_manage;
}

bool logfile_check_record_head_valid(store_record_info_t *record_header)
{
    if (record_header->magic == RECORD_HEAD_MAGIC) {
        uint16_t crc = uapi_crc16(0, (uint8_t *)record_header, sizeof(store_record_info_t) - sizeof(uint16_t));
        if (crc == record_header->crc) {
            return true;
        }
    }
    return false;
}

void logfile_init_file_head(store_file_info_t *file_info)
{
#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)
    file_info->file_head.version = 2;   // flash存储version值为2
#else
    file_info->file_head.version = 1;   // 文件存储version值为1
#endif
    file_info->file_head.cur_pos = (uint32_t)sizeof(file_info->file_head);
    file_info->file_head.file_size = file_info->file_cfg.file_size;
    file_info->file_head.first_record_pos = (uint32_t)sizeof(file_info->file_head);
    file_info->file_head.offset = (uint8_t)sizeof(file_info->file_head);
    file_info->file_head.start_flag = FILE_HEAD_START_FLAG;
    file_info->file_head.records = 0;
    file_info->file_head.service_type = file_info->type;
    file_info->file_head.crc =
        uapi_crc16(0, (uint8_t *)&(file_info->file_head), sizeof(store_file_head_t) - sizeof(uint16_t));
}

STATIC errcode_t logfile_write_to_cache(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    store_cache_t *cache = file_info->cache;
    int32_t tmp_pos = 0;

    tmp_pos = (int32_t)cache->cache_read_pos;
    uint32_t record_len_written = cache->cache_size - cache->cache_write_pos;
    /* 判断数据是否需要翻转至cache头存储 */
    if (record_len_written >= data_len) {
        if (memcpy_s(&(cache->data[cache->cache_write_pos]), record_len_written, data, data_len) != EOK) {
            return ERRCODE_FAIL;
        }
        cache->cache_write_pos += data_len;
    } else {
        uint32_t record_len_remained = data_len - record_len_written;
        if (record_len_written != 0 &&
            memcpy_s(&(cache->data[cache->cache_write_pos]), record_len_written, data, record_len_written) != EOK) {
            return ERRCODE_FAIL;
        }
        cache->cache_write_pos = 0;

        if (record_len_remained != 0 &&
            memcpy_s(&(cache->data[cache->cache_write_pos]), (uint32_t)tmp_pos, data + record_len_written,
                     record_len_remained) != EOK) {
            return ERRCODE_FAIL;
        }
        cache->cache_write_pos += record_len_remained;
    }

    return ERRCODE_SUCC;
}

STATIC int logfile_save_process(void *arg)
{
    int32_t ret = 0;
    store_file_manage_t *manage = (store_file_manage_t *)arg;
    while (true) {
        for (int i = 0; i < (int)(sizeof(manage->file_info) / sizeof(manage->file_info[0])); i++) {
            manage->file_info[i].finish_flag = true;
        }

        ret = osal_event_read(&(manage->event), LOGFILE_SAVE_EVENT_MASK, OSAL_WAIT_FOREVER,
                              OSAL_WAITMODE_OR | OSAL_WAITMODE_CLR);

        for (int i = 0; i < (int)(sizeof(manage->file_info) / sizeof(manage->file_info[0])); i++) {
            if ((((uint32_t)ret & (1 << (uint32_t)i)) == (uint32_t)(1 << (uint32_t)i)) &&
                (manage->file_info[i].run_flag == true)) {
                manage->file_info[i].finish_flag = false;
#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)
                logfile_write_cache_to_flash(&manage->file_info[i]);
#else
                logfile_write_cache_to_file(&manage->file_info[i]);
#endif
            }
        }
    }

    return 0;
}

STATIC void logfile_timer_handler(unsigned long data)
{
    unused(data);
    osal_timer *timer = &(g_logfile_manage.timer_handle);

    for (int i = 0; i < (int)(sizeof(g_logfile_manage.file_info) / sizeof(g_logfile_manage.file_info[0]) - 1); i++) {
        if (g_logfile_manage.file_info[i].cache != NULL) {
            osal_event_write(&(g_logfile_manage.event), 1 << (uint32_t)i);
        }
    }

    osal_timer_start(timer);
}

STATIC void logfile_timer_stop(void)
{
    osal_timer *timer = &(g_logfile_manage.timer_handle);
    osal_timer_stop(timer);
}

STATIC errcode_t logfile_timer_init(void)
{
    osal_timer *timer = &(g_logfile_manage.timer_handle);
    timer->handler = logfile_timer_handler;
    timer->data = 0;

    timer->interval = LOGFILE_TIMER_PERIOD;
    if (osal_timer_init(timer) < 0) {
        return ERRCODE_FAIL;
    }
    osal_timer_start(timer);
    return ERRCODE_SUCC;
}

STATIC errcode_t logfile_create_thread(void)
{
    errcode_t ret = ERRCODE_DFX_LOGFILE_EVENT_FAILURE;
    if (g_logfile_manage.task_handle != NULL) {
        return ERRCODE_SUCC;
    }

    if ((osal_event_init(&g_logfile_manage.event)) != OSAL_SUCCESS) {
        goto err1;
    }
    ret = ERRCODE_DFX_LOGFILE_MUTEX_FAILURE;
    if (osal_mutex_init(&g_logfile_manage.file_write_mutex) != OSAL_SUCCESS) {
        goto err2;
    }

    osal_kthread_lock();
    g_logfile_manage.task_handle = osal_kthread_create(logfile_save_process,
        &g_logfile_manage, "log_save", LOGFILE_SAVE_TASK_SIZE);

    if (g_logfile_manage.task_handle == NULL) {
        ret = ERRCODE_DFX_LOGFILE_THREAD_FAILURE;
        osal_kthread_unlock();
        goto err3;
    }
    osal_kthread_set_priority(g_logfile_manage.task_handle, THREAD_PRIORITY_NUM);
    osal_kthread_unlock();

    if (logfile_timer_init() != OSAL_SUCCESS) {
        ret = ERRCODE_DFX_LOGFILE_TIMER_FAILURE;
        goto err4;
    }
    return ERRCODE_SUCC;
err4:
    if (g_logfile_manage.task_handle != NULL) {
        osal_kthread_destroy(g_logfile_manage.task_handle, 0);
    }
err3:
    osal_mutex_destroy(&g_logfile_manage.file_write_mutex);
err2:
    (void)osal_event_destroy(&g_logfile_manage.event);
err1:
    (void)memset_s(&g_logfile_manage, (sizeof(g_logfile_manage) - sizeof(store_file_info_t) * STORE_MAX),
                   0, (sizeof(g_logfile_manage) - sizeof(store_file_info_t) * STORE_MAX));
    dfx_log_err("logfile_create_thread failed. ret = 0x%x\r\n", ret);
    return ret;
}

STATIC errcode_t logfile_create_cache(store_file_info_t *file_info, store_file_cfg_t *cfg)
{
    if (file_info->cache == NULL) {
        file_info->cache = (store_cache_t *)osal_kzalloc_align(sizeof(store_cache_t) + cfg->cache_size,
                                                               OSAL_GFP_ZERO, 4); /* align 4 bytes */
        if (file_info->cache == NULL) {
            dfx_log_err("cache space create failed!\r\n");
            return ERRCODE_MALLOC;
        }
        file_info->cache->cache_size = cfg->cache_size;
        file_info->cache->threshold_size = cfg->cache_size * 1 / 4; /* threshold size is 1/4 of cache size */
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t logfile_file_write_with_cache(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    store_cache_t *cache = file_info->cache;
    int32_t cache_space_left;

    if (cache != NULL) {
        store_record_info_t record_info;
        record_info.magic = RECORD_HEAD_MAGIC;
        record_info.type = file_info->file_head.service_type;
        record_info.len = (uint16_t)sizeof(store_record_info_t) + (uint16_t)data_len;
        record_info.rev = 1;
#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO
        if (file_info->file_cfg.mult_files != 1 || file_info->file_cfg.cache_size <= 0) {
            return ERRCODE_INVALID_PARAM;
        }
        record_info.index = (uint16_t)(file_info->index);
        file_info->index = (file_info->index == MAX_INDEX_NUM) ? 1 : (file_info->index + 1);
#endif
        record_info.crc = uapi_crc16(0, (uint8_t *)&record_info, sizeof(store_record_info_t) - sizeof(uint16_t));

        if (cache->cache_read_pos > cache->cache_write_pos) {
            cache_space_left = (int32_t)(cache->cache_read_pos - cache->cache_write_pos);
        } else {
            cache_space_left = (int32_t)(cache->cache_size - cache->cache_write_pos + cache->cache_read_pos);
        }

        /* 如果cache空间不足，先把当前cache中数据全部写入flash或file */
        if (cache_space_left <= (int32_t)record_info.len) {
#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)
            logfile_write_cache_to_flash(file_info);
#else
            logfile_write_cache_to_file(file_info);
#endif
        }

        logfile_write_to_cache(file_info, (uint8_t *)&record_info, (uint32_t)sizeof(store_record_info_t));

        logfile_write_to_cache(file_info, data, data_len);

        if (cache_space_left < (int32_t)cache->threshold_size) {
            /* cache剩余空间小于门限时，触发保存 */
            osal_event_write(&(g_logfile_manage.event), 1 << (uint32_t)file_info->type);
        }
    }

    return ERRCODE_SUCC;
}

STATIC void logfile_free_file_info(store_file_info_t *file_info)
{
#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    if (file_info->idx_fd != 0) {
        close(file_info->idx_fd);
        file_info->idx_fd = 0;
    }

    if (file_info->fd != 0) {
        close(file_info->fd);
        file_info->fd = 0;
    }
#else
    file_info->fd = 0;
#endif

    if (file_info->cache != NULL) {
        osal_kfree(file_info->cache);
        file_info->cache = NULL;
    }

    (void)memset_s(file_info, sizeof(store_file_info_t), 0, sizeof(store_file_info_t));
}

STATIC void logfile_free_os_resouce(void)
{
    if (g_logfile_manage.task_handle != NULL) {
        logfile_timer_stop();
        (void)osal_timer_destroy(&g_logfile_manage.timer_handle);
        osal_kthread_destroy(g_logfile_manage.task_handle, 0);

        (void)osal_event_destroy(&g_logfile_manage.event);
        osal_mutex_destroy(&g_logfile_manage.file_write_mutex);

        (void)memset_s(&g_logfile_manage, (sizeof(g_logfile_manage) - sizeof(store_file_info_t) * STORE_MAX),
            0, (sizeof(g_logfile_manage) - sizeof(store_file_info_t) * STORE_MAX));
    }
}

STATIC errcode_t logfile_write(store_file_info_t *file_info, uint8_t *data, uint32_t data_len)
{
    errcode_t ret = ERRCODE_SUCC;
    if (file_info->file_cfg.mult_files < 1) {
        return ERRCODE_INVALID_PARAM;
    }

    if (file_info->run_flag == false) {
        return ERRCODE_DFX_LOGFILE_SUSPENDED;
    }

    if (file_info->file_cfg.enable_cache == 0) {
#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
        if (file_info->file_cfg.mult_files <= 1) {
            ret = logfile_single_file_write(file_info, data, data_len);
        } else {
            ret = logfile_multi_file_write(file_info, data, data_len);
        }
#endif
    } else {
        ret = logfile_file_write_with_cache(file_info, data, data_len);
    }
    return ret;
}

errcode_t uapi_logfile_write(store_service_t service_type, uint8_t sub_type, uint8_t *data, uint32_t data_len)
{
    errcode_t ret;

    unused(sub_type);
    if (service_type >= STORE_MAX || data == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);
    osal_mutex_lock(&g_instance_mutex);
    ret = logfile_write(file_info, data, data_len);
    osal_mutex_unlock(&g_instance_mutex);
    return ret;
}

errcode_t uapi_logfile_init(void)
{
    if (g_instance_mutex.mutex != NULL) {
        return ERRCODE_SUCC;
    }

    if (osal_mutex_init(&g_instance_mutex) != OSAL_SUCCESS) {
        return ERRCODE_DFX_LOGFILE_MUTEX_FAILURE;
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t logfile_open(store_file_info_t *file_info)
{
    errcode_t ret = ERRCODE_SUCC;
    /* 判断文件是否打开 */
    if (file_info->fd > 0) {
        return ERRCODE_DFX_LOGFILE_ALREADY_OPEN;
    }

#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)
    if (file_info->file_cfg.enable_cache != 1) {
        return ERRCODE_INVALID_PARAM;
    }

    if (logfile_flash_prepare(file_info) != ERRCODE_SUCC) {
        ret = ERRCODE_DFX_LOGFILE_FLASH_PREPARE_FAIL;
        goto err;
    }
    file_info->fd = (int32_t)file_info->type + 1; /* 在无文件系统场景下fd作为open成功的标记 */
#else
    if (file_info->file_cfg.path == NULL || file_info->file_cfg.name == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (logfile_create_path(&(file_info->file_cfg)) != ERRCODE_SUCC) {
        ret = ERRCODE_DFX_LOGFILE_MKDIR_FATAL;
        goto err;
    }

    ret = logfile_prepare_file_fd(file_info, &(file_info->file_cfg));
    if (ret != ERRCODE_SUCC) {
        goto err;
    }
#endif
    if (file_info->file_cfg.enable_cache != 0) {
        ret = logfile_create_cache(file_info, &(file_info->file_cfg));
        if (ret != ERRCODE_SUCC) {
            goto err;
        }
        ret = logfile_create_thread();
        if (ret != ERRCODE_SUCC) {
            goto err;
        }
    }
    file_info->run_flag = true;
    return ERRCODE_SUCC;
err:
    logfile_free_file_info(file_info);
    return ret;
}

errcode_t uapi_logfile_open(store_service_t service_type, const store_file_cfg_t *cfg)
{
    errcode_t ret;
    if (service_type >= STORE_MAX || cfg == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);
    file_info->type = service_type;
    file_info->file_cfg = *cfg;

    osal_mutex_lock(&g_instance_mutex);
    ret = logfile_open(file_info);
    osal_mutex_unlock(&g_instance_mutex);
    return ret;
}

STATIC errcode_t logfile_close(store_file_info_t *file_info)
{
    /* 如果句柄为NULL，说明已经close */
    if (file_info->fd == 0) {
        return ERRCODE_SUCC;
    }

    file_info->run_flag = false;
    if (file_info->file_cfg.enable_cache == 1) {
        /* 等待线程执行完成 */
        while (file_info->finish_flag == false) {
            osal_msleep(CLOSE_SLEEP_PERIOD);
        }
    }

    logfile_free_file_info(file_info);

    uint32_t i;
    for (i = 0; i < STORE_MAX; i++) {
        if (g_logfile_manage.file_info[i].fd != 0) {
            break;
        }
    }
    /* 如果全部文件关闭，删除线程和Timer */
    if (i >= STORE_MAX) {
        logfile_free_os_resouce();
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_logfile_close(store_service_t service_type)
{
    errcode_t ret;
    if (service_type >= STORE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);

    osal_mutex_lock(&g_instance_mutex);
    ret = logfile_close(file_info);
    osal_mutex_unlock(&g_instance_mutex);
    return ret;
}

errcode_t uapi_logfile_fsync(store_service_t service_type)
{
    if (service_type >= STORE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);
    if (file_info->fd == 0) {
        return ERRCODE_SUCC;
    }

#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
    osal_mutex_lock(&g_instance_mutex);
    fsync(file_info->fd);
    osal_mutex_unlock(&g_instance_mutex);
#endif

    return ERRCODE_SUCC;
}

errcode_t uapi_logfile_suspend(store_service_t service_type)
{
    if (service_type >= STORE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);
    osal_mutex_lock(&g_instance_mutex);
    file_info->run_flag = false;
    osal_mutex_unlock(&g_instance_mutex);
    return ERRCODE_SUCC;
}

errcode_t uapi_logfile_resume(store_service_t service_type)
{
    if (service_type >= STORE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);
    osal_mutex_lock(&g_instance_mutex);
    file_info->run_flag = true;
    osal_mutex_unlock(&g_instance_mutex);
    return ERRCODE_SUCC;
}

errcode_t uapi_logfile_reset(store_service_t service_type, store_file_cfg_t *cfg)
{
    errcode_t ret;
    if (service_type >= STORE_MAX || cfg == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    store_file_info_t *file_info = &(g_logfile_manage.file_info[service_type]);
    osal_mutex_lock(&g_instance_mutex);
    /* 判断文件是否打开 */
    if (file_info->fd > 0) {
        osal_mutex_unlock(&g_instance_mutex);
        return ERRCODE_DFX_LOGFILE_ALREADY_OPEN;
    }

#if defined(CONFIG_DFX_SUPPORT_FILE_SYSTEM) && (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)
    ret = logfile_flash_erase(service_type, cfg);
#else
    ret = logfile_remove_files(service_type, cfg);
#endif
    osal_mutex_unlock(&g_instance_mutex);
    return ret;
}

#endif /* CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE */