/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  share memory configurations
 *
 * Create:  2021-06-16
 */

#ifndef SHARE_MEM_CONFIG_H
#define SHARE_MEM_CONFIG_H

#include "td_type.h"

/* ROM的data段在初始阶段需要从flash拷贝到ram */
extern unsigned int __rom_data_begin__;
extern unsigned int __rom_data_load__;
extern unsigned int __rom_data_size__;
/* ROM的bss段在初始阶段需要清0 */
extern unsigned int __rom_bss_begin__;
extern unsigned int __rom_bss_end__;

/* FlashPatch机制用到的remap table & cmp table; 和ROM2.0机制的跳转表 */
extern unsigned int __rom_patch_begin__;
extern unsigned int __rom_patch_load__;
extern unsigned int __rom_patch_size__;

/* TCM的text&rodata&data段在初始阶段需要从flash拷贝到ram */
extern unsigned int __tcm_text_begin__;
extern unsigned int __tcm_text_end__;
extern unsigned int __tcm_text_load__;
extern unsigned int __tcm_text_size__;
extern unsigned int __tcm_data_begin__;
extern unsigned int __tcm_data_load__;
extern unsigned int __tcm_data_size__;
/* TCM的bss段在初始阶段需要清0 */
extern unsigned int __tcm_bss_begin__;
extern unsigned int __tcm_bss_end__;

/* 放在SRAM运行的代码的text&rodata段在初始阶段需要从flash拷贝到ram */
extern unsigned int __sram_text_begin__;
extern unsigned int __sram_text_end__;
extern unsigned int __sram_text_load__;
extern unsigned int __sram_text_size__;

/* 除去放在ROM&TCM运行的代码，其他代码的data段，在初始阶段需要从flash拷贝到ram */
extern unsigned int __data_begin__;
extern unsigned int __data_load__;
extern unsigned int __data_size__;

/* 除去放在ROM&TCM运行的代码，其他代码的bss段，在初始阶段需要清0 */
extern unsigned int __bss_begin__;
extern unsigned int __bss_end__;

extern unsigned int __text_begin__;
extern unsigned int __text_end__;

#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
extern unsigned int __mem_rsv_load__;
extern unsigned int __mem_rsv_begin__;
extern unsigned int __mem_rsv_end__;
extern unsigned int __mem_rsv_size__;

static inline uintptr_t mem_rsv_load_addr_get(void)
{
    return (uintptr_t)(&__mem_rsv_load__);
}

static inline uintptr_t mem_rsv_start_addr_get(void)
{
    return (uintptr_t)(&__mem_rsv_begin__);
}

static inline uintptr_t mem_rsv_size_get(void)
{
    return (uintptr_t)(&__mem_rsv_size__);
}
#endif // end of CONFIG_MEMORY_CUSTOMIZE_RSV

#endif
