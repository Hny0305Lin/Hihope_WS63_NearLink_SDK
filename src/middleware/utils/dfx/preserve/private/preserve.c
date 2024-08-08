/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:PRESERVE INTERFACE for security core
 * Author:
 * Create:
 */
#include "preserve.h"
#include "securec.h"
#if defined(BUILD_APPLICATION_STANDARD)
#include "log_printf.h"
#include "systick.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif
#include "debug_print.h"
#include "hal_reboot.h"
#ifdef SUPPORT_CRASH_DATA_RAM
#include "crash_data.h"
#endif

#define RIGHT_SHIFT_32BITS  32
#define WORD_MASK_VALUE  0xFFFFFFFF
#define TIME_S_TO_MS  1000
#define RESERVED_PC_INDEX 0
#define RESERVED_LR_INDEX 1
#define RESERVED_SP_INDEX 2

// all security core preserve data here, if want to add new,should add tail for compatible befor version
#pragma pack(4)
typedef struct {
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    exc_context_t exc_context;
#else
    exception_stack_frame_t exception_stack_frame;  // 32
#endif
    exception_info_t exception_info;                // 52
    panic_desc_t last_panic;                        // 12
    uint32_t entry_sleep_time[2];  // The size of the array is 2, which stores the high and low bits of sleep time.
    uint32_t rtc_delay_count[2];  // The size of the array is 2, which stores the high and low bits of rtc delay count.
    uint32_t excepted_delay_time;
    uint32_t exception_time_s;
    uint32_t reserved[3];                             // The size of the reserved array n is 3, total 12 bytes.
    reboot_cause_t cpu_utils_reset_cause;             // 4
    bool update_reset_cause_on_boot;                // 4
    unsigned long fault_reason;                     // 4
    unsigned long fault_address;                    // 4
    uint32_t intid;                                   // error int num
    uint32_t magic;                             // exception flag
    uint32_t reboot_status;
    unsigned int reboot_count;       // reboot count since coldboot
    unsigned int ep_reboot_count;    // exception reboot count continuously
    unsigned int enable_sha_check;   // total size until here: 172 Byte
} preserve_data_t;
#pragma pack()

typedef struct {
    uint32_t pc;
    uint32_t lr;
    uint32_t sp;
} preserve_cpu_info_t;

#ifdef __ICCARM__
#pragma location=PRESERVED_REGION_ORIGIN
__no_init preserve_data_t g_preserve_data_lib;
#else
PRESERVE preserve_data_t g_preserve_data_lib;
#endif
static preserve_data_t g_preserve_data_lib_ram;
#if !defined(BUILD_APPLICATION_ROM)
static preserve_data_t *g_preserve_data_lib_ptr = (preserve_data_t *)(uintptr_t)(PRESERVED_REGION_ORIGIN);
#endif

static void duplicate_preserve_mem(void)
{
    UNUSED(g_preserve_data_lib_ram);
#if CORE == MASTER_BY_ALL
    memcpy_s((void *)&g_preserve_data_lib_ram, sizeof(g_preserve_data_lib_ram), (void *)&g_preserve_data_lib,
             sizeof(g_preserve_data_lib_ram));
#endif
}

#if (defined(CHIP_SW39) && (CHIP_SW39 == 1))
void duplicate_app_preserve_mem(void)
{
    preserve_data_t *app_preserve_data = (preserve_data_t *)(uintptr_t)(APP_PRESERVED_REGION_ORIGIN);
    if (app_preserve_data->magic != STANDARD_REBOOT_MAGIC && app_preserve_data->magic != ABNORMAL_REBOOT_MAGIC) {
        memcpy_s((void *)app_preserve_data, sizeof(preserve_data_t), (void *)&g_preserve_data_lib_ram,
                 sizeof(g_preserve_data_lib_ram));
    }
}
#endif

bool is_preserve_data_saved(uintptr_t preserve_ptr)
{
    preserve_data_t *preserve_data = (preserve_data_t *)preserve_ptr;
    return preserve_data->magic == STANDARD_REBOOT_MAGIC || preserve_data->magic == ABNORMAL_REBOOT_MAGIC;
}

static inline void set_chip_system_boot_magic(uint32_t magic)
{
    g_preserve_data_lib.magic = magic;
}

