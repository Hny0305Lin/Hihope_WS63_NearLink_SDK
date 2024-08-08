/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: the header file for main.
 *
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

__attribute__((section(".text.runtime.init"))) void copy_bin_to_ram(unsigned int *start_addr,
    const unsigned int *const load_addr, unsigned int size);
__attribute__((section(".text.runtime.init"))) void init_mem_value(unsigned int *start_addr,
    const unsigned int *const end_addr, unsigned int init_val);
__attribute__((section(".text.runtime.init"))) void do_relocation(void);
__attribute__((section(".text.runtime.init"))) void runtime_init(void);

extern void LOS_PrepareMainTask(void);

#ifdef WIFI_TASK_EXIST
extern int wifi_host_task(void *param);
extern __attribute__((weak)) int demo_init(void *param);
#endif

#ifdef CONFIG_SUPPORT_HILINK
extern __attribute__((weak)) int hilink_entry(void *param);
#endif
#ifdef BGLE_TASK_EXIST
extern void bt_thread_handle(void *para);
#ifdef SPARKLYZER_ACTIVATED
extern void splz_thread_handle(void *para);
#endif
#endif

#ifdef AT_COMMAND
void at_uart_init(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // __MAIN_H__
