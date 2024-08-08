/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: PRESERVE INTERFACE for security core
 * Author:
 * Create:
 */
#ifndef PRESERVE_H
#define PRESERVE_H

#include "exception.h"
#include "chip_core_definition.h"
#include "memory_config.h"
#include "panic.h"
#include "non_os_reboot.h"

/**
 * @defgroup connectivity_libs_preserve PRESERVE
 * @ingroup  connectivity_libs
 * @{
 */
#define PRESERVE var_segment("preserve")

typedef enum {
    STEP_DUMP_START = 1 << 0,

    STEP_BT_DUMP_RAM_FLASH_ERASE = 1 << 1,
    STEP_BT_DUMP_RAM_FLASH_WRITE = 1 << 2,
    STEP_APP_DUMP_RAM_FLASH_ERASE = 1 << 3,
    STEP_APP_DUMP_RAM_FLASH_WRITE = 1 << 4,
    STEP_HIFI_DUMP_RAM_FLASH_ERASE = 1 << 5,
    STEP_HIFI_DUMP_RAM_FLASH_WRITE = 1 << 6,
    STEP_DUMP_SHAREMEM_FLASH_ERASE = 1 << 7,
    STEP_DUMP_SHAREMEM_FLASH_WRITE = 1 << 8,
    STEP_SYSTEM_CORE_DUMP_BIT_ERASE = 1 << 9,
    STEP_SYSTEM_CORE_DUMP_BIT_WRITE = 1 << 10,

    STEP_DUMP_END = 1 << 31,
} dump_panic_step_t;

#if (defined(CHIP_SW39) && (CHIP_SW39 == 1))
void duplicate_app_preserve_mem(void);
#endif
bool is_preserve_data_saved(uintptr_t preserve_ptr);

void system_boot_status_init(void);

unsigned int get_system_boot_status(void);

panic_desc_t *get_last_panic_handle(void);

void set_last_panic(panic_desc_t *last_panic);
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
void set_exception_info_riscv(exc_context_t *exc_data);
task_context_t *get_exception_info_riscv(void);
#else
exception_stack_frame_t *get_exception_stack_frame_handle(void);

exception_info_t *get_exception_info(void);
#endif
void set_exception_stack_frame(exception_stack_frame_t *exception_stack_frame_src);
void set_exception_info(exception_info_t *exception_info);
void set_fault_address(uint32_t address);
void set_fault_reason(uint32_t reason);
void set_fault_intid(uint32_t intid);
void set_exception_dump_step(dump_panic_step_t step);

uint32_t get_fault_address(void);

uint32_t get_fault_reason(void);

uint32_t get_cpu_utils_check_sha_value(void);

void set_cpu_utils_check_sha_fault_value(void);

uint32_t get_exception_dump_step(void);

void clear_exception_dump_step(void);

uint8_t get_last_panic_id(void);

uint32_t get_last_panic_code(void);

uint32_t get_last_panic_caller(void);

uint32_t get_cpu_utils_exc_pc(void);

uint32_t get_cpu_utils_exc_lr(void);

void set_system_boot_status(unsigned int reboot_status);

void set_cpu_utils_system_boot_magic(void);

void set_cpu_utils_reset_cause(reboot_cause_t reset_cause);
unsigned int get_cpu_utils_reset_count(void);
unsigned int get_cpu_utils_epreset_count(void);
reboot_cause_t get_cpu_utils_reset_cause(void);

void set_update_reset_cause_on_boot(bool reset_cause_on_boot);

bool get_update_reset_cause_on_boot(void);

void set_pc_lr_sp_value(uint32_t pc_value, uint32_t lr_value, uint32_t sp_value);
void get_pc_lr_sp_value(uint32_t *pc_value, uint32_t *lr_value, uint32_t *sp_value);

void set_entry_sleep_time(uint64_t time);
void set_rtc_delay_count(uint64_t time);
void set_excepted_sleep_time(uint32_t time);
void set_exception_time_stamp(void);

uint8_t get_last_slave_panic_id(cores_t core);

uint32_t get_last_slave_panic_code(cores_t core);

uint32_t get_last_slave_panic_caller(cores_t core);

void system_boot_reason_print(void);

void system_boot_reason_process(void);

uint32_t get_system_magic(void);

void show_reboot_info(void);

/**
 * @}
 */
#endif