static void system_set_reboot_status_by_crash_cause(void)
{
    reboot_cause_t cause = g_preserve_data_lib.cpu_utils_reset_cause;
    if ((REBOOT_CAUSE_BT_RESET_UNKNOWN <= cause && cause <= REBOOT_CAUSE_BT_END) ||
        (REBOOT_CAUSE_GNSS_GLOBAL <= cause && cause <= REBOOT_CAUSE_GNSS_END) ||
        (REBOOT_CAUSE_PROTOCOL_GLOBAL <= cause && cause <= REBOOT_CAUSE_PROTOCOL_END)) {
        g_preserve_data_lib.reboot_status = REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_HARDFAULT) {
        g_preserve_data_lib.reboot_status = REBOOT_HARD_FAULT_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_XIP_CTRL) {
        g_preserve_data_lib.reboot_status = REBOOT_NMI_XIP_CTRL_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_XIP_CACHE) {
        g_preserve_data_lib.reboot_status = REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_CHIP_WDT) {
        g_preserve_data_lib.reboot_status = REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_PANIC) {
        g_preserve_data_lib.reboot_status = REBOOT_SOFT_PANIC_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_STD_BT_WDT_FRST) {
        g_preserve_data_lib.reboot_status = REBOOT_BT_WDGTIMEOUT_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_STD_ULP_WDT_FRST) {
        g_preserve_data_lib.reboot_status = REBOOT_ULP_WDGTIMEOUT_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_APPLICATION_STD_ULP_PIN_FRST) {
        g_preserve_data_lib.reboot_status = REBOOT_ULP_PIN_RST_TRIGER_STATUS;
    } else if (cause == REBOOT_CAUSE_POWER_ON) {
        g_preserve_data_lib.reboot_status = REBOOT_POWER_ON_TRIGGER_STATUS;
    } else if (cause == REBOOT_CAUSE_UPG_COMPLETION) {
        g_preserve_data_lib.reboot_status = REBOOT_UPG_COMPLETION_TRIGER_STATUS;
    } else {
        g_preserve_data_lib.reboot_status = REBOOT_UNDEFINED_TRIGER_STATUS;
    }
}

#ifdef weak
weak void watchdog_porting_pmu_reboot(void)
{
}
#else
__attribute__((weak)) void watchdog_porting_pmu_reboot(void)
{
}
#endif

void system_boot_status_init(void)
{
    bool is_hardwdg_timeout = false;
    if (!hal_reboot_get_ulp_aon_no_poweroff_flag()) {
        hal_reboot_set_ulp_aon_no_poweroff_flag();
        memset_s((void *)&g_preserve_data_lib, sizeof(g_preserve_data_lib), 0, sizeof(g_preserve_data_lib));
    }
    if (g_preserve_data_lib.magic == 0) {    // Hard power-off restart
        g_preserve_data_lib.reboot_status = REBOOT_POWER_ON_TRIGGER_STATUS;
        g_preserve_data_lib.cpu_utils_reset_cause = REBOOT_CAUSE_POWER_ON;
        g_preserve_data_lib.update_reset_cause_on_boot = true;
        watchdog_porting_pmu_reboot();
    }
    is_hardwdg_timeout = hal_reboot_hard_wdg_timeout((uint16_t)g_preserve_data_lib.cpu_utils_reset_cause);
    if (is_hardwdg_timeout) {           // hard watchdog tiemout
        set_update_reset_cause_on_boot(false);
        g_preserve_data_lib.reboot_status = REBOOT_HARD_WDGTIMEOUT_TRIGER_STATUS;
        g_preserve_data_lib.cpu_utils_reset_cause = REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST;
    } else {                           // normal soft restart
        if (g_preserve_data_lib.magic == STANDARD_REBOOT_MAGIC) {
            g_preserve_data_lib.reboot_status = REBOOT_SOFT_RESET_TRIGER_STATUS;
        } else {
            system_set_reboot_status_by_crash_cause();
        }
    }

    if (get_update_reset_cause_on_boot()) {  // the system is restarted by writing the reset register.
        set_cpu_utils_reset_cause(REBOOT_CAUSE_UNKNOWN);
    }

    if (g_preserve_data_lib.reboot_status != REBOOT_POWER_ON_TRIGGER_STATUS) {
        g_preserve_data_lib.reboot_count++;
        if (g_preserve_data_lib.reboot_status == REBOOT_SOFT_RESET_TRIGER_STATUS) {
            g_preserve_data_lib.ep_reboot_count = 0;
            g_preserve_data_lib.enable_sha_check = 0;
        } else {
            g_preserve_data_lib.ep_reboot_count++;
        }
    } else {
        g_preserve_data_lib.reboot_status = REBOOT_POWER_ON_TRIGGER_STATUS;
        g_preserve_data_lib.cpu_utils_reset_cause = REBOOT_CAUSE_POWER_ON;
    }
    g_preserve_data_lib.magic = STANDARD_REBOOT_MAGIC;
}

