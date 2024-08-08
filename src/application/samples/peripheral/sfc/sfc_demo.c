/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides SFC sample source \n
 *
 * History: \n
 * 2024-03-04, Create file. \n
 */
#include "soc_osal.h"
#include "securec.h"
#include "sfc.h"
#include "sfc_porting.h"
#include "app_init.h"
#include "memory_config_common.h"

#define SFC_TASK_PRIO                   24
#define SFC_TASK_STACK_SIZE             0x1000
#define SFC_SAMPLE_LEN                  0x80000
#define SFC_PRINT_BUFF_LEN              32
uint8_t g_print_data_buff[SFC_PRINT_BUFF_LEN] = {0};
uint8_t g_write_data_buff[SFC_PRINT_BUFF_LEN] = {0};

static void sfc_sample_start_api_test(void)
{
    osal_printk("API test start\r\n");
    uint32_t remained_len = CONFIG_SFC_SAMPLE_USER_SIZE;
    uint32_t start_addr = CONFIG_SFC_SAMPLE_USER_ADDR;
    while (remained_len > 0) {
        uint32_t cur_len = remained_len > SFC_PRINT_BUFF_LEN ? SFC_PRINT_BUFF_LEN : remained_len;
        uapi_sfc_reg_read(start_addr, g_print_data_buff, cur_len);
        for (uint8_t i = 0; i < cur_len; i++) {
            osal_printk("%02x ", g_print_data_buff[i]);
        }
        uapi_sfc_reg_write(start_addr, g_write_data_buff, cur_len);
        start_addr += cur_len;
        remained_len -= cur_len;
        osal_printk("\r\n");
    }
    start_addr = CONFIG_SFC_SAMPLE_USER_ADDR;
    remained_len = CONFIG_SFC_SAMPLE_USER_SIZE;
    while (remained_len > 0) {
        uint32_t cur_len = remained_len > SFC_PRINT_BUFF_LEN ? SFC_PRINT_BUFF_LEN : remained_len;
        uapi_sfc_reg_read(start_addr, g_print_data_buff, cur_len);
        for (uint8_t i = 0; i < cur_len; i++) {
            osal_printk("%02x ", g_print_data_buff[i]);
        }
        start_addr += cur_len;
        remained_len -= cur_len;
        osal_printk("\r\n");
    }
}

static void *sfc_task(const char *arg)
{
    unused(arg);
    for (uint8_t i = 0; i < SFC_PRINT_BUFF_LEN; i++) {
        g_write_data_buff[i] = i;
    }
    /* Erase User space */
    osal_printk("Erasing for API sample...\r\n");
    errcode_t ret = uapi_sfc_reg_erase(CONFIG_SFC_SAMPLE_USER_ADDR, CONFIG_SFC_SAMPLE_USER_SIZE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("flash erase failed! ret = %x\r\n", ret);
        return NULL;
    }
    osal_printk("Start API read sample...\r\n");
    sfc_sample_start_api_test();
    return NULL;
}

static void sfc_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sfc_task, 0, "SFCTask", SFC_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SFC_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the spi_master_entry. */
app_run(sfc_entry);