unsigned int get_system_boot_status(void)
{
    return g_preserve_data_lib.reboot_status;
}

uint32_t get_system_magic(void)
{
    return g_preserve_data_lib.magic;
}

void set_cpu_utils_system_boot_magic(void)
{
    unsigned int magic = STANDARD_REBOOT_MAGIC;

    switch (get_cpu_utils_reset_cause()) {
        case REBOOT_CAUSE_BT_SYSRESETREQ:
        case REBOOT_CAUSE_PROTOCOL_SYSRESETREQ:
        case REBOOT_CAUSE_APPLICATION_SYSRESETREQ:
        case REBOOT_CAUSE_UNKNOWN:
        case REBOOT_CAUSE_POWER_ON:
        case REBOOT_CAUSE_UPG_COMPLETION:
            magic = STANDARD_REBOOT_MAGIC;
            break;

        default:
            /* unidentified normal reboot cause */
            magic = ABNORMAL_REBOOT_MAGIC;
            break;
    }

    set_chip_system_boot_magic(magic);
}

void set_cpu_utils_reset_cause(reboot_cause_t reset_cause)
{
    set_update_reset_cause_on_boot(false);
    g_preserve_data_lib.cpu_utils_reset_cause = reset_cause;
    duplicate_preserve_mem();
}

unsigned int get_cpu_utils_reset_count(void)
{
    return g_preserve_data_lib.reboot_count;
}

unsigned int get_cpu_utils_epreset_count(void)
{
    return g_preserve_data_lib.ep_reboot_count;
}

reboot_cause_t get_cpu_utils_reset_cause(void)
{
    return g_preserve_data_lib.cpu_utils_reset_cause;
}

void set_update_reset_cause_on_boot(bool reset_cause_on_boot)
{
    g_preserve_data_lib.update_reset_cause_on_boot = reset_cause_on_boot;
}

bool get_update_reset_cause_on_boot(void)
{
    return g_preserve_data_lib.update_reset_cause_on_boot;
}

uint32_t get_cpu_utils_check_sha_value(void)
{
    return g_preserve_data_lib.enable_sha_check;
}

void set_cpu_utils_check_sha_fault_value(void)
{
    g_preserve_data_lib.enable_sha_check = 0;
}

uint8_t get_last_panic_id(void)
{
    return g_preserve_data_lib.last_panic.origin;
}

uint32_t get_last_panic_code(void)
{
    return g_preserve_data_lib.last_panic.code;
}

uint32_t get_last_panic_caller(void)
{
    return g_preserve_data_lib.last_panic.caller;
}

uint32_t get_cpu_utils_exc_pc(void)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    return g_preserve_data_lib.exc_context.task_context.mepc;
#else
    return g_preserve_data_lib.exception_stack_frame.stacked_pc;
#endif
}

uint32_t get_cpu_utils_exc_lr(void)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    return g_preserve_data_lib.exc_context.task_context.ra;
#else
    return g_preserve_data_lib.exception_stack_frame.stacked_lr;
#endif
}

void set_entry_sleep_time(uint64_t time)
{
    g_preserve_data_lib.entry_sleep_time[0] = (uint32_t)(time & WORD_MASK_VALUE);
    g_preserve_data_lib.entry_sleep_time[1] = (uint32_t)((time >> RIGHT_SHIFT_32BITS) & WORD_MASK_VALUE);
}

void set_rtc_delay_count(uint64_t time)
{
    if ((get_system_boot_status() == REBOOT_POWER_ON_TRIGGER_STATUS) ||
        (get_system_boot_status() == REBOOT_SOFT_RESET_TRIGER_STATUS)) {
        g_preserve_data_lib.rtc_delay_count[0] = (uint32_t)(time & WORD_MASK_VALUE);
        g_preserve_data_lib.rtc_delay_count[1] = (uint32_t)((time >> RIGHT_SHIFT_32BITS) & WORD_MASK_VALUE);
    }
}

void set_excepted_sleep_time(uint32_t time)
{
    g_preserve_data_lib.excepted_delay_time = time;
}

void set_exception_time_stamp(void)
{
#if defined(BUILD_APPLICATION_STANDARD)
    g_preserve_data_lib.exception_time_s = (uint32_t)((uapi_systick_get_ms() + get_log_basetime_ms()) / TIME_S_TO_MS);
#endif
}

void set_system_boot_status(unsigned int reboot_status)
{
    g_preserve_data_lib.reboot_status = reboot_status;
}

panic_desc_t *get_last_panic_handle(void)
{
    return &g_preserve_data_lib.last_panic;
}

void set_last_panic(panic_desc_t *last_panic)
{
    memcpy_s((void *)&g_preserve_data_lib.last_panic, sizeof(panic_desc_t), (void *)last_panic, sizeof(panic_desc_t));
    duplicate_preserve_mem();
#ifdef SUPPORT_CRASH_DATA_RAM
    crash_data_set_panic(last_panic);
#endif
}

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
void set_exception_info_riscv(exc_context_t *exc_data)
{
    memcpy_s((void *)&g_preserve_data_lib.exc_context, sizeof(exc_context_t),
             (void *)exc_data, sizeof(exc_context_t));
    duplicate_preserve_mem();
}

task_context_t *get_exception_info_riscv(void)
{
    return &g_preserve_data_lib.exc_context.task_context;
}
#else
exception_stack_frame_t *get_exception_stack_frame_handle(void)
{
    return &g_preserve_data_lib.exception_stack_frame;
}

exception_info_t *get_exception_info(void)
{
    return &g_preserve_data_lib.exception_info;
}
#endif

void set_exception_info(exception_info_t *exception_info)
{
    if (exception_info == NULL) {
        return;
    }
    memcpy_s((void *)&g_preserve_data_lib.exception_info, sizeof(exception_info_t),
             (void *)exception_info, sizeof(exception_info_t));
    duplicate_preserve_mem();
#ifdef SUPPORT_CRASH_DATA_RAM
    crash_data_set_exception_info(exception_info);
#endif
}

void set_exception_stack_frame(exception_stack_frame_t *exception_stack_frame_src)
{
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    g_preserve_data_lib.exc_context.task_context.ra = (uint32_t)exception_stack_frame_src->stacked_lr;
    g_preserve_data_lib.exc_context.task_context.mepc = (uint32_t)exception_stack_frame_src->stacked_pc;
#else
    memcpy_s((void *)&g_preserve_data_lib.exception_stack_frame, sizeof(exception_stack_frame_t),
             (void *)exception_stack_frame_src, sizeof(exception_stack_frame_t));
#endif
    duplicate_preserve_mem();
}

void set_fault_address(uint32_t address)
{
    g_preserve_data_lib.fault_address = address;
    duplicate_preserve_mem();
}

uint32_t get_fault_address(void)
{
    return (uint32_t)g_preserve_data_lib.fault_address;
}

void set_fault_reason(uint32_t reason)
{
    g_preserve_data_lib.fault_reason = reason;
    duplicate_preserve_mem();
}

void set_fault_intid(uint32_t intid)
{
    g_preserve_data_lib.intid = intid;
    duplicate_preserve_mem();
}

uint32_t get_fault_reason(void)
{
    return (uint32_t)g_preserve_data_lib.fault_reason;
}

void set_pc_lr_sp_value(uint32_t pc_value, uint32_t lr_value, uint32_t sp_value)
{
    g_preserve_data_lib.reserved[RESERVED_PC_INDEX] = pc_value;
    g_preserve_data_lib.reserved[RESERVED_LR_INDEX] = lr_value;
    g_preserve_data_lib.reserved[RESERVED_SP_INDEX] = sp_value;
}

void get_pc_lr_sp_value(uint32_t *pc_value, uint32_t *lr_value, uint32_t *sp_value)
{
    if (pc_value == NULL || lr_value == NULL || sp_value == NULL) {
        return;
    }
    *pc_value = g_preserve_data_lib.reserved[RESERVED_PC_INDEX];
    *lr_value = g_preserve_data_lib.reserved[RESERVED_LR_INDEX];
    *sp_value = g_preserve_data_lib.reserved[RESERVED_SP_INDEX];
}


#if !defined(BUILD_APPLICATION_ROM)
uint8_t get_last_slave_panic_id(cores_t core)
{
    UNUSED(core);
    return g_preserve_data_lib_ptr->last_panic.origin;
}

uint32_t get_last_slave_panic_code(cores_t core)
{
    UNUSED(core);
    return g_preserve_data_lib_ptr->last_panic.code;
}

uint32_t get_last_slave_panic_caller(cores_t core)
{
    UNUSED(core);
    return g_preserve_data_lib_ptr->last_panic.caller;
}
#endif

#if defined(BUILD_APPLICATION_STANDARD)
static preserve_cpu_info_t get_cpu_info(void)
{
    preserve_cpu_info_t temp;
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
    temp.pc = g_preserve_data_lib.exc_context.task_context.mepc;
    temp.lr = g_preserve_data_lib.exc_context.task_context.ra;
    temp.sp = g_preserve_data_lib.exc_context.task_context.sp;
#else
    temp.pc = g_preserve_data_lib.exception_stack_frame.stacked_pc;
    temp.lr = g_preserve_data_lib.exception_stack_frame.stacked_lr;
    temp.sp = g_preserve_data_lib.exception_stack_frame.stacked_psr;
#endif
    return temp;
}

void system_boot_reason_print(void)
{
    switch (get_system_boot_status()) {
        case REBOOT_SOFT_RESET_TRIGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, "System Boot Normal");
            break;
        case REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                              "System Boot Abnoraml, watchdog timeout");
            break;
        case REBOOT_SOFT_PANIC_TRIGGER_STATUS:
            /* reboot from cpu fault & wdt int etc. */
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, "System Boot Panic");
            break;
        case REBOOT_NMI_XIP_CTRL_TRIGER_STATUS:
        case REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                              "System Boot Abnoraml, xip crash");
            break;
        case REBOOT_HARD_FAULT_TRIGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                              "System Boot Abnoraml, hard fault crash");
            break;
        case REBOOT_POWER_ON_TRIGGER_STATUS:
            /* first power on, vsys power reset or power_on reset */
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, "System Power On");
            break;
        case REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS:
            oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO,
                              "System Boot Abnoraml, other core crash");
            break;
        default:
            oml_pf_log_print1(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                              "Unkown Sytem Boot Type:0x%x", get_system_boot_status());
            break;
    }
}

void system_boot_reason_process(void)
{
    unsigned int reset_cause = (unsigned int)get_cpu_utils_reset_cause();
    unsigned int reset_count = get_cpu_utils_reset_count();
    unsigned int ep_reboot_count = get_cpu_utils_epreset_count();
    oml_pf_log_print3(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO,\
                      "System Reboot cause:0x%x , total reboot count:%u, exception reboot count:%u",\
                      reset_cause, reset_count, ep_reboot_count);
    if ((reset_cause == REBOOT_CAUSE_APPLICATION_HARDFAULT) || (reset_cause == REBOOT_CAUSE_BT_HARDFAULT)) {
        oml_pf_log_print2(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO,\
                          "Exception reboot pc:0x%x , lr:%x",\
                          get_cpu_utils_exc_pc(), get_cpu_utils_exc_lr());
    }
    switch (reset_cause) {
        case REBOOT_CAUSE_BT_PANIC:
            oml_pf_log_print3(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO,"panic id:0x%x, \
                              panic code: 0x%x, panic caller: 0x%x" , get_last_slave_panic_id(CORES_BT_CORE), \
                              get_last_slave_panic_code(CORES_BT_CORE), get_last_slave_panic_caller(CORES_BT_CORE));
            break;
        case REBOOT_CAUSE_APPLICATION_PANIC:
            oml_pf_log_print3(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO,"panic id:0x%x, \
                              panic code: 0x%x, panic caller: 0x%x" , get_last_panic_id(), \
                              get_last_panic_code(), get_last_panic_caller());
            break;
        case REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST:
            panic(PANIC_CHIP_WDT_FRST, 0);
            break;
        case REBOOT_CAUSE_BT_WATCHDOG:
        case REBOOT_CAUSE_BT_HARDFAULT:
        case REBOOT_CAUSE_BT_NNMIFAULT:
            oml_pf_log_print2(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                              "bt exception reboot with pc:0x%x, lr: 0x%x" , \
                              get_cpu_info().pc, get_cpu_info().lr);
            break;
        case REBOOT_CAUSE_APPLICATION_HARDFAULT:
        case REBOOT_CAUSE_APPLICATION_NNMIFAULT:
        case REBOOT_CAUSE_APPLICATION_CHIP_WDT:
        case REBOOT_CAUSE_APPLICATION_XIP_CTRL:
        case REBOOT_CAUSE_APPLICATION_XIP_CACHE:
        case REBOOT_CAUSE_APPLICATION_MDMA:
        case REBOOT_CAUSE_APPLICATION_SMDMA:
            oml_pf_log_print2(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                              "app exception reboot pc:0x%x, lr: 0x%x" , \
                              get_cpu_info().pc, get_cpu_info().lr);
            break;
        default:
            break;
    }
}
#endif

static void print_system_boot_status(void)
{
    unsigned int reset_cause = (unsigned int)get_cpu_utils_reset_cause();

    switch (get_system_boot_status()) {
        case REBOOT_SOFT_RESET_TRIGER_STATUS:
            PRINT("System Boot Noraml" NEWLINE);
            break;
        case REBOOT_NMI_WDGTIMEOUT_TRIGGER_STATUS:
            PRINT("System Boot watchdog timeout" NEWLINE);
            break;
        case REBOOT_NMI_XIP_CTRL_TRIGER_STATUS:
        case REBOOT_NMI_XIP_CACHE_TRIGGER_STATUS:
            PRINT("System Boot Abnoraml, XIP crash" NEWLINE);
            break;
        case REBOOT_HARD_FAULT_TRIGER_STATUS:
            PRINT("System Boot Abnoraml, hard fault crash" NEWLINE);
            break;
        case REBOOT_POWER_ON_TRIGGER_STATUS:
            /* normal system reboot, except reboot */
            PRINT("System Power On" NEWLINE);
            break;
        case REBOOT_SOFT_PANIC_TRIGGER_STATUS:
            PRINT("System Boot Abnoraml, panic crash" NEWLINE);
            break;
        case REBOOT_OTHER_CORE_ABNORMAL_TRIGER_STATUS:
            PRINT("System Boot Abnoraml, other core crash" NEWLINE);
            break;
        case REBOOT_HARD_WDGTIMEOUT_TRIGER_STATUS:
            PRINT("System Boot Abnoraml, chip watchdog timeout" NEWLINE);
            break;
        default:
            PRINT("Unkown Sytem Boot Type 0x%x" NEWLINE, get_system_boot_status());
            break;
    }
    PRINT("System Reboot cause:0x%x, total reboot count:%u, exception reboot count:%u\r\n",\
          reset_cause, get_cpu_utils_reset_count(), get_cpu_utils_epreset_count());
    if (reset_cause == REBOOT_CAUSE_BT_PANIC) {
        PRINT("panic id:0x%x, panic code: 0x%x, panic caller: 0x%x\r\n", get_last_slave_panic_id(CORES_BT_CORE), \
              get_last_slave_panic_code(CORES_BT_CORE), get_last_slave_panic_caller(CORES_BT_CORE));
    } else if  (reset_cause == REBOOT_CAUSE_APPLICATION_PANIC) {
        PRINT("panic id:0x%x, panic code: 0x%x, panic caller: 0x%x\r\n", get_last_panic_id(), \
              get_last_panic_code(), get_last_panic_caller());
    } else if ((reset_cause == REBOOT_CAUSE_APPLICATION_HARDFAULT) || (reset_cause == REBOOT_CAUSE_BT_HARDFAULT)) {
        PRINT("Exception reboot pc:0x%x , lr:%x", get_cpu_utils_exc_pc(), get_cpu_utils_exc_lr());
    }
}

void show_reboot_info(void)
{
    system_boot_status_init();
    print_system_boot_status();
}
